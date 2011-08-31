#include "lib/lib.h"

extern "C" void _main()
{
	io::Init();

	for(int i = 0; i < 26; i+=2)
	{
		io::PrintString4bit(0,i,"Coordinate Increment Testing!!!!");
	}

	int frame_count = 0;

	//enable up to 1/4 reduction
	*vdp2::regs::zmctl = 3;

	while(1)
	{
		simple::DoNothing();
		frame_count++;

		//this ought to demonstrate the full range of values
		u32 table_value = tables::kSine11bit[frame_count & 2047] << 8;

		//gotta do this by hand since we want 32bit writes
		*((vu32*)(0x25F80078)) = table_value;
		*((vu32*)(0x25F8007C)) = table_value;
	}
}