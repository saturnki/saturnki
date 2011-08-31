//template <bool is_master>
struct Sh2
{

	struct Interrupt
	{
		int vector;
		int level;

		Interrupt(int vector_, int level_) : vector(vector_), level(level_) {}
	};

	std::vector<Interrupt> interrupts_pending;

	//boilerplate
	struct InterruptSortingPredicate {
		bool operator()(const Interrupt &left, const Interrupt &right) {
			return left.level < right.level;
		}
	};

	void SendInterruptRequest(int vector, int level)
	{
		Interrupt i(vector,level);

		int end = interrupts_pending.size();

		for(int j = 0; j < end; j++)
		{
			if(interrupts_pending[j].level == i.level)
				return;
		}

		interrupts_pending.push_back(i);
		std::sort(interrupts_pending.begin(), interrupts_pending.end(),InterruptSortingPredicate());//alas no lambdas
	}

	void CheckInterruptRequests()
	{
		if(interrupts_pending.size() > 0)
		{
			ServiceInterrupts(interrupts_pending[0]);
			interrupts_pending.pop_back();
		}
	}

	void AddCycles(const int &cycles_to_add)
	{
		u32 c = cycles_to_add;

		//fix me this is ridiculous
		while(c)
		{
			regs.cycles++;
	//		peripherals.ExecuteFreeRunningTimer(regs.cycles);
			c--;
		}
		//scheduler blah
	}
	Saturn &saturn;
	Sh2(Saturn &saturn_) : saturn(saturn_),peripherals(*this) {}
	//all can cause interrupts

#include "sh2_peripheral.h"

	bool ServiceInterrupts(const Interrupt &interrupt)
	{
		//TODO need to check for non-maskable interrupts

		if(interrupt.level > regs.sr.i0)
		{
			//save sr to stack
			regs.r[15] -= 4;
			Write<kLong>(regs.r[15],regs.sr);

			//save pc to stack
			regs.r[15] -= 4;
			Write<kLong>(regs.r[15],regs.pc);

			//copy accepted interrupt level
			regs.sr.i0 = interrupt.level;

			//read vector number
			regs.pc = Read<kLong>(regs.vbr + (interrupt.vector << 2));

			//read exception vector table?????

			//branch to service routine
			return true;
		}
		return false;
	}
	void PowerOn()
	{
		regs.pc = Read<kLong>(regs.vbr);
		regs.r[15] = Read<kLong>(regs.vbr+4);
	}

	bool is_master;

	void Reset()
	{
		regs.sr.i0 = 0xf;

		//i dunno if this is correct
		peripherals.regs.brr = 0xff;
		peripherals.regs.tdr = 0xff;
		peripherals.regs.ssr = 0x84;

		//master can't become slave etc
		if(is_master)
			peripherals.regs.bcr1.bus_arbitration_mode = 1;
		else
			peripherals.regs.bcr1.bus_arbitration_mode = 0;

		peripherals.regs.bcr1.long_wait_for_areas_2_3 = 3;
		peripherals.regs.bcr1.long_wait_for_area_1 = 3;
		peripherals.regs.bcr1.long_wait_for_area_0 = 3;
	}
	struct Registers
	{
		array::Array<u32,16> r;
		u32 gbr;
		u32 vbr;
		u32 mach;
		u32 macl;
		u32 pr;
		u32 pc;

		struct Sr
		{
			u12 rc0;
			u1 dmy0;
			u1 dmx0;
			u1 m;
			u1 q;
			u4 i0;
			u1 rf10;
			u1 rf00;
			u1 s;
			u1 t;

			//ehh i'm not sure what the best thing is
			//duping these funcs or not
			void SetT(int input)
			{
				if(input)
					t = 1;
				else
					t = 0;
			}

			void SetQ(int input)
			{
				if(input)
					q = 1;
				else
					q = 0;
			}

			void SetS(int input)
			{
				if(input)
					s = 1;
				else
					s = 0;
			}

			void SetM(int input)
			{
				if(input)
					m = 1;
				else
					m = 0;
			}

			operator int() const {
				return t | (s << 1) | (i0 << 4) | (q << 8) | (m << 9);
			}

			int operator  =(const int i) {
				t = i & 1;
				s = i >> 1;
				i0 = i >> 4;
				q = i >> 8;
				m = i >> 9;
				return t | (s << 1) | (i0 << 4) | (q << 8) | (m << 9);
			}
		};

		Sr sr;

		//yeah it's not really a register
		//but i like it better here
		u64 cycles;
	};

	Registers regs;

	//abcd is arbitrary, we have to name it something
	struct Instruction
	{
//		u4 a;
//		u4 b;
//		u4 c;
//		u4 d;
//		u8 cd;
//		u12 bcd;

		u16 all;

		inline u4 a()
		{
			return all >> 12;
		}

		inline u4 b()
		{
			return all >> 8;
		}

		inline u4 c()
		{
			return all >> 4;
		}

		inline u4 d()
		{
			return all;
		}

		inline u8 cd()
		{
			return all;
		}

		inline u12 bcd()
		{
			return all;
		}

		Instruction()
		{
		}

		Instruction(int i)
		{
			all = i;
#if 0
			a = i >> 12;
			b = i >> 8;
			c = i >> 4;
			d = i;
			cd = i;
			bcd = i;
#endif
		}
	};

	enum ArithmeticOperation
	{
		kAnd,
		kOr,
		kXor,
		kAdd,
		kSubtract,
		kIsEqual,
		kIsGreaterThanOrEqual,
		kIsGreaterThan,
		kNot
	};
	/*
	functions that factor out some shared functionality of multiple ops
	*/
	template <ArithmeticOperation op, typename T1, typename T2>
	u32 Math(T1 left_value, T2 right_value)
	{
		if(op == kAdd)
			return left_value + right_value;

		if(op == kSubtract)
			return left_value - right_value;

		if(op == kIsEqual)
			return left_value == right_value;

		if(op == kIsGreaterThanOrEqual)
			return left_value >= right_value;

		if(op == kIsGreaterThan)
			return left_value > right_value;

		if(op == kAnd)return (left_value & right_value);
		if(op == kXor)return (left_value ^ right_value);
		if(op == kOr) return (left_value | right_value);
		if(op == kNot) return ~right_value; 

		assert(false);
		return false;
	}

	template <OperationSize size>
	void Write(unsigned int address, unsigned int data)
	{
		int result1;

		switch(address >> 29)
		{
		case 0:
			//is cr set?
			//otherwise no cache
			result1 = saturn.CpuBusTransfer<size,kWrite>(address & 0x1FFFFFFF,data);
			break;
		case 1:
			//no cache
			result1 = saturn.CpuBusTransfer<size,kWrite>(address & 0x1FFFFFFF,data);
			break;
		case 7:
			peripherals.regs.Write<size>(address,data);
			break;
		}
	}
	template <OperationSize size>
	int Read(unsigned int address)
	{
		unsigned int result1;

		switch(address >> 29)
		{
		case 0:
			//is cr set?
			//otherwise no cache
			result1 = saturn.CpuBusTransfer<size,kRead>(address & 0x1FFFFFFF,0);
			break;
		case 1:
			//no cache
			result1 = saturn.CpuBusTransfer<size,kRead>(address & 0x1FFFFFFF,0);
			break;
		case 7:
			result1 = peripherals.regs.Read<size>(address);
			break;
		}
		return result1;
	}

	Instruction i;

	//ADD Rm,Rn
	//SUB Rm,Rn
	template<ArithmeticOperation op>
	void MathRmRn()
	{
		s32 m = i.c();
		s32 n = i.b();
		regs.r[n]=Math<op>(regs.r[n],regs.r[m]);
		regs.pc+=2;
		AddCycles(1);
	}

	//andi ori xori
	template<ArithmeticOperation op>
	void MathImmediate()
	{
		s32 cd = i.cd();
		regs.r[0] = Math<op>(regs.r[0],cd);
		regs.pc+=2;
		AddCycles(1);
	}

	//orm, xorm, andm,tstm
	template<ArithmeticOperation op, bool tstm>
	void MathM()
	{
		s32 cd = i.cd();
		int result = static_cast<s32>(Read<kByte>(regs.gbr + regs.r[0]));
		result = Math<op>(result,cd);
		if(tstm)
			regs.sr.SetT(result==0);
		else
			Write<kByte>(regs.gbr + regs.r[0],result);
		regs.pc+=2;
		AddCycles(3);
	}

	//ADDC Rm,Rn
	//SUBC Rm,Rn
	template<ArithmeticOperation op>
	void MathCarryRmRn() 
	{
		s32 m = i.c();
		s32 n = i.b();
		u32 temp1 = Math<op>(regs.r[n],regs.r[m]);
		u32 temp0 = regs.r[n];
		regs.r[n]=Math<op>(temp1,static_cast<u32>(regs.sr.t));//cast to shut up template error
		if(op == kAdd) 
		{
			regs.sr.SetT(temp0>temp1);
			if(temp1>regs.r[n])
				regs.sr.t = 1;
		}
		if(op == kSubtract) 
		{
			regs.sr.SetT(temp0<temp1);
			if(temp1<regs.r[n])
				regs.sr.t = 1;
		}
		regs.pc+=2;
		AddCycles(1);
	}


	template<ArithmeticOperation op>
	void MathVRmRn()//ADDV Rm,Rn
	{
		s32 m = i.c();
		s32 n = i.b();

		int destination;

		if(static_cast<long>(regs.r[n]) >= 0)
			destination = 0;
		else
			destination = 1;

		int source;

		if(static_cast<long>(regs.r[m]) >= 0)
			source = 0;
		else
			source = 1;

		source += destination;

		regs.r[n] = Math<op>(regs.r[n],regs.r[m]);

		int ans;

		if(static_cast<long>(regs.r[n]) >= 0)
			ans = 0;
		else 
			ans = 1;

		ans += destination;

		if(op == kAdd)
		{
			if(source == 0 || source == 2)
			{
				regs.sr.SetT(ans == 1);
			}
			else
				regs.sr.t = 0;
		}
		if(op == kSubtract)
		{
			if(source == 1)
			{
				regs.sr.SetT(ans == 1);
			}
		}

		regs.pc+= 2;
		AddCycles(1);
	}

	enum Destinations
	{
		kSr,
		kGbr,
		kVbr,
		kMach,
		kMacl,
		kPr
	};

	template <Destinations destination, typename T>
	void SetDestination(T value)
	{
		if(destination == kSr)
			regs.sr = value;
		else if(destination == kGbr)
			regs.gbr = value;
		else if(destination == kVbr)
			regs.vbr = value;
		else if(destination == kMach)
			regs.mach = value;
		else if(destination == kMacl)
			regs.macl = value;
		else if(destination == kPr)
			regs.pr = value;
		else
			return assert(false);
	}

	template <Destinations source>
	u32 GetSource()
	{
		if(source == kSr)
			return regs.sr;
		else if(source == kGbr)
			return regs.gbr;
		else if(source == kVbr)
			return regs.vbr;
		else if(source == kMach)
			return regs.mach;
		else if(source == kMacl)
			return regs.macl;
		else if(source == kPr)
			return regs.pr;
		else
			assert(false);
		return 0;
	}


	template <Destinations destination>
	void Ldc()
	{
		s32 m = i.b();
		SetDestination<destination>(regs.r[m]);
		regs.pc+=2;
		AddCycles(1);
	}

	template <Destinations destination>
	void Ldcm()
	{
		s32 m = i.b();
		SetDestination<destination>(Read<kLong>(regs.r[m]));
		regs.r[m]+=4;
		regs.pc+=2;
		AddCycles(3);
	}

	//the extra casting and sign extension make this a special case i think
	//even though it follows the MathRmRn pattern
	template<ArithmeticOperation comparison, typename T>
	void Cmp()
	{
		s32 m = i.c();
		s32 n = i.b();
		regs.sr.SetT(Math<comparison>(static_cast<T>(regs.r[n]),static_cast<T>(regs.r[m])));
		regs.pc+=2;
		AddCycles(1);
	}

	template<ArithmeticOperation comparison>
	void Cmp()
	{
		s32 n = i.b();
		regs.sr.SetT(Math<comparison>(static_cast<long>(regs.r[n]),0));
		regs.pc+=2;
		AddCycles(1);
	}

	template<Destinations destination>
	void Ldsm()
	{
		s32 m = i.b();
		SetDestination<destination>(Read<kLong>(regs.r[m]));
		regs.r[m]+=4;
		regs.pc+=2;
		AddCycles(1);
	}


	template <Destinations source>
	void Stc()
	{
		s32 n = i.b();
		regs.r[n]=GetSource<source>();
		regs.pc+=2;
		AddCycles(1);
	}

	template <Destinations source>
	void Stcm()
	{
		s32 n = i.b();
		regs.r[n]-=4;
		Write<kLong>(regs.r[n],GetSource<source>());
		regs.pc+=2;
		AddCycles(2);
	}

	void Nop()
	{
		regs.pc+=2;
		AddCycles(1);
	}

	//todo this could be folded into a regular read/write probably?
	u32 FetchInstruction(int address)
	{
		//not exactly sure this is correct but it gets rid of some junk at the top i guess
		address &= 0xFFFFFFF;
		//bios fetch
		if(address >= 0 && address < 0x80000)
			return saturn.CpuBusTransfer<kWord,kRead>(address);
		//low work ram
		else if(address >= 0x200000 && address < (0x200000+0x100000))
			return saturn.CpuBusTransfer<kWord,kRead>(address);
		//a bus cs0
		else if(address >= 0x2000000 && address < 0x3000000)
			return saturn.CpuBusTransfer<kWord,kRead>(address);
		//high work ram
		else if(address >= 0x6000000 && address < (0x6000000+0x100000))
			return saturn.CpuBusTransfer<kWord,kRead>(address);

		//everything else is impossible
		assert(false);
		return 0;
	}

	void Delay(int address)
	{
		u32 result = FetchInstruction(address);
		i = Instruction(result);

		(this->*instruction_table[result])();
		regs.pc-=2;
	}

	void IllegalOperation() {
		bool use_hle_bios = true;
		if(use_hle_bios)
		{
			saturn.bios_hle.BiosHandleFunc(*this);
			return;
		}
		printf("illegal op at %x\n",regs.pc);
		printf("cycles at %x\n",regs.cycles);

		assert(false);
	}

	template <typename T1, typename T2>
	void Mul()
	{
		s32 m = i.c();
		s32 n = i.b();
		T1 temp1 = static_cast<T1>(static_cast<T2>(regs.r[n]));
		T1 temp2 = static_cast<T1>(static_cast<T2>(regs.r[m]));
		regs.macl = temp1*temp2;
		regs.pc+=2;
		AddCycles(2);//actually 2-4 or something fixme
	}

	void Clrmac()
	{
		regs.macl = 0;
		regs.mach = 0;
		regs.pc+=2;
		AddCycles(1);
	}

	template <int value>
	void SetOrClearT()
	{
		regs.sr.t = value;
		regs.pc+=2;
		AddCycles(1);
	}

	void Div0u()
	{
		regs.sr.m = regs.sr.q = regs.sr.t = 0;
		regs.pc+=2;
		AddCycles(1);
	}

	template <Destinations source>
	void Sts()
	{
		s32 n = i.b();
		regs.r[n] = GetSource<source>();
		regs.pc+=2;
		AddCycles(1);
	}

	template <Destinations source>
	void Stsm()
	{
		s32 n = i.b();
		regs.r[n]-=4;
		Write<kLong>(regs.r[n],GetSource<source>());
		regs.pc+=2;
		AddCycles(1);
	}

	void Sleep()
	{
		AddCycles(3);//check me
	}

	void Rts()
	{
		int old_pc = regs.pc;
		regs.pc = regs.pr;
		AddCycles(2);
		Delay(old_pc + 2);
	}

	void Rte()
	{
		int old_pc = regs.pc;
		regs.pc = Read<kLong>(regs.r[15]);
		regs.r[15] += 4;
		regs.sr = Read<kLong>(regs.r[15]) & 0x3F3;
		regs.r[15] += 4;
		AddCycles(4);
		Delay(old_pc + 2);
	}

	//fix copypasting
	void Macl()
	{
		s32 m = i.c();
		s32 n = i.b();

		s32 tempn = (s32) Read<kLong>(regs.r[n]);
		regs.r[n] += 4;
		s32 tempm = (s32) Read<kLong>(regs.r[m]);
		regs.r[m] += 4;

		s32 fnLmL;

		if ((s32) (tempn^tempm) < 0)
			fnLmL =- 1;//uhh math on an uninitalized value?
		else
			fnLmL = 0;
		if (tempn < 0)
			tempn = 0 - tempn;
		if (tempm < 0)
			tempm = 0 - tempm;

		u32 temp1 = (u32) tempn;
		u32 temp2 = (u32) tempm;

		u32 RnL = temp1 & 0x0000FFFF;
		u32 RnH = (temp1 >> 16) & 0x0000FFFF;
		u32 RmL = temp2 & 0x0000FFFF;
		u32 RmH = (temp2 >> 16) & 0x0000FFFF;

		u32 temp0 = RmL * RnL;
		temp1 = RmH * RnL;
		temp2 = RmL * RnH;
		u32 temp3 = RmH * RnH;

		u32 Res2 = 0;
		u32 Res1 = temp1 + temp2;
		if (Res1 < temp1)
			Res2 += 0x00010000;

		temp1 = (Res1 << 16) & 0xFFFF0000;
		u32 Res0 = temp0 + temp1;
		if (Res0 < temp0)
			Res2++;

		Res2=Res2+((Res1>>16)&0x0000FFFF)+temp3;

		if(fnLmL < 0)
		{
			Res2=~Res2;
			if (Res0==0)
				Res2++;
			else
				Res0=(~Res0)+1;
		}
		if(regs.sr.s == 1)
		{
			Res0=regs.macl+Res0;
			if (regs.macl>Res0)
				Res2++;
			if (regs.mach & 0x00008000);
			else Res2 += regs.mach | 0xFFFF0000;
			Res2+=(regs.mach&0x0000FFFF);
			if(((s32)Res2<0)&&(Res2<0xFFFF8000))
			{
				Res2=0x00008000;
				Res0=0x00000000;
			}
			if(((s32)Res2>0)&&(Res2>0x00007FFF))
			{
				Res2=0x00007FFF;
				Res0=0xFFFFFFFF;
			};

			regs.mach=Res2;
			regs.macl=Res0;
		}
		else
		{
			Res0=regs.macl+Res0;
			if (regs.macl>Res0)
				Res2++;
			Res2+=regs.mach;

			regs.mach=Res2;
			regs.macl=Res0;
		}

		regs.pc+=2;
		AddCycles(3);//fix me
	}

	void Div0s()
	{
		s32 m = i.c();
		s32 n = i.b();
		regs.sr.SetQ(regs.r[n]&(1<<31));
		regs.sr.SetM(regs.r[m]&(1<<31));
		regs.sr.t = !(regs.sr.m == regs.sr.q);
		regs.pc+=2;
		AddCycles(1);

	}

	void Cmpstr()
	{
		s32 m = i.c();
		s32 n = i.b();
		unsigned long temp = regs.r[n]^regs.r[m];
		long hh = (temp >> 12) & 0xFF;
		long hl = (temp >> 8) & 0xFF;
		long lh = (temp >> 4) & 0xFF;
		long ll = temp & 0xFF;
		hh = hh&&hl&&lh&&ll;
		regs.sr.SetT(hh==0);
		regs.pc+=2;
		AddCycles(1);
	}

	void Xtrct()
	{
		s32 m = i.c();
		s32 n = i.b();
		unsigned long temp = (regs.r[m]<<16)&0xffff0000;
		regs.r[n] = (regs.r[n]>>16)&0xFFFF;
		regs.r[n] |= temp;
		regs.pc+=2;
		AddCycles(1);
	}

	void Div1()
	{
		u32 tmp0;
		u8 tmp1;

		s32 m = i.c();
		s32 n = i.b();
		
		u8 old_q = regs.sr.q;
		regs.sr.q = (u8)((0x80000000 & regs.r[n])!=0);
		regs.r[n] <<= 1;
		regs.r[n]|=(u32) regs.sr.t;

		switch(old_q)
		{
		case 0:
			switch(regs.sr.m)
			{
			case 0:
				tmp0 = regs.r[n];
				regs.r[n] -= regs.r[m];
				tmp1 = (regs.r[n] > tmp0);
				switch(regs.sr.q)
				{
				case 0:
					regs.sr.q = tmp1;
					break;
				case 1:
					regs.sr.q = (u8) (tmp1 == 0);
					break;
				}
				break;
			case 1:
				tmp0 = regs.r[n];
				regs.r[n] += regs.r[m];
				tmp1 = (regs.r[n] < tmp0);
				switch(regs.sr.q)
				{
				case 0:
					regs.sr.q = (u8) (tmp1 == 0);
					break;
				case 1:
					regs.sr.q = tmp1;
					break;
				}
				break;
			}
			break;
		case 1:
			switch(regs.sr.m)
			{
			case 0:
				tmp0 = regs.r[n];
				regs.r[n] += regs.r[m];
				tmp1 = (regs.r[n] < tmp0);
				switch(regs.sr.q)
				{
				case 0:
					regs.sr.q = tmp1;
					break;
				case 1:
					regs.sr.q = (u8) (tmp1 == 0);
					break;
				}
				break;
			case 1:
				tmp0 = regs.r[n];
				regs.r[n] -= regs.r[m];
				tmp1 = (regs.r[n] > tmp0);
				switch(regs.sr.q)
				{
				case 0:
					regs.sr.q = (u8) (tmp1 == 0);
					break;
				case 1:
					regs.sr.q = tmp1;
					break;
				}
				break;
			}
			break;
		}
		regs.sr.t = (regs.sr.q == regs.sr.m);
		regs.pc += 2;
		AddCycles(1);
	}

	void Dmulu()
	{
		u32 RnL,RnH,RmL,RmH,Res0,Res1,Res2;
		u32 temp0,temp1,temp2,temp3;
		s32 m = i.c();
		s32 n = i.b();

		RnL = regs.r[n] & 0x0000FFFF;
		RnH = (regs.r[n] >> 16) & 0x0000FFFF;
		RmL = regs.r[m] & 0x0000FFFF;
		RmH = (regs.r[m] >> 16) & 0x0000FFFF;

		temp0 = RmL * RnL;
		temp1 = RmH * RnL;
		temp2 = RmL * RnH;
		temp3 = RmH * RnH;

		Res2 = 0;
		Res1 = temp1 + temp2;
		if (Res1 < temp1)
			Res2 += 0x00010000;

		temp1 = (Res1 << 16) & 0xFFFF0000;
		Res0 = temp0 + temp1;
		if (Res0 < temp0)
			Res2++;

		Res2 = Res2 + ((Res1 >> 16) & 0x0000FFFF) + temp3;

		regs.mach = Res2;
		regs.macl = Res0;
		regs.pc += 2;
		AddCycles(2);
	}
	void Dmuls()
	{
		u32 RnL,RnH,RmL,RmH,Res0,Res1,Res2;
		u32 temp0,temp1,temp2,temp3;
		s32 tempm,tempn,fnLmL;
		s32 m = i.c();
		s32 n = i.b();

		tempn = (s32)regs.r[n];
		tempm = (s32)regs.r[m];
		if (tempn < 0)
			tempn = 0 - tempn;
		if (tempm < 0)
			tempm = 0 - tempm;
		if ((s32) (regs.r[n] ^ regs.r[m]) < 0)
			fnLmL = -1;
		else
			fnLmL = 0;

		temp1 = (u32) tempn;
		temp2 = (u32) tempm;

		RnL = temp1 & 0x0000FFFF;
		RnH = (temp1 >> 16) & 0x0000FFFF;
		RmL = temp2 & 0x0000FFFF;
		RmH = (temp2 >> 16) & 0x0000FFFF;

		temp0 = RmL * RnL;
		temp1 = RmH * RnL;
		temp2 = RmL * RnH;
		temp3 = RmH * RnH;

		Res2 = 0;
		Res1 = temp1 + temp2;
		if (Res1 < temp1)
			Res2 += 0x00010000;

		temp1 = (Res1 << 16) & 0xFFFF0000;
		Res0 = temp0 + temp1;
		if (Res0 < temp0)
			Res2++;

		Res2 = Res2 + ((Res1 >> 16) & 0x0000FFFF) + temp3;

		if (fnLmL < 0)
		{
			Res2 = ~Res2;
			if (Res0 == 0)
				Res2++;
			else
				Res0 =(~Res0) + 1;
		}
		regs.mach = Res2;
		regs.macl = Res0;
		regs.pc += 2;
		AddCycles(2);
	}

	enum ShiftDirection
	{
		kLeft,
		kRight
	};

	template<ShiftDirection direction, int bits>
	u32 Shift(const u32 &input)//must be unsigned or right will shift in 1s in the case of 0xF at the top
	{
		int l = input<<bits;
		int r = input>>bits;

		if(direction == kLeft)
			return input<<bits;
		if(direction == kRight)
			return input>>bits;

		assert(false);
		return 0;
	}

	//SHAL Rn(Same as SHLL)
	template<ShiftDirection d, int mask>
	void ShllShlr()
	{
		s32 n = i.b();
		regs.sr.SetT((regs.r[n]&mask));
		regs.r[n] = Shift<d,1>(regs.r[n]);
		regs.pc+=2;
		AddCycles(1);
	}

	//SHLL2, 8, 16
	template<int shift>
	void Shll()
	{
		s32 n = i.b();
		regs.r[n]<<=shift;
		regs.pc+=2;
		AddCycles(1);
	}
	//SHLR2 8, 16
	template<int shift, int mask>
	void Shlr()
	{
		s32 n = i.b();
		regs.r[n]>>=shift;
		regs.r[n]&=mask;
		regs.pc+=2;
		AddCycles(1);
	}

	void Dt()
	{
		s32 n = i.b();
		regs.r[n]--;
		regs.sr.SetT(regs.r[n]==0);
		regs.pc+=2;
		AddCycles(1);
	}

	//b
	void Shar()
	{
		s32 n = i.b();

		regs.sr.SetT(regs.r[n]&1);

		s32 temp;

		if((regs.r[n]&(1<<31))==0)
			temp = 0;
		else
			temp = 1;

		regs.r[n] >>= 1;

		if(temp == 1)
			regs.r[n] |= (1<<31);
		else
			regs.r[n] &= 0x7FFFFFFF;

		regs.pc+=2;
		AddCycles(1);
	}

	template <u32 mask1,u32 mask2,u32 mask3, ShiftDirection direction>
	void Rotc()
	{
		s32 n = i.b();
		s32 temp;
		if ((regs.r[n]&mask1)==0)
			temp=0;
		else
			temp=1;

		regs.r[n] = Shift<direction,1>(regs.r[n]);

		Factored<mask2,mask3>(n);

		regs.sr.SetT(temp);

		regs.pc+=2;
		AddCycles(1);
	}

	template <int mask1, int mask2>
	void Factored(long n)
	{
		if(regs.sr.t)
			regs.r[n]|=mask1;
		else
			regs.r[n]&=mask2;
	}
	template <ShiftDirection direction,u32 mask1, u32 mask2>
	void Rot()
	{
		s32 n = i.b();
		regs.sr.SetT(regs.r[n]&(1<<31));
		regs.r[n] = Shift<direction,1>(regs.r[n]);

		Factored<mask1,mask2>(n);

		regs.pc+=2;
		AddCycles(1);
	}

	template<typename T1, typename T2>
	void Ext()
	{
		s32 m = i.c();
		s32 n = i.b();
		regs.r[n] = static_cast<T1>(static_cast<T2>(regs.r[m]));
		regs.pc+=2;
		AddCycles(1);
	}

	void Bsr(int displacement)
	{
		u32 old_pc = regs.pc;
		if(displacement&0x800)
			displacement |= 0xFFFFF000;
		regs.pr = regs.pc + 4;
		regs.pc = regs.pc + (displacement << 1) + 4;

		AddCycles(2);
		Delay(old_pc+2);
	}

	void Bfs()
	{
		s32 d = i.cd();
		if(regs.sr.t == 0)
		{
			s32 displacement = static_cast<s32>(static_cast<s8>(d));
			int old_pc = regs.pc;

			regs.pc = regs.pc + (displacement << 1) + 4;
			AddCycles(2);
			Delay(old_pc + 2);
		}
		else
		{
			regs.pc+=2;
			AddCycles(1);
		}
	}

	void Addi()
	{
		s32 cd = i.cd();
		s32 b = i.b();
		cd = static_cast<s32>(static_cast<s8>(cd));
		regs.r[b] += cd;
		regs.pc+=2;
		AddCycles(1);
	}
	void Ldspr()
	{
		s32 b = i.b();
		regs.pr = regs.r[b];
		regs.pc += 2;
		AddCycles(1);
	}

#include "sh2_move.h"

	//BRANCH INSTRUCTIONS
	template<int mask1, int mask2, int mask3>
	long BranchMask(int d)
	{
		if ((d&mask1)==0) 
			return (mask2 & static_cast<long>(d));
		else 
			return (mask3 | static_cast<long>(d));
	}

	template <bool is_bfs>
	void BfBfs()/* BF disp */
	{
		s32 d = i.cd();

		long disp = BranchMask<0x80,0xFF,0xFFFFFF00>(d);
		if (regs.sr.t==0) 
		{
			u32 old_pc = regs.pc;
			regs.pc=regs.pc+(disp<<1)+4;
			if(is_bfs)
			{
				AddCycles(2);//fixme
				Delay(old_pc+2);
			}
			else
				AddCycles(3);//bf
		}
		else 
		{
			AddCycles(1);
			regs.pc+=2;
		}
	}

	void Bra() /* BRA disp */
	{
		s32 d = i.bcd();
		long disp = BranchMask<0x800,0x00000FFF,0xFFFFF000>(d);
		u32 old_pc=regs.pc;
		regs.pc=regs.pc+(disp<<1)+4;
		AddCycles(2);
		Delay(old_pc+2);
	}
	void Bsr() /* BSR disp */
	{
		s32 d = i.bcd();
		long disp = BranchMask<0x800,0x00000FFF,0xFFFFF000>(d);
		regs.pr=regs.pc+4;
		u32 old_pc = regs.pc;
		regs.pc=regs.pc+(disp<<1) + 4;
		AddCycles(2);
		Delay(old_pc+2);
	}
	void Braf() /* BRAF Rm */
	{
		s32 m = i.b();
		unsigned long old_pc=regs.pc;
		regs.pc+=regs.r[m] + 4;
		AddCycles(2);
		Delay(old_pc+2);
	}

	void Bsrf() /* BSRF Rm */
	{
		s32 m = i.b();
		regs.pr=regs.pc+4;
		regs.pc+=regs.r[m];
		AddCycles(2);
		Delay(regs.pr+2);
	}

	//bt, if is_bts == true bts
	//BT disp, BTS disp
	template<bool is_bts>
	void BtBts() /* BTS disp */
	{
		s32 d = i.cd();
		long disp = BranchMask<0x80,0x000000FF,0xFFFFFF00>(d);
		unsigned long old_pc=regs.pc;
		if (regs.sr.t==1) {
			regs.pc=regs.pc+(disp<<1)+4;
			if(is_bts)
			{
				AddCycles(2);
				Delay(old_pc+2);
			}
			else
				AddCycles(3);
		}
		else 
		{
			AddCycles(1);
			regs.pc+=2;
		}
	}
	//ENB OF BRANCH INSTRUCTIONS


	//JUMP
	template <bool is_jsr>
	void JmpJsr()
	{
		s32 m = i.b();
		u32 old_pc = regs.pc;
		if(is_jsr)
			regs.pr = regs.pc + 4;
		regs.pc = regs.r[m];
		AddCycles(2);
		Delay(old_pc + 2);
	}
	///END JUMP

	//SWAP

	//m = c, n = b
	void Swapb()
	{
		s32 m = i.c();
		s32 n = i.b();
		u32 temp0 = regs.r[m]&0xFFFF0000;
		u32 temp1 = (regs.r[m]&0xFF)<<8;
		regs.r[n] = (regs.r[m]>>8)&0xFF;
		regs.r[n]= regs.r[n] | temp1 | temp0;
		regs.pc +=2;
		AddCycles(1);
	}

	void Swapw()
	{
		s32 m = i.c();
		s32 n = i.b();
		u32 temp = (regs.r[m]>>16)&0xFFFF;
		regs.r[n]=regs.r[m]<<16;
		regs.r[n]|=temp;
		regs.pc+=2;
		AddCycles(1);
	}

	//END SWAP

	//cd
	void Cmpim()
	{
		s32 i_ = i.cd();
		s32 immediate = static_cast<s32>(static_cast<s8>(i_));
		regs.sr.SetT(regs.r[0] == immediate);
		regs.pc+=2;
		AddCycles(1);
	}

	//TST
	//c,b
	void Tst()
	{
		s32 m = i.c();
		s32 n = i.b();
		regs.sr.SetT((regs.r[n]&regs.r[m])==0);
		regs.pc += 2;
		AddCycles(1);
	}
	//cd
	void Tsti()
	{
		s32 i_ = i.cd();
		s32 temp=regs.r[0]&i_;
		regs.sr.SetT(temp == 0);
		regs.pc+=2;
		AddCycles(1);
	}
	//cd
	void Tstm(long i)
	{
		s32 temp= Read<kByte>(regs.gbr+regs.r[0]);
		temp&=i;
		regs.sr.SetT(temp == 0);
		regs.pc+=2;
		AddCycles(3);
	}
	//TST


	//NEG
	template <bool is_negc>
	void Neg()
	{
		s32 m = i.c();
		s32 n = i.b();
		if(is_negc)
		{
			u32 temp=0-regs.r[m];
			regs.r[n] = temp - regs.sr.t;
			regs.sr.SetT(0 < temp);
			if (temp < regs.r[n])
				regs.sr.t=1;
		}
		else
			regs.r[n] = 0-regs.r[m];
		regs.pc+=2;
		AddCycles(1);
	}
	//END NEG
	//Movl4<Byte,0,0x80,0xFF,0xFFFFFF00>(m,d); return;//MOVBL4 MOV.B @(disp,Rm),R0 

	int GetWatchdogTimerDivisor(int input)
	{
		switch(input & 7)
		{
		case 0: return 2;
		case 1: return 64;
		case 2: return 128;
		case 3: return 256;
		case 4: return 512;
		case 5: return 1024;
		case 6: return 4096;
		case 7: return 8192;
		}
		assert(false);
	}

	int GetIncrementValue(int mode)
	{
		switch(mode)
		{
		case 0: return 0;
		case 1: return 1;
		case 2: return -1;
		default: assert(false); break;
		}
		assert(false);
		return 0;
	}

	template <OperationSize size>
	void FactoredOutDma(
		u32 &sar, 
		u32 &dar,
		int source_increment,
		int destination_increment)
	{
		unsigned int source_data = saturn.CpuBusTransfer<size,kRead>(sar);
		saturn.CpuBusTransfer<size,kWrite>(dar,source_data);
		sar += source_increment;
		dar += destination_increment;
	}

	template <OperationSize size, bool sixteen_byte>
	void DmaDoTransfer(
		int source_increment, 
		int destination_increment,
		int tcr,
		u32 &sar, u32 dar)
	{
		source_increment *= size;
		destination_increment *= size;

		if(sixteen_byte)
		{
			for (int i = 0; i < tcr; i+=4) 
			{
				for(int j = 0; j < 4; j++) 
				{
					FactoredOutDma<size>(sar,dar,source_increment,destination_increment);
				}
			}
		}
		else
		{
			//dma without timing, fix me
			for(int i = 0; i < tcr; i++)
			{
				FactoredOutDma<size>(sar,dar,source_increment,destination_increment);
			}
		}

		tcr = 0;
	}

	void DmaExecute(
		OnChipPeripherals::Registers::Chcr& chcr,
		u8& vcrdma,
		u24& tcr,
		u32 &sar, 
		u32 &dar
		)
	{
		if(!chcr.transfer_end)
		{
			int destination_increment = GetIncrementValue(chcr.destination_address_mode);
			int source_increment = GetIncrementValue(chcr.source_address_mode);

			switch(chcr.transfer_size)
			{
			case 0://1 byte
				DmaDoTransfer<kByte,false>(source_increment,destination_increment,tcr,sar,dar);
				break;
			case 1://2 byte
				DmaDoTransfer<kWord,false>(source_increment,destination_increment,tcr,sar,dar);
				break;
			case 2://4 byte
				DmaDoTransfer<kLong,false>(source_increment,destination_increment,tcr,sar,dar);
				break;
			case 3://16 bytes
				DmaDoTransfer<kLong,true>(source_increment,destination_increment,tcr,sar,dar);
				break;
			}
		}

		if(chcr.interrupt_enable)
		{
			SendInterruptRequest(vcrdma,peripherals.regs.ipra.dma_controller_interrupt_priority_level);
		}

		chcr.transfer_end = true;
	}

	template <int channel>
	void DmaExecuteChannel()
	{
		//we might as well just have arrays instead.
		if(channel == 0)
			DmaExecute(
			peripherals.regs.chcr0,
			peripherals.regs.vcrdma0,
			peripherals.regs.tcr0,
			peripherals.regs.sar0,
			peripherals.regs.dar0);
		else if(channel == 1)
			DmaExecute(
			peripherals.regs.chcr1,
			peripherals.regs.vcrdma1,
			peripherals.regs.tcr1,
			peripherals.regs.sar1,
			peripherals.regs.dar1);
	}

	void DmaChannelPriority()
	{

		if(peripherals.regs.dmaor.address_error_flag 
			|| peripherals.regs.dmaor.nmi_flag)
			return;

		//afaict none of this matters until we get timing

		if(peripherals.regs.chcr0.dma_enable)
			DmaExecuteChannel<0>();
		if(peripherals.regs.chcr1.dma_enable)
			DmaExecuteChannel<1>();
#if 0
		if(peripherals.regs.chcr0.dma_enable && peripherals.regs.chcr1.dma_enable)
		{
			if(round_robin)
			{
				//doesn't matter until we have dma timing.
				DmaExecuteChannel<0>();
				DmaExecuteChannel<1>();
			}
			else
			{
				//0 > 1 priority
				DmaExecuteChannel<0>();
				DmaExecuteChannel<1>();
			}
		}
		else
		{
			//one channel wants dma
		}
#endif
	}

	typedef void (Sh2::*OpcodeType)();
	OpcodeType instruction_table[0x10000];

	void InitializeOpcodeTable()
	{
		for(int i = 0; i < 0x10000; i++)
		{
			Instruction j(i);
			instruction_table[i] = DecodeInstruction(j);
		}
	}

	void RunThisOp(int op)
	{
		static bool table_initialized = false;
		if(!table_initialized)
		{
			InitializeOpcodeTable();
			table_initialized = true;
		}
		i = Instruction(op);
		(this->*instruction_table[op])();
	}

	int Execute()
	{
		static bool table_initialized = false;
		if(!table_initialized)
		{
			InitializeOpcodeTable();
			table_initialized = true;
		}
		int old_cycles = regs.cycles;

		u32 op = FetchInstruction(regs.pc);
		i = Instruction(op);// = FetchInstruction(regs.pc);


//		int pc;
//		static int execs = 0;
//		  execs++;
//		if(execs == 0x00598c90)
//			int zzz = 0;

		u32 cycles_before = regs.cycles;
		//DecodeAndExecuteInstruction(i);
		(this->*instruction_table[op])();
		u32 cycles_after = regs.cycles;
#ifdef GLUE
		glue::sh2trace::CompareTrace(regs.pc,regs.r);
		glue::Sh2TimingTest(cycles_after - cycles_before);
#endif
		//frc
		//watchdog

		return regs.cycles - old_cycles;//how many cycles did we execute?
	}

#include "sh2_decoding.h"
};