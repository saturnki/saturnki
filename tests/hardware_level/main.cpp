#include "lib/lib.h"

extern "C" void _main()
{
	vdp1::Clear();
	vdp2::Clear();

	u32 back_screen_table_address = 0x40000;

	vdp2::regs::bktau = 0x8000 | ((back_screen_table_address >> 17) & 7);
	vdp2::regs::bktal = 0x0000 | ((back_screen_table_address >> 1) & 0xFFFF);

	vdp2::regs::tvmd = 0x8000;//main screen turn on

	vu16* back_screen_table_pointer = (vu16 *)(vdp2::kVramBaseAddress + back_screen_table_address);

	while(1)
	{
		vdp2::WaitForVblank();

		{
			int i;
			u8 r = 0, g = 255, b = 0;
			for(i = 0; i < 480; i++)
			{
				back_screen_table_pointer[i] = util::MakeColor16(r,g,b,0);
				g--;
				r++;
				if(i&1)
					b++;
			}
		}
	}
}