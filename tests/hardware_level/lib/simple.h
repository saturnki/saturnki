
namespace simple
{
	template <typename T, int color_depth>
	void WriteRgbBitmapPixel(T* address, u16 x, u16 y, u8 r, u8 g, u8 b)
	{
		if(color_depth == 16)
			address[(y * 512) + x] = util::MakeColor16(r,g,b,1);
		else
			address[(y * 512) + x] = util::MakeColor32(r,g,b,1);
	}

	template <typename T, int bits>
	void WriteRgbBitmapTestPattern()
	{
		int r = 0, b = 0;
		int g = 0;

		T *vram_ptr = (T*) 0x5e00000;

		int width = 320;
		int height = 240;

		for (int y = 0 ; y < height; y++)
		{
			g = 0;

			for (int x = 0; x < width; x++)
			{
				r = y;
				if(x&3)
					g++;
				b = 224 - y;
				WriteRgbBitmapPixel<T,bits>(vram_ptr, x, y, r, g, b);
			}
		}
	}

	void MakeDefaultGouraudTables(u32 starting_address)
	{
		vu16* p = (vu16 *)(0x25C00000 + starting_address);

		u16 lime   = util::MakeColor16(0x9f,0xee,0x00,1);
		u16 purple = util::MakeColor16(0x71,0x09,0xaa,1);
		u16 yellow = util::MakeColor16(0xff,0xff,0x00,1);
		u16 maroon = util::MakeColor16(0xcd,0x00,0x74,1);

		//tetradic yellow lime maroon purple
		p[0] = yellow;
		p[1] = lime;
		p[2] = maroon;
		p[3] = purple;

		u16 green  = util::MakeColor16(0x00,0xcc,0x00,1);
		u16 red    = util::MakeColor16(0xff,0x00,0x00,1);
		u16 orange = util::MakeColor16(0xff,0x74,0x00,1);
		u16 teal   = util::MakeColor16(0x00,0x99,0x99,1);

		//tetradic red orange teal green
		p[4] = green;
		p[5] = teal;
		p[6] = orange;
		p[7] = red;
	}

	void Vdp1Setup()
	{
		vdp1::Clear();
		vdp2::Clear();

		*vdp1::regs::tvmr = 0x0000;
		*vdp1::regs::fbcr = 0x0000;
		*vdp1::regs::ptmr = 0x0002;
		*vdp1::regs::ewdr = util::MakeColor16(7,7,7,1);    //gray

		//0,0 -> 352,240
		*vdp1::regs::ewlr = 0x0000;    
		*vdp1::regs::ewrr = 0x50DF;

		*vdp2::regs::prisa = 0x0101;
		*vdp2::regs::spctl = 1 << 5;

		vdp1::command::ResetList();
	}

	void Vdp1ListStart()
	{
		vdp1::command::ResetList();
		vdp1::command::SystemClipping(320,224);
		vdp1::command::UserClipping(0,0,320,224);
		vdp1::command::LocalCoordinate(0,0);
	}

	void DoNothing()
	{
		vdp2::WaitForVblank();
		simple::Vdp1ListStart();
		vdp1::command::DrawEnd();
	}
}
