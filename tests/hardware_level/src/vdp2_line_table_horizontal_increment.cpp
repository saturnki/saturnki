#include "lib/lib.h"

extern "C" void _main()
{
	io::Init();

	u16 color_table[64];
	for(int i = 0; i < 64; i++)
		color_table[i] = util::MakeColor16(i,i,0,1);

	int frame_count = 0;

	//make a row pattern
	const int x_max = 512;
	for(int x = 0; x < x_max; x++)
	{
		vu8* vram = (vu8*)(vdp2::kVramBaseAddress);
		for(int y = 0; y < 256; y++)
		{
			u32 value = tables::kSine4bit[x & 63];
			vram[(y * 512 >> 1) + x] = value | value << 4;
		}
	}

	*vdp2::regs::scrctl = (1 << 3) | (1 << 2) | (1 << 1);

	//up to 1/4 reduction
	*vdp2::regs::zmctl = 3;

	u32 line_scroll_table_address = 0x7E000;
	vu32* const lsta0 = (vu32 *)0x25F800A0;

	*lsta0 = line_scroll_table_address >> 1;

	while(1)
	{
		simple::DoNothing();

		frame_count++;

		u32 height = 240;
		vu32* p = (vu32*)(vdp2::kVramBaseAddress+0x7E000);

		for(int i = 0; i < height; i++)
		{
			u32 table_value = tables::kSine8bit[(frame_count + i) & 511];
			*p++ = 0 << 16 | 0;//fractional is 0
			*p++ = 0 << 16 | 0;//fractional is 0
			*p++ = (1 << 16 ) | table_value << 8;
		}
	}
}