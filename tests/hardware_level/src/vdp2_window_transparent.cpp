#include "lib/lib.h"

extern "C" void _main()
{
	io::Init();

	for(int i = 1; i < 26; i+=2)
		io::PrintString4bit(1,i,"Window 0, Transparent, Enable Inside");

	int frame_count = 0;

	//window settings
	*vdp2::regs::wpsx0 = 64;
	*vdp2::regs::wpsy0 = 64;
	*vdp2::regs::wpex0 = 256;
	*vdp2::regs::wpey0 = 128;

	*vdp2::regs::wctla = 1 << 1;

	while(1)
	{
		simple::DoNothing();
	}
}