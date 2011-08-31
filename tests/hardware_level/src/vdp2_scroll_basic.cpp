#include "lib/lib.h"

extern "C" void _main()
{
	io::Init();

	for(int i = 1; i < 26; i+=2)
	{
		io::PrintString4bit(1,i,"Scrolling the full 11 bit range in the x and y directions");
	}

	int frame_count = 0;

	while(1)
	{
		simple::DoNothing();
		frame_count++;

		u16 table_value = tables::kSine11bit[frame_count & 2047];

		*vdp2::regs::scxin0 = table_value;
		*vdp2::regs::scyin0 = table_value;
	}
}