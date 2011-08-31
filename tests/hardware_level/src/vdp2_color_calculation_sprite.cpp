#include "lib/lib.h"

u16 SpriteTypeMakeData(
	bool shadow, 
	int priority_number,
	int color_calculation_number,
	int color
	)
{
	return 
		(shadow << 15) 
		| (priority_number << 12)
		| (color_calculation_number << 9)
		| color;
}

extern "C" void _main()
{
#if 0
	vdp1::Clear();
	vdp2::Clear();
#endif

	simple::Vdp1Setup();
	io::Init();

	*vdp2::regs::prisa = 0x0100;
	*vdp2::regs::prisb = 0x0302;
	*vdp2::regs::prisc = 0x0504;
	*vdp2::regs::prisd = 0x0706;

	for(int i = 0; i < 31; i++)
		io::PrintString4bit(0,i,"Sprite Color Calculation!! Sprites!!   ",0);

	//enable for sprite
	*vdp2::regs::ccctl = 1 << 6;

	//color calc when priority >= cc number
	//cc number is 4
	//sprite type 7
	*vdp2::regs::spctl = (2 << 12) | (4 << 8) | 7;

	simple::Vdp1ListStart();

	util::Points p = util::GetPoints(16,16,48,48);
	u16 data = SpriteTypeMakeData(0,7,7,12);
	vdp1::command::Polygon(p,data,0,0,0);

	p = util::GetPoints(80,16,48,48);
	data = SpriteTypeMakeData(0,6,6,12);
	vdp1::command::Polygon(p,data,0,0,0);

	p = util::GetPoints(144,16,48,48);
	data = SpriteTypeMakeData(0,5,5,12);
	vdp1::command::Polygon(p,data,0,0,0);

	p = util::GetPoints(208,16,48,48);
	data = SpriteTypeMakeData(0,4,4,12);
	vdp1::command::Polygon(p,data,0,0,0);

	vdp1::command::DrawEnd();

	int frame_count = 0;
	while(1)
	{
		vdp2::WaitForVblank();

		frame_count++;

		*vdp2::regs::ccrsa = tables::kSine5bit[frame_count&127] | (tables::kSine5bit[(frame_count+64)&127] << 8);
		*vdp2::regs::ccrsb = tables::kSine5bit[frame_count&127] | (tables::kSine5bit[(frame_count+64)&127] << 8);
		*vdp2::regs::ccrsc = tables::kSine5bit[frame_count&127] | (tables::kSine5bit[(frame_count+64)&127] << 8);
		*vdp2::regs::ccrsd = tables::kSine5bit[frame_count&127] | (tables::kSine5bit[(frame_count+64)&127] << 8);
	}
}