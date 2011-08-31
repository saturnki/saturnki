#include "lib/lib.h"

extern "C" void _main()
{
	simple::WriteRgbBitmapTestPattern<vu32,32>();

	//enable bitmap for layer 0
	//set 32 bit color
	//set 512x256 size
	*vdp2::regs::chctla = (vdp2::ColorCount::k16770000 << 4) | (vdp2::BitmapSize::k512x256 << 2) | (1 << 1);

	//map offset of 0
	*vdp2::regs::mpofn = 0;

	*vdp2::regs::tvmd = 0x8000;//turn on the screen

	while(1)
	{
		simple::DoNothing();
	}
}