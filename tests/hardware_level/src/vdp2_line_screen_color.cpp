#include "lib/lib.h"

extern "C" void _main()
{
#if 0
	vdp1::Clear();
	vdp2::Clear();
#endif

	io::Init();

	int i;
	int interval = 64;
	for(i = 0; i < interval; i++)
	{
		int g = tables::kSine5bit[(i+64)&127];
		vdp2::cram[i] = util::MakeColor16(0,g,0,0);
	}
	for(; i < interval*2; i++)
	{
		int r = tables::kSine5bit[i&127];
		vdp2::cram[i] = util::MakeColor16(r,0,0,0);
	}
	for(; i < interval*3; i++)
	{
		int b = tables::kSine5bit[(i+64)&127];
		vdp2::cram[i] = util::MakeColor16(0,0,b,0);
	}	
	for(; i < interval*4; i++)
	{
		int b = tables::kSine5bit[i&127];
		vdp2::cram[i] = util::MakeColor16(b,0,b,0);
	}	
	for(int i = 0; i < 31; i++)
		io::PrintString4bit(0,i,"Hello Line Screen!!  Hello Line Screen!!",0);

	*vdp2::regs::lnclen = 0x1f;
	*vdp2::regs::ccctl = 0x1f;

	u32 line_screen_table_address = 0x40000;

	*vdp2::regs::lctau = 0x8000 | ((line_screen_table_address >> 17) & 7);
	*vdp2::regs::lctal = 0x0000 | ((line_screen_table_address >> 1) & 0xFFFF);

	vu16* line_screen_table_pointer = (vu16 *)(vdp2::kVramBaseAddress + line_screen_table_address);

	int frame_count = 0;
	while(1)
	{
		simple::DoNothing();
		frame_count++;

		int i;
		for(i = 0; i < 480; i++)
		{
			line_screen_table_pointer[i] = i;
		}

		*vdp2::regs::ccrna = tables::kSine5bit[frame_count&127];
	}
}