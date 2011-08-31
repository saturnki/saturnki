//this can probably be simpler but let's do it
//explicitly for now

static bool SpritePixelIsNormalShadow(const u32&sprite_type, const u32& pixel)
{
	switch(sprite_type)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 5:
		return (pixel == 0x7FE);
	case 4:
	case 6:
		return (pixel == 0x3FE);
	case 7:
		return (pixel == 0x1FE);
	case 0xc:
	case 0xd:
	case 0xf:
		return (pixel == 0xFE);
	case 0x8:
		return (pixel == 0x7E);
	case 0x9:
	case 0xa:
	case 0xb:
		return (pixel == 0x3E);
	default:
		assert(false);
		break;
	}
	assert(false);
	return false;
}

static bool SpritePixelIsSpriteShadow(const u16& pixel)
{
	return (pixel > 0x8000);
}

bool SpritePixelIsTransparentShadow(const u16&pixel)
{
	if(!regs.sdctl.tpsdl)
		return false;//transparent shadow is disabled

	return (pixel == 0x8000);
}

u32 Shadow(u32 pixel)
{
	s16 r = (pixel & 0xFF);
	s16 g = ((pixel >> 8) & 0xFF);
	s16 b = ((pixel >> 16) & 0xFF);

	r = r/2;
	g = g/2;
	b = b/2;

	pixel = (b << 16) | (g << 8) | r;

	return pixel;
}

#if 0
static u32 Shadow(const u32&i)
{
	Color c(i);

	//c -= 127;
	c.r -= 127;
	c.g -= 127;
	c.b -= 127;

	return c.Compose();
}
#endif
u32 DoShadow(const Sprite::SpritePixelResult &s, const u32 &top_layer_pixel, const u32 &second_layer_pixel)
{
//	if(s.raw_data & 0x8000)
//		top_layer_pixel = rand();

//	return;

	u32 result = 0;

	if(SpritePixelIsNormalShadow(regs.spctl.sptype,s.raw_data))
	{
		//blend the sprite into the background

		//uhh top_layer_pixel is a misnomer since sprite is on top.
		result = Shadow(s.color);
		return BlendNormal(result,second_layer_pixel,15);
	}

	if(s.raw_data > 0x8000)
	{
		return Shadow(s.color);//return s.color/2;
	}
	
	if(SpritePixelIsSpriteShadow(s.raw_data))
	{
		//darken the sprite
		return Shadow(s.color);
	}
	else if(SpritePixelIsTransparentShadow(s.raw_data))
	{
		//darken the background

//		int layer = 0; //fixme

		//if(regs.sdctl.enable[layer])
		{
			 return Shadow(second_layer_pixel);
		}
	}

	return s.color;
}