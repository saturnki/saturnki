template <int msb, int lo_mask, int hi_mask>
void SignExtend(long n)
{
	if((regs.r[n]&msb)==0)
		regs.r[n] & lo_mask;
	else
		regs.r[n]|= hi_mask;
}

template <OperationSize size>
void SignExtend(long n)
{
	if(size == kByte) SignExtend<0x80,0xFF,0xFFFFFF00>(n);
	if(size == kWord) SignExtend<0x8000,0xFFFF,0xFFFF0000>(n);
	//long, do nothing
}

//MOV Rm,Rn
void Mov()
{
	s32 m = i.c();
	s32 n = i.b();
	regs.r[n]=regs.r[m];
	regs.pc+=2;
	AddCycles(1);
}

//movbs, movws, movls MOV.B Rm,@Rn, MOV.W Rm,@Rn, MOV.L Rm,@Rn
template <OperationSize size>
void Movs()
{
	s32 m = i.c();
	s32 n = i.b();
	Write<size>(regs.r[n],regs.r[m]);
	regs.pc+=2;
	AddCycles(1);
}

//MOVBL(long m,long n) /* MOV.B @Rm,Rn */
//MOVWL(long m,long n) /* MOV.W @Rm,Rn */
//MOVLL(long m,long n) /* MOV.L @Rm,Rn */
template <OperationSize size>
void Movl()
{
	s32 m = i.c();
	s32 n = i.b();
	regs.r[n] = Read<size>(regs.r[m]);
	SignExtend<size>(n);
	regs.pc+=2;
	AddCycles(1);
}

//movbm, movwm, movlm MOV.B Rm,@–Rn, MOV.W Rm,@–Rn, MOV.L Rm,@–Rn
template <OperationSize size, int sub>
void Movm()
{
	s32 m = i.c();
	s32 n = i.b();
	int q = sub;
	int f = regs.r[n]-sub;
	Write<size>(regs.r[n]-sub,regs.r[m]);
	regs.r[n]-=sub;
	regs.pc+=2;
	AddCycles(1);
}

//MOVBP MOV.B @Rm+,Rn , MOVWP MOV.W @Rm+,Rn, MOVLP MOV.L @Rm+,Rn
template <OperationSize size, int increment>
void Movp()
{
	s32 m = i.c();
	s32 n = i.b();
	regs.r[n] = static_cast<long>(Read<size>(regs.r[m]));
	SignExtend<size>(n);
	if(n != m)
		regs.r[m]+= increment;
	regs.pc+=2;
	AddCycles(1);
}

//MOVBS0 MOV.B Rm,@(R0,Rn),MOVWS0 MOV.W Rm,@(R0,Rn) MOVLS0 MOV.L Rm,@(R0,Rn)
template <OperationSize size>
void Movs0()
{
	s32 m = i.c();
	s32 n = i.b();
	Write<size>(regs.r[m]+regs.r[0],regs.r[n]);
	regs.pc+=2;
	AddCycles(1);
}

//MOVBL0(long m,long n) /* MOV.B @(R0,Rm),Rn */
//MOVWL0(long m,long n) /* MOV.W @(R0,Rm),Rn */
//MOVLL0(long m,long n) /* MOV.L @(R0,Rm),Rn */
template <OperationSize size>
void Movl0()
{
	s32 m = i.c();
	s32 n = i.b();
	regs.r[n]=Read<size>(regs.r[m]+regs.r[0]);
	SignExtend<size>(n);
	regs.pc+=2;
	AddCycles(1);
}
//this instruction is sort of its own thing
//MOV (Move Immediate Data): Data Transfer Instruction
//MOVI(long i,long n) /* MOV #imm,Rn */
void Movi_() /* MOV #imm,Rn */
{
	s32 i_ = i.cd();
	s32 n = i.b();
	regs.r[n] = i_;
	SignExtend<kByte>(n);
	regs.pc+=2;
	AddCycles(1);
}

//MOVWI(long d,long n) /* MOV.W @(disp,PC),Rn */
//MOVLI(long d,long n) /* MOV.L @(disp,PC),Rn */
template <OperationSize size, int shift>
void Movi()
{
	s32 d = i.cd();
	s32 n = i.b();
	long displacement = 0xFF & static_cast<long>(d);
	long read_address;
	if(size == kWord)
		read_address = regs.pc + (displacement << 1) + 4;
	else
		read_address = ((regs.pc+4) & 0xFFFFFFFC)+(displacement << shift);
	regs.r[n] = static_cast<long>(Read<size>(read_address));
	SignExtend<size>(n);
	regs.pc+=2;
	AddCycles(1);
}

//MOV (Move Peripheral Data): Data Transfer Instruction
//MOVBLG(long d) /* MOV.B @(disp,GBR),R0 */MOVWLG(long d) /* MOV.W @(disp,GBR),R0 */MOVLLG(long d) /* MOV.L @(disp,GBR),R0 */
template <OperationSize size, int shift>
void Movlg(long d)
{
	long displacement = 0xFF & static_cast<long>(d);
	regs.r[0] = static_cast<long>(Read<size>(regs.gbr+(displacement << shift)));
//what is this supposed to be?
//	SignExtend(0);
	regs.pc+=2;
	AddCycles(1);
}

//MOVBSG MOV.B R0,@(disp,GBR) MOVWSG MOV.W R0,@(disp,GBR) MOVLSG MOV.L R0,@(disp,GBR)
template <OperationSize size, int shift>
void Movsg()
{
	s32 d = i.cd();
	long displacement = 0xFF & static_cast<long>(d);
	Write<size>(regs.gbr+(displacement << shift),regs.r[0]);
	regs.pc+=2;
	AddCycles(1);
}

//MOV (Move Structure Data): Data Transfer Instruction
//MOVBS4 MOV.B R0,@(disp,Rn) MOVWS4 MOV.W R0,@(disp,Rn) MOVLS4 MOV.L Rm,@(disp,Rn)
//WARNING this is a special case
//MOVLS4 does not write from r[0], but from r[m]
//m argument is 0 for MOVBS4, MOVWS4
template <OperationSize size, int shift>
void Movs4()//long m, long d, long n)
{
	s32 m = 0;
	s32 d = i.d();
	s32 n = i.c();
	if(size == kLong)
	{
		m = i.c();
		n = i.b();
	}
	long displacement = 0xF & static_cast<long>(d);
	Write<size>(regs.r[n]+(displacement << shift),regs.r[m]);
	regs.pc+=2;
	AddCycles(1);
}

//MOVBL4 MOV.B @(disp,Rm),R0 , MOVWL4 MOV.W @(disp,Rm),R0, MOVLL4 MOV.L @(disp,Rm),Rn
//this is another special case
//n and m are 0 forMOVBL4 and MOVWL4
template <OperationSize size, int shift>
void Movl4()//long m, long d, long n)
{
	s32 m = i.c();
	s32 d = i.d();
	s32 n = 0;

	if(size == kLong)
	{
		n = i.b();
	}

	long displacement = 0xF & static_cast<long>(d);
	regs.r[n]=Read<size>(regs.r[m]+(displacement<<shift));
	SignExtend<size>(0);//0 is ok because Long doesn't need it
	regs.pc+=2;
	AddCycles(1);
}

//MOVA (Move Effective Address): Data Transfer Instruction
void Mova() /* MOVA @(disp,PC),R0 */
{
	s32 d = i.cd();
	long disp=0xFF & static_cast<long>(d);
	regs.r[0]=((regs.pc+4)&0xFFFFFFFC)+(disp<<2);
	regs.pc+=2;
	AddCycles(1);
}

//MOVT (Move T Bit): Data Transfer Instruction
// MOVT Rn
void Movt() /* */
{
	s32 n = i.b();
	regs.r[n]= regs.sr.t;
	regs.pc+=2;
	AddCycles(1);
}
