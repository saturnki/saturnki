#include "lib/lib.h"

u32 MakePatternNameDataEntry32(
	u16 character_number,
	u8 palette_number,
	bool special_color_calculation,
	bool special_priority,
	bool horizontal_flip,
	bool vertical_flip)
{
	return 
		  (vertical_flip << 31)
		| (horizontal_flip << 30)
		| (special_priority << 29)
		| (special_color_calculation << 28)
		| (palette_number << 16)
		| character_number;
}

void WritePatternNameDataEntry(int x, int y,
	u32 base_address,
	u32 tile_address,
	u32 name,
	u32 palette_number)
{
	vu32* p = (vu32*)(vdp2::kVramBaseAddress + base_address);
	p[y*32+x] = 
		MakePatternNameDataEntry32(
		(tile_address >> 5) | (name << 2),
		palette_number,
		0,0,0,0);
}

void WriteNameTables(u32 tile_address)
{
	for(int y = 0; y < 32; y++)
	{
		for(int x = 0; x < 32; x++)
		{
			WritePatternNameDataEntry(
				x,
				y,
				0,
				tile_address,
				0,
				0);
		}
	}
}

void LoadTiles(u32 tile_address)
{
    u8 chars[] = {'0','1','2','3'};
	vu32 *p = (vu32 *)(vdp2::kVramBaseAddress+tile_address);

	int i,y;
    for(i = 0; i < 4; i++)
    {
		for(y = 0; y < 8; y++)
		{
			u32 b = io::font4bit[((chars[i] - 0x20) * 8) + y];
			p[(i * 8) + y] = b;
		}
	}
}

extern "C" void _main()
{

	//make nbg0 visible
	*vdp2::regs::prina = 0x0007;

	//turn nbg0 on
	*vdp2::regs::bgon = 0x0001;

	//16x16 tiles (character size is 2x2 cells)
	*vdp2::regs::chctla = 0x0001;

	//plane contains 1x1 pages
	*vdp2::regs::plsz = 0x0000;

	//pattern name data is 2 words
	*vdp2::regs::pncn0 = 0x0000;

	//no zooming
	*vdp2::regs::zmctl = 0;
	*vdp2::regs::zmxin0 = 1;
	*vdp2::regs::zmxin1 = 1;

	*vdp2::regs::mpofn = 0;
	*vdp2::regs::mpabn0 = 0;
	*vdp2::regs::mpcdn0 = 0;

	*vdp2::regs::tvmd = 0x8010;

	//4 banks
	*vdp2::regs::ramctl = 0x0300;

	//we store tile pixel data here
	u32 tile_address = 0x40000;

	WriteNameTables(tile_address);

	int frame_count = 0;

	LoadTiles(tile_address);

	vu32* CYCA0 = (vu32*) 0x25F80010;
	vu32* CYCA1 = (vu32*) 0x25F80014;
	vu32* CYCB0 = (vu32*) 0x25F80018;
	vu32* CYCB1 = (vu32*) 0x25F8001C;

    *CYCA0  = 0x0123FFFF;
    *CYCA1  = 0xCDFFFFFF;
    *CYCB0  = 0x4567FFFF;
    *CYCB1  = 0xFFFFFFFF;

	for(int i = 0; i < 32; i++)
		vdp2::cram[i] = util::MakeColor16(i,i,0,1);

	while(1)
	{
		simple::DoNothing();

		frame_count++;

		*vdp2::regs::scxin0 = frame_count;

	}
}