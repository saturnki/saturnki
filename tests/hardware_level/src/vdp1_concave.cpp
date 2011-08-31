#include "lib/lib.h"

extern "C" void _main()
{
	const u32 gouraud_table_starting_address = 0x60000;

	simple::Vdp1Setup();

	simple::MakeDefaultGouraudTables(gouraud_table_starting_address);

	//turn on the screen
	*vdp2::regs::tvmd = 0x8000;

	while(1)
	{
		//i guess this is bad practice, it would be better to set up an interrupt
		vdp2::WaitForVblank();

		simple::Vdp1ListStart();

		util::Points p = {};

		p.xa = 312;
		p.ya = 8;

		p.xc = 312;
		p.yc = 216;

		p.xb = 8;
		p.yb = 216;

		p.xd = 8;
		p.yd = 8;

		vdp1::command::Polygon(p,
			util::MakeColor16(15,15,15,1),
			vdp1::command::kGouraudShading,
			gouraud_table_starting_address);

		vdp1::command::DrawEnd();
	}
}