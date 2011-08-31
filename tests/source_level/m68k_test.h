

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
////TESTING//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

enum AddressingModes
{
	Dn = 0,
	An = 1,
	_An_ = 2,
	_An_plus = 3,
	Minus_An_ = 4,
	_d16_An_ = 5,
	_d8_An_Xn = 6
};
unsigned int MakeSource(unsigned int addressing_mode, int source_register)
{
	return source_register | (addressing_mode << 3);
}
unsigned int MakeDestination(unsigned int destination_mode, int destination_register)
{
	return 
		(destination_register << 9) 
		| (destination_mode << 6);
}

template <OperationSize size>
int GetSizeBytes()
{
	if(size == kByte)
		return 1;
	if(size == kWord)
		return 3;
	if(size == kLong)
		return 2;
}
template <OperationSize size>
unsigned int MakeMoveInstruction(
								 int source_addressing_mode,
								 int destination_addressing_mode)
{
	unsigned int size_bytes = GetSizeBytes<size>();

	return 
		(size_bytes << 12)
		| MakeSource(source_addressing_mode,0) 
		| MakeDestination(destination_addressing_mode,1)
		;
}

enum AddressingModesAlt
{
	k_xxxW = 0,
	k_xxxL = 1,
	k_pound_data = 4,
	k_d16_pc = 2,
	k_d8_pc_xn = 3
};

template <OperationSize size>
int MakeMoveInstructionAltDest(
							   int source_addressing_mode,
							   AddressingModesAlt destination_addressing_mode)
{
	unsigned int size_bytes = GetSizeBytes<size>();

	return 
		(size_bytes << 12)
		| MakeSource(source_addressing_mode,0) 
		| MakeDestination(7,destination_addressing_mode)
		;
}

template <OperationSize size>
int MakeMoveInstructionAltSrc(
							  AddressingModesAlt source_addressing_mode,
							  int destination_addressing_mode)
{
	unsigned int size_bytes = GetSizeBytes<size>();

	return 
		(size_bytes << 12)
		| MakeSource(7,source_addressing_mode) 
		| MakeDestination(destination_addressing_mode,2)
		;
}

void CheckCycles(int cycles_desired)
{
	//fuck prefetching
	if(regs.cycles_executed != cycles_desired - 4)
		assert(false);
}
void TestMoveWordCycles()
{
	int instruction = 0;

	//dn dn
	regs.cycles_executed = 0;
	instruction = MakeMoveInstruction<kWord>(Dn,Dn);
	DecodeAndExecuteInstruction(instruction);
	CheckCycles(4);

	//dn an
	regs.cycles_executed = 0;
	instruction = MakeMoveInstruction<kWord>(Dn,An);
	DecodeAndExecuteInstruction(instruction);
	CheckCycles(4);

	//dn (an)
	regs.cycles_executed = 0;
	instruction = MakeMoveInstruction<kWord>(Dn,_An_);
	DecodeAndExecuteInstruction(instruction);
	CheckCycles(8);

	//dn (an)+
	regs.cycles_executed = 0;
	instruction = MakeMoveInstruction<kWord>(Dn,_An_plus );
	DecodeAndExecuteInstruction(instruction);
	CheckCycles(8);

	//dn -(an)
	regs.cycles_executed = 0;
	instruction = MakeMoveInstruction<kWord>(Dn,Minus_An_  );
	DecodeAndExecuteInstruction(instruction);
	CheckCycles(8);

	//(d16,an)
	regs.cycles_executed = 0;
	instruction = MakeMoveInstruction<kWord>(Dn,_d16_An_  );
	DecodeAndExecuteInstruction(instruction);
	CheckCycles(12);

	//(d8,an,xn)
	regs.cycles_executed = 0;
	instruction = MakeMoveInstruction<kWord>(Dn,_d8_An_Xn  );
	DecodeAndExecuteInstruction(instruction);
	CheckCycles(14);

	//(xxx).w
	regs.cycles_executed = 0;
	instruction = MakeMoveInstructionAltDest<kWord>(Dn,k_xxxW   );
	DecodeAndExecuteInstruction(instruction);
	CheckCycles(12);

	//(xxx).l
	regs.cycles_executed = 0;
	instruction = MakeMoveInstructionAltDest<kWord>(Dn,k_xxxL    );
	DecodeAndExecuteInstruction(instruction);
	CheckCycles(16);

	//an,dn
	regs.cycles_executed = 0;
	instruction = MakeMoveInstruction<kWord>(An,Dn  );
	DecodeAndExecuteInstruction(instruction);
	CheckCycles(4);
#if 1
	//(an),dn
	//		regs.cycles_executed = 0;
	//		instruction = MakeMoveInstruction<kWord>(_An_,Dn  );
	//		DecodeAndExecuteInstruction(instruction);
	//		CheckCycles(8);

	//(an)+,dn
	regs.cycles_executed = 0;
	instruction = MakeMoveInstruction<kWord>(_An_plus,Dn  );
	DecodeAndExecuteInstruction(instruction);
	CheckCycles(8);

	//-(an),dn
	regs.cycles_executed = 0;
	instruction = MakeMoveInstruction<kWord>(Minus_An_,Dn  );
	DecodeAndExecuteInstruction(instruction);
	CheckCycles(10);

	//(d16,an),dn
	regs.cycles_executed = 0;
	instruction = MakeMoveInstruction<kWord>(_d16_An_ ,Dn  );
	DecodeAndExecuteInstruction(instruction);
	CheckCycles(12);

	//(d8,an,xn),dn
	regs.cycles_executed = 0;
	instruction = MakeMoveInstruction<kWord>(_d8_An_Xn  ,Dn  );
	DecodeAndExecuteInstruction(instruction);
	CheckCycles(14);
#endif
	//(xxx).w, dn
	regs.cycles_executed = 0;
	instruction = MakeMoveInstructionAltSrc<kWord>(k_xxxW ,Dn    );
	DecodeAndExecuteInstruction(instruction);
	CheckCycles(12);

	//(xxx).l,dn
	regs.cycles_executed = 0;
	instruction = MakeMoveInstructionAltSrc<kWord>(k_xxxL    ,Dn    );
	DecodeAndExecuteInstruction(instruction);
	CheckCycles(16);

	//i don't understand this one
#if 0
	//(d16,pc),Dn
	regs.cycles_executed = 0;
	instruction = MakeMoveInstructionAltSrc<kWord>(k_d16_pc   ,Dn    );
	DecodeAndExecuteInstruction(instruction);
	CheckCycles(12);
#endif

	//(d8,pc,xn),Dn
	regs.cycles_executed = 0;
	instruction = MakeMoveInstructionAltSrc<kWord>(k_d8_pc_xn  ,Dn    );
	DecodeAndExecuteInstruction(instruction);
	CheckCycles(14);

	//x<data>,Dn
	regs.cycles_executed = 0;
	instruction = MakeMoveInstructionAltSrc<kWord>(k_pound_data ,Dn    );
	DecodeAndExecuteInstruction(instruction);
	CheckCycles(8);

	//move long
}

void Test(int cycles_expected, int instruction)
{
	regs.cycles_executed = 0;
	DecodeAndExecuteInstruction(instruction);
	CheckCycles(cycles_expected);
}

void TestJmpJsrLeaPeaMovemCycles()
{
	//jmp an
	Test(8,(0x13B << 6) | (_An_ << 3) | 1);

	//jmp d16 an
	Test(10,(0x13B << 6) | (_d16_An_ << 3) | 1);

	//jsr an

	//jsr d16 an
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
////END OF TESTING//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////