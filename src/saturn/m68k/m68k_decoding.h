template <OperationSize size>
void DoImmediateOperation(const u32&i)
{
	switch(i >> 8)
	{
	case 0:
		//ori
		ImmediateOperation<size,kOr>(i);
		break;
	case 2:
		//andi
		ImmediateOperation<size,kAnd>(i);
		break;
	case 4:
		//subi
		ImmediateOperation<size,kSub>(i);
		break;
	case 6:
		//addi
		ImmediateOperation<size,kAdd>(i);
		break;
	case 0xa:
		//eori
		ImmediateOperation<size,kEor>(i);
		break;
	case 0xc:
		//cmpi
		ImmediateOperation<size,kCmp>(i);
		break;
	}
}

void BitManipulationMovepImmediate(const u32 & i)
{
	if((i >> 8) == 8)
	{
		//btst, bchg, bclr, bset static and dynamic
		BitOps which_op = static_cast<BitOps>(((i >> 6) & 3));
		bool is_static = (i >> 11) & 1;
		return BitOp(which_op,is_static,i);
	}

	//ori, andi, eori, to ssr, ccr
	if(i == 0x3C)
		return OperationToCcr<kOr>(i);
	if(i == 0x23C)
		return OperationToCcr<kAnd>(i);
	if(i == 0xA3C)
		return OperationToCcr<kEor>(i);

	if(i == 0x7C)
		return OperationToSr<kOr>(i);
	if(i == 0x27C)
		return OperationToSr<kAnd>(i);
	if(i == 0xA7C)
		return OperationToSr<kEor>(i);

	//ori, andi, subi, addi, eori, cmpi
	switch((i >> 6) & 3)
	{
	case 0: return DoImmediateOperation<kByte>(i);
	case 1: return DoImmediateOperation<kWord>(i);
	case 2: return DoImmediateOperation<kLong>(i);
	}

	//need rtm, callm, cmp2, chk2, 
	//moves, cas2, cas, movep

	assert(false);
}



void Miscellaneous(unsigned long i)
{
	if((i >> 6) == 0x103)
		return MoveFromSr(i);

//	if((i >> 6) == 0x10B)
//		return MoveFromCcr(i);

	if((i >> 8) == 0x40)
	{
		assert(false);
//		switch((i >> 6) & 3)
//		{
//		case 0: return Negx(i);
//		case 1:
//		case 2:
//		}
	}

	if((i >> 8) == 0x42)
	{
		switch((i >> 6) & 3)
		{
		case 0: return guh<kByte,kClr>(i);
		case 1: return guh<kWord,kClr>(i);
		case 2: return guh<kLong,kClr>(i);
		}
	}

//	if((i >> 6) == 0x113)
//		return MoveToCcr(i);
	
	if((i >> 8) == 0x44)
	{
		switch((i >> 6) & 3)
		{
		case 0: return guh<kByte,kNeg>(i);
		case 1: return guh<kWord,kNeg>(i);
		case 2: return guh<kLong,kNeg>(i);
		}
	}

	if((i >> 8) == 0x46)
	{
		switch((i >> 6) & 3)
		{
		case 0: return guh<kByte,kNot>(i);
		case 1: return guh<kWord,kNot>(i);
		case 2: return guh<kLong,kNot>(i);
		}
	}

	if((i >> 6) == 0x11B)
		return MoveToSr(i);

	if(((i >> 3) & 7) == 0)
	{
		if((i >> 9) == 0x24)
		{
			switch((i >> 6) & 7)
			{
			case 2: return Ext<kByte>(i);
			case 3: return Ext<kWord>(i);
			case 7: return Ext<kLong>(i);
			}
			//ext,extb
		}
	}

//	if((i >> 3) == 0x901)
//		return Link(i);(long)

//	if((i >> 6) == 0x120)
//		return Nbcd(i);

	if((i >> 3) == 0x908)
		return Swap(i);

//	if((i >> 3) == 0x909)
//		return Bkpt(i);

//	if((i >> 6) == 0x121)
//		return Pea(i);

//	if(i == 0x4AFA)
//		return Bgnd();

//	if(i == 0x4AFC)
//		return Illegal();

	//tas

	if((i>> 8) == 0x4A)
		switch((i >> 6) & 3)
		{
		case 0: return Tst<kByte>(i);
		case 1: return Tst<kWord>(i);
		case 2: return Tst<kLong>(i);
		}

	//mulu

	//muls

	//divu,divul

	//divs,divsl

	//trap

	//link (word)

	//unlk

	//move usp

	//reset

	if(i == 0x4e71)//nop
		return AddCycles(1);

//	if(i == 0x4E72)
//		return Stop();

	if(i == 0x4E73)
		return Rte();

//	if(i == 0x4E74)
//		return Rtd();

	if(i == 0x4E75)
		return Rts();

//	if(i == 0x4E76)
//		return Trapv();

//	if(i == 0x4E77)
//		return Rtr();

//	if((i >> 1) == 0x273D)
//		return Movec(i);

	if((i >> 6) == 0x13A)
		return Jsr(i);

	if((i >> 6) == 0x13B)
		return Jmp(i);

	if((i >> 11) == 9)
	{
		if(((i >> 7) & 7) == 1)
		{
			bool direction = (i >> 10) & 1;
			if((i >> 6) & 1)//size
				return Movem<kLong>(i,direction);
			else
				return Movem<kByte>(i,direction);
		}
	}

	if(((i >> 6) & 7) == 7)
		return Lea(i);

//	if(((i >> 6) & 1) == 0)
//		return Chk(i);

	assert(false);
}

void DbccTrapccScc(unsigned long i)
{
	if(((i >> 3) & 0x1f) == 0x19)
	{
		//dbcc
		Dbcc(i);return;
	}
	else if (((i >> 3) & 0x1f) == 0x1f)
	{
		//trapcc
	}
	//		if ((i & 0x38) == 0x08)
	//		{
	//			
	//		}
	else
	{
		//scc
		Scc(i);return;
	}
}
void AddqSubqSccDbccTrap(unsigned long i)
{
	switch((i >> 8) & 0xf)
	{
	case 0:
	case 2:
	case 4:
	case 6:
	case 8:
	case 0xa:
	case 0xC:
	case 0xe:
		switch((i >> 6) & 3)
		{
		case 0:
			//ADDQ.B
			Genericq<kByte,kAdd>(i); return;
			break;
		case 1:
			//ADDQ.W
			Genericq<kWord,kAdd>(i); return;
			break;
		case 2:
			//ADDQ.L
			Genericq<kLong,kAdd>(i); return;
			break;
		case 3:
			DbccTrapccScc(i); return;
			break;
		}
	case 1:
	case 3:
	case 5:
	case 7:
	case 9:
	case 0xb:
	case 0xd:
	case 0xf:
		switch((i >> 6) & 3)
		{
		case 0:
			//SUBQ.B
			Genericq<kByte,kSub>(i); return;
			break;
		case 1:
			//SUBQ.W
			Genericq<kWord,kSub>(i); return;
			break;
		case 2:
			//SUBQ.L
			Genericq<kLong,kSub>(i); return;
			break;
		case 3:
			DbccTrapccScc(i); return;
			break;
		}
	}
	assert(false);
}
void BccBsrBra(unsigned long i)
{
	switch((i >> 8) & 0xf)
	{
	case 0:
		Bra(i);return;
		break;
	case 1:
		switch((i >> 6) & 3)
		{
		case 0:
			Bsr<kWord>(i);return;
			break;
		case 1:
			Bsr<kByte>(i);return;
			break;
		case 2:
			Bsr<kLong>(i);return;
			break;
		case 3:
			//???
			break;
		}
		break;
	default:
		Bcc(i); return;
	}
	assert(false);
}
void OrDivSbcd(unsigned long i)
{
	if(((i >> 6) & 7) == 3)
		return Divu(i);//divul?

//	if(((i >>4) & 0x1F) == 0x10)
//		return Sbcd(i);

//	if(((i >>4) & 0x1F) == 0x14)
//		return Pack(i);

//	if(((i >>4) & 0x1F) == 0x18)
//		return Unpk(i);

//	if(((i >> 6) & 7) == 7)
//		return Divs(i);//divsl?
	
	switch((i >> 6) & 3)
	{
	case 0:return GenericMath<kByte,kOr>(i);
	case 1:return GenericMath<kWord,kOr>(i);
	case 2:return GenericMath<kLong,kOr>(i);
	}
}

void SubSubx(unsigned long i)
{
	switch((i >> 6) & 3)
	{
	case 0:
		//SUB.B
		GenericMath<kByte,kSub>(i);return;
		break;
	case 1:
		//SUB.W
		GenericMath<kWord,kSub>(i);return;
		break;
	case 2:
		//SUB.L
		GenericMath<kLong,kSub>(i);return;
		break;
	case 3:
		//SUBA.W
		break;
	}
}
void CmpEor(unsigned long i)
{
	//cmpm, cmp, cmpa, eor
	/*
	so, eor opmodes are 100 b, 101 w, 110 l
	but cmpm is always 001 in bits 5,4,3
	and eor has that addressing mode disabled
	*/	
	if(((i >> 3) & 7) == 1 && (((i >> 3) & 7) == 1) && (((i >> 6) & 3) != 3))//size of 3 is not possible with cmpm
	{
		assert(false);
//		switch((i >> 6) & 3)
//		{
//		case 0: return Cmpm<kByte>(i);
//		case 1: return Cmpm<kWord>(i);
//		case 2: return Cmpm<kLong>(i);
//		}
	}
	else
	{
		switch((i >> 6) & 7)
		{
		case 0: return GenericMath<kByte,kCmp>(i);
		case 1: return GenericMath<kWord,kCmp>(i);
		case 2: return GenericMath<kLong,kCmp>(i);
		case 3: return Generica<kWord,kCmp>(i);
		case 4: return GenericMath<kByte,kEor>(i);
		case 5: return GenericMath<kWord,kEor>(i);
		case 6: return GenericMath<kLong,kEor>(i);
		case 7: return Generica<kLong,kCmp>(i);
		}
	}
}

void AndMulAbcdExg(unsigned long i)
{
	switch((i >> 8) & 0xf)
	{
	case 0:
	case 2:
	case 4:
	case 6:
	case 8:
	case 0xA:
	case 0xC:
	case 0xE:
		switch((i >> 6) & 3)
		{
		case 0:
			//AND.B
			assert(false);
			break;
		case 1:
			//AND.W
			assert(false);
			break;
		case 2:
			//AND.L
			assert(false);
			break;
		case 3:
			//MULU.W
			Mulu(i);return;
			//				assert(false);
			break;
		}
		break;
	case 1:
	case 3:
	case 5:
	case 7:
	case 9:
	case 0xB:
	case 0xD:
	case 0xF:
		switch((i >> 6) & 3)
		{
		case 0:
			//ABCD.B / AND.B
			assert(false);
			break;
		case 1:
			//EXG / AND.W
			assert(false);
			break;
		case 2:
			//EXG / AND.L
			assert(false);
			break;
		case 3:
			//MULS.W
			assert(false);
			break;
		}
		break;
	}
}
void AddAddx(unsigned long i)
{
	switch((i >> 8) & 0xf)
	{
	case 0:
	case 2:
	case 4:
	case 6:
	case 8:
	case 0xA:
	case 0xC:
	case 0xE:
		switch((i >> 6) & 7)
		{
		case 0:
			GenericMath<kByte,kAdd>(i);return;
			//add.b
			break;
		case 1:
			GenericMath<kWord,kAdd>(i);return;
			//add.w
			break;
		case 2:
			GenericMath<kLong,kAdd>(i);return;
			//add.l
			break;
		case 3:
			Generica<kWord,kAdd>(i);return;
			//adda.w
			break;
		}
		break;
	case 1:
	case 3:
	case 5:
	case 7:
	case 9:
	case 0xB:
	case 0xD:
	case 0xF:
		switch((i >> 6) & 3)
		{
		case 0:
			//ADDX.B / ADD.B
			GenericMath<kByte,kAdd>(i);return;
			break;
		case 1:
			//ADDX.W / ADD.W
			GenericMath<kWord,kAdd>(i);return;
			break;
		case 2:
			//ADDX.L / ADD.L
			GenericMath<kLong,kAdd>(i);return;
			break;
		case 3:
			//ADDA.L
			Generica<kLong,kAdd>(i);return;
			break;
		}
		break;
	}
	assert(false);
}

template <OperationSize size>
void FactorOut(unsigned long i)
{
	//register shift
	switch((i >> 3) & 3)
	{
	case 0:
		AslAsrLslLsr<size,false>(i);break;
		break;
	case 1:
		AslAsrLslLsr<size,true>(i);break;
		break;
	case 2:
		assert(false);
		//RoxlRoxr<size>(i);break;
		break;
	case 3:
		assert(false);
		//RolRor<size>(i);break;
		break;
	}
}
void ShiftRotateBitField(unsigned long i)
{

	//if the size field == 11 (bits)
	//then it's a memory shift
	//otherwise it specifies the size for
	//a register shift 00 byte, 01, word, 10 long

	int size = (i >> 6) & 3;

	switch(size)
	{
	case 0:
		//Byte
		FactorOut<kByte>(i);return;
		break;
	case 1:
		//Word
		FactorOut<kWord>(i);return;
		break;
	case 2:
		//Long
		FactorOut<kLong>(i);return;
		break;
	case 3:
		//Memory
		switch((i >> 9) & 3)
		{
		case 0:
			assert(false);
			//AslAsr(i);break;
			break;
		case 1:
			assert(false);
			//LslLsr(i);break;
			break;
		case 2:
			assert(false);
			//RoxlRoxr(i);break;
			break;
		case 3:
			assert(false);
			//RolRor(i);break;
			break;
		}
		break;
	}
	assert(false);
}

void DecodeAndExecuteInstruction(unsigned long i)
{

	if(!reset_called)
		assert("Call Reset first");

	switch((i >> 12) & 0xF)
	{
	case 0:
		//Ori Andi Subi Rtm Callm Cmp2 Chk2 etc
		BitManipulationMovepImmediate(i);
		break;
	case 1:
		Move<kByte,signed char>(i);
		break;
	case 2:
		Move<kLong, signed long>(i);
		break;
	case 3:
		Move<kWord, signed short>(i);
		break;
	case 4:
		//Miscellaneous
		Miscellaneous(i);
		break;
	case 5:
		//ADDQ/SUBQ/Scc/DBcc/TRAPc
		AddqSubqSccDbccTrap(i);
		break;
	case 6:
		BccBsrBra(i);
		//Bcc/BSR/BRA
		break;
	case 7:
		MoveQ32(i);
		//assert(false);
		//MOVEQ
		break;
	case 8:
		OrDivSbcd(i);
		//OR/DIV/SBCD
		break;
	case 9:
		SubSubx(i);
		//			assert(false);
		//SUB/SUBX
		break;
	case 0xA:
		assert(false);
		//(Unassigned, Reserved)
		break;
	case 0xB:
		CmpEor(i);
		//CMP/EOR
		break;
	case 0xC:
		AndMulAbcdExg(i);
		//AND/MUL/ABCD/EXG
		break;
	case 0xD:
		AddAddx(i);
		//ADD/ADDX
		break;
	case 0xE:
		ShiftRotateBitField(i);
		//Shift/Rotate/Bit Field
		break;
	case 15:
		assert(false);
		//Coprocessor Interface/MC68040
		break;
	}
}