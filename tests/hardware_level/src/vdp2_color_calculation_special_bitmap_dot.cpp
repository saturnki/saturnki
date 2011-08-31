#include "lib/lib.h"

extern "C" void _main()
{
	io::Init();

	u16 color_table[64];
	for(int i = 0; i < 64; i++)
		color_table[i] = util::MakeColor16(i,i,0,1);

	u32 back_screen_table_address = 0x40000;

	*vdp2::regs::bktau = 0x0000 | ((back_screen_table_address >> 17) & 7);
	*vdp2::regs::bktal = 0x0000 | ((back_screen_table_address >> 1) & 0xFFFF);

	vu16* back_screen_table_pointer = (vu16 *)(vdp2::kVramBaseAddress + back_screen_table_address);

	back_screen_table_pointer[0] = util::MakeColor16(0,7,7,1);

	*vdp2::regs::bmpna = 1 << 4;//turn on special color calc bit for bitmap
	*vdp2::regs::ccctl = 1; //enable color calc for nbg0
	*vdp2::regs::sfccmd = 2;//enable per dot for nbg0
	*vdp2::regs::sfcode = 1 << 7;//set special function code a to 0xe or 0xf

	int frame_count = 0;

	for(int x = 0; x < 160; x++)
	{
		vu8* vram = (vu8*)(vdp2::kVramBaseAddress);
		for(int y = 0; y < 224; y++)
		{
			int num = x & 0xf;
			vram[(y * 512 >> 1) + x] = num | num << 4;
		}
	}

	io::PrintString4bit(1,1,"This text should color calculate");
	io::PrintString4bit(1,2,"But some of the background shouldn't");

	while(1)
	{
		simple::DoNothing();

		frame_count++;

		*vdp2::regs::ccrna = tables::kSine5bit[frame_count & tables::kSine5bitMask];
	}
}