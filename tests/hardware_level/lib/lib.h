#include "types.h"

#include "sh2.h"
#include "util.h"
#include "vdp1.h"
#include "vdp2.h"
#include "simple.h"
#include "tables.h"

namespace io
{
	void Init()
	{
		*vdp2::regs::ramctl = vdp2::ColorRamMode::k5bit1024 << 12;

		for(int i = 0; i < 0x10; i++)
			vdp2::cram[i] = util::MakeColor16(i,i,i,1);

		//enable bitmap for layer 0
		//set 11 bit color
		//set 512x256 size
		*vdp2::regs::chctla = (vdp2::ColorCount::k16 << 4) | (vdp2::BitmapSize::k512x256 << 2) | (1 << 1);

		//map offset of 0
		*vdp2::regs::mpofn = 0;

		//turn on the screen
		*vdp2::regs::tvmd = 0x8000;
	}


#include "font4bit.h"

	//8x8 grid
	void Print4bit(int grid_x, int grid_y, u8 character)
	{
		vu32* vram = (vu32*) vdp2::kVramBaseAddress;

		character -= 0x20;//fix the invisible ascii char offset

		u32 screen_width_in_pixels = 512;
		u32 pitch = screen_width_in_pixels / 8;//not sure why i have to divide by 8

		u8 character_size_in_pixels = 8;

		for(int column = 0; column < 8; column++)
		{
			//get the 32bit value representing this column of the character
			u32 data = font4bit[(character * character_size_in_pixels) + column];
			u32 offset = ((column + (grid_y*character_size_in_pixels)) * pitch) + grid_x;
			vram[offset] = data;
		}
	}
	const char hex_table[] = {"0123456789ABCDEF"};

	void PrintByte(int x, int y, u8 value)
	{
		Print4bit(x,y,hex_table[(value >> 4) & 0xF]);
		Print4bit(x+1,y,hex_table[value & 0xF]);
	}

	void PrintString4bit(int grid_x, int grid_y, char* string)
	{
		for(int i = 0; string[i] != 0; i++)
			Print4bit(grid_x + i, grid_y, string[i]);
	}
}