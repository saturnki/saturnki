#include "lib/lib.h"

extern "C" void _main()
{
	io::Init();

	for(int i = 1; i < 26; i+=2)
	{
		io::PrintString4bit(1,i,"Fractional scrolling on nbg0, does this do anything?");
	}

	int frame_count = 0;

	while(1)
	{
		simple::DoNothing();
		frame_count++;

		u16 table_value = tables::kSine8bit[frame_count & 511] << 8;

		*vdp2::regs::scxdn0 = table_value;
		*vdp2::regs::scydn0 = table_value;
	}
}