#include "lib/lib.h"

extern "C" void _main()
{
	io::Init();

	for(int i = 1; i < 26; i+=2)
		io::PrintString4bit(1,i,"Color Calculation! Color Calculation! Color Calculation!");

	int frame_count = 0;

	while(1)
	{
		simple::DoNothing();

		frame_count++;

		//enable color calc for nbg0
		//select per top screen side
		//alpha blend
		*vdp2::regs::ccctl = 1;
		*vdp2::regs::ccrna = tables::kSine5bit[frame_count & 127];
	}
}