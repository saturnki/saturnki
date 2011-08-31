#include "lib/lib.h"

namespace vdp2
{
	const u32 kVramA0 = 0x25e00000;
	const u32 kVramA1 = 0x25e20000;
	const u32 kVramB0 = 0x25e40000;
	const u32 kVramB1 = 0x25e60000;
}

const u32 rotation_map_address = 0;
const u32 bitmap_address = 0x2000;
const u32 rotation_parameter_table_address = 0x1000;

struct Matrix
{
	u32 d[3][3];

	void RotateX(const u32 &angle)
	{
		u32 sin = retry::SinFixed(angle * 65536);
		u32 cos = retry::CosFixed(angle * 65536);

		d[0][0] = 1 * 65536;
		d[0][1] = 0;
		d[0][2] = 0;
		d[1][0] = 0;
		d[1][1] = cos;
		d[1][2] = sin;
		//d[2][0] = 0;
		//d[2][1] = -sin;
		//d[2][2] = cos;
	}

	void RotateY(const u32 &angle)
	{
		u32 sin = retry::SinFixed(angle * 65536);
		u32 cos = retry::CosFixed(angle * 65536);

		d[0][0] = cos;
		d[0][1] = 0;
		d[0][2] = -sin;
		d[1][0] = 0;
		d[1][1] = 1 * 65536;
		d[1][2] = 0;
		//d[2][0] = sin;
		//d[2][1] = 0;
		//d[2][2] = cos;
	}

	void RotateZ(const u32 &angle)
	{
		u32 sin = retry::SinFixed(angle * 65536);
		u32 cos = retry::CosFixed(angle * 65536);

		d[0][0] = cos;
		d[0][1] = sin;
		d[0][2] = 0;
		d[1][0] = -sin;
		d[1][1] = cos;
		d[1][2] = 0;
		//d[2][0] = 0;
		//d[2][1] = 0;
		//d[2][2] = 1 * 65536;
	}
};

Matrix m;


void SetFromMatrix(rotation::Table &t, Matrix &m)
{
	t.A = m.d[0][0];
	t.B = m.d[1][0];
	t.C = m.d[2][0];
	t.D = m.d[0][1];
	t.E = m.d[1][1];
	t.F = m.d[2][1];
}


extern "C" void _main()
{
	*vdp2::regs::tvmd = 0x8100;

	*vdp2::regs::tvstat = 0x2;

	*vdp2::regs::ramctl = 0x327;

	*vdp2::regs::cyca0l = 0xfeee;
	*vdp2::regs::cyca0u = 0xeeee;

	*vdp2::regs::cyca1l = 0xfeee;
	*vdp2::regs::cyca1u = 0xeeee;

	*vdp2::regs::cycb0l = 0xffff;
	*vdp2::regs::cycb0u = 0xeeee;

	*vdp2::regs::cycb1l = 0x044f;
	*vdp2::regs::cycb1u = 0xeeee;

	//enable rbg0
	*vdp2::regs::bgon = 0x10;

	*vdp2::regs::chctla = 0x1010;

	//16 color, bitmap, size 512x256
	*vdp2::regs::chctlb = (1 << 9);

	//no offset
	*vdp2::regs::mpofr = 0x44;

	//map offset shit

	*vdp2::regs::rpmd = 3;

	*vdp2::regs::ktctl = 0;//0x6161;

	*vdp2::regs::rptau = 0x12f1;
	*vdp2::regs::rptal = 0xff00;

	*vdp2::regs::prir = 7;

	*vdp2::regs::wctld = 1 << 1;

	*vdp2::regs::wpsx0 = 64;
	*vdp2::regs::wpsy0 = 64;
	*vdp2::regs::wpex0 = 290 << 1;
	*vdp2::regs::wpey0 = 160;

	int frame_count = 0;

	for(int i = 0; i < 20; i+=2)
	{
		io::PrintString4bit(0,i,
			"<====  4bpp 512x256 Rbg0 Bitmap 4bpp 512x256 Rbg0 Bitmap   ====>",0);
		io::PrintString4bit(0,i+1,
			"                                                                ",0);
	}
	while(1)
	{
		simple::DoNothing();
		frame_count++;

		{
			rotation::Table t = {};

			retry::RotateTable(t,0);//this initializes some stuff that is necessary

			m.RotateZ(frame_count%360);

			u32 shift = 0;

			SetFromMatrix(t,m);

			u32 addr = vdp2::kVramBaseAddress + 0x3fe80;

			t.Mx = frame_count * 0x10000;
			t.My = frame_count * 0x10000;
			rotation::WriteTable(t,addr);

		}

		rotation::Table t = {};

		retry::RotateTable(t,0);

		m.RotateZ(360-(frame_count%360));
		SetFromMatrix(t,m);

		u32 addr = vdp2::kVramBaseAddress + 0x3fe00;
		rotation::WriteTable(t,addr);
	}
}