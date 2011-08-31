struct Vdp2
{
	//only connected to the vdp2, bbus access goes through vdp2
	//so they belong here i guess
	array::RamArray<0x80000> vdp2_ram;
	array::RamArray<0x1000> vdp2_color_ram;

	Vdp1&vdp1;
	
	Vdp2(Vdp1 &vdp1_) : vdp1(vdp1_), nbg0(vdp2_ram,vdp2_color_ram,regs),
		nbg1(vdp2_ram,vdp2_color_ram,regs),
		nbg2(vdp2_ram,vdp2_color_ram,regs),
		nbg3(vdp2_ram,vdp2_color_ram,regs),
		rbg0(vdp2_ram,vdp2_color_ram,regs){

			//todo fix this initialization stuff
			for(int i = 0; i < 4; i++)
			{
				//	regs.mpabcd[i].a = regs.mpabcd[i].b = regs.mpabcd[i].c = regs.mpabcd[i].d = 3;
			}
			//		regs.pncn[0].pattern_name_data_size = 1;
			regs.bgon[0].transparency_on = 0;
			regs.prin[0] = 7;
			regs.bgon[kRbg0].on = 0;
			regs.zoom[0].x.integer_part = 1;
			regs.zoom[0].y.integer_part = 1;
			regs.zoom[1].x.integer_part = 1;	
			regs.zoom[1].y.integer_part = 1;

	}

#include "vdp2_registers.h"
#include "vdp2_rotation.h"
#include "vdp2_sprite.h"
#include "vdp2_window.h"
#include "vdp2_shadow.h"
#include "vdp2_color_calculation.h"

	Sprite sprite_layer;

	u32 GetColorFromColorRam(u32 address, u2 crmd)
	{
		switch(crmd)
		{
		case 0://each channel 5 bits, 1024 color
		case 1://each channel 5 bits, 2048 color
			return vdp2_color_ram.Read<kWord>((address << 1) & 0xFFF);
		case 2://each channel 8 bits, 2048 color
			return vdp2_color_ram.Read<kLong>((address << 2) & 0xFFF);
		default:
			assert(false);
			return 0;
		}
	}

	struct BackScreen
	{
		u32 GeneratePixel(
			u16 scanline_count, 
			Registers &regs, 
			array::RamArray<0x80000> &vdp2_ram)
		{
			u32 color_read_address = regs.bkta.bkta_ * 2;

			if(regs.bkta.bkclmd)//per line color
			{
				color_read_address += 2*scanline_count;
				u16 pixel = vdp2_ram.Read<kWord>(color_read_address);
				return ConvertColorWordToLong(pixel);
			}
			else//single color
			{
				return vdp2_ram.Read<kWord>(color_read_address);
			}
		}
	}back_screen;
struct PatternNameData
{
	u15 character_number;
	//u7 palette_number;
	u32 palette_number;
	u1 special_priority;
	u1 special_color_calculation;
	u2 reverse_function;
};
	struct BackgroundPixel
	{
		bool is_transparent;
		u32 color;
		u7 line_color_data;
		u32 special_function;
		u32 raw_dot;
		PatternNameData pattern_name_data;
	};

	static u32 ConvertColorWordToLong(const u32 &i)
	{
		return ((i & 0x1F) << 3) | ((i & 0x03E0) << 6) | ((i & 0xFC00) << 9);
	}

	//factor out some stuff that isn't layer specific to reduce template expansion bloat
	//msvc generates the same code for each layer, dunno about gcc
	//todo more can be factored out
	struct BackgroundCommon
	{

		static const u32 page_width_and_height_bits = 9;
		static const u32 page_width_and_height_in_pixels = 64 * 8;//64 cells * 8 pixels per cell
		static const u32 page_width_and_height_in_pixels_mask = page_width_and_height_in_pixels - 1;//0x1ff;

		static u32 GetCurrentPageAddress(
			const u32 &x, 
			const u32 &y, 
			const u32 &character_pattern_width_and_height_in_cells,
			const u32 &plane_width,
			const u32 &plane_height,
			const u32 &pattern_data_reg)
		{
			const u32 plane_width_shift = (plane_width + 8);
			const u32 plane_height_shift = (plane_height + 8);

			const u32 cell_width_height = character_pattern_width_and_height_in_cells + 2;
			const u32 plane_width_bits = plane_width >> 1;//either equal 1 or 0
			const u32 page_width_height_bits = 6-(character_pattern_width_and_height_in_cells >> 1);
			const u32 page_size_bits = page_width_height_bits*2;

			const u32 one =   ((y >> page_width_and_height_bits) << page_size_bits)  << plane_width_bits;
			const u32 two =    (x >> page_width_and_height_bits) << page_size_bits;
			const u32 three = ((y &  page_width_and_height_in_pixels_mask) >> cell_width_height) << page_width_height_bits;
			const u32 four =   (x &  page_width_and_height_in_pixels_mask) >> cell_width_height;

			u32 pattern_data_size;
			if(pattern_data_reg)
				pattern_data_size = 1;
			else
				pattern_data_size = 2;

			return (one + two + three + four) << pattern_data_size;
		}
		static int GetColorRamColorAddress(const int &pixel, const int &palette_address, const int &color_depth)
		{
			switch(color_depth)
			{
			case 0:
				return palette_address | (pixel & 0xF);
			case 1:
				return palette_address | (pixel & 0xFF);
			default:
				assert(false);
				return 0;
			}
		}
		static bool IsTransparent(const int &pixel, const int &color_depth)
		{
			//if(transparency_enabled)
			{
				switch(color_depth)
				{
				case 0:
					if ((pixel & 0xF) == 0)
						return true;
					break;
				case 1:
					if ((pixel & 0xFF) == 0)
						return true;
					break;
				case 3:
					if(pixel & 0x8000)
						return false;
					break;
				case 4:
					if(pixel & 0x80000000)
						return false;
					break;
				default:
					assert(false);
					break;
				}
			}
			//else
			return false;
		}

		struct BitmapSize
		{
			u32 x_size;
			u32 y_size;
		};

		static BitmapSize GetBitmapSize(const u2 &bmsz)
		{
			BitmapSize b;

			if((bmsz & 1) == 0)
				b.y_size = 256;
			else
				b.y_size = 512;

			if((bmsz & 2) == 0)
				b.x_size = 512;
			else
				b.x_size = 1024;
			return b;
		}

	};

	template <Layers layer>
	struct Background
	{
		array::RamArray<0x80000> &vdp2_ram;
		array::RamArray<0x1000> &vdp2_color_ram;
		Registers &regs;

		Background(	array::RamArray<0x80000> &vdp2_ram_,
			array::RamArray<0x1000> &vdp2_color_ram_, Registers &regs_)
			: vdp2_ram(vdp2_ram_),
			vdp2_color_ram(vdp2_color_ram_), regs(regs_) {}

		u32 RawPixelLookup(
			u32 x,
			u32 y,
			u32 character_address,
			u32 color_depth,
			u32 cell_width)
		{
			const u32 mask = 0x7FFFF;

			u32 new_address = character_address + ((y * cell_width) + x);

			switch(color_depth)
			{
			case 0:
				{
					u32 addr = ((character_address + ((y * cell_width) + x) / 2) & 0x7FFFF);
					u32 pixel = vdp2_ram.Read<kByte>(addr);
					if (!(x & 0x1)) 
						pixel >>= 4;
					return pixel;
				}
			case 1:
				return vdp2_ram.Read<kByte>(new_address & mask);
			case 3:
				{
					u32 addr = ((character_address + ((y * cell_width) + x) * 2) & 0x7FFFF);
					return vdp2_ram.Read<kWord>(addr);
				}
			case 4:
				{
					u32 addr = ((character_address + ((y * cell_width) + x) * 4) & 0x7FFFF);
					return vdp2_ram.Read<kLong>(addr);
				}
			default:
				assert(false);
				return 0;
			}
		}

		struct DebuggingStorage
		{
			u32 cell_width_bits;
			u32 color_depth;
			u32 character_address;
			u32 raw_pixel;
			u32 computed_address;

			u32 x;
			u32 y;
			u32 linescrollx;
			u32 info_linescrolltbl;
			u32 info_verticalscrolltbl;

			u32 prefetch_x;
			u32 prefetch_y;

			u32 coeftbladdr;
			u32 coefdatasize;

			Fixed32 kx;
			Fixed32 ky;
			u32 coef_addr;
			u32 calcumaleted_addr;
		}debugging_storage;

		enum CharacterPatternSize
		{
			kOneCell = 0,
			kTwoCells = 1
		};

		template <bool get_x_coordinate>
		u32 GetDisplayAreaMask()
		{
			//fixme add reduction setting stuff

			u32 mask;

			if(!regs.chctl[layer].bmen)
			{
				//cell format

				//this gets used more than once.
				PlaneSize p = GetNumberOfPatternNameTablesInPlane(regs.plsz[layer]);

				if(layer == kRbg0)
					p = GetNumberOfPatternNameTablesInPlane(regs.plsz[kRotA]);

				u23 plane_size;

				if(get_x_coordinate)
					plane_size = p.horizontal_pages;
				else
					plane_size = p.vertical_pages;

				mask = 1024 * plane_size;

				if(layer == kRbg0 || layer == kRbg1)
					mask *= 2;
			}
			else
			{
				//bitmap format
				//use bitmap size

				BackgroundCommon::BitmapSize b = BackgroundCommon::GetBitmapSize(regs.chctl[layer].bmsz);

				u32 bitmap_size;

				if(get_x_coordinate)
					bitmap_size = b.x_size;
				else
					bitmap_size = b.y_size;

				mask = bitmap_size;
			}

			return mask-1;
		}

		void DoVerticalCellScroll(u32 &x, u32 &y)
		{
			u32 vertical_cell_scroll_table_address = (regs.vcsta & 0x7FFFE) * 2;

			if(regs.scrctl[layer].vcsc)
			{
				u32 address_ = vertical_cell_scroll_table_address;
				address_+= (x >> 3) * 4;//increment adddress every 8 pixels * size of one entry
				y += vdp2_ram.Read<kLong>(address_) >> 16;
			}

			debugging_storage.info_verticalscrolltbl = vertical_cell_scroll_table_address;

		}

		void DoLineScroll(u32 &x, u32 &y,const u32&vcnt)
		{

			if(layer == kNbg0 || layer == kNbg1)
			{
			}
			else
				assert(false);

			u32 line_scroll_table_start_address = (regs.lsta[layer] & 0x7FFFE) * 2;

			//so, depending on what we have enabled,
			//the table is read differently.
			//we need to know the increment to go
			//to the right spot in the table
			u32 line_scroll_table_increment = 0;

			if(regs.scrctl[layer].lscx)
				line_scroll_table_increment+=4;
			if(regs.scrctl[layer].lscy)
				line_scroll_table_increment+=4;
			if(regs.scrctl[layer].lzmx)
				line_scroll_table_increment+=4;//line zoom

			u32 horizontal_scroll_value = 0;

			if(regs.scrctl[layer].lscx)//horizontal scroll enabled
			{
				line_scroll_table_start_address += (line_scroll_table_increment * vcnt);
				horizontal_scroll_value = vdp2_ram.Read<kLong>(line_scroll_table_start_address);
				horizontal_scroll_value >>= 16;
				horizontal_scroll_value &= 0x7ff;
				line_scroll_table_start_address+=4;//prepare for possible second value in the table
			}

			u32 vertical_scroll_value = 0;

			if(regs.scrctl[layer].lscy)
			{
				vertical_scroll_value = vdp2_ram.Read<kWord>(line_scroll_table_start_address);
				vertical_scroll_value &= 0x7ff;
				y+=vertical_scroll_value;
				line_scroll_table_start_address+=4;//prepare for possible third value in the table
			}
			else
				y = vcnt;

			if(regs.scrctl[layer].lzmx)//line zoom enabled
			{
				//not really correct because the real thing has some fixed point style brokenness
				//this doesn't emulate
				float value = vdp2_ram.Read<kLong>(line_scroll_table_start_address);
				value /= 65536.0;
				x*=value;
			}

			debugging_storage.linescrollx = horizontal_scroll_value;
			debugging_storage.info_linescrolltbl = line_scroll_table_start_address;
		}

		struct MosaicResult
		{
			u32 x, y;
		};
		MosaicResult DoMosaic(u32 x, u32 y)
		{
			u32 x_value;
			u32 y_value;
			if(regs.mzctl.enabled[layer])
			{
				u32 x_value = regs.mzctl.mzszh - 1;
				u32 y_value = regs.mzctl.mzszv - 1;

				if(x_value > 0)
					x = x / x_value * x_value;
				if(y_value > 0)
					y = y / y_value * y_value;
			}

			MosaicResult m;

			m.x = x;
			m.y = y;

			return m;

		}

		struct ScrollResult
		{
			u32 x, y;
		};

		ScrollResult GetScroll()
		{
			ScrollResult s;

			if(layer == kNbg0 || layer == kNbg1)
			{
				//this is wrong, should be fixed point
				//actually, x and y should probably be 
				//fixed point, the fractional part
				//saved between pixels or something
				//like that.
				s.x = regs.scxn[layer] >> 16;
				s.y = regs.scyn[layer] >> 16;
			}
			else if(layer == kNbg2)
			{
				s.x = regs.scxn2 ;
				s.y = regs.scyn2 ;
			}
			else if(layer == kNbg3)
			{
				s.x = regs.scxn3;
				s.y = regs.scyn3;
			}
			else
			{
				//no scroll for other layers?
				s.x = 0;
				s.y = 0;
			}

			return s;
		}

		struct
		{
			u32 previous;
			u32 previous_previous;
			u32 current;
		}latched_pixels;//used for gradation, also mixed res once i implement that i guess

#include "vdp2_tiling.h"

		struct CoordinateIncrementResult
		{
			float x,y;
		};

		CoordinateIncrementResult DoCoordinateIncrement(const u32 &x, const u32&y)
		{
			//todo get rid of the floating point
			float x_value = (regs.zoom[layer].x.integer_part << 16) | regs.zoom[layer].x.fractional_part;
			x_value /= 65536.0;

			float y_value = (regs.zoom[layer].y.integer_part << 16) | regs.zoom[layer].y.fractional_part;
			y_value /= 65536.0;

			//x*=x_value;
			//y*=y_value;

			CoordinateIncrementResult c;

			c.x = x_value;
			c.y = y_value;

			return c;
		}
		template <RotationParameter parameter>
		BackgroundPixel GeneratePixel(u32 hcnt, u32 vcnt, Vdp2 &vdp2)
		{ 

			u32 color_depth = vdp2.regs.chctl[layer].chcn;
			u32 character_address = vdp2.regs.map_offset[layer] * 0x20000;
			u32 x = hcnt;
			u32 y = 0;

			if(layer == kNbg2 || layer == kNbg3 || layer == kRbg0)
			{
				y = vcnt;
			}
			else
			{
				DoLineScroll(x,y,vcnt);
				DoVerticalCellScroll(x,y);
			}

			MosaicResult mosaic_result = DoMosaic(x,y);
			ScrollResult scroll_value = GetScroll();
			CoordinateIncrementResult coordinate_increment = DoCoordinateIncrement(x,y);

			if(layer == kNbg2 || layer == kNbg3)
			{
				coordinate_increment.x = coordinate_increment.y = 1;
			}

			//coodinate increment will negate the mosaic / hcnt vcnt if it's zero apparently?
			x = scroll_value.x + mosaic_result.x * coordinate_increment.x;
			y = scroll_value.y + mosaic_result.y * coordinate_increment.y;

			//fixme screen over/ display over
			u32 x_mask = GetDisplayAreaMask<true>();
			u32 y_mask = GetDisplayAreaMask<false>();

			x &= x_mask;
			y &= y_mask;

			if(debugging_storage.linescrollx)
			{
				x+=debugging_storage.linescrollx;
				x&=0x3ff;
			}

			u32 palette_address = vdp2.regs.bmpn[layer].bmp << 8;
#if 1

			bool coefficient_data_makes_this_pixel_transparent = false;

			CoefficientData coefficient_data = {0};

			if(layer == kRbg0 || layer == kRbg1)
			{
				//hmm....
				u32 rotation_table_address = vdp2.GetRotationTableLeadAddress<parameter>();

				RotationParameters rotation_parameters = vdp2.RotationReadTable(rotation_table_address);

				u32 coefficient_table_address= vdp2.GetCoefficientTableLeadAddress<parameter>(rotation_parameters.KAst);

				debugging_storage.coeftbladdr = coefficient_table_address;

				u32 mode = 0;//fixme

				//copy paste fixme
				u32 table_data_size_in_bytes;
				if(vdp2.regs.coefficient_table_control[parameter].data_size == 1)
					table_data_size_in_bytes = 2;
				else
					table_data_size_in_bytes = 4;

				//	u32 ything = vcnt * rotation_parameters.dKAst.ToInt();
				Fixed32 ything = vcnt * rotation_parameters.dKAst.data;
				//coefficient_table_address += ything * table_data_size_in_bytes;


				Fixed32 xthing = hcnt * rotation_parameters.dKAx.data;

				Fixed32 new_one = xthing + ything;

				coefficient_table_address += new_one.ToInt() * table_data_size_in_bytes;

				debugging_storage.calcumaleted_addr = coefficient_table_address;
				CoefficientData coefficient_data = vdp2.ReadCoefficientDataForMode<parameter>(rotation_parameters,mode,coefficient_table_address);
				if(coefficient_data.is_transparent)
					coefficient_data_makes_this_pixel_transparent = true;

				bool coefficient_enabled = regs.coefficient_table_control[parameter].table_enable;

				RotationResult rotation_result = vdp2.RotationGetDisplayCoordinates(rotation_parameters,hcnt,vcnt,x_mask,y_mask,coefficient_enabled);
				x = rotation_result.x;
				y = rotation_result.y;
			}


#endif

			debugging_storage.prefetch_x = x;
			debugging_storage.prefetch_y = y;


			u32 cell_width_bits;
			CellFormatResult point = {0};
			//layers 2, 3 can't draw bitmaps
			if(!vdp2.regs.chctl[layer].bmen || layer == kNbg2 || layer == kNbg3)
			{
				point =  DrawCellFormat(x, y, parameter, 0xdeadbeef, 0xdeadbeef, 0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef);
				x = point.x;
				y = point.y;
				character_address = point.pattern_name_data.character_number;
				character_address *= 0x20;
				//palette_address = 0x30;//point.pattern_name_data.palette_number << 4;
				palette_address = point.pattern_name_data.palette_number << 4;
				cell_width_bits = 8;
			}
			else
			{
				BackgroundCommon::BitmapSize b = BackgroundCommon::GetBitmapSize(regs.chctl[layer].bmsz);

				cell_width_bits = b.x_size;
			}



			debugging_storage.x = x;
			debugging_storage.y = y;

			u32 raw_pixel = RawPixelLookup(x,y,character_address,color_depth,cell_width_bits);

			debugging_storage.cell_width_bits = cell_width_bits;
			debugging_storage.color_depth = color_depth;
			debugging_storage.character_address = character_address;
			debugging_storage.raw_pixel = raw_pixel;
			//		debugging_storage.computed_address = ;

			BackgroundPixel p;

			p.line_color_data = coefficient_data.line_color_screen_data;

			if(!vdp2.regs.bgon[layer].transparency_on)
				p.is_transparent = BackgroundCommon::IsTransparent(raw_pixel,color_depth);
			else
				p.is_transparent = false;

			if(coefficient_data_makes_this_pixel_transparent)
				p.is_transparent = true;

			//beware this is probably a bug due to the masking
			u32 color_offset = vdp2.regs.color_ram_address_offset[layer] << 8;

			if(color_depth < 3) 
			{
				u32 jank = vdp2.regs.ramctl.crmd;
				int color_address = BackgroundCommon::GetColorRamColorAddress(raw_pixel,palette_address,color_depth);
				color_address+= color_offset;
				u32 color = vdp2.GetColorFromColorRam(color_address, vdp2.regs.ramctl.crmd);
				if(vdp2.regs.ramctl.crmd < 2)
					color = ConvertColorWordToLong(color);

				p.color = color;
			}
			else
			{
				if(color_depth == 3)
					p.color = ConvertColorWordToLong(raw_pixel);
				else
					p.color =  raw_pixel;

			}

			p.raw_dot = raw_pixel;
			p.special_function = point.pattern_name_data.special_color_calculation;
			p.pattern_name_data = point.pattern_name_data;

			latched_pixels.previous_previous = latched_pixels.previous;
			latched_pixels.previous = latched_pixels.current;
			latched_pixels.current = p.color;

			return p;
			assert(false);
		}
	};

	Background<kNbg0> nbg0;
	Background<kNbg1> nbg1;
	Background<kNbg2> nbg2;
	Background<kNbg3> nbg3;
	Background<kRbg0> rbg0;

	Sprite::SpritePixelResult previous_sprite_pixel;

	//todo this is basically wrong.  640 and 704 are possible
	//by combining 2 background layers so that it writes 2 pixels
	//for every hcnt.  this is why all the stuff like linescroll,
	//windows, etc work the way they do in high res

	u16 GetHorizontalResolution()
	{
		switch(regs.tvmd.hreso)
		{
		case 0:
		case 4:
			return 320;
		case 1:
		case 5:
			return 352;
		case 2:
		case 6:
			return 640;
		case 3:
		case 7:
			return 704;
		}
		assert(false);
		return 0;
	}

	u16 GetVerticalResolution()
	{
		switch(regs.tvmd.vreso)
		{
		case 0:
			return 224;
		case 1:
			return 240;
		case 2:
			return 256;
		}
		assert(false);
		return 0;
	}

	u16 GetVdp1HorizontalMultiple(Vdp1 &vdp1)
	{
		switch(vdp1.regs.tvmr.tvm)
		{
		case 0:
		case 2:
		case 3:
		case 4:
			return 1;//half res 320 or 352
		case 1:
			return 2;//full res 640 or 704
		default:
			assert(false);
			break;
		}
		assert(false);
		return 0;
	}

	bool DoubleDensityInterlace()
	{
		if(regs.tvmd.lsmd == 3)
			return true;
		else
			return false;
	}

	//this needs testing, i don't really get it.
	bool Vdp1Interlace(Vdp1&vdp1)
	{
		if(vdp1.regs.fbcr.die)
			return true;

		return false;
	}

	bool GradationIsEnabled(int layer)
	{
		if(!regs.color_calculation_control.gradation_enable)
			return false;

		switch(regs.color_calculation_control.gradation_layer_select)
		{
		case 0:
			return layer == kSprite;
		case 1:
			return layer == kRbg0;
		case 2:
			return layer == kNbg0;//rbg1
		case 3:
			assert(false);
			return 0;
		case 4:
			return layer == kNbg1;
		case 5:
			return layer == kNbg2;
		case 6:
			return layer == kNbg3;
		case 7:
			assert(false);
			return false;
		default:
			assert(false);
			return false;
		}
		assert(false);
		return false;
	}

	s16 AddOrSub(s32 offset, s16 value)
	{
		//sign extend if it's negative
		if(offset & 0x100)
			offset |= 0xFFFFFF00;
		else
			offset & 0xFF;

		value += offset;

		if(value < 0)
			value = 0;
		if(value > 255)
			value = 255;

		return value;
	}
	u32 DoColorOffset(u32 layer, u32 pixel)
	{

		Color c(pixel);

		if(!regs.clofen[layer])
			return pixel;

		u32 which = regs.clofsl[layer];
		u32 red_value = regs.color_offset[which].red;
		u32 green_value = regs.color_offset[which].green;
		u32 blue_value = regs.color_offset[which].blue;

		c.r = AddOrSub(red_value,c.r);
		c.g = AddOrSub(green_value,c.g);
		c.b = AddOrSub(blue_value,c.b);

		return c.Compose();
	}

	void Draw(int x, int y, BackgroundPixel* pixels, Vdp1 &vdp1, Sprite::SpritePixelResult &sprite_pixel)
	{
		//note we cannot make the optimization of not generating
		//in the case of priority 0 because special prioity can set 0 to 1
		if(regs.bgon[kNbg0].on)
			pixels[kNbg0] = nbg0.GeneratePixel<kParameterA>(x,y,*this);
		if(regs.bgon[kNbg1].on)
			pixels[kNbg1] = nbg1.GeneratePixel<kParameterA>(x,y,*this);
		if(regs.bgon[kNbg2].on)
			pixels[kNbg2] = nbg2.GeneratePixel<kParameterA>(x,y,*this);
		if(regs.bgon[kNbg3].on)
			pixels[kNbg3] = nbg3.GeneratePixel<kParameterA>(x,y,*this);

		sprite_pixel = sprite_layer.GeneratePixel(x,y,*this,vdp1);

		if(regs.bgon[kRbg0].on && regs.prin[kRbg0] != 0)
		{
			switch(regs.rotation_parameter_mode)
			{
			case 0:
				pixels[kRbg0] = rbg0.GeneratePixel<kParameterA>(x,y,*this);
				break;
			case 1:
				pixels[kRbg0] = rbg0.GeneratePixel<kParameterB>(x,y,*this);
				break;
			case 2:
				pixels[kRbg0] = rbg0.GeneratePixel<kParameterA>(x,y,*this);
				if(pixels[kRbg0].is_transparent)
					pixels[kRbg0] = rbg0.GeneratePixel<kParameterB>(x,y,*this);
				break;
			case 3:
				{
					WindowResult wr = DoWindow<kRotationParameter>(x,y,regs);

					if(!wr.transparent)
						pixels[kRbg0] = rbg0.GeneratePixel<kParameterA>(x,y,*this);
					else
						pixels[kRbg0] = rbg0.GeneratePixel<kParameterB>(x,y,*this);
				}
				break;
			}
		}
	}

	template <Layers layer>
	void DoTransparentWindow(int &x, int &y, BackgroundPixel* pixels)
	{
		WindowResult wr = DoWindow<layer>(x,y,regs);

		if(wr.transparent)
			pixels[layer].is_transparent = true;
	}

	void DoTransparentWindows(int &x, int &y, BackgroundPixel* pixels)
	{
		DoTransparentWindow<kNbg0>(x,y,pixels);
		DoTransparentWindow<kNbg1>(x,y,pixels);
		DoTransparentWindow<kNbg2>(x,y,pixels);
		DoTransparentWindow<kNbg3>(x,y,pixels);
	}
	template <Layers layer>
	void DoSpriteWindow(int &x, int &y, const Sprite::SpritePixelResult &sprite_pixel, BackgroundPixel* pixels)
	{
		if(regs.spctl.spwinen)
		{
			if(regs.window_control[layer].sprite.enable)
			{
				if(sprite_pixel.raw_data & (1 << 15))
				{
					pixels[layer].is_transparent = true;
				}
			}
		}
	}
	struct MyPixel
	{
		u32 color;
		u32 ratio;
		u32 layer;
		u32 special_function;
		u32 raw_dot;
	};
	
	u32 GetLayerPriority(BackgroundPixel *pixels, Layers layer)
	{
		u32 layer_priority = regs.prin[layer];

		if(regs.sfprmd[layer] == 0)
		{
			return layer_priority;
		}
		else if(regs.sfprmd[layer] == 1)
		{
			if(pixels[layer].pattern_name_data.special_priority)
				layer_priority |= 1;
			else
				layer_priority &= ~1;
		}
		else if(regs.sfprmd[layer] == 2)
		{
			if(pixels[layer].pattern_name_data.special_priority)
			{
				u32 special_function_code_register = GetSpecialFunctionCodeRegister(layer);

				if(PixelIsSpecialFunctionCode(special_function_code_register,pixels[layer].raw_dot))
					layer_priority |= 1;
				else
					layer_priority &= ~1;
			}
		}

		return layer_priority;
	}

	template <Layers layer>
	void PriorityProcessLayer(int priority, u8 &current_pixel_depth, BackgroundPixel *pixels, MyPixel* pixel_stack)
	{
		if(regs.bgon[layer].on)
		{
			u32 layer_priority = GetLayerPriority(pixels,layer);

			if(layer_priority == priority)
			{
				if(!pixels[layer].is_transparent)
				{
					pixel_stack[current_pixel_depth].raw_dot = pixels[layer].raw_dot;
					pixel_stack[current_pixel_depth].special_function = pixels[layer].special_function;
					pixel_stack[current_pixel_depth].color = DoColorOffset(layer,pixels[layer].color);
					pixel_stack[current_pixel_depth].ratio = regs.ccr[layer];
					pixel_stack[current_pixel_depth].layer = layer;
					current_pixel_depth++;
				}
			}
		}
	}

	u32 DoComposite(BackgroundPixel* pixels,Sprite::SpritePixelResult &sprite_pixel, int y)
	{
		//todo fix rbg0 line color shading
		if(false)
		{
			u32 line_color_table_address = regs.line_color.address * 2;
			u32 line_color = vdp2_ram.Read<kWord>(line_color_table_address);
			line_color &= 0x780;
			if(pixels[kRbg0].line_color_data == 0x6F)
				goto skipit;
			line_color |= pixels[kRbg0].line_color_data;
			u32 color = GetColorFromColorRam(line_color,regs.ramctl.crmd);
			pixels[kRbg0].color = ColorCalculate(pixels[kRbg0].color,color,15);
		}
skipit:
		if(GradationIsEnabled(kNbg0))
			pixels[kNbg0].color = DoGradationCalculation(
			nbg0.latched_pixels.current,
			nbg0.latched_pixels.previous,
			nbg0.latched_pixels.previous_previous);



		MyPixel pixel_stack[4] = {0};
		u8 current_pixel_depth = 0;
		for(int priority = 7; priority > 0; priority--)
		{
			if(current_pixel_depth >= 3)
				break;

			if(sprite_pixel.priority == priority)
			{
				if(!sprite_pixel.transparent)
				{
					//pixel_stack[current_pixel_depth].raw_dot = sprite_pixel.raw_data;
					//pixel_stack[current_pixel_depth].special_function = sprite_pixel.raw_data;
					pixel_stack[current_pixel_depth].color = DoColorOffset(kSprite,sprite_pixel.color);
					pixel_stack[current_pixel_depth].ratio = sprite_pixel.color_calculation_ratio;//regs.ccr[kSprite];
					pixel_stack[current_pixel_depth].layer = kSprite;
					current_pixel_depth++;
				}
			}

			PriorityProcessLayer<kRbg0>(priority,current_pixel_depth,pixels,pixel_stack);
			PriorityProcessLayer<kNbg0>(priority,current_pixel_depth,pixels,pixel_stack);
			PriorityProcessLayer<kNbg1>(priority,current_pixel_depth,pixels,pixel_stack);
			PriorityProcessLayer<kNbg2>(priority,current_pixel_depth,pixels,pixel_stack);
			PriorityProcessLayer<kNbg3>(priority,current_pixel_depth,pixels,pixel_stack);
		}

		//whateher it's enabled or not always depends on the top layer
		if(ColorCalculationIsEnabled(
			pixel_stack[0].layer,
			pixel_stack[0].special_function,
			pixel_stack[0].raw_dot,
			pixel_stack[0].color,sprite_pixel))
		{
			if(regs.lnclen[pixel_stack[0].layer])
			{
				u32 line_color_screen_address = regs.line_color.address * 2;
				line_color_screen_address += 2 * y;
				u32 color_ram_address = vdp2_ram.Read<kWord>(line_color_screen_address);

				//bug for 32bit color mode fixme
				u32 line_color_screen_color = ConvertColorWordToLong(GetColorFromColorRam(color_ram_address,regs.ramctl.crmd));

				pixel_stack[0].color = ColorCalculate(line_color_screen_color,pixel_stack[0].color,pixel_stack[0].ratio);
			}
			else if(regs.color_calculation_control.ratio_mode == 0)
			{
				//use top screen ratio
				pixel_stack[0].color = ColorCalculate(pixel_stack[0].color,pixel_stack[1].color,pixel_stack[0].ratio);
			}
			else
			{
				//use 2nd layer ratio
				pixel_stack[0].color = ColorCalculate(pixel_stack[0].color,pixel_stack[1].color,pixel_stack[1].ratio);
			}
		}


		//i guess by definition the sprite pixel would be in pixelstack[0] right?
		//DoShadow(sprite_pixel,pixel_stack[0],pixel_stack[1]);

		//not sure about using the top pixel here, i guess it could be anywhere and work
///		if(pixel_stack[0].layer == kSprite)
//		{
//			pixel_stack[0].color = DoShadow(sprite_pixel,pixel_stack[0].raw_dot,pixel_stack[1].color);
//		}

		//not sure what res it runs at vertically... not particulary important anyway
		pixel_stack[current_pixel_depth].color = back_screen.GeneratePixel(y,regs,vdp2_ram);

		return pixel_stack[0].color;
	}
	u32 GeneratePixel(int &x, int &y,Vdp1 &vdp1)
	{
		BackgroundPixel pixels[15] = {0};

		Sprite::SpritePixelResult sprite_pixel;

		Draw(x,y,pixels,vdp1,sprite_pixel);

		DoTransparentWindows(x,y,pixels);

		//DoSpriteWindow(x,y,sprite_pixel,pixels);

	//	void DoSpriteWindow(int &x, int &y, const Sprite::SpritePixelResult &sprite_pixel, BackgroundPixel* pixels)
		DoSpriteWindow<kNbg0>(x,y,sprite_pixel,pixels);
		DoSpriteWindow<kNbg1>(x,y,sprite_pixel,pixels);
		DoSpriteWindow<kNbg2>(x,y,sprite_pixel,pixels);
		DoSpriteWindow<kNbg3>(x,y,sprite_pixel,pixels);

		return DoComposite(pixels,sprite_pixel,y);
	}
};