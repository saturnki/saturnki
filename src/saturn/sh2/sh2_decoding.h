OpcodeType DecodeInstruction(Instruction &i)
{
	switch (i.a())
	{
	case 0:
		switch (i.d())
		{
		case 2:
			switch (i.c())
			{
			case 0: return &Sh2::Stc<kSr>;
			case 1: return &Sh2::Stc<kGbr>;
			case 2: return &Sh2::Stc<kVbr>;
			default: return &Sh2::IllegalOperation;
			}
		case 3:
			switch (i.c())
			{
			case 0: return &Sh2::Bsrf;
			case 2: return &Sh2::Braf; 
			default: return &Sh2::IllegalOperation;
			}
		case 4: return &Sh2::Movs0<kByte>; 
		case 5: return &Sh2::Movs0<kWord>;
		case 6: return &Sh2::Movs0<kLong>;
		case 7: return &Sh2::Mul<long,long>;
		case 8:
			switch (i.c())
			{
			case 0: return &Sh2::SetOrClearT<0>;
			case 1: return &Sh2::SetOrClearT<1>;
			case 2: return &Sh2::Clrmac;
			default: return &Sh2::IllegalOperation;
			}     
		case 9:
			switch (i.c())
			{
			case 0: return &Sh2::Nop;
			case 1: return &Sh2::Div0u;
			case 2: return &Sh2::Movt;
			default: return &Sh2::IllegalOperation;
			}     
		case 10:
			switch (i.c())
			{
			case 0: return &Sh2::Sts<kMach>;
			case 1: return &Sh2::Sts<kMacl>;
			case 2: return &Sh2::Sts<kPr>;
			default: return &Sh2::IllegalOperation;
			}     
		case 11:
			switch (i.c())
			{
			case 0: return &Sh2::Rts;
			case 1: return &Sh2::Sleep;
			case 2: return &Sh2::Rte;
			default: return &Sh2::IllegalOperation;
			}    
		case 12: return &Sh2::Movl0<kByte>;
		case 13: return &Sh2::Movl0<kWord>;
		case 14: return &Sh2::Movl0<kLong>;
		case 15: return &Sh2::Macl;
		default: return &Sh2::IllegalOperation;
		}

	case 1: return &Sh2::Movs4<kLong,2>;
	case 2:
		switch (i.d())
		{
		case 0: return &Sh2::Movs<kByte>;
		case 1: return &Sh2::Movs<kWord>;
		case 2: return &Sh2::Movs<kLong>;
		case 4: return &Sh2::Movm<kByte,1>;
		case 5: return &Sh2::Movm<kWord,2>;
		case 6: return &Sh2::Movm<kLong,4>;
		case 7: return &Sh2::Div0s;
		case 8: return &Sh2::Tst;
		case 9: return &Sh2::MathRmRn<kAnd>;
		case 10: return &Sh2::MathRmRn<kXor>;
		case 11: return &Sh2::MathRmRn<kOr>;
		case 12: return &Sh2::Cmpstr;
		case 13: return &Sh2::Xtrct;
		case 14: return &Sh2::Mul<unsigned long,unsigned short>;
		case 15: return &Sh2::Mul<long,short>;
		default: return &Sh2::IllegalOperation;
		}
	case 3:
		switch(i.d())
		{
		case 0:  return &Sh2::Cmp<kIsEqual,long>;
		case 2:  return &Sh2::Cmp<kIsGreaterThanOrEqual,unsigned long>;
		case 3:  return &Sh2::Cmp<kIsGreaterThanOrEqual,long>;
		case 4:  return &Sh2::Div1;
		case 5:  return &Sh2::Dmulu;
		case 6:  return &Sh2::Cmp<kIsGreaterThan,unsigned long>;
		case 7:  return &Sh2::Cmp<kIsGreaterThan,long>;
		case 8:  return &Sh2::MathRmRn<kSubtract>;
		case 10: return &Sh2::MathCarryRmRn<kSubtract>;
		case 11: return &Sh2::MathVRmRn<kSubtract>;
		case 12: return &Sh2::MathRmRn<kAdd>;
		case 13: return &Sh2::Dmuls;
		case 14: return &Sh2::MathCarryRmRn<kAdd>;
		case 15: return &Sh2::MathVRmRn<kAdd>;
		default: return &Sh2::IllegalOperation;
		}
	case 4:
		switch(i.d())
		{
		case 0:
			switch(i.c())
			{
			case 0: return &Sh2::ShllShlr<kLeft,1<<31>;
			case 1: return &Sh2::Dt;
			case 2: return &Sh2::ShllShlr<kLeft,1<<31>;
			default: return &Sh2::IllegalOperation;
			}
		case 1:
			switch(i.c())
			{
			case 0: return &Sh2::ShllShlr<kRight,1>;
			case 1: return &Sh2::Cmp<kIsGreaterThanOrEqual>;
			case 2: return &Sh2::Shar;
			default: return &Sh2::IllegalOperation;
			}     
		case 2:
			switch(i.c())
			{
			case 0: return &Sh2::Stsm<kMach>;//stsmmach
			case 1: return &Sh2::Stsm<kMacl>;
			case 2: return &Sh2::Stsm<kPr>;
			default: return &Sh2::IllegalOperation;
			}
		case 3:
			switch(i.c())
			{
			case 0: return &Sh2::Stcm<kSr>;//stcmsr
			case 1: return &Sh2::Stcm<kGbr>;
			case 2: return &Sh2::Stcm<kVbr>;
			default: return &Sh2::IllegalOperation;
			}
		case 4:

			switch(i.c())
			{
			case 0: return &Sh2::Rot<kLeft,1,0xFFFFFFFE>;
			case 2: return &Sh2::Rotc<1<<31,1,0xFFFFFFFE,kLeft>;
			default: return &Sh2::IllegalOperation;
			}     
		case 5:
			switch(i.c())
			{
			case 0: return &Sh2::Rot<kRight,1<<31,0x7FFFFFFF>;
			case 1: return &Sh2::Cmp<kIsGreaterThan>;
			case 2: return &Sh2::Rotc<1,1<<31,0x7FFFFFFF,kRight>;
			default: return &Sh2::IllegalOperation;
			}                 
		case 6:
			switch(i.c())
			{
			case 0: return &Sh2::Ldsm<kMach>;
			case 1: return &Sh2::Ldsm<kMacl>;
			case 2: return &Sh2::Ldsm<kPr>;
			default: return &Sh2::IllegalOperation;
			}     
		case 7:
			switch(i.c())
			{
			case 0: return &Sh2::Ldcm<kSr>;
			case 1: return &Sh2::Ldcm<kGbr>;
			case 2: return &Sh2::Ldcm<kVbr>;
			default: return &Sh2::IllegalOperation;
			}     
		case 8:
			switch(i.c())
			{
			case 0: return &Sh2::Shll<2>;
			case 1: return &Sh2::Shll<8>;
			case 2: return &Sh2::Shll<16>;
			default: return &Sh2::IllegalOperation;
			}     
		case 9:
			switch(i.c())
			{
			case 0: return &Sh2::Shlr<2, 0x3FFFFFFF>;
			case 1: return &Sh2::Shlr<8, 0x00FFFFFF>;
			case 2: return &Sh2::Shlr<16,0x0000FFFF>;
			default: return &Sh2::IllegalOperation;
			}     
		case 10:
			switch(i.c())
			{
			case 0: return &Sh2::IllegalOperation; //return &SH2ldsmach<WHICH_SH2>;
			case 1: return &Sh2::IllegalOperation;//return &SH2ldsmacl<WHICH_SH2>;
			case 2: return &Sh2::Ldspr;
			default: return &Sh2::IllegalOperation;
			}     
		case 11:
			switch(i.c())
			{
			case 0: return &Sh2::JmpJsr<true>;
			case 1: return &Sh2::IllegalOperation;// return &SH2tas<WHICH_SH2>;
			case 2: return &Sh2::JmpJsr<false>;
			default: return &Sh2::IllegalOperation;
			}     
		case 14:
			switch(i.c())
			{
			case 0: return &Sh2::Ldc<kSr>;
			case 1: return &Sh2::Ldc<kGbr>;
			case 2: return &Sh2::Ldc<kVbr>;
			default: return &Sh2::IllegalOperation;
			}
		case 15: return &Sh2::IllegalOperation;//&SH2macw<WHICH_SH2>;
		default: return &Sh2::IllegalOperation;
		}
	case 5: return &Sh2::Movl4<kLong,2>;
	case 6:
		switch (i.d())
		{
		case 0: return &Sh2::Movl<kByte>;
		case 1: return &Sh2::Movl<kWord>;
		case 2: return &Sh2::Movl<kLong>;
		case 3: return &Sh2::Mov;
		case 4: return &Sh2::Movp<kByte,1>;
		case 5: return &Sh2::Movp<kWord,2>;
		case 6: return &Sh2::Movp<kLong,4>;
		case 7: return &Sh2::MathRmRn<kNot>;
		case 8: return &Sh2::Swapb;
		case 9: return &Sh2::Swapw;
		case 10: return &Sh2::Neg<true>;
		case 11: return &Sh2::Neg<false>;
		case 12: return &Sh2::Ext<u32,u8>;
		case 13: return &Sh2::Ext<u32,u16>;
		case 14: return &Sh2::Ext<u32,s8>;
		case 15: return &Sh2::Ext<u32,s16>;
		}
	case 7: return &Sh2::Addi;
	case 8:
		switch (i.b())
		{				
		case 0:  return &Sh2::Movs4<kByte,0>;
		case 1:  return &Sh2::Movs4<kWord,1>;
		case 4:  return &Sh2::Movl4<kByte,0>;
		case 5:  return &Sh2::Movl4<kWord,1>;
		case 8:  return &Sh2::Cmpim;
		case 9:  return &Sh2::BtBts<false>;
		case 11: return &Sh2::BfBfs<false>;
		case 13: return &Sh2::BtBts<true>;
		case 15: return &Sh2::Bfs;
		default: return &Sh2::IllegalOperation;
		}   
	case 9: return &Sh2::Movi<kWord,1>;
	case 10: return &Sh2::Bra;
	case 11: return &Sh2::Bsr;
	case 12:
		switch(i.b())
		{
		case 0:  return &Sh2::Movsg<kByte,0>;
		case 1:  return &Sh2::Movsg<kWord,1>;
		case 2:  return &Sh2::Movsg<kLong,2>;
		case 3:  return &Sh2::IllegalOperation;//return &SH2trapa<WHICH_SH2>;
		case 4:  return &Sh2::IllegalOperation;//return &SH2movblg<WHICH_SH2>;
		case 5:  return &Sh2::IllegalOperation;//return &SH2movwlg<WHICH_SH2>;
		case 6:  return &Sh2::IllegalOperation;//return &SH2movllg<WHICH_SH2>;
		case 7:  return &Sh2::Mova;
		case 8:  return &Sh2::Tsti;
		case 9:  return &Sh2::MathImmediate<kAnd>;
		case 10: return &Sh2::MathImmediate<kXor>;
		case 11: return &Sh2::MathImmediate<kOr>;
		case 12:  return &Sh2::MathM<kAnd,true>;
		case 13:  return &Sh2::MathM<kAnd,false>;
		case 14:  return &Sh2::MathM<kXor,false>;
		case 15:  return &Sh2::MathM<kOr,false>;
		}
	case 13: return &Sh2::Movi<kLong,2>;
	case 14: return &Sh2::Movi_;
	default: return &Sh2::IllegalOperation;
	}
}