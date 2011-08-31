struct Color
{
	u16 r,g,b;

	void Test()
	{
		Color x(128,128,128);
		Color y(0x00808080);

		Color z = x + y;

		Color q(128,128,128);
		Color r(0x00808080);

#if 0
		q += r;

		Color s(128,128,128);

		s -= 0x80;
#endif


	}

	Color(const u32 &i)
	{
		r = i & 0xFF;
		g = (i >> 8) & 0xFF;
		b = (i >> 16) & 0xFF;
	}

	Color(const u16 &_r,const u16 &_g,const u16 &_b)
	{
		r = _r & 0xff;
		g = _g & 0xff;
		b = _b & 0xff;
	}

	u32 Compose()
	{
		return (b << 16) | (g << 8) | r;
	}

	void ClampHigh(u16 & _r, u16 &_g, u16 &_b)
	{
		if(_r > 255)
			_r = 255;
		if(_g > 255)
			_g = 255;
		if(_b > 255)
			_g = 255;
	}

	void ClampLow(u16 & _r, u16 &_g, u16 &_b)
	{
		if(_r < 0)
			_r = 0;
		if(_g < 0)
			_g = 0;
		if(_b < 0)
			_b = 0;
	}
#if 0
	Color operator += (const Color &c)
	{
		r += c.r;
		g += c.g;
		b += c.b;

		ClampHigh(r,g,b);
	}

	Color operator -= (const u32 &i)
	{
		r -= i;
		g -= i;
		b -= i;

		ClampLow(r,g,b);
	}
#endif
	Color operator * (const u32 &i)
	{
		u16 _r = r * i;
		u16 _g = g * i;
		u16 _b = b * i;

	//	_r >>= 5;
	//	_g >>= 5;
	//	_b >>= 5;

		ClampHigh(_r,_g,_b);

		Color c(_r,_g,_b);
		

		return c;
	}
	Color operator + (const Color &i)
	{
		u16 _r = r + i.r;
		u16 _g = g + i.g;
		u16 _b = b + i.b;

		if(_r > 255)
			_r = 255;
		if(_g > 255)
			_g = 255;
		if(_b > 255)
			_g = 255;

		Color c(_r,_g,_b);
	//	c.ClampHigh();

		return c;
	}
	Color operator >> (const u32 &i)
	{
		u8 _r = r >> i;
		u8 _g = g >> i;
		u8 _b = b >> i;

		Color c(_r,_g,_b);
		return c;
	}
};

u32 BlendNormal(
	const u32&first_pixel,
	const u32&second_pixel,
	const u32&color_calculation_ratio)
{
	u8 top_ratio = 31 - color_calculation_ratio;
	u8 bottom_ratio = color_calculation_ratio;

	Color top(first_pixel);
	Color bottom(second_pixel);

	top.r = top.r * top_ratio >> 5;
	top.g = top.g * top_ratio >> 5;
	top.b = top.b * top_ratio >> 5;

	bottom.r = bottom.r * bottom_ratio >> 5;
	bottom.g = bottom.g * bottom_ratio >> 5;
	bottom.b = bottom.b * bottom_ratio >> 5;

//	top = (top * top_ratio);
//	bottom = (bottom * bottom_ratio);

	return (top + bottom).Compose();
}

u32 BlendAdditive
	(
	const u32&first_pixel,
	const u32&second_pixel)
{
	Color c1(first_pixel);
	Color c2(second_pixel);
#if 0
	c1.r += c2.r;
	c1.g += c2.g;
	c1.b += c2.b;

	c1.ClampHigh(c1.r,c1.g,c1.b);

	return c1.Compose();
#endif
	return (c1 + c2).Compose();
}

u32 ColorCalculate(
	const u32&first_pixel,
	const u32&second_pixel,
	const u32&color_calculation_ratio)
{
	if(regs.color_calculation_control.mode)
		return BlendAdditive(first_pixel,second_pixel);
	else
		return BlendNormal(first_pixel,second_pixel,color_calculation_ratio);
}

#if 0



u32 ExtendedColorCalculation(Images i, Ratios r)
{
	u32 second_to_third_ratio;
	if(r.third == 1)
		second_to_third_ratio = 23;//2:1
	else
		second_to_third_ratio = 15;//2:2

	u32 color = ColorCalculate(i.second,i.third,second_to_third_ratio);

	//only one situation where this happens
	if(r.third)
		color = ColorCalculate(i.third,i.fourth,15);

	return color;
}

void DoColorMode0()
{
	if(!line_color_screen_inserts)
	{
		if(second_image.color_calculation_enabled)
		{
			//2:2:0
			second_image.result_pixel = 
				ExtendedColorCalculation(
				i,r);
		}
	}
	else
	{
		if(second_image.color_calculation_enabled)
		{
			if(!third_image.color_calculation_enabled)
			{
				//2:2:0
				second_image.result_pixel = 
					ExtendedColorCalculation(
					i,r);
			}
			else
			{
				//2:1:0
				r.third = 1;
				second_image.result_pixel = 
					ExtendedColorCalculation(
					i,r);
			}
		}
	}
}

void DoColorModes23()
{
	if(!line_color_screen_inserts)
	{
		if(third_image.is_palette_format)
		{
			//4:0:0
		}
		else
		{
			if(!second_image.color_calculation_enabled)
			{
				//4:0:0
			}
			else
			{
				//2:2:0
				second_image.result_pixel = 
					ExtendedColorCalculation(
					i,r);
			}
		}
	}
	else
	{
		if(second_image.is_palette_format)
		{
			//4:0:0
		}
		else
		{
			if(third_image.is_palette_format)
			{
				if(!second_image.color_calculation_enabled)
				{
					//4:0:0
				}
				else
				{
					//2:2:0
					second_image.result_pixel = 
						ExtendedColorCalculation(
						i,r);
				}
			}
			else
			{
				if(!second_image.color_calculation_enabled)
				{
					//4:0:0
				}
				else
				{
					if(!third_image.color_calculation_enabled)
					{
						//2:2:0
						second_image.result_pixel = 
							ExtendedColorCalculation(
							i,r);
					}
					else
					{
						//2:1:1
						r.third = 1;
						r.fourth = 1;
						second_image.result_pixel = 
							ExtendedColorCalculation(
							i,r);
					}
				}
			}
		}
	}
}

void DoExtendedColorCalculation()
{
	Ratios r;

	//this is the most common case so let's set it
	r.second = 2;
	r.third = 2;
	r.fourth = 0;
	if(color_ram_mode == 0)
	{
		DoColorMode0(i,r);
	}
	else
	{
		DoColorModes23(i,r);
	}
}
#endif

//aka blurring the background
u32 DoGradationCalculation(u32 current, u32 previous, u32 previous_previous)
{
	//we blend the current layer into itself

	//i guess the vdp2 must latch the last 2 generated pixels for each given
	//layer or something so that this is possible?

	u32 current_pixel_minus_2 = previous_previous;
	u32 current_pixel_minus_1 = previous;
	u32 current_pixel = current;

	//we blend 1:1:2
	u32 intermediate_result = BlendNormal(
		current_pixel_minus_2,
		current_pixel_minus_1,
		7);

	u32 result = BlendNormal(
		current_pixel,
		intermediate_result,
		15);

	return result;
}

static bool PixelIsSpecialFunctionCode(
	u8 special_function_code_register,
	u32 pixel)
{
	//lower 4 bits only
	pixel &=0xf;

	//a or b
	switch(special_function_code_register)
	{
	case 0:
		if(pixel == 0 || pixel == 1)
			return true;
		break;
	case 2:
		if(pixel == 2 || pixel == 3)
			return true;
		break;
	case 4:
		if(pixel == 4 || pixel == 5)
			return true;
		break;
	case 8:
		if(pixel == 6 || pixel == 7)
			return true;
		break;
	case 16:
		if(pixel == 8 || pixel == 9)
			return true;
		break;
	case 32:
		if(pixel == 0xa || pixel == 0xb)
			return true;
		break;
	case 64:
		if(pixel == 0xc || pixel == 0xd)
			return true;
		break;
	case 128:
		if(pixel == 0xe || pixel == 0xf)
			return true;
		break;
	default:
		assert(false);
		break;
	}

	return false;
}
#if 0

#endif

u8 GetSpecialFunctionCodeRegister(u8 layer)
{
	//are we using a or b?
	u8 a_or_b = regs.sfsel[layer];//1 or 0
	//get the code
	return regs.sfcode.sfcd[a_or_b];
}

//doing the special color calcing and all that
bool ColorCalculationIsEnabled(u8 layer,u32 special_color_calculation,u32 raw_dot, u32 pixel_color,Sprite::SpritePixelResult sp)
{
//	return true;
	//return regs.color_calculation_control.enable[layer];

	if(!regs.color_calculation_control.enable[layer])
		return false;

	if(layer == kSprite)
	{
		switch(regs.spctl.spcccs)
		{
		case 0:
			return (sp.priority <= regs.spctl.spccn);
		case 1:
			return (sp.priority == regs.spctl.spccn);
		case 2:
			return (sp.priority >= regs.spctl.spccn);
		case 3:
			break;
		}
	}

	switch(regs.special_color_calculation_mode[layer])
	{
	case 0:
		return regs.color_calculation_control.enable[layer];
		break;
#if 1
	case 1://cc if special color calculation bit in pattern name data is set
		if(special_color_calculation)
			return true;
		break;
	case 2:
		{
		//pattern name data bit must be set
	//	if(!special_color_calculation)
	//		return false;

		if(regs.chctl[layer].bmen)
		{
			//warning probable corruption here if it's the wrong layer
			if(regs.bmpn[layer].bmcc)
			{
			}
			else
				return false;
		}
		else
		{
			if(!special_color_calculation)
				return false;
		}

		//now check the dot for the special function code
		u32 special_function_code_register = GetSpecialFunctionCodeRegister(layer);

	//	printf("spec: %x\n",spec);
		if(PixelIsSpecialFunctionCode(special_function_code_register,raw_dot))
			return true;
		break;
		}
		//rgb format invalid...
	case 3:
		//this actually does use the color ram lookup apparently
		//so we previously converted to 24 bit
		
		//what probably ought to happen is that
		//we take the original color fetch and use that without the conversion
		//also if it's rgb it's always on

		return pixel_color & (0x8000 << 9);//1000000, shouldn't it be 800000? makes no sense 
		break;
#endif
	}
	return false;
	assert(false);
	
}