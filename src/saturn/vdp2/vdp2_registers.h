//it makes more sense to reorganize the the regs
//than duplicate the way they are laid out in
//the memory map
//todo give more regs human-readable names

	enum Layers
	{
		kNbg0,
		kNbg1,
		kNbg2,
		kNbg3,
		kRotA,//only used for map offset and plsz
		kRotB,//only used for map offset and plsz
		kRbg0,
		kRbg1,
		kBack,
		kSprite,
		kLineColor,
		kRotationParameter,
		kColorCalculation,
		kNumberOfLayers
	};
	enum RotationParameter
	{
		kParameterA,
		kParameterB
	};
struct Registers
{
	struct tvmd
	{
		u1 disp;
		u1 bdclmd;
		u2 lsmd;
		u2 vreso;
		u3 hreso;
	}tvmd;
	struct exten
	{
		u1 exlten;
		u1 exsyen;
		u1 dasel;
		u1 exbgen;
	}exten;
	struct tvstat
	{
		u1 exltfg;
		u1 exsyfg;
		u1 vblank;
		u1 hblank;
		u1 odd;
		u1 pal;
	}tvstat;
	struct vrsize
	{
		u1 vramsz;
		u4 ver;
	}vrsize;

	u10 hct;
	u10 vct;

	//reserve

	struct ramctl
	{
		u1 crtke;
		u2 crmd;
		u1 vrbmd;
		u1 vramd;
		u4 rdbsb;
		u4 rdbsa;
	}ramctl;

	//vram cycle pattern
	struct cyca0l
	{
		u4 vcp0a0;
		u4 vcp1a0;
		u4 vcp2a0;
		u4 vcp3a0;
	}cyca0l;
	struct cyca0u
	{
		u4 vcp4a0;
		u4 vcp5a0;
		u4 vcp6a0;
		u4 vcp7a0;
	}cyca0u;
	struct cyca1l
	{
		u4 vcp0a1;
		u4 vcp1a1;
		u4 vcp2a1;
		u4 vcp3a1;
	}cyca1l;
	struct cyca1u
	{
		u4 vcp4a1;
		u4 vcp5a1;
		u4 vcp6a1;
		u4 vcp7a1;
	}cyca1u;
	struct cycb0l
	{
		u4 vcp0b0;
		u4 vcp1b0;
		u4 vcp2b0;
		u4 vcp3b0;
	}cycb0l;
	struct cycb0u
	{
		u4 vcp4b0;
		u4 vcp5b0;
		u4 vcp6b0;
		u4 vcp7b0;
	}cycb0u;
	struct cycb1l
	{
		u4 vcp0b1;
		u4 vcp1b1;
		u4 vcp2b1;
		u4 vcp3b1;
	}cycb1l;
	struct cycb1u
	{
		u4 vcp4b1;
		u4 vcp5b1;
		u4 vcp6b1;
		u4 vcp7b1;
	}cycb1u;


	struct Bgon
	{
		u1 transparency_on;
		u1 on;
	};

	array::Array<Bgon,kNumberOfLayers> bgon;

	struct mzctl
	{
		u4 mzszv;
		u4 mzszh;

		array::Array<u1,kNumberOfLayers> enabled;
	}mzctl;

	array::Array<u1,kNumberOfLayers> sfsel;

	struct sfcode
	{
		array::Array<u8,2> sfcd;
	}sfcode;

	u2 sfprmd[kNumberOfLayers];

	struct Chctl
	{
		u3 chcn;
		u2 bmsz;
		u1 bmen;
		u1 chsz;
	};

	array::Array<Chctl,kNumberOfLayers> chctl;

	struct Bmpn
	{
		u1 bmpr;
		u1 bmcc;
		u1 bmp;
	};

	array::Array<Bmpn,kNumberOfLayers> bmpn;

	struct PatternNameDataReg
	{
		u1 pattern_name_data_size;//pnb
		u1 character_number_supplement;
		u1 special_priority;
		u1 special_color_calculation;
		u3 supplementary_palette_number;
		u5 supplementary_character_number;
	};

	array::Array<PatternNameDataReg,kNumberOfLayers> pncn;

	array::Array <u2, kNumberOfLayers> plsz;
	array::Array <u2, 2> plsz_rot;

	array::Array <u3, kNumberOfLayers> map_offset;
	array::Array <u3, 2>map_offset_parameter;

	//0x180040 to 0x18004E

	struct 
	{
		struct Background
		{
			array::Array<u6,4> plane;
		};

		array::Array<Background,4> background;

		struct Rotation
		{
			array::Array<u6,16> plane;
		};

		array::Array<Rotation,2> rotation;
	}map;

	//scxin0, scyin0 etc
	//refactor this
	array::Array<u32,2> scxn;
	array::Array<u32,2> scyn;
	u11 scxn2;
	u11 scyn2;
	u11 scxn3;
	u11 scyn3;

	struct Zoom
	{
		struct
		{
			u3 integer_part;
			u16 fractional_part;
		}x,y;
	};

	array::Array <Zoom,kNumberOfLayers> zoom;

	//zmctl?

	struct Scrctl
	{
		u2 lss;
		u1 lzmx;
		u1 lscy;
		u1 lscx;
		u1 vcsc;
	};

	array::Array<Scrctl,2> scrctl;

	//18 bit but the lowest bit is missing..
	u19 vcsta;

//	struct Lsta
//	{
		//18 bit but the lowest bit is missing..
//		u19 lsta_;
//	};

	array::Array<u19,2> lsta;

	struct
	{
		u1 lcclmd;
		u19 address;
	}line_color;

	struct Bkta
	{
		u1 bkclmd;
		u19 bkta_;
	}bkta;

	//rotation stuff
	u2 rotation_parameter_mode;

	//rprctl

	struct Ktctl
	{
		u1 line_color_enable;
		u2 data_mode;
		u1 data_size;
		u1 table_enable;
	};

	array::Array<Ktctl,2> coefficient_table_control;

	array::Array<u3,2> coefficient_table_address_offset;//ktaof

	array::Array<u16,2> screen_over_pattern_name;

	u3 rptau;

	u16 rptal;
	
	//window
	struct WindowPosition
	{
		struct
		{
			u10 start;
			u10 end;
		}x;
		struct
		{
			u9 start;
			u9 end;
		}y;
	};

	array::Array<WindowPosition,2> window_position;

	struct WindowControl
	{
		u1 logic;
		struct WindowReg
		{
			u1 enable;
			u1 outside;
		}window[2],sprite;
	};

	array::Array<WindowControl,kNumberOfLayers> window_control;

	struct Lwta
	{
		u1 enabled;
		u19 address;
	};

	array::Array<Lwta,2> lwta;

	//sprite control etc

	struct spctl
	{
		u2 spcccs;
		u3 spccn;
		u1 spclmd;
		u1 spwinen;
		u4 sptype;
	}spctl;

	struct 
	{
		u1 tpsdl;
		array::Array<u1,kNumberOfLayers> enable;
	}sdctl;

	array::Array<u3, kNumberOfLayers> color_ram_address_offset;

	array::Array<u1, kNumberOfLayers> lnclen;

	//sfprmd

	struct
	{
		u1 gradation_enable;
		u3 gradation_layer_select;
		u1 extended_enabled;
		u1 ratio_mode;
		u1 mode;
		array::Array<u1,kNumberOfLayers> enable;
	}color_calculation_control;

	array::Array<u2,kNumberOfLayers> special_color_calculation_mode;

	//priority regs
	array::Array<u3,kNumberOfLayers> pris;

	u3 prin[8];

	//reserve 0xfe

	//color calculation regs
	array::Array<u5,8> ccrs;

	u5 ccr[11];

	//color offset regs
	u1 clofen[kNumberOfLayers];
	u1 clofsl[kNumberOfLayers];

	struct
	{
		u9 red;
		u9 green;
		u9 blue;
	}color_offset[2];

	//end of regs




	enum Planes
	{
		kPlaneA,
		kPlaneB,
		kPlaneC,
		kPlaneD,
		kPlaneE,
		kPlaneF,
		kPlaneG,
		kPlaneH,
		kPlaneI,
		kPlaneJ,
		kPlaneK,
		kPlaneL,
		kPlaneM,
		kPlaneN,
		kPlaneO,
		kPlaneP
	};

	u16 Read(int address)
	{

		address &= 0x1FF;
		unsigned int result = 0;
		switch(address)
		{			
		case 4:
			//looks like we do some extra logic
			tvstat.exltfg = 0;
			tvstat.exsyfg = 0;

			result |= tvstat.exltfg << 9;
			result |= tvstat.exsyfg << 8;
			result |= tvstat.vblank << 3;
			result |= tvstat.hblank << 2;
			result |= tvstat.odd << 1;
			result |= tvstat.pal;
			if(tvmd.disp)
				return result;
			else
				return result | 0x8;
		case 0xe:
			result |= ramctl.crmd << 12;
			result |= ramctl.vrbmd << 9;
			result |= ramctl.vramd << 8;
			result |= ramctl.rdbsb << 4;
			result |= ramctl.rdbsa;
			return result;
		}

		assert(false);
		return 0;
	}
	template <Layers which>
	void SetWindowControl(u8 data)
	{
		window_control[which].logic = data >> 7;
		window_control[which].sprite.enable = data >> 5;
		window_control[which].sprite.outside = data >> 4;
		window_control[which].window[1].enable = data >> 3;
		window_control[which].window[1].outside = data >> 2;
		window_control[which].window[0].enable = data >> 1;
		window_control[which].window[0].outside = data;
	}

	template<RotationParameter parameter>
	void SetRotationParameterMap(u16 address, u16 data)
	{
		switch(address)
		{
		case 0x0:
			map.rotation[parameter].plane[kPlaneB] = data >> 8;
			map.rotation[parameter].plane[kPlaneA] = data;
			break;
		case 0x2:
			map.rotation[parameter].plane[kPlaneD] = data >> 8;
			map.rotation[parameter].plane[kPlaneC] = data;
			break;
		case 0x4:
			map.rotation[parameter].plane[kPlaneF] = data >> 8;
			map.rotation[parameter].plane[kPlaneE] = data;
			break;
		case 0x6:
			map.rotation[parameter].plane[kPlaneH] = data >> 8;
			map.rotation[parameter].plane[kPlaneG] = data;
			break;
		case 0x8:
			map.rotation[parameter].plane[kPlaneJ] = data >> 8;
			map.rotation[parameter].plane[kPlaneI] = data;
			break;
		case 0xa:
			map.rotation[parameter].plane[kPlaneL] = data >> 8;
			map.rotation[parameter].plane[kPlaneK] = data;
			break;
		case 0xc:
			map.rotation[parameter].plane[kPlaneN] = data >> 8;
			map.rotation[parameter].plane[kPlaneM] = data;
			break;
		case 0xe:
			map.rotation[parameter].plane[kPlaneP] = data >> 8;
			map.rotation[parameter].plane[kPlaneO] = data;
			break;
		}
	}

	template <Layers layer>
	void SetPncn(const u16& data)
	{
		pncn[layer].pattern_name_data_size = data >> 15;
		pncn[layer].character_number_supplement = data >> 14;
		pncn[layer].special_priority = data >> 9;
		pncn[layer].special_color_calculation = data >> 8;
		pncn[layer].supplementary_palette_number = data >> 5;
		pncn[layer].supplementary_character_number = data;
	}

	template <Layers layer>
	void SetScrollZoom(const u32&address, const u16&data)
	{
		switch(address)
		{
		case 0x0:
			scxn[layer] = (scxn[layer] & 0xFFFF) | (data << 16);
			break;
		case 0x2:
			scxn[layer] = (scxn[layer] & 0xFFFF0000) | data;
			break;
		case 0x4:
			scyn[layer] = (scyn[layer] & 0xFFFF) | (data << 16);
			break;
		case 0x6:
			scyn[layer] = (scyn[layer] & 0xFFFF0000) | data;
			break;
		case 0x8:
			zoom[layer].x.integer_part = data;
			break;
		case 0xa:
			zoom[layer].x.fractional_part = data;
			break;
		case 0xc:
			zoom[layer].y.integer_part = data;
			break;
		case 0xe:
			zoom[layer].y.fractional_part = data;
			break;
		}
	}

	template <int which>
	void SetWindowPosition(u32 address, u16 data)
	{
		switch(address)
		{
		case 0x0:
			window_position[which].x.start = data;
			break;
		case 0x2:
			window_position[which].y.start = data;
			break;
		case 0x4:
			window_position[which].x.end = data;
			break;
		case 0x6:
			window_position[which].y.end = data;
			break;
		}
	}
	void Write(int address, u16 data)
	{
		address &= 0x1ff;
	//	printf("addr: %x, data: %x\n",address,data);
		switch(address)
		{
		case 0:
			tvmd.disp = data >> 15;
			tvmd.bdclmd = data >> 8;
			tvmd.lsmd = data >> 6;
			tvmd.vreso = data >> 4;
			tvmd.hreso = data;
			break;
		case 0xe:
			ramctl.crtke = data >> 15;
			ramctl.crmd = data >> 12;
			ramctl.vrbmd = data >> 9;
			ramctl.vramd = data >> 8;
			ramctl.rdbsb = data >> 4;
			ramctl.rdbsa = data;
			break;
		case 0x20:
			bgon[kRbg0].transparency_on = data >> 12;
			bgon[kNbg3].transparency_on = data >> 11;
			bgon[kNbg2].transparency_on = data >> 10;
			bgon[kNbg1].transparency_on = data >> 9;
			bgon[kNbg0].transparency_on = data >> 8;
			bgon[kRbg1].on = data >> 5;
			bgon[kRbg0].on = data >> 4;
			bgon[kNbg3].on = data >> 3;
			bgon[kNbg2].on = data >> 2;
			bgon[kNbg1].on = data >> 1;
			bgon[kNbg0].on = data;
			break;
		case 0x22:
			mzctl.mzszv = data >> 12;
			mzctl.mzszh = data >> 8;
			mzctl.enabled[kRbg0] = data >> 4;
			mzctl.enabled[kNbg3] = data >> 3;
			mzctl.enabled[kNbg2] = data >> 2;
			mzctl.enabled[kNbg1] = data >> 1;
			mzctl.enabled[kNbg0] = data;
			break;
		case 0x24:
			sfsel[kRbg0] = data >> 4;
			sfsel[kNbg3] = data >> 3;
			sfsel[kNbg2] = data >> 2;
			sfsel[kNbg1] = data >> 1;
			sfsel[kNbg0] = data;
			break;
		case 0x26:
			sfcode.sfcd[1] = data >> 8;//b
			sfcode.sfcd[0] = data;//a
			break;
		case 0x28:
			chctl[kNbg1].chcn = (data >> 12) & 3;//2 bits
			chctl[kNbg1].bmsz = data >> 10;
			chctl[kNbg1].bmen = data >> 9;
			chctl[kNbg1].chsz = data >> 8;

			chctl[kNbg0].chcn = data >> 4;
			chctl[kNbg0].bmsz = data >> 2;
			chctl[kNbg0].bmen = data >> 1;
			chctl[kNbg0].chsz = data;
			break;
		case 0x2a:
			chctl[kRbg0].chcn = data >> 12;
			chctl[kRbg0].bmsz = data >> 10;
			chctl[kRbg0].bmen = data >> 9;
			chctl[kRbg0].chsz = data >> 8;
			
			//bitmaps don't work on layers 2,3
			//this should probably be done differently
			//i think...
			chctl[kNbg3].bmsz = 0;//invalid
			chctl[kNbg3].bmen = 0;//invalid
			chctl[kNbg2].bmsz = 0;//invalid
			chctl[kNbg2].bmen = 0;//invalid

			chctl[kNbg3].chcn = (data >> 5) & 1;//only 1 bit
			chctl[kNbg3].chsz = data >> 4;
			chctl[kNbg2].chcn = (data >> 1) & 1;//only 1 bit
			chctl[kNbg2].chsz = data;
			break;
		case 0x2c:
			bmpn[kNbg1].bmpr = data >> 13;
			bmpn[kNbg1].bmcc = data >> 12;
			bmpn[kNbg1].bmp = data >> 8;
			bmpn[kNbg0].bmpr = data >> 5;
			bmpn[kNbg0].bmcc = data >> 4;
			bmpn[kNbg0].bmp = data;
			break;
		case 0x2e:
			bmpn[kRbg0].bmpr = data >> 5;
			bmpn[kRbg0].bmcc = data >> 4;
			bmpn[kRbg0].bmp = data;
			break;
		case 0x30:
			//pncn0
			SetPncn<kNbg0>(data);
			break;
		case 0x32:
			//pncn1
			SetPncn<kNbg1>(data);
			break;
		case 0x34:
			//pncn2
			SetPncn<kNbg2>(data);
			break;
		case 0x36:
			//pncn3
			SetPncn<kNbg3>(data);
			break;
		case 0x38:
			//pncr
			SetPncn<kRbg0>(data);
			break;
		case 0x3a:
			//THIS REG IS NOT COMPLETE FIXME
			plsz_rot[kParameterA] = data >> 12;
			plsz_rot[kParameterB] = data >> 8;
			plsz[kNbg3] = data >> 6;
			plsz[kNbg2] = data >> 4;
			plsz[kNbg1] = data >> 2;
			plsz[kNbg0] = data;
			break;
		case 0x3c:
			map_offset[kNbg3] = data >> 12;
			map_offset[kNbg2] = data >> 8;
			map_offset[kNbg1] = data >> 4;
			map_offset[kNbg0] = data;
			break;

		case 0x3e:
			map_offset_parameter[kParameterB] = data >> 4;
			map_offset_parameter[kParameterA] = data;
			break;
		case 0x40:
			map.background[kNbg0].plane[kPlaneB] = data >> 8;
			map.background[kNbg0].plane[kPlaneA] = data;
			break;
		case 0x42:
			map.background[kNbg0].plane[kPlaneD] = data >> 8;
			map.background[kNbg0].plane[kPlaneC] = data;
			break;
		case 0x44:
			map.background[kNbg1].plane[kPlaneB] = data >> 8;
			map.background[kNbg1].plane[kPlaneA] = data;
			break;
		case 0x46:
			map.background[kNbg1].plane[kPlaneD] = data >> 8;
			map.background[kNbg1].plane[kPlaneC] = data;
			break;
		case 0x48:
			map.background[kNbg2].plane[kPlaneB] = data >> 8;
			map.background[kNbg2].plane[kPlaneA] = data;
			break;
		case 0x4a:
			map.background[kNbg2].plane[kPlaneD] = data >> 8;
			map.background[kNbg2].plane[kPlaneC] = data;
			break;
		case 0x4C:
			map.background[kNbg3].plane[kPlaneB] = data >> 8;
			map.background[kNbg3].plane[kPlaneA] = data;
			break;
		case 0x4e:
			map.background[kNbg3].plane[kPlaneD] = data >> 8;
			map.background[kNbg3].plane[kPlaneC] = data;
			break;

			//rotation parameter a map
		case 0x50:
		case 0x52:
		case 0x54:
		case 0x56:
		case 0x58:
		case 0x5a:
		case 0x5c:
		case 0x5e:
			SetRotationParameterMap<kParameterA>(address - 0x50,data);
			break;
			//rotation parameter b map
		case 0x60:
		case 0x62:
		case 0x64:
		case 0x66:
		case 0x68:
		case 0x6a:
		case 0x6c:
		case 0x6e:
			SetRotationParameterMap<kParameterB>(address - 0x60,data);
			break;
		case 0x70:
		case 0x72:
		case 0x74:
		case 0x76:
		case 0x78:
		case 0x7a:
		case 0x7c:
		case 0x7e:
			SetScrollZoom<kNbg0>(address - 0x70, data);
			break;
		case 0x80:
		case 0x82:
		case 0x84:
		case 0x86:
		case 0x88:
		case 0x8a:
		case 0x8c:
		case 0x8e:
			SetScrollZoom<kNbg1>(address - 0x80, data);
			break;
		case 0x90:
			scxn2 = data;
			break;
		case 0x92:
			scyn2 = data;
			break;
		case 0x94:
			scxn3 = data;
			break;
		case 0x96:
			scyn3 = data;
			break;
		case 0xa8:
			line_color.lcclmd = data >> 15;
			line_color.address = (line_color.address & 0xFFFF) | ((data & 7) << 16);
			break;
		case 0xaa:
			line_color.address = (line_color.address & 0x70000) | (data & 0xFFFF);
			break;
		case 0xac:
			bkta.bkclmd = data >> 15;
			bkta.bkta_ = (bkta.bkta_ & 0xFFFF) | (data << 16);
			break;
		case 0xae:
			bkta.bkta_ = (bkta.bkta_ & 0x70000) | data & 0xFFFF;
			break;

		case 0x9a:
			scrctl[1].lss = data >> 12;
			scrctl[1].lzmx = data >> 11;
			scrctl[1].lscy = data >> 10;
			scrctl[1].lscx = data >> 9;
			scrctl[1].vcsc = data >> 8;

			scrctl[0].lss = data >> 4;
			scrctl[0].lzmx = data >> 3;
			scrctl[0].lscy = data >> 2;
			scrctl[0].lscx = data >> 1;
			scrctl[0].vcsc = data;
			break;

		case 0x9c:
			vcsta = (vcsta & 0xFFFE) | (data << 16);
			break;
		case 0x9e:
			vcsta = (vcsta & 0x70000) | (data & 0xFFFE);
			break;
		case 0xa0:
			lsta[0] = (lsta[0] & 0xFFFE) | (data << 16);
			break;
		case 0xa2:
			lsta[0] = (lsta[0] & 0x70000) | (data & 0xFFFE);
			break;
			//copy paste fixme
		case 0xa4:
			lsta[1] = (lsta[1] & 0xFFFE) | (data << 16);
			break;
		case 0xa6:
			lsta[1] = (lsta[1] & 0x70000) | (data & 0xFFFE);
			break;
		//rotation parameter stuff
		case 0xb0:
			rotation_parameter_mode = data;
			break;
		case 0xb2:
			break;
		case 0xb4:
			//i don't think making this a function to abtract the params saves any code
			coefficient_table_control[kParameterB].line_color_enable = data >> 12;
			coefficient_table_control[kParameterB].data_mode = data >> 10;
			coefficient_table_control[kParameterB].data_size = data >> 9;
			coefficient_table_control[kParameterB].table_enable = data >> 8;

			coefficient_table_control[kParameterA].line_color_enable = data >> 4;
			coefficient_table_control[kParameterA].data_mode = data >> 2;
			coefficient_table_control[kParameterA].data_size = data >> 1;
			coefficient_table_control[kParameterA].table_enable = data;
			break;
		case 0xb6:
			coefficient_table_address_offset[kParameterB] = data >> 8;
			coefficient_table_address_offset[kParameterA] = data;
			break;
		case 0xb8:
			screen_over_pattern_name[kParameterA] = data;
			break;
		case 0xba:
			screen_over_pattern_name[kParameterB] = data;
			break;
		case 0xbc:
			rptau = data;
			break;
		case 0xbe:
			rptal = data;
			break;
		case 0xc0:
		case 0xc2:
		case 0xc4:
		case 0xc6:
			SetWindowPosition<0>(address - 0xc0, data);
			break;
		case 0xc8:
		case 0xca:
		case 0xcc:
		case 0xce:
			SetWindowPosition<1>(address - 0xc8, data);
			break;
			//window
		case 0xd0:
			SetWindowControl<kNbg1>(data >> 8);
			SetWindowControl<kNbg0>(data);
			break;
		case 0xd2:
			SetWindowControl<kNbg3>(data >> 8);
			SetWindowControl<kNbg2>(data);
			break;
		case 0xd4:
			SetWindowControl<kSprite>(data >> 8);
			SetWindowControl<kRbg0>(data);
			break;
		case 0xd6:
			SetWindowControl<kColorCalculation>(data >> 8);
			SetWindowControl<kRotationParameter>(data);
			break;
			//line window
		case 0xd8:
			lwta[0].enabled = data >> 15;
			lwta[0].address = (lwta[0].address & 0xFFFF) | (data & 7) << 16;
			break;
		case 0xda:
			lwta[0].address = (lwta[0].address & 0x70000) | (data & 0xFFFF);
			break;
		case 0xdc:
			lwta[1].enabled = data >> 15;
			lwta[1].address = (lwta[1].address & 0xFFFF) | (data & 7) << 16;
			break;
		case 0xde:
			lwta[1].address = (lwta[1].address & 0x70000) | (data & 0xFFFF);
			break;


		case 0xe0:
			spctl.spcccs = data >> 12;
			spctl.spccn = data >> 8;
			spctl.spclmd = data >> 5;
			spctl.spwinen = data >> 4;
			spctl.sptype = data;
			break;
		case 0xe2:
			sdctl.tpsdl = data >> 8;
			sdctl.enable[kBack] = data >> 5;
			sdctl.enable[kRbg0] = data >> 4;
			sdctl.enable[kNbg3] = data >> 3;
			sdctl.enable[kNbg2] = data >> 2;
			sdctl.enable[kNbg1] = data >> 1;
			sdctl.enable[kNbg0] = data;
			break;
		case 0xe4:
			color_ram_address_offset[kNbg3] = data >> 12;
			color_ram_address_offset[kNbg2] = data >> 8;
			color_ram_address_offset[kNbg1] = data >> 4;
			color_ram_address_offset[kNbg0] = data;
			break;
		case 0xe6:
			color_ram_address_offset[kSprite] = data >> 4;
			color_ram_address_offset[kRbg0] = data;
			break;
		case 0xe8:
			lnclen[kSprite] = data >> 5;
			lnclen[kRbg0] = data >> 4;
			lnclen[kNbg3] = data >> 3;
			lnclen[kNbg2] = data >> 2;
			lnclen[kNbg1] = data >> 1;
			lnclen[kNbg0] = data;
			break;
		case 0xea:
			printf("fixmE!!!!!");
			sfprmd[kNbg0] = data;
			break;
		case 0xec:
			color_calculation_control.gradation_enable = data >> 15;

			color_calculation_control.gradation_layer_select = data >> 12;

			color_calculation_control.extended_enabled = data >> 10;
			color_calculation_control.ratio_mode = data >> 9;
			color_calculation_control.mode = data >> 8;

			color_calculation_control.enable[kSprite] = data >> 6;
			color_calculation_control.enable[kLineColor] = data >> 5;
			color_calculation_control.enable[kRbg0] = data >> 4;
			color_calculation_control.enable[kNbg3] = data >> 3;
			color_calculation_control.enable[kNbg2] = data >> 2;
			color_calculation_control.enable[kNbg1] = data >> 1;
			color_calculation_control.enable[kNbg0] = data;
			break;
		case 0xee:
			special_color_calculation_mode[kRbg0] = data >> 8;
			special_color_calculation_mode[kNbg3] = data >> 6;
			special_color_calculation_mode[kNbg2] = data >> 4;
			special_color_calculation_mode[kNbg1] = data >> 2;
			special_color_calculation_mode[kNbg0] = data;
			break;

			//sprite priority
		case 0xf0:
			pris[1] = data >> 8;
			pris[0] = data;
			break;
		case 0xf2:
			pris[3] = data >> 8;
			pris[2] = data;
			break;
		case 0xf4:
			pris[5] = data >> 8;
			pris[4] = data;
			break;
		case 0xf6:
			pris[7] = data >> 8;
			pris[6] = data;
			break;
			//layer priority
		case 0xf8:
			prin[kNbg1] = data >> 8;
			prin[kNbg0] = data;
			break;
		case 0xfa:
			prin[kNbg3] = data >> 8;
			prin[kNbg2] = data;
			break;
		case 0xfc:
			prin[kRbg0] = data;
			break;



			//sprite color calculation
		case 0x100:
			ccrs[1] = data >> 8;
			ccrs[0] = data;
			break;
		case 0x102:
			ccrs[3] = data >> 8;
			ccrs[2] = data;
			break;
		case 0x104:
			ccrs[5] = data >> 8;
			ccrs[4] = data;
			break;
		case 0x106:
			ccrs[7] = data >> 8;
			ccrs[6] = data;
			break;

			//bg color calculation
		case 0x108:
			ccr[kNbg1] = data >> 8;
			ccr[kNbg0] = data;
			break;
		case 0x10a:
			ccr[kNbg3] = data >> 8;
			ccr[kNbg0] = data;
			break;
		case 0x10c:
			ccr[kRbg0] = data;
			break;
		case 0x10e:
			ccr[kBack] = data >> 8;
			ccr[kLineColor] = data;
			break;



			//color offset
		case 0x110:
			clofen[kSprite] = data >> 6;
			clofen[kBack] = data >> 5;
			clofen[kRbg0] = data >> 4;
			clofen[kNbg3] = data >> 3;
			clofen[kNbg2] = data >> 2;
			clofen[kNbg1] = data >> 1;
			clofen[kNbg0] = data;
			break;
		case 0x112:
			clofsl[kSprite] = data >> 6;
			clofsl[kBack] = data >> 5;
			clofsl[kRbg0] = data >> 4;
			clofsl[kNbg3] = data >> 3;
			clofsl[kNbg2] = data >> 2;
			clofsl[kNbg1] = data >> 1;
			clofsl[kNbg0] = data;
			break;

			//color offset a
		case 0x114:
			color_offset[0].red = data;
			break;
		case 0x116:
			color_offset[0].green = data;
			break;
		case 0x118:
			color_offset[0].blue = data;
			break;

			//color offset b
		case 0x11a:
			color_offset[1].red = data;
			break;
		case 0x11c:
			color_offset[1].green = data;
			break;
		case 0x11e:
			color_offset[1].blue = data;
			break;
		}
	}
}regs;
