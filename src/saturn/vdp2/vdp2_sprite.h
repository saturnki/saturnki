struct Sprite
{
	static int GetPriorityRegister(const int &priority_register_number, Registers &regs)
	{
		return regs.pris[priority_register_number];
	}
	static int GetColorCalculationRegister(Registers &regs, const int &cc)
	{
		return regs.ccrs[cc];
	}

	static bool PixelHasColorCalculationEnabled(const Registers &regs, const u32 &priority)
	{
		switch(regs.spctl.spcccs)
		{
		case 0: 
			if(priority <= regs.spctl.spccn)
				return true;
		case 1:
			if(priority == regs.spctl.spccn)
				return true;
		case 2:
			if(priority >= regs.spctl.spccn)
				return true;
		case 3:
			assert(false);
			//if(p. & 0x8000)
			//return true;
		}
	}


	struct SpriteData
	{
		u1 shadow;
		u3 priority;
		u3 color_calc_ratio;
		u11 color_data;
	};
	static SpriteData GetSpriteData(const u32 &pixel, const u8 &sprite_type)
	{
		SpriteData s;

		switch(sprite_type)
		{
		case 0:
			s.priority = pixel >> 14;
			s.color_calc_ratio = pixel >> 11;
			s.color_data = pixel;
			s.shadow = 0;
			break;
		case 1:
			s.priority = pixel >> 13;
			s.color_calc_ratio = (pixel >> 11) & 3;//only 2 bits
			s.color_data = pixel;
			s.shadow = 0;
			break;
		case 2:
			s.shadow = pixel >> 15;
			s.priority = (pixel >> 14) & 1;//1 bit
			s.color_calc_ratio = pixel >> 11;
			s.color_data = pixel;
			break;
		case 3:
			s.shadow = pixel >> 15;
			s.priority = (pixel >> 13) & 3;//2bit
			s.color_calc_ratio = (pixel >> 11) & 3;//2bit
			s.color_data = pixel;
			break;
		case 4:
			s.shadow = pixel >> 15;
			s.priority = (pixel >> 13) & 3;
			s.color_calc_ratio = pixel >> 10;
			s.color_data = pixel & 0x3FF;//10 bits
			break;
		case 5:
			s.shadow = pixel >> 15;
			s.priority = pixel >> 12;
			s.color_calc_ratio = (pixel >> 11) & 1;//1 bit
			s.color_data = pixel;
			break;
		case 6:
			s.shadow = pixel >> 15;
			s.priority = pixel >> 12;
			s.color_calc_ratio = (pixel >> 10) & 3;//2 bits
			s.color_data = pixel & 0x3FF;//10 bits
			break;
		case 7:
			s.shadow = pixel >> 15;
			s.priority = pixel >> 12;
			s.color_calc_ratio = pixel >> 9;
			s.color_data = pixel & 0x1FF;//9 bits
			break;
		default:
			assert(false);
			break;
		}

		return s;
	}
	static SpriteData GetSpriteData8(const u32 &pixel, const u8 &sprite_type)
	{
		SpriteData s = {0};
		switch(sprite_type)
		{
		case 8:
			s.priority = (pixel >> 7) & 1;
			s.color_data = pixel & 0x7F;
			break;
		case 9:
			s.priority = (pixel >> 7) & 1;
			s.color_calc_ratio = (pixel >> 6) & 1;
			s.color_data = pixel & 0x3F;
			break;
		case 0xa:
			s.priority = (pixel >> 6) & 3;
			s.color_data = pixel & 0x3F;
			break;
		case 0xb:
			s.color_calc_ratio = (pixel >> 6) & 3;
			s.color_data = pixel & 0x3F;
			break;
		case 0xc:
			//shared bits
			s.priority = (pixel >> 7) & 1;
			s.color_data = pixel & 0xFF;
			break;
		case 0xd:
			s.priority = (pixel >> 7) & 1;
			s.color_calc_ratio = (pixel >> 6) & 1;
			s.color_data = pixel & 0xFF;
			break;
		case 0xf:
			s.color_calc_ratio = (pixel >> 6) & 3;
			s.color_data = pixel & 0xFF;
			break;
		}
		return s;
	}
	struct SpritePixelResult
	{
		bool normal_shadow;
		bool msb_shadow;
		u3 color_calculation_ratio;
		u3 priority;
		bool transparent;
		u32 color;
		u16 raw_data;

		SpriteData sprite_data;

		SpritePixelResult() : 
		normal_shadow(false), msb_shadow(false), color_calculation_ratio(0), priority(0), transparent(false),color(0), raw_data(0) {}
	};

	SpritePixelResult DoRgbPixel(u16&raw_framebuffer_pixel, Vdp2 &vdp2)
	{
		SpritePixelResult s;

		u32 color = raw_framebuffer_pixel;

		color = vdp2.ConvertColorWordToLong(color);//fix this

		s.color = color;

		s.priority = vdp2.regs.pris[0];
		return s;
	}

	SpritePixelResult DoColorBankPixel(u16&raw_framebuffer_pixel, Vdp2 &vdp2)
	{
		SpritePixelResult s;

		SpriteData sprite_data = GetSpriteData(raw_framebuffer_pixel,vdp2.regs.spctl.sptype);
		s.sprite_data = sprite_data;

		u32 offset = vdp2.regs.color_ram_address_offset[kSprite] << 8;
		u32 color = vdp2.GetColorFromColorRam(sprite_data.color_data + offset, vdp2.regs.ramctl.crmd);

		if(vdp2.regs.ramctl.crmd < 2)
			color = vdp2.ConvertColorWordToLong(color);

		s.raw_data = raw_framebuffer_pixel;
		s.color = color;
		s.color_calculation_ratio = vdp2.regs.ccrs[sprite_data.color_calc_ratio];
		s.priority = vdp2.regs.pris[sprite_data.priority];
		s.normal_shadow = sprite_data.shadow;
		return s;
	}

	SpritePixelResult DoEightBitPixel(u8 raw_framebuffer_pixel,  Vdp2 &vdp2)
	{
		//8 bit framebuffer pixels

		SpriteData sprite_data = GetSpriteData8(raw_framebuffer_pixel,vdp2.regs.spctl.sptype);

		u32 color = vdp2.GetColorFromColorRam(sprite_data.color_data, vdp2.regs.ramctl.crmd);

	//	if(vdp2.regs.ramctl.crmd < 2)
			color = vdp2.ConvertColorWordToLong(color);

//		assert(false);
		SpritePixelResult s;
		s.color = color;//raw_framebuffer_pixel;
		s.priority = sprite_data.priority;
		s.color_calculation_ratio = sprite_data.color_calc_ratio;
		s.raw_data = raw_framebuffer_pixel;
		return s;
	}

	SpritePixelResult DoSixteenBitPixel(
		const u32 &x, const u32 &y, const u32&vdp1_framebuffer_width,
		Vdp1 &vdp1, Vdp2 &vdp2)
	{
		u32 address = (y * vdp1_framebuffer_width) + x;
		address*=2;
		u16 raw_framebuffer_pixel = vdp1.framebuffer.back.Read<kWord>(address);

		if(raw_framebuffer_pixel == 0)
		{
			SpritePixelResult s;
			s.transparent = true;
			return s;
		}

		if(raw_framebuffer_pixel & 0x8000 && vdp2.regs.spctl.spclmd)
		{
			return DoRgbPixel(raw_framebuffer_pixel,vdp2);
		}
		else
		{
			return DoColorBankPixel(raw_framebuffer_pixel,vdp2);
		}
	}

	SpritePixelResult GeneratePixel(u32 x, u32 y, Vdp2&vdp2, Vdp1 &vdp1)
	{
		if(
			vdp1.regs.tvmr.tvm == 0 ||
			vdp1.regs.tvmr.tvm == 2 ||
			vdp1.regs.tvmr.tvm == 4)
		{
			return DoSixteenBitPixel(x,y,vdp1.screen_mode.frame_buffer_width,vdp1,vdp2);
		}
		else
		{
			u16 address = (y * vdp1.screen_mode.frame_buffer_width) + x;
			u16 raw_framebuffer_pixel = vdp1.framebuffer.back.Read<kByte>(address);
			return DoEightBitPixel(raw_framebuffer_pixel,vdp2);
		}
	}
};