#include "lib/lib.h"

template <typename T>
void MakeRgbSpectrum32(T* destination)
{
	u8 r = 0, g = 0, b = 0;

	int i = 0;

	while ((r < 255) && (g < 255) && (b < 255))
	{
		if(r < 255)
			r++;
		if((r == 255) && (g < 255))
			g++;
		if((r == 255) && (g == 255) && (b < 255))
			b++;

		i++;

		destination[i] = util::MakeColor32(r,g,b,1);
	}
}

extern "C" void _main()
{
	//11 bit doesn't use bitmap pal reg

	simple::WriteRgbBitmapTestPattern<vu16,16>();

	*vdp2::regs::ramctl = vdp2::ColorRamMode::k5bit1024 << 12;

#if 1
	for(int i = 0; i < 0x0800; i++)
	{
		u8 r = i;
		u8 g = i ^ 0xff;
		u8 b = 0;
		vdp2::cram[i] = util::MakeColor16(r,g,b,1);
	}
#endif


	//enable bitmap for layer 0
	//set 11 bit color
	//set 512x256 size
	*vdp2::regs::chctla = (vdp2::ColorCount::k2048 << 4) | (vdp2::BitmapSize::k512x256 << 2) | (1 << 1);

	//map offset of 0
	*vdp2::regs::mpofn = 0;

	*vdp2::regs::tvmd = 0x8000;//turn on the screen

	while(1)
	{
		simple::DoNothing();
	}
}