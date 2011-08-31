#define INSTANT_INTERRUPT_TIMING
void SetInterruptRequest(unsigned int new_interrupt_level)
{
	unsigned int old_level = regs.interrupt_level;
	regs.interrupt_level = new_interrupt_level;
#ifdef INSTANT_INTERRUPT_TIMING
	//changing from less than seven to seven always causes an interrupt (non maskable)
	//7 can also be maskable
	if(old_level != 0x07 && regs.interrupt_level == 0x07)
		ServiceInterruptRequest(7);
	else
		CheckInterrupts();
#else
	//we service it later
#endif

}

void SetStatusRegister(unsigned int data)
{
	SetStatusRegisterNoInterrupt(data);
	CheckInterrupts();
	//	printf("probably broked it");
}
//todo template the interrupt part
void SetStatusRegisterNoInterrupt(unsigned int data)//0x2000
{
	regs.sr.interrupt_priority_mask = data >> 8;
	regs.sr.ccr = data;
	SetSAndMFlags<kBoth>((data >> 13) & 1,(data >> 12) & 1);
}

enum Which
{
	kSupervisor,
	kMaster,
	kBoth
};
template<Which which>
void SetSAndMFlags(bool supervisor, bool master)//4
{

	//save old stack pointer
	// s == 1, m == 1, master stack pointer
	// s == 1, m == 0, interrupt stack pointer
	// s == 0, user stack pointer
	if(regs.sr.master && regs.sr.supervisor)
		regs.stack_pointers.master = regs.a[7];
	else if(regs.sr.master == 0)
	{
		if(regs.sr.supervisor == 0)
			regs.stack_pointers.user = regs.a[7];
		else
			regs.stack_pointers.interrupt = regs.a[7];
	}

	//set s and m flags
	regs.sr.supervisor = supervisor;
	if(which == kBoth)
		regs.sr.master= master;

	//set new stack pointer
	//todo fix copypaste
	if(regs.sr.master && regs.sr.supervisor)
		regs.a[7] = regs.stack_pointers.master;
	else if(regs.sr.master == 0)
	{
		if(regs.sr.supervisor == 0)
			regs.a[7] = regs.stack_pointers.user;
		else
			regs.a[7] = regs.stack_pointers.interrupt;
	}
}

void CheckInterrupts(void)
{
	//beware since ipm will be unshifted here i guess
	if(regs.interrupt_level > regs.sr.interrupt_priority_mask)//200 > 0
		ServiceInterruptRequest(regs.interrupt_level);
}
enum InterruptAutovectorStuff
{
	kExceptionInterruptAutovector = 24,
	kInterruptAcknowledgeAutovector = 0xFFFFFFFF,
	kExceptionUninitializedInterrupt = 15
};

void ServiceInterruptRequest(unsigned int int_level)//2
{
	unsigned int vector = InterruptAcknowledge(int_level);

	if(vector == kInterruptAcknowledgeAutovector)
		vector = kExceptionInterruptAutovector+int_level;
	unsigned int sr = InitiateExceptionProcessing();

	regs.sr.interrupt_priority_mask = int_level;

	unsigned int new_pc = Read<kLong>((vector<<2) + regs.vbr);

	if(new_pc == 0)
		new_pc = Read<kLong>((kExceptionUninitializedInterrupt<<2) + regs.vbr);

	GenerateStackFrame(regs.pc, sr);

	Jump(new_pc);//16e0

	regs.interrupt_level = 0;
}

int InterruptAcknowledge(int int_level)//2
{
	regs.interrupt_level = 0;
	return kInterruptAcknowledgeAutovector;
}

unsigned int InitiateExceptionProcessing(void)
{
	unsigned int sr = regs.sr;

	SetSAndMFlags<kSupervisor>(true,false);

	return sr;
}

template<OperationSize size>
void PushStack(unsigned int data)
{
	regs.a[7] -= size;
	Write<size>(regs.a[7],data);
}

void GenerateStackFrame(unsigned int pc, unsigned int sr)
{
	PushStack<kLong>(pc);
	PushStack<kWord>(sr);
}