#include "lib/lib.h"

extern "C" void _main()
{
	*vdp2::regs::prisa = 0x0707;
	*vdp2::regs::prisb = 0x0707;
	*vdp2::regs::prisc = 0x0707;
	*vdp2::regs::prisd = 0x0707;

	//enable normal shadow
	*vdp2::regs::sdctl = 0x0001;

	*vdp2::regs::spctl = 0;

	io::Init();

	io::PrintString4bit(1,0,"This text should be lighter",0);
	io::PrintString4bit(1,1,"This text should be darker",0);
	int frame_count = 0;
	while(1)
	{
		vdp2::WaitForVblank();
		simple::Vdp1ListStart();
		util::Points p;
		p.xa = 8;
		p.ya = 8;
		p.xb = 312;
		p.yb = 8;
		p.xc = 312;
		p.yc = 216;
		p.xd = 8;
		p.yd = 216;
		//7FE
		vdp1::command::Polygon(p,0x7FFE,0,0,0);
		vdp1::command::DrawEnd();

		frame_count++;
	}
}
