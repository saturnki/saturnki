#include "lib/lib.h"

extern "C" void _main()
{
	io::Init();

	for(int i = 0; i < 26; i+=2)
	{
		io::PrintString4bit(0,i,"Color Offset Testing!!!!");
	}

	int frame_count = 0;

	*vdp2::regs::clofen = 1;

	while(1)
	{
		simple::DoNothing();

		frame_count++;

		*vdp2::regs::coar = tables::kSine9bit[frame_count & 511];
	}
}