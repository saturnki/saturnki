#include "lib/lib.h"

extern "C" void _main()
{
	io::Init();

	u16 color_table[64];
	for(int i = 0; i < 64; i++)
		color_table[i] = util::MakeColor16(i,i,0,1);

	int frame_count = 0;

	//make a column pattern
	const int x_max = 512 / 2;
	for(int x = 0; x < x_max; x++)
	{
		vu8* vram = (vu8*)(vdp2::kVramBaseAddress);
		for(int y = 0; y < 256; y++)
		{
			u32 value = tables::kSine4bit[x & 63];
			vram[(y * 512 >> 1) + x] = value | value << 4;
		}
	}

	io::PrintString4bit(0,0,"Line Scroll Table Test!");

	//line scroll interval every line
	//no line zoom
	//enable line scroll x, y 

	*vdp2::regs::scrctl = (1 << 3) | (1 << 2) | (1 << 1);
//	*vdp2::regs::scrctl = (1 << 2) | (1 << 1);
//	*vdp2::regs::scrctl = (1 << 1);

	u32 line_scroll_table_address = 0x7E000;
	vu32* const lsta0 = (vu32 *)0x25F800A0;

	*lsta0 = line_scroll_table_address >> 1;

	while(1)
	{
		simple::DoNothing();

		frame_count++;

		u32 height = 240;
#if 0
		vu16* p = (vu16 *)(vdp2::kVramBaseAddress+0x7E000);

		//i think this table is not correct, it's mixing
		//up horiz and vertical scrolling i htink
		for(int i = 0; i < height; i++)
		{
			p[i*2+0] = tables::kSine11bit[(frame_count + i) & 2047];
			p[i*2+1] = 0x0000;//fractional to 0
		}
#endif
		vu32* p = (vu32*)(vdp2::kVramBaseAddress+0x7E000);

		for(int i = 0; i < height; i++)
		{
			//horiz scroll
			u32 table_value =  tables::kSine11bit[(frame_count + i) & 2047];
			*p++ = table_value << 16 | 0;//fractional is 0
//			p[i+1] = table_value << 16 | 0;
//			p[i+2] = table_value << 16 | 0;
//			p[i] = //tables::kSine11bit[(frame_count + i) & 2047] << 16;
		}
#if 0
		for(int i = 0; i < height; i++)
		{
			p[i*3+0] = tables::kSine11bit[(frame_count + i) & 2047];
			p[i*3+1] = 0x0000;//fractional to 0
			p[i*3+2] = 0x0000;//vert screen scroll
			p[i*3+3] = 0x0000;//fractional to 0
			p[i*3+4] = 0x0000;//horiz coord inc
			p[i*3+5] = 0x0000;//fractional to 0
		}
#endif
	}
}