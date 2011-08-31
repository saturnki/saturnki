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

		int width = 128 + 16;
		int height = 208;
		int spacer = 8;
		int x = spacer;

		p = util::GetPoints(x,spacer,width,height);

		vdp1::command::Polygon(p,
			util::MakeColor16(15,15,15,1),
			vdp1::command::kGouraudShading,
			gouraud_table_starting_address);

		x += width + spacer;
		x += spacer;

		p = util::GetPoints(x,spacer,width,height);
		
		vdp1::command::Polygon(p,
			util::MakeColor16(15,15,15,1),
			vdp1::command::kGouraudShading,
			gouraud_table_starting_address + 8);

		vdp1::command::DrawEnd();
	}
}