#include "lib/lib.h"

extern "C" void _main()
{
	io::Init();

	vu8* vram = (vu8*)(vdp2::kVramBaseAddress);
	for(int x = 0; x < 160; x++)//half size since there are 2 pixels in byte
	{
		for(int y = 0; y < 224; y++)
		{
			vram[(y * 512 >> 1) + x] = tables::kSine4bit[y & 63] | tables::kSine4bit[y & 63] << 4;
		}
	}

	u16 color_table[64];// = util::MakeColor16(i,i,0,1);
	for(int i = 0; i < 64; i++)
		color_table[i] = util::MakeColor16(i,i,0,1);

	u32 back_screen_table_address = 0x40000;

	*vdp2::regs::bktau = 0x0000 | ((back_screen_table_address >> 17) & 7);
	*vdp2::regs::bktal = 0x0000 | ((back_screen_table_address >> 1) & 0xFFFF);

	vu16* back_screen_table_pointer = (vu16 *)(vdp2::kVramBaseAddress + back_screen_table_address);

	back_screen_table_pointer[0] = util::MakeColor16(0,7,7,1);

	int frame_count = 0;

	while(1)
	{
		simple::DoNothing();

		frame_count++;

		//enable color calc for nbg0
		//select per top screen side
		//alpha blend
		*vdp2::regs::ccctl = (1 << 8) | 1;
		*vdp2::regs::ccrna = tables::kSine5bit[frame_count & 127];

		//palette cycling animation
		for(int i = 0; i < 64; i++)
			vdp2::cram[i] = util::MakeColor16(tables::kSine5bit[(frame_count + i) & 127],0,0,1);
	}
}