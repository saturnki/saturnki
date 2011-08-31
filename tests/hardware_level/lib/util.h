namespace util
{
	u16 MakeColor16(u8 r, u8 g, u8 b, u8 msb)
	{
		return (
			(r & 0x1F)
			| ((g & 0x1F) << 5)
			| ((b & 0x1F) << 10)
			|  (msb << 15));
	}

	u32 MakeColor32(u8 r, u8 g, u8 b, u8 msb)
	{
		return (
			r 
			| (g << 8)
			| (b << 16)
			| 0x80000000);//(msb << 32)
	}

	struct Points
	{
		u16 xa,ya,xb,yb,xc,yc,xd,yd;
	};

	//a b
	//d c
	struct Points GetPoints(u16 upper_left_x, u16 upper_left_y, u16 width, u16 height)
	{
		struct Points p = {};

		p.xa = upper_left_x;
		p.ya = upper_left_y;

		p.xb = upper_left_x + width;
		p.yb = upper_left_y;

		p.xc = upper_left_x + width;
		p.yc = upper_left_y + height;

		p.xd = upper_left_x;
		p.yd = upper_left_y + height;

		return p;
	}

}