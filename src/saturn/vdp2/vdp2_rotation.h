typedef fixed::FixedPoint<s32,16> Fixed32;

struct RotationParameters
{
	Fixed32 Xst, Yst, Zst;//screen start coordinates
	Fixed32 dXst, dYst;//screen vertical coordinate increments
	Fixed32 dX,dY;//screeen horizontal coordinate increments
	Fixed32 A,B,C,D,E,F;//rotation matrix parameter
	Fixed32 Px,Py,Pz;//viewpoint coordinates
	Fixed32 Cx,Cy,Cz;//center coordinates
	Fixed32 Mx,My;//amount of horizontal shift
	Fixed32 kx,ky;//scaling coefficients

	Fixed32 KAst;//coefficient table start address
	Fixed32 dKAst; //coefficient table vertical address increment
	Fixed32 dKAx;//coefficient table horizontal address increment
};


struct RotationResult
{
	u32 x,y;
};

//i overcalculate here, some of this can be done per line instead of per pixel
RotationResult RotationGetDisplayCoordinates(RotationParameters &r, const u32 &Hcnt, const u32 &Vcnt, const u32 &x_mask, const u32 &y_mask,const bool &coefficient_enabled)
{
	Fixed32 Vcnt_f;
	Vcnt_f.IntToFixed(Vcnt);

	Fixed32 Hcnt_f;
	Hcnt_f.IntToFixed(Hcnt);	

	Fixed32 Xsp = 
		r.A * ((r.Xst + r.dXst * Vcnt_f) - r.Px) + 
		r.B * ((r.Yst + r.dYst * Vcnt_f) - r.Py) + 
		r.C * (r.Zst - r.Pz);

	Fixed32 Ysp = 
		r.D * ((r.Xst + r.dXst * Vcnt_f) - r.Px) + 
		r.E * ((r.Yst + r.dYst * Vcnt_f) - r.Py) +
		r.F * (r.Zst - r.Pz);

	Fixed32 Xp =
		r.A * (r.Px - r.Cx) +
		r.B * (r.Py - r.Cy) +
		r.C * (r.Pz - r.Cz) +
		r.Cx + r.Mx;

	Fixed32 Yp =
		r.D * (r.Px - r.Cx) +
		r.E * (r.Py - r.Cy) +
		r.F * (r.Pz - r.Cz) +
		r.Cy + r.My;

	Fixed32 dX = r.A * r.dX + r.B * r.dY;
	Fixed32 dY = r.D * r.dX + r.E * r.dY;

	RotationResult result;

	if(!coefficient_enabled)
	{
		result.x = ((Xsp + dX * Hcnt_f) + Xp).ToInt() & x_mask;
		result.y = ((Ysp + dY * Hcnt_f) + Yp).ToInt() & y_mask;
	}
	else
	{
		result.x = (r.kx * (Xsp + dX * Hcnt_f) + Xp).ToInt() & x_mask;
		result.y = (r.ky * (Ysp + dY * Hcnt_f) + Yp).ToInt() & y_mask;
	}

	return result;
}

u32 ArbitrarySignExtend(u32 input, u32 sign_bit_number)
{
	if(input & (1 << sign_bit_number))
		return input | (0xFFFFFFFF << (sign_bit_number+1));
	else
		return input;
}
//convert everything to the same fixed point format
template <u32 sign_bit_number,u32 mask>
Fixed32 ReadAndUnifyFixedLong(u32 &address)
{
	u32 result = ReadIncremented<kLong>(address);

	u32 sign_bit_number_adjust = sign_bit_number + 16;

	result &= mask;//note that the stuff above the sign bits is masked off

	result = ArbitrarySignExtend(result,sign_bit_number_adjust);

	return Fixed32(result);
}
template <OperationSize size>
u32 ReadIncremented(u32 &address)
{
	u32 result = vdp2_ram.Read<size>(address);
	address+=size;
	return result;
}

Fixed32 GetViewpointOrCenterCoordinate(u32 &address)
{
	u32 result = ReadIncremented<kWord>(address);
	Fixed32 f;
	return f.IntToFixed(ArbitrarySignExtend(result & 0x3FFF,13));
}

//read the table and sign extend everything
RotationParameters RotationReadTable(u32 address)
{
	RotationParameters r;
	r.Xst = ReadAndUnifyFixedLong<12,0x1FFFFFC0>(address);
	r.Yst = ReadAndUnifyFixedLong<12,0x1FFFFFC0>(address);
	r.Zst = ReadAndUnifyFixedLong<12,0x1FFFFFC0>(address);

	r.dXst = ReadAndUnifyFixedLong<2,0x0007FFC0>(address);
	r.dYst = ReadAndUnifyFixedLong<2,0x0007FFC0>(address);

	r.dX = ReadAndUnifyFixedLong<2,0x0007FFC0>(address);
	r.dY = ReadAndUnifyFixedLong<2,0x0007FFC0>(address);

	r.A = ReadAndUnifyFixedLong<3,0x000FFFC0>(address);
	r.B = ReadAndUnifyFixedLong<3,0x000FFFC0>(address);
	r.C = ReadAndUnifyFixedLong<3,0x000FFFC0>(address);
	r.D = ReadAndUnifyFixedLong<3,0x000FFFC0>(address);
	r.E = ReadAndUnifyFixedLong<3,0x000FFFC0>(address);
	r.F = ReadAndUnifyFixedLong<3,0x000FFFC0>(address);

	//viewpoint coordinates are only
	//integer part + sign
	r.Px = GetViewpointOrCenterCoordinate(address);
	r.Py = GetViewpointOrCenterCoordinate(address);
	r.Pz = GetViewpointOrCenterCoordinate(address);

	address+=2;//blank spot here

	r.Cx = GetViewpointOrCenterCoordinate(address);
	r.Cy = GetViewpointOrCenterCoordinate(address);
	r.Cz = GetViewpointOrCenterCoordinate(address);

	address+=2;//blank spot here

	r.Mx = ReadAndUnifyFixedLong<13,0x3FFFFFC0>(address);
	r.My = ReadAndUnifyFixedLong<13,0x3FFFFFC0>(address);

	r.kx = ReadAndUnifyFixedLong<7,0x00FFFFFF>(address);
	r.ky = ReadAndUnifyFixedLong<7,0x00FFFFFF>(address);

	//i'm filling this in regardless of
	//whether we are using coefficient tables or not
	r.KAst.data = ReadIncremented<kLong>(address) & 0xFFFFFFC0;
	r.dKAst = ReadAndUnifyFixedLong<9,0x03FFFFC0>(address);
	r.dKAx = ReadAndUnifyFixedLong<9,0x03FFFFC0>(address);

	return r;
}

void ReadRotationParameterTables(Registers &regs)
{
	u32 address = 32432 << 1;//regs.rpta << 1;

	RotationParameters a = RotationReadTable(address & 0x000FFF7C);
	RotationParameters b = RotationReadTable((address & 0x000FFFFC) | (1<<7));
}

//for modes 0,1,2
struct CoefficientData
{
	//note that different modes
	//have different data formats
	u1 is_transparent;
	u7 line_color_screen_data;
	Fixed32 data;
};

//modes 0,1,2
template <RotationParameter parameter>
CoefficientData ReadCoefficientData(u32 address)
{
	CoefficientData c;

	if(regs.coefficient_table_control[parameter].data_size == 1)// 1 word size
	{
		u32 data = vdp2_ram.Read<kWord>(address & 0x7fffe);
		c.is_transparent = data >> 15;
		c.line_color_screen_data = 0xdeadbeef;
		c.data = ArbitrarySignExtend((data & 0x7FFF),14) * 64;
	}
	else//long size
	{
		u32 data = vdp2_ram.Read<kLong>(address & 0x7fffc);
		c.is_transparent = data >> 31;
		c.line_color_screen_data = data >> 24;
		c.data = ArbitrarySignExtend(data & 0x00FFFFFF,23);
	}
	return c;
}

template <RotationParameter parameter>
CoefficientData ReadCoefficientDataForMode(RotationParameters &r, u32 mode, u32 address)
{
	//u32 address = GetCoefficientTableLeadAddress<parameter>(r.KAst);
	CoefficientData c = ReadCoefficientData<parameter>(address);
	switch(mode)
	{
	case 0://used as scale coefficient kx and ky
		r.kx = r.ky = c.data;
		break;
	case 1://used as scale coefficient kx
		r.kx = c.data;
		break;
	case 2://used as scale coefficient ky
		r.ky = c.data;
		break;
	case 4://used as viewpoint coordinate Xp after rotation conversion
		assert(false);
		break;
	}

	rbg0.debugging_storage.kx = r.kx;
	rbg0.debugging_storage.ky = r.ky;

	return c;
}
template <RotationParameter parameter>
u32 GetCoefficientTableLeadAddress( Fixed32 &KAst)
{
	u32 value = regs.coefficient_table_address_offset[parameter];

	u32 table_data_size_in_bytes;

	if(regs.coefficient_table_control[parameter].data_size == 1)
		table_data_size_in_bytes = 2;
	else
		table_data_size_in_bytes = 4;

	rbg0.debugging_storage.coefdatasize = table_data_size_in_bytes;

	u16 KAst_ = KAst.ToInt();//chop off the bits
	return value * 0x10000 + KAst_ * table_data_size_in_bytes;
}

template <RotationParameter parameter>
u32 GetRotationTableLeadAddress()
{
	u32 address = (regs.rptau << 16) | (regs.rptal & 0xFFFE);//mask here is not necessary

	address <<= 1;

	//rpta6 bit is ignored
	//set to 0 for A and 1 for B
	if(parameter == kParameterA)
		address &= 0x000FFF7C;
	else
		address = (address & 0x000FFFFC) | 0x00000080;

	return address;

}