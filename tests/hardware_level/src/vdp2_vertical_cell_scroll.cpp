#include "lib/lib.h"

extern "C" void _main()
{
	io::Init();

	u16 color_table[64];
	for(int i = 0; i < 64; i++)
		color_table[i] = util::MakeColor16(i,i,0,1);

	int frame_count = 0;

	//make a row pattern
	const int x_max = 512 / 2;
	for(int x = 0; x < x_max; x++)
	{
		vu8* vram = (vu8*)(vdp2::kVramBaseAddress);
		for(int y = 0; y < 512; y++)
		{
			u32 value = tables::kSine4bit[y & 63];
			vram[(y * 512 >> 1) + x] = value | value << 4;
		}
	}

	for(int y = 0; y < 40; y+=9)
	{
		io::PrintString4bit(0,y,"Cell Scroll Table Test Cell Scroll Table ",0);
		io::PrintString4bit(0,y+3,"0123456789ABCDEF0123456789ABCDEF01234567",0);

	}

	*vdp2::regs::scrctl = 1;

	vu32* CYCA0 = (vu32*) 0x25F80010;
	vu32* CYCA1 = (vu32*) 0x25F80014;
	vu32* CYCB0 = (vu32*) 0x25F80018;
	vu32* CYCB1 = (vu32*) 0x25F8001C;

    *CYCA0  = 0x4FFFFFFF;
    *CYCA1  = 0xFFFFFFFF;
    *CYCB0  = 0xCFFFFFFF;
    *CYCB1  = 0xFFFFFFFF;

	u32 cell_scroll_table_address = 0x7E000;
	vu32* const vcstau = (vu32 *)0x25F8009C;

	*vcstau = cell_scroll_table_address >> 1;

	while(1)
	{
		simple::DoNothing();

		frame_count++;

		u32 height = 240;

		vu32* p = (vu32*)(vdp2::kVramBaseAddress+cell_scroll_table_address);

		for(int i = 0; i < height; i++)
		{
			//horiz scroll
			u32 table_value =  tables::kSine11bit[(frame_count + i) & 2047];
			*p++ = table_value << 16 | 0;//fractional is 0
		}
	}
}