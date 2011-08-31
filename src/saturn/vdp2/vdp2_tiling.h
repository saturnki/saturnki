//#define VDP2_CACHING



static PatternNameData GetPatternNameDataWordSize(
	const u32 &character_color_count,
	const u32 &character_number_supplement_mode,
	const u32 &character_size,
	const u16 &data,
	const Registers::PatternNameDataReg &r)
{
	PatternNameData p;

	if(character_color_count == 16)
		p.palette_number = (data >> 12) & 0xF | r.supplementary_palette_number << 4;
	else
		p.palette_number = ((data >> 12) & 7) << 4;

	p.special_priority = r.special_priority;
	p.special_color_calculation = r.special_color_calculation;

	if(character_number_supplement_mode == 0)
		p.reverse_function = data >> 10;
	else
		p.reverse_function = 0;

	if(character_size == 1)
	{
		if(character_number_supplement_mode == 0)
			p.character_number = (data & 0x1ff) | (r.supplementary_character_number << 10);
		else
			p.character_number = (data & 0x7ff) | (r.supplementary_character_number << 10);
	}
	else
	{
		if(character_number_supplement_mode == 0)
			p.character_number = ((data & 0x3ff) << 2) | (r.supplementary_character_number & 3) | (r.supplementary_character_number >> 2) << 12;
		else
			p.character_number = ((data & 0x7ff) << 2) | (r.supplementary_character_number & 3) | (r.supplementary_character_number >> 4) << 14;
	}
	return p;
}

static PatternNameData GetPatternNameDataLongSize(const u16 &data1, const u16 &data2)
{
	PatternNameData p;
	p.character_number = data2;
	p.reverse_function = data1 >> 14;
	p.palette_number = data1;
	p.special_priority = data1 >> 13;
	p.special_color_calculation = data1 >> 12;
	return p;
}


PatternNameData GetPatternNameData(
	const u32 &address,
	const u32 &character_color_count, 
	const u32 &character_number_supplement_mode,
	const u32 &character_size,
	const u32 &pattern_name_data_size
	)
{
	PatternNameData p;

	if(pattern_name_data_size == 1)//1 word
	{
		const u16 data = vdp2_ram.Read<kWord>(address);

		p = GetPatternNameDataWordSize(
			character_color_count,
			character_number_supplement_mode,
			character_size,
			data,
			regs.pncn[layer]);
	}
	//2 words
	else
	{
		const u16 data1 = vdp2_ram.Read<kWord>(address);
		const u16 data2 = vdp2_ram.Read<kWord>(address+2);
		p = GetPatternNameDataLongSize(data1,data2);
	}

	//todo is this the correct spot for this?
	//what about bitmaps?
	if(!regs.vrsize.vramsz)
		p.character_number &= 0x3fff;

	return p;
}

struct PlaneSize
{
	u8 horizontal_pages;
	u8 vertical_pages;
};

static PlaneSize GetNumberOfPatternNameTablesInPlane(const u8 &plsz)
{
	PlaneSize p;
	switch(plsz)
	{
	case 0:
		p.horizontal_pages = p.vertical_pages = 1;
		break;
	case 1:
		p.horizontal_pages = 1;
		p.vertical_pages = 2;
		break;
	case 2:
		assert(false);
		break;
	case 3:
		p.horizontal_pages = p.vertical_pages = 2;
		break;
	}
	return p;
}

u32 GetMapRegisterValueForCurrentPlane(const u32 &plane_number, const u8 &which_parameter)
{
	u32 map_selection;

	if(layer != kRbg0)
		map_selection = regs.map.background[layer].plane[plane_number];
	else
		map_selection = regs.map.rotation[which_parameter].plane[plane_number];

	u32 map_offset;

	if(layer == kRbg0 || layer == kRbg1)
		map_offset = regs.map_offset_parameter[which_parameter];
	else
		map_offset = regs.map_offset[layer];

	return map_offset << 6 | map_selection;
}
#ifdef VDP2_CACHING
u32 plane_address_cache[0x200][3][3][3][3];

/*
so, how the cache works.
todo reduce the [3] to 2s, those are booleans 
but i use 1 and 2 as the values, when i should be using 0 and 1

so regs.map_offset is a u3 and it gets shifted << 6
therefore the maximum value of GetMapRegisterValueForCurrentPlane is 0x1FF
the rest of the arguments are booleans so this is cachable without taking up too much space
*/
void GeneratePlaneAddressCache()
{
	PlaneSize p;
	for(int a = 0; a < 0x200; a++)
		for(int b = 0; b < 3; b++)
			for(int c = 0; c < 3; c++)
				for (int d = 0; d < 3; d++)
					for (int e = 0; e < 3; e++)
					{
						p.horizontal_pages = b;
						p.vertical_pages = c;
						plane_address_cache[a][b][c][d][e] = GetCurrentPlaneAddress(a,p,d,e);
					}
}
#endif
//this looks easy to cache
static u32 GetCurrentPlaneAddress(
	const u32 &map_offset_register, 
	const PlaneSize &plane_size, 
	const u32 &pattern_data_size, 
	const u32 &character_pattern_width_and_height_in_cells)
{
	int deca = plane_size.vertical_pages + plane_size.horizontal_pages - 2;
	int multi = plane_size.vertical_pages * plane_size.horizontal_pages;

	if (pattern_data_size == 1)
	{
		if (character_pattern_width_and_height_in_cells == 1)
			return ((map_offset_register & 0x3F) >> deca) * (multi * 0x2000);
		else
			return (map_offset_register >> deca) * (multi * 0x800);
	}
	else
	{
		if (character_pattern_width_and_height_in_cells == 1)
			return ((map_offset_register & 0x1F) >> deca) * (multi * 0x4000);
		else
			return ((map_offset_register & 0x7F) >> deca) * (multi * 0x1000);
	}
}

#ifdef VDP2_CACHING
u32 page_address_cache[1024][1024][3][4][3][3];

void GeneratePageAddressCache()
{
	for(int x = 0; x < 1024; x++)
		for(int y = 0; y < 1024; y++)
			for(int a = 0; a < 3; a++)
				for(int b = 0; b < 4; b++)
					for(int c = 0; c < 3; c++)
						for(int d = 0; d < 3; d++)
							page_address_cache[x][y][a][b][c][d] = GetCurrentPageAddress(x,y,a,b,c,d);
}
#endif


struct GetPositionWithinTileResult
{
	u32 x, y;
};
#ifdef VDP2_CACHING
GetPositionWithinTileResult tile_lookup_cache[1024][1024][3][4];

void GenerateTileLookupCache()
{
	GetPositionWithinTileResult r;

	for(int x = 0; x < 1024; x++)
		for(int y = 0; y < 1024; y++)
			for(int a = 0; a < 3; a++)
				for(int b = 0; b < 4; b++)
				{
					tile_lookup_cache[x][y][a][b] = GetPositionWithinTile(x,y,a,b);
				}
}
#endif
static GetPositionWithinTileResult GetPositionWithinTile(
	u32 x, 
	u32 y, 
	const u32 &character_pattern_width_and_height_in_cells,
	const u32 &pnd_reverse_function)
{
	if (character_pattern_width_and_height_in_cells == 1)
	{
		x &= 8-1;
		y &= 8-1;

		switch(pnd_reverse_function)
		{
		case 0: //none
			break;
		case 1: //horizontal flip
			x = 8 - 1 - x;
			break;
		case 2: // vertical flip
			y = 8 - 1 - y;
			break;
		case 3: //flip both
			x = 8 - 1 - x;
			y = 8 - 1 - y;
			break;
		}
	}
	else
	{
		if (pnd_reverse_function)
		{
			y &= 16 - 1;
			if (pnd_reverse_function & 0x2)
			{
				if (!(y & 8))
					y = 8 - 1 - y + 16;
				else
					y = 16 - 1 - y;
			}
			else if (y & 8)
				y += 8;

			if (pnd_reverse_function & 0x1)
			{
				if (!(x & 8))
					y += 8;

				x &= 8-1;
				x = 8 - 1 - x;
			}
			else if (x & 8)
			{
				y += 8;
				x &= 8-1;
			}
			else
				x &= 8-1;
		}
		else
		{
			y &= 16 - 1;

			if (y & 8)
				y += 8;
			if (x & 8)
				y += 8;
			x &= 8-1;
		}
	}

	GetPositionWithinTileResult r;
	r.x = x;
	r.y = y;

	return r;
}

struct CellFormatResult
{
	u32 x,y;
	PatternNameData pattern_name_data;
};

CellFormatResult DrawCellFormat(
	u32 x, u32 y, u8 which_rotation_parameter, u32 debug_planenum, u32 debug_plane_address, 
	u32 debug_page_address, u32 debug_character_number, u32 debug_x, u32 debug_y, u32 debug_addr)
{
	u32 map_width_and_height_in_planes = 2;

	if(layer == kRbg0)
		map_width_and_height_in_planes = 4;

	PlaneSize p;
	if(layer == kRbg0)
		p = GetNumberOfPatternNameTablesInPlane(regs.plsz[which_rotation_parameter]);
	else
		p = GetNumberOfPatternNameTablesInPlane(regs.plsz[layer]);


	u32 plane_width = p.horizontal_pages;
	u32 plane_height = p.vertical_pages;

	u32 plane_width_shift = (plane_width + 8);
	u32 plane_height_shift = (plane_height + 8);

	u32 plane_number = 
		(x >> plane_width_shift) + 
		((y >> plane_height_shift) * map_width_and_height_in_planes);

	if(debug_planenum == 0xdeadbeef)
	{}
	else
		assert(plane_number == debug_planenum);

	//mask the coordinates
	//either 0x1ff or 0x3ff depending
	//on plane size
	x&=(1 << plane_width_shift)-1;
	y&=(1 << plane_height_shift)-1;

	u32 map_register_value = GetMapRegisterValueForCurrentPlane(plane_number,which_rotation_parameter);

	u32 character_pattern_width_and_height_in_cells;
	if(regs.chctl[layer].chsz)//fixme
		character_pattern_width_and_height_in_cells = 2;
	else
		character_pattern_width_and_height_in_cells = 1;

	u32 pattern_data_size = regs.pncn[layer].pattern_name_data_size;//this gets reused fixme
#ifdef VDP2_CACHING
	static bool plane_cached = false;
	if(!plane_cached)
	{
		GeneratePlaneAddressCache();
		plane_cached = true;
	}

	u32 current_plane_address = plane_address_cache[map_register_value][p.horizontal_pages][p.vertical_pages][pattern_data_size][character_pattern_width_and_height_in_cells];
#else
	u32 current_plane_address = GetCurrentPlaneAddress(map_register_value,p,pattern_data_size,character_pattern_width_and_height_in_cells);

#endif
	if(debug_plane_address == 0xdeadbeef)
	{}
	else
		assert(current_plane_address == debug_plane_address);

	//we have the current map and plane,
	//next we get the page that the pixel is located on


#ifndef VDP2_CACHING
	//x? y? chr bool/1or2, pds - bool
	u32 current_page_address = BackgroundCommon::GetCurrentPageAddress(
		x,y,character_pattern_width_and_height_in_cells,plane_width,plane_height,pattern_data_size);
#else
	static bool page_address_cached = false;
	if(!page_address_cached)
	{
		GeneratePageAddressCache();
		page_address_cached = true;
	}
	u32 current_page_address = page_address_cache[x][y][character_pattern_width_and_height_in_cells][plane_width][plane_height][pattern_data_size];
#endif
	current_page_address+= current_plane_address;

	if(debug_page_address == 0xdeadbeef) {}
	else

		assert(current_page_address == debug_page_address);

	//read the pattern name data from the current page
	PatternNameData pnd = GetPatternNameData(
		current_page_address,
		regs.chctl[layer].chcn,
		regs.pncn[layer].character_number_supplement ,
		character_pattern_width_and_height_in_cells,
		pattern_data_size);

	//	if(debug_character_number == 0xdeadbeef) {}
	//	else
#if 0
	current_page_address+=4;
	if(debug_addr == 0xdeadbeef) {}
	else
	{
		assert(current_page_address == debug_addr);
	}
#endif
	//the boundary of the character pattern
	//from the character number is always 0x20
	u32 character_number = pnd.character_number;
	character_number *= 0x20;
	if(debug_character_number == 0xdeadbeef) {}
	else
		assert(character_number == debug_character_number);

	//	printf("BEFORE conv x: %x, y: %x\n",x,y);
#ifdef VDP2_CACHING
	static bool tile_cached = false;
	if(!tile_cached)
	{
		GenerateTileLookupCache();
		tile_cached = true;
	}
	GetPositionWithinTileResult get_r = tile_lookup_cache[x][y][character_pattern_width_and_height_in_cells][pnd.reverse_function];
#else
	GetPositionWithinTileResult get_r = GetPositionWithinTile(x,y,character_pattern_width_and_height_in_cells,pnd.reverse_function);
#endif
	x = get_r.x;
	y = get_r.y;

	if(debug_x == 0xdeadbeef)
	{
	}
	else
	{
		assert(x == debug_x);
		assert(y == debug_y);
	}

	CellFormatResult point;

	point.x=x;
	point.y = y;
	point.pattern_name_data = pnd;
	return point;
}