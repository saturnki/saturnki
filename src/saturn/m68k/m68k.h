//todo this needs a massive cleanup and a lot of bugfixes

struct M68k
{
	Scsp & scsp;
	M68k(Scsp & scsp_) : scsp(scsp_)
	{
		reset_called = false;
	}

	bool reset_called;

	struct Registers
	{
		array::Array<u32,8> d;
		array::Array<u32,8> a;
		u32 pc;

		struct StackPointers
		{
			u32 user;
			u32 interrupt;
			u32 master;
		};

		StackPointers stack_pointers;

		struct StatusRegister
		{
			u2 trace_enable;
			u1 supervisor;//1 = supervisor, 0 = user
			u1 master;//1 = master, 0 = interrupt
			//0
			u3 interrupt_priority_mask;
			//0 0 0
			struct ConditionCodeRegister
			{
				u1 x;//extend
				u1 n;//negative
				u1 z;//zero
				u1 v;//overflow
				u1 c;//carrry				

				operator int() const {
					return (x << 4) | (n << 3) | (z << 2) | (v << 1) | c;
				}

				int operator  =(const int i) {
					x = i >> 4;
					n = i >> 3;
					z = i >> 2;
					v = i >> 1;
					c = i;
					return i & 0x1f;
				}
				enum ConditionalPredicates
				{
					kTrue,
					kFalse,
					kHigh,
					kLowOrSame,
					kCarryClear,
					kCarrySet,
					kNotEqual,
					kEqual,
					kOverflowClear,
					kOverflowSet,
					kPlus,
					kMinus,
					kGreaterOrEqual,
					kLessThan,
					kGreaterThan,
					kLessOrEqual
				};

				bool ConditionIsSatisfied(const unsigned int &input)
				{
					ConditionalPredicates predicate = static_cast<ConditionalPredicates>(input);

					switch (predicate)
					{
					case kTrue: // T
						return true;
					case kFalse: // F
						return false;
					case kHigh: // HI
						return !c && !z;
					case kLowOrSame: // LS
						return c || z;
					case kCarryClear: // CC(HI)
						return !c;
					case kCarrySet: // CS(LO)
						return c;
					case kNotEqual: // NE
						return !z;
					case kEqual: // EQ
						return z;
					case kOverflowClear: // VC
						return !v;
					case kOverflowSet: // VS
						return v;
					case kPlus: // PL
						return !n;
					case kMinus: // MI
						return n;
					case kGreaterOrEqual: // GE
						return n && v || !n && !v;
					case kLessThan: // LT
						return n && !v || !n && v;
					case kGreaterThan: // GT
						return n && v && !z || !n && !v && !z;
					case kLessOrEqual: // LE
						return z || n && !v || !n && v;
					default:
						assert(false);
					}

					assert(false);
					return 0;
				}
			}ccr;

			operator int() const {
				return  (trace_enable << 14) | (supervisor << 13) | (master << 12) | (interrupt_priority_mask << 8) | ccr;
			}

			int operator  =(const int i) {
				trace_enable = i >> 14;
				supervisor = i >> 13;
				master = i >> 12;
				interrupt_priority_mask = i >> 8;
				ccr = i;
				return *this;
			}

		}sr;

		int vbr;
		int interrupt_level;

		u64 cycles_executed;//yeah it's not really a reg but whatever
	}regs;

	void Jump(unsigned int new_pc)
	{
		regs.pc = new_pc;
	}

	void Reset()
	{
		regs.sr.interrupt_priority_mask = 7;
		regs.vbr = 0;
		//switch to supervisor mode
		SetSAndMFlags<kBoth>(true,false);
		Jump(0);
		//stack pointer

		regs.a[7] = FetchImmediate<kLong>(regs.pc);
		regs.pc = FetchImmediate<kLong>(regs.pc);

		printf("REMOVE ME REMOVE ME HACKS regs.sr.ccr.z = true;\n");
		regs.sr.ccr.z = true;
		Jump(regs.pc);

		reset_called = true;
	}

	void AddCycles(int cycles_to_add)
	{
		regs.cycles_executed += cycles_to_add;
	}

	template <OperationSize size>
	void IoDoCycles()
	{
		if(size == kByte || size == kWord)
		{
			AddCycles(4);
		}
		if(size == kLong)
		{
			//long reads are actually 2 different accesses
			//access 1
			AddCycles(4);
			//access 2
			AddCycles(4);
		}
	}
	template <OperationSize size>
	void Write(int address, int data)
	{
		IoDoCycles<size>();

		if (address < 0x80000)
			return scsp.sound_ram.Write<size>(address,data);
		else
			scsp.Write<size>(address,data);
	}
	template <OperationSize size>
	unsigned long Read(int address)
	{
		IoDoCycles<size>();

		if(address < 0x80000)
			return scsp.sound_ram.Read<size>(address & 0x7FFFF);
		else
			return scsp.Read<size>(address);
	}

	template <OperationSize size>
	unsigned int FetchImmediate(u32 &pc) {
		if(size == kByte)
		{
			pc+=2;
			return Read<kWord>(pc-2);
		}
		else
		{
			pc+=size;
			return Read<size>(pc-size);
		}
	}

	void SetFlagsNZNoVNoC(unsigned int result)
	{
		regs.sr.ccr.n = result >> 24;
		regs.sr.ccr.z = result == 0;
		regs.sr.ccr.v = 0;
		regs.sr.ccr.c = 0;
	}

	unsigned int Index(unsigned int an)
	{
		unsigned long extension = FetchImmediate<kWord>(regs.pc);

		unsigned int xn = regs.d[extension >> 12];
		if((extension & 0x800) == 0)
		{
			//make it a word instead of a long
			xn = static_cast<signed short>(xn);
			int qrs = 0;
			int jah = qrs;
		}

		//it apparently takes 2 cycles to calc the index
		//or something
		AddCycles(2);

		return an + xn + static_cast<signed char>(extension);
	}

	template <OperationSize size>
	int PopStack()
	{
		regs.a[7] = regs.a[7] + size;
		return Read<size>(regs.a[7] - size);
	}

	template <OperationSize size>
	unsigned int MaskedCombine(unsigned int upper_part, unsigned int lower_part)
	{
		unsigned int upper_mask;
		unsigned int lower_mask;
		if(size == kWord)
		{
			upper_mask = 0xFFFF0000;
			lower_mask = 0xFFFF;
		}
		if(size == kByte)
		{
			upper_mask = 0xFFFFFF00;
			lower_mask = 0xFF;
		}
		if(size == kLong)
		{
			return lower_part;
		}

		unsigned int high = upper_part & (upper_mask);
		unsigned int low = lower_part & lower_mask;
		unsigned int result = low | high;
		return result;
	}

	enum OperationType
	{
		kAdd,
		kAnd,
		kSub,
		kCmp,
		kEor,
		kOr
	};

	template <OperationSize size, OperationType type, typename T>
	bool IsOverflow(T s, T d, T r)
	{
		if(type == kAdd)
		{
			if(size == kByte)
				return ((((s^r) & (d^r)) >> 7)&1) == 1;
			if(size == kWord)
				return ((((s^r) & (d^r)) >>15)&1) == 1;
			if(size == kLong)
				return ((((s^r) & (d^r)) >>31)&1) == 1;
		}
		else if(type == kSub || type == kCmp)
		{
			if(size == kByte)
				return ((((s^d) & (r^d))>> 7)&1) == 1;
			if(size == kWord)
				return ((((s^d) & (r^d))>>15)&1) == 1;
			if(size == kLong)
				return ((((s^d) & (r^d))>>31)&1) == 1;
		}
		else if(type == kAnd || type == kEor || type == kOr)
		{
			//dunno
			return false;
		}
		assert(false);
	}

	//note that this requires 64 bit input
	template <OperationSize size, typename T>
	bool IsCarry(T i)
	{
		if(size == kByte)
			return i & 0x100;
		if(size == kWord)
			return i & 0x10000;
		if(size == kLong)
			return i & 0x100000000;
		assert(false);
		return 0;
	}

	template <OperationSize size, typename T>
	bool IsNegative(T i)
	{
		if(size == kByte)
			return i & 0x80;
		if(size == kWord)
			return i & 0x8000;
		if(size == kLong)
			return i & 0x80000000;

		assert(false);
		return 0;
	}
	template <OperationSize size, typename T>
	bool IsZero(T i)
	{
		if(size == kByte)
			return (i & 0xFF) == 0;
		if(size == kWord)
			return (i & 0xFFFF) == 0;
		if(size == kLong)
			return (i & 0xFFFFFFFF) == 0;
		assert(false);
		return 0;
	}

	template <OperationSize size, OperationType type, typename T>
	void SetFlagStuff(T source, T destination, T result)
	{
		regs.sr.ccr.n = IsNegative<size>(result);

		if(type == kAnd || type == kOr)
			regs.sr.ccr.v = 0;
		else
			regs.sr.ccr.v = IsOverflow<size,type>(source,destination,result);

		if(type == kAnd || type == kOr)
			regs.sr.ccr.c = 0;
		else
			regs.sr.ccr.c = IsCarry<size>(result);

		//what?
		//		if(type == kAdd && size == kWord)
		//			regs.sr.ccr.c = (result & 0x10000) != 0;

		//cmp doesn't touch the x flag
		if(type != kCmp)
			regs.sr.ccr.x = regs.sr.ccr.c;
		regs.sr.ccr.z = IsZero<size>(result);
	}

	//there's several similar ops, so let's combine them
	//this crosses into 2 categories hmm.
	//aka logical ops and data movement
	enum MiscOps
	{
		kClr,
		kNot,
		kNeg
	};

#include "m68K_interrupts.h"
#include "m68k_addressing.h"

	//integer arithmetic

	template <OperationSize size>
	void SetFlagsGeneric(const u64&result)
	{
		regs.sr.ccr.n = IsNegative<size>(result);
		regs.sr.ccr.z = IsZero<size>(result);
		//	regs.sr.ccr.v = IsOverflow<size,kCmp>(r.data,destination,static_cast<u32>(result));
		regs.sr.ccr.c = IsCarry<size>(result);
	}

#if 0
	template <OperationSize size>
	void Add(unsigned long i)
	{
		FetchResult r = EffectiveAddressFetch<size>((i>>3)&7,i&7,true);

		signed int data_reg = SizeMask<size>(regs.d[(i >> 9) & 7]);
		u64 result = ((s64)r.data) + data_reg;//result needs to have carry flag
		unsigned int value_to_store = MaskedCombine<size>(regs.d[(i >> 9) & 7], result);

		if(((i >> 8) & 1) == 0)
		{
			//< ea > + Dn -> Dn
			regs.d[(i >> 9) & 7] = value_to_store;
		}
		else
		{
			//Dn + < ea > -> < ea > 
			EffectiveAddressSet<size>((i>>3)&7,i&7,r.address,value_to_store,false);
		}
		SetFlagStuff<size,kAdd>((u64)r.data,(u64)data_reg,result);//have to use result to preserve the carry flag
	}
#endif
	template <OperationSize size, OperationType type>
	void Genericq(const u32& i)
	{
		//get the data from the destination
		FetchResult r = EffectiveAddressFetch<size,true>((i>>3)&7,i&7);

		u32 immediate = (((i >> 9) - 1) & 7) + 1;

		u32 result;
		if(type == kAdd)
			result = r.data + immediate;
		else
			result = r.data - immediate;

		//store to destination
		EffectiveAddressSet<size,false>((i>>3)&7,i&7,r.address,result);
		//reg to reg doesn't set flags
		if(((i>>3)&7)!= 1)
			SetFlagStuff<size,type>(immediate,r.data,result);

	}

#if 0
	template <OperationSize size>
	void Adda(unsigned long i)
	{
		FetchResult r = EffectiveAddressFetch<size>((i>>3)&7,i&7,true);
		//a is always the destination for adda
		unsigned int destination = regs.a[(i>>9)&7];
		regs.a[(i>>9)&7] = MaskedCombine<size>(destination, r.data + destination);
	}
#endif
	template <OperationSize size, OperationType operation_type>
	void Generica(const u32&i)
	{
		FetchResult r = EffectiveAddressFetch<size,true>((i>>3)&7,i&7);
		u32 reg = (i>>9)&7;
		u32 destination = regs.a[reg];

		u64 result;

		if(operation_type == kAdd)
			result = r.data + destination;
		else if(operation_type == kSub || operation_type == kCmp)
			result = destination - static_cast<u64>(r.data);//cast to preserve carry flag. is there a better way?
		else
			assert(false);

		if(operation_type != kCmp)
			regs.a[(i>>9)&7] = MaskedCombine<size>(destination, result);
		else
			SetFlagsGeneric<size>(result);
	}

	void Mulu(unsigned long i)
	{
		FetchResult f = EffectiveAddressFetch<kWord,true>((i>>3)&7,i&7);

		unsigned int destination = regs.d[(i >> 9) & 7];
		unsigned int result = SizeMask<kWord>(f.data) * SizeMask<kWord>(destination);

		regs.d[(i >> 9) & 7] = result;

		SetSomeOfDemFlags<kWord>(result);
		//its actually a LONG negative check
		regs.sr.ccr.n = IsNegative<kLong>(result);
	}
	template <OperationSize size>
	void Ext(unsigned long i)
	{
		int reg = i & 7;
		regs.d[reg] = SignExtend<size>(regs.d[reg]);
		SetSomeOfDemFlags<size>(regs.d[reg]);
		regs.sr.ccr.z = SizeMask<size>(regs.d[reg]) == 0;
	}
#if 0
	template <OperationSize size>
	void Sub(unsigned long i)
	{
		bool direction = (i >> 8) & 1;

		FetchResult r = EffectiveAddressFetch<size>((i>>3)&7,i&7,true);
		unsigned int ea = r.data;
		signed long long Dn = SizeMask<size>(regs.d[(i >> 9) & 7]);
		u64 result;

		if(direction)//< ea > – Dn -> < ea > 
		{
			result = ea - Dn;
			SetFlagStuff<size,kSub>((u64)Dn,(u64)ea,result);
			EffectiveAddressSet<size>((i>>3)&7,i&7,r.address,result,false);
		}
		else//Dn – < ea > -> Dn 
		{
			result = Dn - ea;
			SetFlagStuff<size,kSub>((u64)ea,(u64)Dn,result);
			regs.d[(i >> 9) & 7] = MaskedCombine<size>(regs.d[(i >> 9) & 7],result);
		}

	}
#endif
	void Divu(unsigned long i )
	{
		FetchResult f = EffectiveAddressFetch<kWord,true>((i>>3)&7,i&7);

		unsigned int ea = f.data;
		unsigned int reg = (i >> 9) & 7;

		if(ea != 0)
		{
			unsigned int quotient = regs.d[reg] / ea;
			unsigned int remainder = regs.d[reg] % ea;

			if(quotient < 0x10000)
			{
				//x not affected
				regs.sr.ccr.n = IsNegative<kWord>(quotient);
				regs.sr.ccr.z = IsZero<kWord>(quotient);
				regs.sr.ccr.v = false;
				regs.sr.ccr.c = false;
				quotient = SizeMask<kWord>(quotient);
				unsigned int result = quotient | (remainder << 16);
				//	result = SizeMask<kWord>(result);
				//	EffectiveAddress<kByte,kSetOperand>((i>>3)&7,i&7,result,r);
				regs.d[reg] = result;
			}
			else
				//overflow
				regs.sr.ccr.v = true;
		}
		else
		{
			//divide by zero causes trap
			assert(false);
		}

	}

#if 0
	template <OperationSize size, bool is_cmpa>
	void Cmp(unsigned long i)
	{
		FetchResult r = EffectiveAddressFetch<size>((i >> 3) & 7,i & 7, true);
		u32 destination ;

		if(!is_cmpa)
			destination	= regs.d[(i >> 9) & 7];
		else
			destination = regs.a[(i >> 9) & 7];

		destination = SizeMask<size>(destination);
		u64 result = destination - static_cast<u64>(r.data);//64 bit result to preserve carry flag

		regs.sr.ccr.n = IsNegative<size>(result);
		regs.sr.ccr.z = IsZero<size>(result);
		regs.sr.ccr.v = IsOverflow<size,kCmp>(r.data,destination,static_cast<u32>(result));
		regs.sr.ccr.c = IsCarry<size>(result);
	}
#endif
	//end integer arithmetic

	//data movement

	template <OperationSize size>//, bool reverse>
	void Movem(unsigned long i, bool reverse)
	{
		unsigned int register_list = FetchImmediate<kWord>(regs.pc);
		FetchResult r = EffectiveAddressFetch<size,false>((i>>3)&7,i&7);//prececrement false
		for(int j = 0; j < 16; j++)
			if(register_list & (1 << j))
			{
				if(reverse)
				{
					if(j > 7)
						regs.a[j-8] = Read<size>(r.address);
					else
						regs.d[j] = Read<size>(r.address);
					r.address += 4;
				}
				else
				{
					r.address -= 4;
					if((15-j) > 7)
						Write<size>(r.address,regs.a[(15-j)-8]);
					else
						Write<size>(r.address,regs.d[(15-j)]);
				}
			}

			//fixme later
			regs.a[7] = r.address;
			//EffectiveAddress<size,unsigned int,kSetOperand>((i>>3)&7,i&7,address);
	}
	void Lea(unsigned long i)
	{
		FetchResult f = EffectiveAddressFetch<kLong,true>((i >> 3) & 7,i & 7);
		regs.a[(i>>9)&7] = f.address;
	}
	template<OperationSize size, typename T>
	void Move(unsigned int i)
	{
		int source_addressing_mode = (i >> 3) & 7;
		int destination_addressing_mode = (i >> 6) & 7;

		FetchResult f = EffectiveAddressFetch<size,true>(source_addressing_mode,i & 7);
		EffectiveAddressSet<size,false>(destination_addressing_mode,(i >> 9) & 7,f.address,f.data);

		//address reg to address reg doesn't set flags apparently
		if(((i >> 6) & 7) != 1)
		{
			regs.sr.ccr.n = IsNegative<size>(f.data);
			regs.sr.ccr.z = f.data == 0;
			regs.sr.ccr.v = 0;
			regs.sr.ccr.c = 0;
		}
	}
	void MoveQ32(unsigned int i)
	{
		unsigned int result = static_cast<signed char>(i);
		regs.d[(i >> 9) & 7] = result;
		SetFlagsNZNoVNoC(result);
	}

	template <OperationSize size, MiscOps type>
	void guh(const u32 &i)
	{
		int addressing_mode = (i >> 3) & 7;
		int reg = i & 7;

		FetchResult f = EffectiveAddressFetch<size,true>(addressing_mode,reg);

		u32 result;

		if(type == kClr)
			result = MaskedCombine<size>(f.data,0);
		else if(type == kNot)
			result = MaskedCombine<size>(f.data,~f.data);

		EffectiveAddressSet<size,false>(addressing_mode,reg,f.address,result);

		SetFlagsGeneric<size>(result);
	}
	//end data movement


	//logical

	/*
	immediate ops
	moveq
	addi
	addq
	andi
	eori
	ori
	andi to sr
	eori to sr
	ori to sr
	stop
	andi to ccr
	eori to ccr
	ori to ccr
	*/


	template <OperationSize size, OperationType operation_type>
	void ImmediateOperation(unsigned long i)
	{
		u64 immediate_data = FetchImmediate<size>(regs.pc);

		FetchResult f = EffectiveAddressFetch<size,true>((i>>3)&7,i&7);

		u64 result;
		if(operation_type == kAdd)
			result = f.data + immediate_data;
		else if(operation_type == kAnd)
			result = f.data  & immediate_data;
		else if(operation_type == kEor)
			result = f.data  ^= immediate_data;
		else if(operation_type == kSub || operation_type == kCmp)
			result = f.data  - immediate_data;
		else if(operation_type == kOr)
			result = f.data  | immediate_data;

		if(operation_type != kCmp)//cmp doesn't store
		{
			f.data = result;
			EffectiveAddressSet<size,false>((i>>3)&7,i&7,f.address,result);
		}

		SetFlagStuff<size,operation_type>(immediate_data,(u64)f.data,result);
	}

	template <OperationSize size, OperationType operation_type>
	void GenericMath(const u32&i)//,const bool&direction
	{
		u32 mode = (i >> 3) & 7;

		FetchResult f = EffectiveAddressFetch<size,true>(mode,i & 7);

		u32 reg = (i >> 9) & 7;
		u64 result;

		u32 data_reg = SizeMask<size>(regs.d[reg]);

		if(operation_type == kOr)
			result = data_reg | f.data;
		if(operation_type == kEor)
			result = SizeMask<size>(data_reg ^ f.data);
		if(operation_type == kAdd)
			result = ((s64)f.data) + data_reg;//result needs to have carry flag
		if(operation_type == kSub)
			result = ((s64)f.data) - data_reg;
		if(operation_type == kCmp)
			result = data_reg - ((s64)f.data);

		u32 value_to_store = MaskedCombine<size>(regs.d[reg], result);

		bool direction = (i >> 8) & 1;

		if(operation_type != kCmp)
		{
			if(direction)
				EffectiveAddressSet<size,false>(mode,i&7,f.address,value_to_store);
			else
				regs.d[reg] = value_to_store;
		}

		if(operation_type == kAdd || operation_type == kSub || operation_type == kCmp)
			SetFlagStuff<size,operation_type>((u64)f.data,(u64)data_reg,result);//have to use result to preserve the carry flag
		else
			SetSomeOfDemFlags<size>(SizeMask<size>(result));
	}

	//end logical


	//program control

	void Scc(unsigned long i)
	{
		unsigned int result = 0;

		FetchResult f = EffectiveAddressFetch<kByte,true>((i>>3)&7,i&7);

		if(regs.sr.ccr.ConditionIsSatisfied((i >> 8 ) & 0xF))
		{
			result = 0x11;
		}
		EffectiveAddressSet<kByte,false>((i>>3)&7,i&7,f.address,result);
	}
	//unsized? what does that mean?
	void Jmp(unsigned long i)
	{
		FetchResult f = EffectiveAddressFetch<kWord,true>((i >> 3) & 7,i & 7);
		Jump(f.address);
	}
	template <OperationSize size>
	void Tst(unsigned long i)
	{
		FetchResult f = EffectiveAddressFetch<size,true>((i >> 3) & 7,i & 7);
		SetSomeOfDemFlags<size>(f.data);
	}


	//todo this is very similar to another function above here
	void Bcc(unsigned long i)
	{
		if(regs.sr.ccr.ConditionIsSatisfied((i >> 8) & 0xF))
		{
			if((i & 0xFF) == 0)
			{
				unsigned int offset = FetchImmediate<kWord>(regs.pc);
				regs.pc-=2;
				regs.pc+=static_cast<signed short>(offset);//must be signed math
				return;
			}
			else if((i & 0xFF) == 0xFF)
			{
				unsigned int offset = FetchImmediate<kLong>(regs.pc);
				regs.pc-=4;
				regs.pc+=offset;
				return;
			}
			//8 bit displacement
			else
			{
				regs.pc += static_cast<signed char>(i);//it gotta be signed math
				return;
			}

		}
		//so i guess an 8 bit doesn't add to the pc or something
		if((i & 0xFF) == 0)
			regs.pc+=2;

	}
	void Rts()
	{
		Jump(PopStack<kLong>());
	}
	void Dbcc(unsigned long i)
	{
		//	if(ConditionIsSatisfied(static_cast<ConditionalPredicates>((i >> 8 & 0xF))))
		{
			unsigned int result = (regs.d[i & 7] - 1) & 0xFFFF;
			regs.d[i & 7] = (regs.d[i & 7] & ~0xFFFF) | result;

			if(result != 0xFFFF)
			{
				unsigned int displacement = FetchImmediate<kWord>(regs.pc);
				regs.pc-=2;
				regs.pc+= static_cast<signed short>(displacement);
				return;
			}
			regs.pc+=2;
		}
	}
	void Jsr(unsigned long i)
	{
		FetchResult f = EffectiveAddressFetch<kLong,true>((i>> 3)&7,i&7);
		//i guess we don't push when we popped?
		if(((i>> 3)&7) != 6)
			PushStack<kLong>(regs.pc);
		regs.pc = f.address;
	}

	template<OperationSize size>
	void Bsr(unsigned long i)
	{

		if(size == kByte)
		{
			regs.pc-=1;
			regs.pc += i & 0xFF;
		}
		else
		{
			int offset = FetchImmediate<size>(regs.pc);
			PushStack<kLong>(regs.pc);
			if(size == kWord)
				regs.pc-=2;
			else
				regs.pc-=4;
			regs.pc += static_cast<signed short>(offset);//has to be signed math
		}
	}

	void Bra(unsigned int i)
	{
		if((i & 0xFF) == 0)
		{
			//16 bit displacement
			unsigned int displacement = FetchImmediate<kWord>(regs.pc);

			regs.pc-=2;
			regs.pc+=displacement;
			//we have to mask it off, not sure if it actually has to sign extend or just clipping is fine?
			//static_cast<signed short>(regs.pc);
			regs.pc = SizeMask<kWord>(regs.pc);
		}
		else if((i & 0xFF) == 0xFF)
		{
			//32 bit displacement
			unsigned int displacement = FetchImmediate<kLong>(regs.pc);

			regs.pc-=4;
			regs.pc+=displacement;
		}
		else//8 bit displacement
			regs.pc += static_cast<signed char>(i);//looks like this needs to be signed math// & 0xFF);
	}

	//end program control


	//shift rotate

	void Swap(unsigned long i)
	{
		unsigned int upper = (regs.d[i & 7] & 0xFFFF) << 16;
		unsigned int lower = (regs.d[i & 7] >> 16) & 0xFFFF;
		unsigned int result = regs.d[i & 7] = upper | lower;
		//x not affected
		regs.sr.ccr.n = IsNegative<kLong>(result);
		regs.sr.ccr.z = IsZero<kLong>(result);
		regs.sr.ccr.v = 0;
		regs.sr.ccr.c = 0;
	}

	template <OperationSize size, bool is_lsr>
	void AslAsrLslLsr(unsigned int i)
	{
		unsigned int instruction = i;
		if(((i >> 6) & 3) != 3)
		{
			//register shifts
			unsigned int source = SizeMask<size>(regs.d[i & 7]);
			int shift;// = (((i >> 9) - 1) & 7) + 1;

			if((i >> 5)&1)//ir
			{
				shift = regs.d[(i >> 9) & 7] % 64;
			}
			else
				shift = (((i >> 9) - 1) & 7) + 1;

			//assert(false);
#if old_one
			u16 result = source >> shift;

			regs.d[i & 7] = (source&0xFFFF0000) | result;
			regs.sr.ccr.x = regs.sr.ccr.c = (source <<( 9-shift)) != 0;
#else
			unsigned int mask;
			if(size == kByte)
				mask = 0x80;
			if(size == kWord)
				mask = 0x8000;
			if(size == kLong)
				mask = 0x80000000;

			ShiftRotateDirections direction;
			if((i >> 8) & 1)//shift left
				direction = kLeft;
			else//shift right
				direction = kRight;

			bool msb_was_set = (source & mask);//used in asl so we can replace the upper bits with the msb

			for (int j = 0; j < shift; j++)
			{
				if((instruction >> 8) & 1)
					regs.sr.ccr.x = regs.sr.ccr.c = (source & mask) > 0;
				else
					regs.sr.ccr.x = regs.sr.ccr.c = (source & 1) > 0;
				if(direction == kRight)
					source >>= 1;
				else
					source <<= 1;

				if(!is_lsr && msb_was_set)
				{
					//shift in ones at the top
					source |= mask;
				}

				//value = (byte)(((this.X) ? 0x80 : 0) | (value >> 1));
				//				if(instruction == 0xe442)
				//				{
				//					//shift in ones in this case?
				//					source |= mask;
				//				}
			}

			source = SizeMask<size>(source);//not necessary?

			if(i == 0xe442)
			{
				//need to shift some stuff in the top
				//asr shifts in ones on the top i guess
			}

			unsigned masked = MaskedCombine<size>(regs.d[i & 7],source);
			regs.d[i & 7] = masked;
#endif
			regs.sr.ccr.n = IsNegative<size>(source);
			regs.sr.ccr.z = source == 0;
			//TODO what is the purpose of that shift?

			//regs.sr.ccr.v = regs.sr.ccr.c = source << (9-shift);
		}
		else
		{
			//memory shifts
		}


	}
	//end shift rotate

	//bit manipulation

	enum BitOps
	{
		kBtst,
		kBchg,
		kBclr,
		kBset
	};

	//template <BitOps bitop, bool is_static>
	void BitOp(const BitOps &bitop, const bool &is_static, const u32 & i)
	{

		u32 test = regs.d[0];
		u8 bit_number; 

		u32 mask; 
		if(is_static)
		{
			bit_number = FetchImmediate<kByte>(regs.pc) & 7;
			mask = (1 << bit_number);
		}
		else
		{
			bit_number = regs.d[(i >> 9) & 7];
			mask = 1 << (bit_number & 31);
		}

		u32 mode = (i >> 3) & 7;
		u32 reg = i & 7;

		FetchResult f;

		if(is_static)
			f = EffectiveAddressFetch<kByte,true>(mode,reg);
		else
			f = EffectiveAddressFetch<kLong,true>(mode,reg);

		u32 result = 0;

		regs.sr.ccr.z = (f.data & mask) == 0;

		//btst falls through
		if(bitop == kBset)
			result = f.data | mask;
		else if(bitop == kBclr)
			result = f.data & ~mask;
		else if(bitop == kBchg)
			result = f.data ^ mask;

		if(bitop != kBtst)
			EffectiveAddressSet<kByte,false>(mode,reg,f.address,result);
	}

	//end bit manipulation


	//system control

	void MoveFromSr(unsigned long i)
	{
		if(regs.sr.supervisor)
		{
			FetchResult f = EffectiveAddressFetch<kWord,true>((i>>3)&7,i&7);
			f.data = regs.sr;
			EffectiveAddressSet<kWord,false>((i>>3)&7,i&7,f.address,f.data);
			return;
		}
		assert(false);
	}
	void MoveToSr(unsigned long i)
	{
		if(regs.sr.supervisor)
		{
			FetchResult f = EffectiveAddressFetch<kWord,true>((i >> 3) & 7,i & 7);
			//i'm not sure when it's supposed to check for interrupts or not
			SetStatusRegister(f.data);
		}
	}

	template <OperationType operation_type>
	void OperationToCcr(const u32&i)
	{
		u8 fetch = FetchImmediate<kByte>(regs.pc);

		if(operation_type == kAnd)
			regs.sr.ccr = regs.sr.ccr & fetch;
		if(operation_type == kEor)
			regs.sr.ccr = regs.sr.ccr ^ fetch;
		if(operation_type == kOr)
			regs.sr.ccr = regs.sr.ccr | fetch;
	}

	template <OperationType operation_type>
	void OperationToSr(const u32&i)
	{
		u16 fetch = FetchImmediate<kWord>(regs.pc);

		if(operation_type == kAnd)
			regs.sr = regs.sr & fetch;
		if(operation_type == kEor)
			regs.sr = regs.sr ^ fetch;
		if(operation_type == kOr)
			regs.sr = regs.sr | fetch;
	}

	//end system control

	enum ShiftRotateDirections
	{
		kLeft,
		kRight
	};

	template <OperationSize size, typename T>
	T SizeMask(T input)
	{
		if(size == kByte)
			return input & 0xFF;
		if(size == kWord)
			return input & 0xFFFF;
		if(size == kLong)
			return input & 0xFFFFFFFF;

		assert(false);
		return 0;
	}


	template <OperationSize size>
	void SetSomeOfDemFlags(unsigned long result)
	{
		regs.sr.ccr.n = IsNegative<size>(result);
		regs.sr.ccr.z = result == 0;
		regs.sr.ccr.c = 0;
		regs.sr.ccr.v = 0;
	}

	///what is this?
	void Rte()
	{
		unsigned int new_sr = PopStack<kWord>();
		unsigned int new_pc = PopStack<kLong>();
		Jump(new_pc);
		SetStatusRegister(new_sr);
	}

	template <OperationSize size,typename T>
	bool MsbIsSet(T input)
	{
		if(size == kByte)
			return input & 0x80;
		if(size == kWord)
			return input & 0x8000;
		if(size == kLong)
			return input & 0x80000000;
		assert(false);
		return 0;
	}
	template <OperationSize size, typename T>
	T SignExtend(T input)
	{

		T upper_part = 0;

		if(MsbIsSet<kByte>(input))
			upper_part = 0xFF00;

		return (input & ~0xffff) | (input & 0xFF) | upper_part;
	}

	int Execute()
	{
		int old_cycles = regs.cycles_executed;
		int i = FetchImmediate<kWord>(regs.pc);
		DecodeAndExecuteInstruction(i);
		return regs.cycles_executed - old_cycles;
	}

#include "m68k_decoding.h"
};