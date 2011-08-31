struct ScspDsp
{
	array::RamArray<0x80000> &sound_ram;

	ScspDsp(array::RamArray<0x80000> &sound_ram_) : sound_ram(sound_ram_)
	{
		regs.stopped = true;
	}
	static s24 Inputs(const u6 & ira, const s24 * mems, const s24 * mixs)
	{
		if(ira <= 0x1f)
			return mems[ira];
		else if(ira <= 0x2f)
			return mixs[ira - 0x20] << 4;
		else if(ira <= 0x31)
			return 0;
	}
	static s24 Xsel(const s24 &inputs, const s24&temp_value, const u1 & xsel)
	{
		if(xsel)
			return inputs;
		else
			return temp_value;
	}

	static s13 Ysel(const u2 &ysel, const s16& coef_value, const s24 &y_reg, const s13 & frc_reg)
	{
		switch(ysel)
		{
		case 0:
			return frc_reg;
		case 1:
			return coef_value >> 3;
		case 2:
			return y_reg >> 11;
		case 3:
			return y_reg >> 4;
		}
	}

	static s26 SelGaInv(const u1 &zero, const u1 &bsel, const u1&negb,const s24&temp_value, const s26 &acc)
	{
		s26 b;

		if(zero)
			return 0;
		else
		{
			if(bsel)
				b = acc;
			else
				b = temp_value;

			if(negb)
				b = 0 - b;

			return b;
		}
	}

	static s24 OverflowProtect(s24 i)
	{
		s32 result = i;
		if(result>0x007FFFFF) 
			result=0x007FFFFF; 
		if(result<(-0x00800000))
			result=-0x00800000;
		return result;
	}

	static s24 Shifter(const s26 &acc, const u2 &shift)
	{
		s24 result;
		switch(shift)
		{
		case 0:
			{
				result=acc;
				u32 blah = result;
				result = OverflowProtect(result);
				u32 blah2 = result;
			}
			break;
		case 1:
			result=acc*2;
			result = OverflowProtect(result);
			break;
		case 2:
			result=acc*2;
			break;
		case 3:
			result=acc;
			break;
		}
		return result;
	}

	static u26 Multi(const s64 &x, const s64 &y)
	{
		return (x * y) >> 12;
	}

	static u32 FrcReg(const u2 & shift, const s24 &shifted)
	{
		if(shift == 3)
			return shifted;
		else
			return shifted >> 11;
	}

	static u12 AselAdrsReg(const s24 &shifted, const u2 &shift, const s24 &inputs)
	{
		if(shift == 3)
			return shifted >> 12;
		else
			return inputs >> 16;
	}

	struct Context
	{
		s32 memval;
		s26 acc;
		s13 frc_reg;
		s24 y_reg;
		u13 adrs_reg;
	}c;

	struct Debug
	{
		s32 shifted;
		s32 x,y,b;
		s32 inputs;
		u32 addr;
	}debug;

	void Exec()
	{
		if(regs.stopped)
			return;

		u16 opcode[4] = {0};

		int pos = regs.pc * 4;

		for(int i = 0; i < 4; i++)
			opcode[i] = regs.mpro[pos+i];

		Op op(opcode);

		u7 temp_index = op.tra+regs.dec;
		s24 temp_value = regs.temp[temp_index];

		s24	inputs = Inputs(op.ira,regs.mems.data_,regs.mixs.data_);

		if(op.iwt)
		{
			regs.mems[op.iwa] = c.memval;
			if(op.ira == op.iwa)
				inputs = c.memval;
		}

		s26 b = SelGaInv(op.zero,op.bsel,op.negb,temp_value,c.acc);

		s24 x = Xsel(inputs,temp_value,op.xsel);

		s13 y = Ysel(op.ysel,regs.coef[op.coef],c.y_reg,c.frc_reg);

		if(op.yrl)
			c.y_reg = inputs;

		s24 shifted = Shifter(c.acc,op.shift);

		s32 v = Multi(x,y);

		c.acc = v + b;

		if(op.twt)
			regs.temp[temp_index] = shifted;

		if(op.frcl)
			c.frc_reg = FrcReg(op.shift,shifted);

//		debug.addr = 0;

		if(op.mrd || op.mwt)
		{
			u32 addr = regs.madrs[op.masa];

			if(!op.table)
				addr+=regs.dec;
			if(op.adreb)
				addr+=c.adrs_reg;
			if(op.nxadr)
				addr++;
			if(!op.table)
				addr&=regs.ring_buffer_length-1;
			else
				addr&=0xffff;

			addr+= regs.ring_buffer_pointer << 12;

			debug.addr = addr;

bool odd_step = regs.pc&1;


			if(op.mrd && odd_step)
			{
#if 0
				if(op.nofl)
					c.memval = sound_ram.Read<kWord>(addr) << 8;
				else
					c.memval = Unpack(sound_ram.Read<kWord>(addr));
#else
#endif
			}

			if(op.mwt && odd_step)
			{
#if 0
				if(op.nofl)
					sound_ram.Write<kWord>(addr,shifted >> 8);
				else
					sound_ram.Write<kWord>(addr,Pack(shifted));
#else
#endif
			}
		}

		if(op.adrl)
			c.adrs_reg = AselAdrsReg(shifted,op.shift,inputs);

		if(op.ewt)
			regs.efreg[op.ewa]+=shifted >> 8;

		debug.shifted = shifted;
		debug.x = x;
		debug.y = y;
		debug.b = b;
		debug.inputs = inputs;
		//debug.addr = addr;

		regs.pc++;

		//regs.dec--; dec is every sample, not every cycle
	}

	struct Registers
	{
		array::Array<s16,64> coef; //16 bit signed
		array::Array<u16,32> madrs; //offsets (in words), 16 bit
		array::Array<u16,128*4> mpro; //128 steps 64 bit
		array::Array<s24,128> temp; //TEMP regs,24 bit signed
		array::Array<s24,32> mems; //MEMS regs,24 bit signed
		u32 dec;

		int ring_buffer_length;
		int ring_buffer_pointer;

		int pc;

		bool stopped;

		array::Array<s16,16> efreg;

		//input
		array::Array<s24,16> mixs; //MIXS, 24 bit signed
		array::Array<s16,2> exts; //External inputs (CDDA) 16 bit signed
	}regs;

	struct Op
	{
		u7 tra;
		u1 twt;
		u7 twa;

		u1 xsel;
		u2 ysel;
		u6 ira;
		u1 iwt;
		u5 iwa;

		u1 table;
		u1 mwt;
		u1 mrd;
		u1 ewt;
		u4 ewa;
		u1 adrl;
		u1 frcl;
		u2 shift;
		u1 yrl;
		u1 negb;
		u1 zero;
		u1 bsel;

		u1 nofl;
		u6 coef;

		u5 masa;
		u1 adreb;
		u1 nxadr;

		Op(u16* opcode)
		{
			tra=opcode[0]>>8;
			twt=opcode[0]>>7;
			twa=opcode[0]>>0;

			xsel=opcode[1]>>15;
			ysel=opcode[1]>>13;
			ira= opcode[1]>>6;
			iwt= opcode[1]>>5;
			iwa= opcode[1]>>0;

			table=opcode[2]>>15;
			mwt=opcode[2]>>14;
			mrd=opcode[2]>>13;
			ewt=opcode[2]>>12;
			ewa=opcode[2]>>8;
			adrl=opcode[2]>>7;
			frcl=opcode[2]>>6;
			shift=opcode[2]>>4;
			yrl=opcode[2]>>3;
			negb=opcode[2]>>2;
			zero=opcode[2]>>1;
			bsel=opcode[2]>>0;

			nofl=opcode[3]>>15;
			coef=opcode[3]>>9;

			masa=opcode[3]>>2;
			adreb=opcode[3]>>1;
			nxadr=opcode[3]>>0;
		}
	};

	static u16 Pack(const s32 &val)
	{
		int sign = (val >> 23) & 0x1;
		u32 temp = (val ^ (val << 1)) & 0xFFFFFF;
		int exponent = 0;

		for (int k=0; k<12; k++)
		{
			if (temp & 0x800000)
				break;
			temp <<= 1;
			exponent += 1;
		}

		s32 result;

		if (exponent < 12)
			result = (val << exponent) & 0x3FFFFF;
		else
			result <<= 11;
		result >>= 11;
		result |= sign << 15;
		result |= exponent << 11;

		return (u16)result;
	}

	static s32 Unpack(const u16 & val)
	{
		int sign = (val >> 15) & 0x1;
		int exponent = (val >> 11) & 0xF;
		int mantissa = val & 0x7FF;
		s32 uval = mantissa << 11;

		if (exponent > 11)
			exponent = 11;
		else
			uval |= (sign ^ 1) << 22;
		uval |= sign << 23;
		uval <<= 8;
		uval >>= 8;
		uval >>= exponent;

		return uval;
	}

	void SetSample(int sample, int isel)
	{
		regs.mixs[isel]+=sample;
	}

	void Start()
	{
	}
	void WriteWord(int address, int data)
	{
		if(address >= 0x700 && address < 0x780)
			regs.coef[(address - 0x700)/2] = data;
		else if(address >= 0x780 && address <= 0x7bf)
			regs.madrs[(address - 0x780)/2] = data;
		else if(address >= 0x800 && address < 0xc00)
			regs.mpro[(address - 0x800)/2] = data;

		if(address == 0xbf0)
			regs.stopped = false;
	}
};
