//all vdp1 shapes are drawn with bresenham lines
//it's not real 3d hardware in any way
//just a line plotting chip with a framebuffer
//the way concave shapes behave is a quirk of this drawing process
//and isn't really intentional on the part of the designers

struct Vdp1 {

	//the ram and framebuffers are connected only to the vdp1 so
	//it makes sense to put them here i think
	//acess to these from b bus has to go through the vdp1
	//first
	array::RamArray<0x80000> ram;

	u32 cycles;//not a very accurate count at all, just a ballpark guess to escape from situations where the game uploads a huge display list

	template <OperationSize size>
	u32 ReadVram(u32 address)
	{
		//not sure if 8 bit reads are faster than 16 bit or not
		cycles++;
		return ram.Read<size>(address);

	}
	struct Vdp1Framebuffer//2 frames, 0x40000 each
	{
		array::RamArray<0x40000> front;
		array::RamArray<0x40000> back;
	}framebuffer;

	template <OperationSize size>
	void WriteFramebuffer(u32 address,u16 data)
	{
		/*
		A 16 bit write to the framebuffer seems to take 1 cycle.  

		This is derived from the framebuffer erase figures:

		ntsc 320x224 has 263 lines total, meaning 38 vblank lines
		320 h means the sh2 is running at 26846587 hz
		26846587 / 263 / 60 fps = 1701.304626108999 cycles per 
		line (or something like that)

		vdp1 manual says 1708 cycles in a scanline
		1708 * 38 vblank lines means 64904 cycles in vblank
		if it can erase 58812 pixels during vblank it means it 
		takes 1.1 cycles to erase a single pixel

		the math is slightly off but this pretty strongly 
		suggests that a word write to the framebuffer takes 1 
		vdp1 cycle.  is an 8 bit write to the framebuffer the 
		same?
		*/
		cycles++;
		framebuffer.back.Write<size>(address,data);
	}
	template <OperationSize size>
	u16 ReadFramebuffer(u32 address)
	{

		cycles++;
		return framebuffer.back.Read<size>(address);

	}
	template <OperationSize size>
	unsigned int  Read(u32 address)
	{

		return 0;
	}

	template <OperationSize size>
	void Write(u32 address, u32 data)
	{
		regs.WriteWord(address,data);
	}

	struct Registers {
		struct Tvmr {
			u1 vbe;
			u3 tvm;
		} tvmr;
		struct Fbcr {
			u1 eos;
			u1 die;
			u1 dil;
			u1 fcm;
			u1 fct;
		}fbcr;

		u2 ptmr;
		u16 ewdr;

		struct Ewlr {
			u6 upper_left_coordinate_x1;
			u9 upper_left_coordinate_y1;
		}ewlr;

		struct Ewrr {
			u7 lower_right_coordinate_x3;
			u9 lower_right_coordinate_y3;
		}ewrr;

		u16 endr;

		struct Edsr {
			u1 cef;
			u1 bef;
		}edsr;

		u16 lopr;//beware of bottom 2 bits
		u16 copr;

		struct Modr {
			u4 ver;
			u1 ptm1;
			u1 eos;
			u1 die;
			u1 dil;
			u1 fcm;
			u1 vbe;
			u3 tvm;
		}modr;

		void WriteWord(int address, u16 data) {
			switch(address & 0xF) 
			{
			case 0:
				tvmr.vbe = data >> 3;
				tvmr.tvm = data & 7;
				break;
			case 2:
				fbcr.eos = data >> 4;
				fbcr.die = data >> 3;
				fbcr.dil = data >> 2;
				fbcr.fcm = data >> 1;
				fbcr.fct = data & 1;
				break;
			case 4:
				ptmr = data;
				break;
			case 6:
				ewdr = data;
				break;
			case 8:
				ewlr.upper_left_coordinate_x1 = data >> 9;
				ewlr.upper_left_coordinate_y1 = data;
				break;
			case 0xA:
				ewrr.lower_right_coordinate_x3 = data >> 9;
				ewrr.lower_right_coordinate_y3 = data;
				break;
			case 0xc:
				//kill drawing, hmmm
				break;
				//rest are read only
			default:
				printf("unhandled vdp1 word write \n");
			}


		}
		u16 ReadWord(u32 address)
		{
			//fixme
			switch(address & 0xf)
			{
			case 0x10:
				break;
			case 0x12:
				break;
			case 0x14:
				break;
			case 0x16:
				break;
			default:
				printf("unhandled vdp1 read\n");
				return 0;
				break;
			}
		}
		Registers() {}
	private:
		DISALLOW_COPY_AND_ASSIGN(Registers);
	}regs;

	struct CommandTable {
		struct Cmdctrl {
			u1 end;
			u3 jp;
			u4 zp;
			u2 dir;
			u4 comm;
		}cmdctrl;

		CommandTable() {}

		u16 cmdlink;

		struct Cmdpmod {
			u1 mon;
			u1 hss;
			u1 pclp;
			u1 clip;
			u1 cmod;
			u1 mesh;
			u1 ecd;
			u1 spd;
			u3 color_mode;
			u3 color_calculation_bits;
		}cmdpmod;

		u16 cmdcolr;
		u16 cmdsrca;

		struct Cmdsize {
			u6 character_size_x_div_8;
			u8 character_size_y;
		}cmdsize;

		//todo what about the sign extension?
		s11 cmdxa;
		s11 cmdya;
		s11 cmdxb;
		s11 cmdyb;
		s11 cmdyc;
		s11 cmdxc;
		s11 cmdxd;
		s11 cmdyd;

		u16 cmdgrda;

		void ReadCommandTable(int address, Vdp1& vdp1) {

#define read(offset) vdp1.ReadVram<kWord>(address+offset)

			//todo ought to wrap around yeah?
			if(address > 58320)
				return;

			int cmdctrl_temp = read(0);

			cmdctrl.end = cmdctrl_temp >> 15;
			cmdctrl.jp = cmdctrl_temp >> 12;
			cmdctrl.zp = cmdctrl_temp >> 8;
			cmdctrl.dir = cmdctrl_temp >> 4;
			cmdctrl.comm = cmdctrl_temp;

			cmdlink = read(0x2);

			int cmdpmod_temp = read(0x4);
			cmdpmod.mon = cmdpmod_temp >> 15;
			cmdpmod.hss = cmdpmod_temp >> 12;
			cmdpmod.pclp = cmdpmod_temp >> 11;
			cmdpmod.clip = cmdpmod_temp >> 10;
			cmdpmod.cmod = cmdpmod_temp >> 9;
			cmdpmod.mesh = cmdpmod_temp >> 8;
			cmdpmod.ecd = cmdpmod_temp >> 7;
			cmdpmod.spd = cmdpmod_temp >> 6;
			cmdpmod.color_mode = cmdpmod_temp >> 3;
			cmdpmod.color_calculation_bits = cmdpmod_temp;

			cmdcolr = read(0x6);

			cmdsrca = read(0x8);

			int cmdsize_temp = read(0xA);
			cmdsize.character_size_x_div_8 = cmdsize_temp >> 8;
			cmdsize.character_size_y = cmdsize_temp;

#define read_(offset) vdp1.ReadVram<kWord>(address+offset);

			cmdxa = read_(0xC);
			cmdya = read_(0xE);
			cmdxb = read_(0x10);
			cmdyb = read_(0x12);
			cmdxc = read_(0x14);
			cmdyc = read_(0x16);
			cmdxd = read_(0x18);
			cmdyd = read_(0x1A);
			cmdgrda = read(0x1C);
#undef read
#undef read_
		}
	private:
		DISALLOW_COPY_AND_ASSIGN(CommandTable);
	};

	struct Clipping {
		struct User {
			int upper_left_xa;
			int upper_left_ya;
			int lower_right_xc;
			int lower_right_yc;
		}user;

		struct System {
			int lower_right_xc;
			int lower_right_yc;
		}system;

		bool cmod;//user clipping enabled or not
		bool clip;//inside or outside

		Clipping() : cmod(false), clip(false) {}

		void SystemClipping(const CommandTable & command) {
			system.lower_right_xc = command.cmdxc;
			system.lower_right_yc = command.cmdyc;
		}
		void UserClipping(const CommandTable & command) {
			user.lower_right_xc = command.cmdxc;
			user.lower_right_yc = command.cmdyc;
			user.upper_left_xa = command.cmdxa;
			user.upper_left_ya = command.cmdya;
		}

		bool CoordinateIsClipped(const int & x, const int & y, const CommandTable & command) {

			//system clipping
			//negative settings for system
			//are incorrect but should be
			//verified with a test
			if(x < 0)
				return true;
			if(y < 0)
				return true;

			if(x > system.lower_right_xc)
				return true;
			if(y > system.lower_right_yc)
				return true;

			//user clipping
			//once again, should verify what
			//happens with the incorrect settings

			//draw inside
			if(!command.cmdpmod.clip) {//TODO fixme
				if(x < user.upper_left_xa)
					return true;
				if(x > user.lower_right_xc)
					return true;
				if(y < user.upper_left_ya)
					return true;
				if(y > user.lower_right_yc)
					return true;
			}
			else {
				//draw outside

				assert(false);
			}

			return false;

		}
	private:
		DISALLOW_COPY_AND_ASSIGN(Clipping);
	}clipping;

	struct ScreenInfo {
		int frame_buffer_width;
		int frame_buffer_height;
		int bits_per_pixel;

		ScreenInfo() : frame_buffer_width(0), frame_buffer_height(0), bits_per_pixel(0) {}

		void Set(const int &width, const int & height, const int & bits)
		{
			frame_buffer_width = width;
			frame_buffer_height = height;
			bits_per_pixel = bits;
		}

		void SetScreenMode(const Registers & regs) {
			enum ScreenModes {
				kNormal,
				kHighResolution,
				kRotation16,
				kRotation8,
				kHdtv
			};
			switch(regs.tvmr.tvm)
			{
			case kNormal:
				Set(512,256,16);
				break;
			case kHighResolution:
				Set(1024,256,8);
				break;
			case kRotation16:
				Set(512,256,16);
				break;
			case kRotation8:
				Set(512,512,8);
				break;
			case kHdtv:
				Set(512,256,16);
				break;
			}
		}
	private:
		DISALLOW_COPY_AND_ASSIGN(ScreenInfo);
	}screen_mode;

	void EraseFrameBuffer() {
		//8bit uses same math as 16 i think?
		int upper_left_x = regs.ewlr.upper_left_coordinate_x1 * 8;
		int lower_right_x = (regs.ewrr.lower_right_coordinate_x3 * 8) - 1;
		int upper_left_y = regs.ewlr.upper_left_coordinate_y1;
		int lower_right_y = regs.ewrr.lower_right_coordinate_y3;
		for(int x = upper_left_x; x < lower_right_x; x++) {
			for(int y = upper_left_y; y < lower_right_y; y++) {
				int write_address = (y*screen_mode.frame_buffer_width)+x;
				write_address*=2;
				WriteFramebuffer<kWord>(write_address,regs.ewdr);
				if(x >= screen_mode.frame_buffer_width)
					break;
				if(y >= screen_mode.frame_buffer_height)
					break;
			}
		}
	}

	void ParseCommands() {
		const int kUnlikelyReturnAddress = 0xdeadbeef;

		int current_address = 0;
		int return_address = kUnlikelyReturnAddress;

		//not really correct
		cycles = 0;

		if(!regs.ptmr)
		  return;

		//hmm these don't match the manual see 52
		regs.edsr.bef = regs.edsr.cef;
		regs.edsr.cef = 0;

		screen_mode.SetScreenMode(regs);

		EraseFrameBuffer();

		CommandTable command;

		command.ReadCommandTable(current_address,*this);
		while(!command.cmdctrl.end)
		{
			//todo pseudo draw continuation
			//also the clock changes according to sh2 clock yeah?
			if(cycles > 28636360)
				break;

			//if not the "jump to next table" command
			if(command.cmdctrl.jp != 4)
			{
				switch(command.cmdctrl.comm) 
				{
				case 0:
					NormalSprite(command);
					break;
				case 1:
					ScaledSprite(command);
					break;
				case 2:
					DistortedSprite(command);
					break;
				case 4:
					Polygon(command);
					break;
				case 5:
					Polyline(command);
					break;
				case 6:
					Line(command);
					break;
				case 8:
					clipping.UserClipping(command);
					break;
				case 9:
					clipping.SystemClipping(command);
					break;
				case 10:
					local_coordinates.LocalCoordinate(command);
					break;
				default:
					assert(false);

				}
			}
			enum JumpModes {
				kJumpNext,
				kJumpAssign,
				kJumpCall,
				kJumpReturn,
				kSkipNext,
				kSkipAssign,
				kSkipCall,
				kSkipReturn
			};

			switch(command.cmdctrl.jp & 3) 
			{
			case kJumpNext:
				current_address+=0x20;
				break;
			case kJumpAssign:
				current_address = command.cmdlink * 8;
				break;
			case kJumpCall:
				if(return_address == kUnlikelyReturnAddress) {
					return_address = current_address + 0x20;
					current_address = command.cmdlink * 8;
				}
				break;
			case kJumpReturn:
				if(return_address != kUnlikelyReturnAddress) {
					current_address = return_address;
					return_address = kUnlikelyReturnAddress;
				}
				else
					return_address = kUnlikelyReturnAddress;
				break;
				//we skipped earlier, now increment the address
			case kSkipNext:
				current_address+=0x20;
				break;
			default:
				printf("unhandled vdp1 command\n");
				//assert(false);
			}
			command.ReadCommandTable(current_address,*this);
		}

		//hmm this doesn't match the manual see 52
		regs.edsr.cef = true;
		printf("VDP1 CYCLES: %d\n",cycles);
	}

	struct Gouraud {
		//todo preferably replace this with something endian agnostic
		typedef union ColorUnion { // xbgr x555
			struct {
#ifdef WORDS_BIGENDIAN
				u16 x:1;
				u16 b:5;
				u16 g:5;
				u16 r:5;
#else
				u16 r:5;
				u16 g:5;
				u16 b:5;
				u16 x:1;
#endif
			};
			u16 value;
		} ColorFixed;

		struct ColorDouble {
			double r;
			double g;
			double b;
			int ToColor() {
				return 
					(((static_cast<int>(r))&0x1F)
					|(((static_cast<int>(g))&0x1F)<<5)
					|(((static_cast<int>(b))&0x1F)<<10) 
					|0x8000 );
			}
		};

		ColorFixed a;
		ColorFixed b;
		ColorFixed c;
		ColorFixed d;

		ColorDouble LineSetup(
			const int & length, 
			const ColorFixed & starting_color_table, 
			const ColorFixed & ending_color_table,
			ColorDouble *color_fixed_to_color_double
			)
		{
			ColorDouble interpolated_color_increment;
			interpolated_color_increment.r =Interpolate(starting_color_table.r,ending_color_table.r,length);
			interpolated_color_increment.g =Interpolate(starting_color_table.g,ending_color_table.g,length);
			interpolated_color_increment.b =Interpolate(starting_color_table.b,ending_color_table.b,length);

			color_fixed_to_color_double->r = starting_color_table.r;
			color_fixed_to_color_double->g = starting_color_table.g;
			color_fixed_to_color_double->b = starting_color_table.b;

			return interpolated_color_increment;
		}

		explicit Gouraud(const CommandTable & command,Vdp1 &vdp1) {
			int gouraud_table_address = command.cmdgrda << 3;
			a.value = vdp1.ReadVram<kWord>(gouraud_table_address);
			b.value = vdp1.ReadVram<kWord>(gouraud_table_address+2);
			c.value = vdp1.ReadVram<kWord>(gouraud_table_address+4);
			d.value = vdp1.ReadVram<kWord>(gouraud_table_address+6);
		}

		//todo the real chip is obviously not floating point
		//probably not fixed point either
		//it's just bresenham i would bet
		static double Interpolate(const double & start_value, const double & end_value, const int & number_of_steps){

			double one_step_increment_value = 0;

			if(number_of_steps == 0)
				return 1;

			one_step_increment_value = (end_value - start_value) / number_of_steps;

			return one_step_increment_value;
		}
	private:
		DISALLOW_COPY_AND_ASSIGN(Gouraud);
	};

	struct Interpolator {
		float current_value;
		int end;
		int number_of_steps;
		int start;

		Interpolator(
			const int &start_, 
			const int &end_,
			const int &number_of_steps_
			) : current_value(start_), end(end_),  counter(0),
			number_of_steps(number_of_steps_),
			increment(0)
		{
			start = start_;
			increment = (float)( end - current_value) / (float)number_of_steps;
		}

		bool Step()
		{
			//we have to stop stepping
			//if we've reached the end
			if( !(counter<=number_of_steps))
				return true;

			current_value += increment;

			counter++;
			return !(counter<=number_of_steps);
		}
	private:
		float increment;
		int counter;
	};

	struct GouraudStepper
	{
		Interpolator red;
		Interpolator green;
		Interpolator blue;

		GouraudStepper(
			int red1, int green1, int blue1,
			int red2, int green2, int blue2,
			int major) 
			: red(Interpolator(red1,red2,major)), 
			green(Interpolator(green1,green2,major)), 
			blue(Interpolator(blue1,blue2,major))
		{
		}

		void Step() {
			red.Step();
			green.Step();
			blue.Step();
		}
	};
	bool PixelIsEndcode(
		const int & color_mode, 
		const int & pixel, 
		const bool & endcodes_disabled)
	{
		struct Endcodes {
			enum {
				k16ColorBank = 0xF,
				k16ColorLut = 0xF,
				k64Color = 0xFF,
				k128Color = 0xFF,
				k256Color = 0xFF,
				kRgb = 0x7FFF
			};
		};

		if(endcodes_disabled)
			return false;

		//TODO examine HSS stuff

		switch(color_mode)
		{
		case k16ColorBank:
			if(pixel == Endcodes::k16ColorBank)
				return true;
			break;
		case k16ColorLut:
			if(pixel == Endcodes::k16ColorLut)
				return true;
			break;
		case k64Color:
			if(pixel == Endcodes::k64Color)
				return true;
			break;
		case k128Color:
			if(pixel == Endcodes::k128Color)
				return true;
			break;
		case k256Color:
			if(pixel == Endcodes::k256Color)
				return true;
			break;
		case kRgb:
			if(pixel == Endcodes::kRgb)
				return true;
			break;
		default:
			assert(false);
		}
		return false;
	}

	bool PixelIsTransparent(
		const int & color_mode, 
		const int & pixel,
		const bool & transparent_pixels_disabled)
	{

		if(transparent_pixels_disabled)
			return false;

		switch(color_mode)
		{
		case k16ColorBank:
			if((pixel & 0xF) == 0)
				return true;
			break;
		case k16ColorLut:
			if((pixel & 0xF) == 0)
				return true;
			break;
		case k64Color:
			if((pixel & 0xFF) == 0)
				return true;
			break;
		case k128Color:
			if((pixel & 0xFF) == 0)
				return true;
			break;
		case k256Color:
			if((pixel & 0xFF) == 0)
				return true;
			break;
		case kRgb:
			//sonic xtreme demo
			if((pixel & 0xFFFF) <= 0x7FFE)
				return true;
			break;
		default:
			assert(false);
		}
		assert(false);
		return false;
	}
	void FlipAdjust(
		const int & direction, 
		const int & character_width, 
		const int& character_height, 
		int * current_line_index, 
		int * line_number) {

			enum FlipSettings {
				kNormal,
				kHorizontal,
				kVertical,
				kBoth
			};			
			switch(direction) {
			case kNormal:
				break;
			case kHorizontal:
				*current_line_index = character_width - *current_line_index-1;
				break;
			case kVertical:
				*line_number = character_height - *line_number-1;
				break;
			case kBoth:
				*line_number = character_height - *line_number-1;
				*current_line_index = character_width - *current_line_index-1;
				break;
			default:
				assert(false);
			}
	}

	enum ColorModes {
		k16ColorBank,
		k16ColorLut,
		k64Color,
		k128Color,
		k256Color,
		kRgb
	};
	bool ShapeIsUntextured(const CommandTable & command) {
		if(
			command.cmdctrl.comm == 4 || 
			command.cmdctrl.comm == 5 || 
			command.cmdctrl.comm == 6)
			return true;

		return false;

	}
	void StepLine(
		int top_left_x,int top_left_y,
		int bottom_left_x,int bottom_left_y,
		int top_right_x,int top_right_y,
		int bottom_right_x,int bottom_right_y,
		const CommandTable &command)
	{
		//apparently if something is beyond this it just gives up and doesn't draw?
		//test me on hardware please
		//also the masking of our command coordinates is wrong probably
		if(top_left_x > 1023 || top_left_x < -1024) return;
		if(top_left_y > 1023 || top_left_y < -1024) return;

		if(bottom_left_x > 1023 || bottom_left_x < -1024) return;
		if(bottom_left_y > 1023 || bottom_left_y < -1024) return;

		if(top_right_x > 1023 || top_right_x < -1024) return;
		if(top_right_y > 1023 || top_right_y < -1024) return;

		if(bottom_right_x > 1023 || bottom_right_x < -1024) return;
		if(bottom_right_y > 1023 || bottom_right_y < -1024) return;

		int character_width = command.cmdsize.character_size_x_div_8 * 8;
		int character_height = command.cmdsize.character_size_y;

#undef max

		int left_length = std::max(get_d(top_left_x,bottom_left_x),get_d(top_left_y,bottom_left_y));
		int right_length = std::max(get_d(top_right_x,bottom_right_x),get_d(top_right_y,bottom_right_y));
		int master_length = std::max(left_length,right_length);

		Interpolator left_x(top_left_x,bottom_left_x,master_length);
		Interpolator left_y(top_left_y,bottom_left_y,master_length);

		Interpolator y_texture_interpolator(0,character_height,master_length);

		Interpolator right_x(top_right_x,bottom_right_x,master_length);
		Interpolator right_y(top_right_y,bottom_right_y,master_length);

		Gouraud gouraud(command,*this);

		GouraudStepper gouraud_stepper_left(
			gouraud.a.r,gouraud.a.g,gouraud.a.b,
			gouraud.d.r,gouraud.d.g,gouraud.d.b, master_length);

		GouraudStepper gouraud_stepper_right(
			gouraud.b.r,gouraud.b.g,gouraud.b.b,
			gouraud.c.r,gouraud.c.g,gouraud.c.b, master_length);

		struct Point
		{
			int x;
			int y;

			Point(int _x, int _y) : x(_x), y(_y) {}
		};

		Point previous_position(top_left_x,top_left_y);

		for(int i = 0; i < master_length;i++)
		{
			int number_of_endcodes_detected = 0;

			int dx = get_d(left_x.current_value,right_x.current_value);
			int dy = get_d(left_y.current_value,right_y.current_value);

			int horizontal_d = std::max(
				dx,
				dy);

			Interpolator horizontal_x_interpolator(left_x.current_value,right_x.current_value,horizontal_d);
			Interpolator horizontal_y_interpolator(left_y.current_value,right_y.current_value,horizontal_d);

			Interpolator x_texture_interpolator(0,character_width,horizontal_d);

			GouraudStepper x_gouraud_stepper(
				gouraud_stepper_left.red.current_value,
				gouraud_stepper_left.green.current_value,
				gouraud_stepper_left.blue.current_value,

				gouraud_stepper_right.red.current_value,
				gouraud_stepper_right.green.current_value,
				gouraud_stepper_right.blue.current_value, 
				horizontal_d);

			for(int i = 0; i < horizontal_d; i++) 
			{
				int pixel_to_write_to_framebuffer = 0;

				if(GetPixel(y_texture_interpolator.current_value,x_texture_interpolator.current_value, &pixel_to_write_to_framebuffer,command))
					number_of_endcodes_detected++;

				if(number_of_endcodes_detected == 2)
					break;

				Gouraud::ColorDouble current_gouraud_color = {
					x_gouraud_stepper.red.current_value,
					x_gouraud_stepper.green.current_value,
					x_gouraud_stepper.blue.current_value};
#if 1
					WritePixelToFramebuffer(
						horizontal_x_interpolator.current_value,
						horizontal_y_interpolator.current_value,
						pixel_to_write_to_framebuffer,command,
						current_gouraud_color);
#endif
					{
						int x = horizontal_x_interpolator.current_value;
						int y = horizontal_y_interpolator.current_value;

#define putpixel(x,y) WritePixelToFramebuffer( \
	x,\
	y,\
	pixel_to_write_to_framebuffer,command,\
	current_gouraud_color);

#if 1
						bool ychanged = previous_position.y != (int)y;
						if(ychanged && abs(dx)>abs(dy)) 
						{
							//travelling right and down
							if(dx > 0 && dy > 0) putpixel(x,y-1);
							//travelling left and down
							if(dx < 0 && dy > 0) putpixel(x+1,y);
							//travelling left and up
							if(dx < 0 && dy < 0) putpixel(x,y+1);
							//travelling right and up
							if(dx > 0 && dy < 0) putpixel(x-1,y);				
						}
						bool xchanged = previous_position.x != (int)x;
						if(xchanged && abs(dy)>abs(dx))
						{
							//travelling right and down
							if(dx>0 && dy>0) putpixel(x,y-1);
							//travelling left and down
							if(dx<0 && dy>0) putpixel(x+1,y);
							//travelling left and up
							if(dx<0 && dy<0) putpixel(x,y+1);
							//travelling right and up
							if(dx>0 && dy<0) putpixel(x-1,y);
						}

						previous_position.x = horizontal_x_interpolator.current_value;
						previous_position.y = horizontal_y_interpolator.current_value;
#endif
					}

					horizontal_x_interpolator.Step();
					horizontal_y_interpolator.Step();
					x_texture_interpolator.Step();
					x_gouraud_stepper.Step();

			}//end horizontal loop

			gouraud_stepper_left.Step();
			gouraud_stepper_right.Step();

			left_x.Step();
			left_y.Step();

			right_x.Step();
			right_y.Step();

			y_texture_interpolator.Step();
		}//end vertical loop
	}

	int GouraudAdjustment(int color, const int & table_value )
	{
		color += (table_value - 0x10);

		if ( color < 0 ) color = 0;
		if ( color > 0x1f ) color = 0x1f;

		return color;
	}

	int ProcessGouraudPixel(
		const int & input_pixel, 
		const CommandTable & command,
		const Gouraud::ColorDouble & gouraud_color) {

#define create_color(r,g,b) (((r)&0x1F)|(((g)&0x1F)<<5)|(((b)&0x1F)<<10) |0x8000 )

			//handle the special case demonstrated in the sgl chrome demo
			//if we are in a paletted bank mode and the other two colors are unused, adjust the index value instead of rgb
			//todo is this actually a special case or am i just handling the math wrong in general?
			if(
				(command.cmdpmod.color_mode != 5) &&
				(command.cmdpmod.color_mode != 1) && 
				(int)gouraud_color.g == 16 && 
				(int)gouraud_color.b == 16) 
			{
				int c = (int)(gouraud_color.r-0x10);
				if(c < 0) c = 0;
				return input_pixel+c;
			}
			return create_color(
				GouraudAdjustment(
				input_pixel&0x001F,
				(int)gouraud_color.r),

				GouraudAdjustment(
				(input_pixel&0x03e0) >> 5,
				(int)gouraud_color.g),

				GouraudAdjustment(
				(input_pixel&0x7c00) >> 10,
				(int)gouraud_color.b)
				);
#undef create_color
	}

	u32 AlphaBlend(u32 d, u32 s, u32 level)
	{
		int r,g,b,sr,sg,sb,dr,dg,db;

		int invlevel = 256-level;
		sr = s & 0x001f; dr = d & 0x001f; 
		r = (sr*level + dr*invlevel)>>8; r&= 0x1f;
		sg = s & 0x03e0; dg = d & 0x03e0;
		g = (sg*level + dg*invlevel)>>8; g&= 0x03e0;
		sb = s & 0x7c00; db = d & 0x7c00;
		b = (sb*level + db*invlevel)>>8; b&= 0x7c00;
		return r|g|b;
	}

	int ProcessHalfTransparentPixel(const int & top_pixel, const int & bottom_pixel) {
		int level = (1 << 7);
		return AlphaBlend(top_pixel,bottom_pixel,level) | ( 1 << 15);
	}

	void WritePixelToFramebuffer(
		const int & x, 
		const int & y, 
		int pixel_to_write, 
		const CommandTable & command,
		const Gouraud::ColorDouble & gouraud_current_color) {

			//let's make this cost some cycles as an approximation
			cycles+=4;

			if(pixel_to_write == 0)
				return;

			if(command.cmdpmod.mesh && (x^y)&1)
				return;

			if(x < 0)
				return;
			if(y < 0)
				return;
			if(x >= screen_mode.frame_buffer_width)
				return;
			if(y >= screen_mode.frame_buffer_height)
				return;

			//if(clipping.CoordinateIsClipped(x,y,command))
			//return;

			enum ColorCalculationSettings{
				kReplace,
				kShadow,
				kHalfLuminance,
				kHalfTransparent,
				kGouraud,
				kProhibited,//5
				kGouraudAndHalfLuminance,
				kGouraudAndHalfTransparent
			};

			u32 write_address = (y*screen_mode.frame_buffer_width)+x;

			int frame_buffer_pixel = 0;

			switch(command.cmdpmod.color_calculation_bits)
			{
			case kReplace:			
				break;
			case kShadow:
				assert(false);
				break;
			case kHalfLuminance:
				assert(false);
				break;
			case kHalfTransparent:
				pixel_to_write = ProcessHalfTransparentPixel(pixel_to_write,frame_buffer_pixel);
				break;
			case kGouraud:
				pixel_to_write = ProcessGouraudPixel(pixel_to_write,command,gouraud_current_color);
				break;
			case kGouraudAndHalfTransparent:
				//i'm not sure what order this happens in
				pixel_to_write = ProcessGouraudPixel(pixel_to_write,command,gouraud_current_color);
				pixel_to_write = ProcessHalfTransparentPixel(pixel_to_write,frame_buffer_pixel);
				break;
			default:
				assert(false);
			}

			//wait, i assume this only works in 16 bit mode right? otherwise
			//you are setting 2 pixels at once?
			if(command.cmdpmod.mon) {
				u16 pixel = ReadFramebuffer<kWord>(write_address*2);
				WriteFramebuffer<kWord>(write_address*2, pixel | (1<<15));
				return;
			}
			if(screen_mode.bits_per_pixel == 16)
				WriteFramebuffer<kWord>(write_address*2,pixel_to_write);
			else
				WriteFramebuffer<kByte>(write_address,pixel_to_write);
	}

	struct LocalCoordinates {
		int AdjustX(const int & x_coordinate) {
			return x_coordinate + offset_xa;
		}

		int AdjustY(const int & y_coordinate) {
			return y_coordinate + offset_ya;
		}
		void LocalCoordinate(const CommandTable & command) {
			offset_xa = command.cmdxa;
			offset_ya = command.cmdya;
		}
		int offset_xa;
		int offset_ya;
	}local_coordinates;

	void DrawEnd(const CommandTable & command)  {assert(false);}

	int get_d(int start, int end)
	{
		int d;
		if (end>start) {
			d = end-start;
		}
		else {
			d = start-end;
		}

		return d;
	}

	u16 Pixel16ColorLookup( u32 base, u32 offset, const CommandTable & command) {

		if(ShapeIsUntextured(command))
			return command.cmdcolr;

		u16 dot = ReadVram<kByte>( ( base + (offset>>1)) & 0x7FFFF );
		if ((offset & 0x1) == 0) 
			dot >>= 4; // Even pixel
		else 
			dot &= 0xF; // Odd pixel
		return dot;
	}
	template <u8 mask>
	u16 PixelByteSizeLookup(u32 base, u32 offset, const CommandTable &command)
	{
		if(ShapeIsUntextured(command))
			return command.cmdcolr;
		return ReadVram<kByte>(( base + offset ) & 0x7FFFF) & mask;
	}

	u16 PixelRgbColorLookup( u32 base, u32 offset, const CommandTable & command) {

		if(ShapeIsUntextured(command))
			return command.cmdcolr;

		return ReadVram<kWord>( ( base + 2*offset) & 0x7FFFF );
	}

	bool CheckEndcode(const int color_mode, const bool endcodes_disabled, int *output_pixel)
	{
		bool is_endcode = false;

		if(PixelIsEndcode(color_mode,*output_pixel,endcodes_disabled))
			is_endcode = true;
		else
			is_endcode = false;

		if(is_endcode)
			return true;

		return false;
	}
	bool CheckTransparent(const int color_mode, const bool transparent_pixels_disabled, int *output_pixel)
	{
		if(PixelIsTransparent(color_mode,*output_pixel,transparent_pixels_disabled))
		{
			*output_pixel = 0;
			return true;
		}
		return false;
	}

	//returns true for an endcode
	bool GetPixel(int x_offset, int y_offset, int *output_pixel, const CommandTable & command) {

		int character_width = command.cmdsize.character_size_x_div_8 * 8;
		int character_height = command.cmdsize.character_size_y;
		int character_address = command.cmdsrca << 3;
		int color_mode = command.cmdpmod.color_mode;
		bool endcodes_disabled = command.cmdpmod.ecd;
		bool transparent_pixels_disabled = command.cmdpmod.spd;

		int color_lut = command.cmdcolr << 3;

		FlipAdjust(command.cmdctrl.dir,character_width,character_height,&y_offset,&x_offset);

		switch(color_mode) 
		{
		case k16ColorBank:
			*output_pixel = Pixel16ColorLookup(character_address + (x_offset*(character_width>>1)), y_offset, command );
			if(CheckEndcode(color_mode,endcodes_disabled,output_pixel))
				return true;
			if(!CheckTransparent(color_mode,transparent_pixels_disabled,output_pixel))
				*output_pixel = command.cmdcolr | *output_pixel;
			break;
		case k16ColorLut:
			*output_pixel = Pixel16ColorLookup(character_address + (x_offset*(character_width>>1)), y_offset, command );
			if(CheckEndcode(color_mode,endcodes_disabled,output_pixel))
				return true;
			if(!CheckTransparent(color_mode,transparent_pixels_disabled,output_pixel))
				*output_pixel = ReadVram<kWord>( (*output_pixel * 2 + color_lut) & 0x7FFFF);
			break;
		case k64Color:
			*output_pixel = PixelByteSizeLookup<0x3f>( character_address + (x_offset*(character_width)), y_offset,command );
			if(CheckEndcode(color_mode,endcodes_disabled,output_pixel))
				return true;
			if(!CheckTransparent(color_mode,transparent_pixels_disabled,output_pixel))
				*output_pixel = command.cmdcolr | *output_pixel;
			break;
		case k128Color:
			*output_pixel = PixelByteSizeLookup<0x7f>( character_address + (x_offset*(character_width)), y_offset,command );
			if(CheckEndcode(color_mode,endcodes_disabled,output_pixel))
				return true;
			if(!CheckTransparent(color_mode,transparent_pixels_disabled,output_pixel))
				*output_pixel = command.cmdcolr | *output_pixel;
			break;
		case k256Color:
			*output_pixel = PixelByteSizeLookup<0xff>( character_address + (x_offset*character_width), y_offset , command);
			if(CheckEndcode(color_mode,endcodes_disabled,output_pixel))
				return true;
			if(!CheckTransparent(color_mode,transparent_pixels_disabled,output_pixel))
				*output_pixel = command.cmdcolr | *output_pixel;
			break;
		case kRgb:
			*output_pixel = PixelRgbColorLookup( character_address + (x_offset*character_width*2), y_offset , command);
			if(CheckEndcode(color_mode,endcodes_disabled,output_pixel))
				return true;
			break;
		default:
			assert(false);
		}
		return false;
	}

	void NormalSprite(const CommandTable & command)  {
		int sprite_width = command.cmdsize.character_size_x_div_8 * 8;
		int sprite_height = command.cmdsize.character_size_y;

		int top_left_x = local_coordinates.AdjustX(command.cmdxa);
		int top_left_y = local_coordinates.AdjustY(command.cmdya);

		int top_right_x = top_left_x + sprite_width;
		int top_right_y = top_left_y;
		int bottom_right_x = top_right_x;
		int bottom_right_y = top_left_y+sprite_height;
		int bottom_left_x = top_left_x;
		int bottom_left_y = bottom_right_y;

		StepLine(top_left_x,top_left_y,bottom_left_x,bottom_left_y,top_right_x,top_right_y,bottom_right_x,bottom_right_y,command);
	}

	void ScaledSprite(const CommandTable & command)  {

		int horizontal_offset = command.cmdctrl.zp & 3;
		int vertical_offset = (command.cmdctrl.zp >> 2) & 3;

		int left_side;
		int top_side;
		int right_side;
		int bottom_side;

		int xa = local_coordinates.AdjustX(command.cmdxa);
		int xb = command.cmdxb;

		int ya = local_coordinates.AdjustY(command.cmdya);
		int yb = command.cmdyb;

		if(command.cmdctrl.zp == 0)
		{
			left_side = xa;
			right_side = local_coordinates.AdjustX(command.cmdxc);
			top_side = ya;
			bottom_side = local_coordinates.AdjustY(command.cmdyc);
		}
		else {
			//xb is x width
			//yb is y width
			switch(horizontal_offset) {
			case 1://left
				left_side = xa;
				right_side = xa + xb;
				break;
			case 2://center
				left_side = xa - xb / 2;
				right_side = xa + (xb + 1) / 2;
				break;
			case 3://right
				left_side = xa - xb;
				right_side = xa;
				break;
			}
			switch(vertical_offset) {
			case 1://top
				top_side = ya;
				bottom_side = ya + yb;
				break;
			case 2://center
				top_side = ya - yb / 2;
				bottom_side = ya + (yb + 1) / 2;
				break;
			case 3://bottom
				top_side = ya - yb;
				bottom_side = ya;
				break;
			}
		}

		int top_left_x = left_side;
		int top_left_y = top_side;

		int top_right_x = right_side;
		int top_right_y = top_side;

		int bottom_left_x = left_side;
		int bottom_left_y = bottom_side;

		int bottom_right_x = right_side;
		int bottom_right_y = bottom_side;

		StepLine(
			top_left_x,top_left_y,
			bottom_left_x,bottom_left_y,
			top_right_x,top_right_y,
			bottom_right_x,bottom_right_y,
			command);
	}

	void DistortedSprite(const CommandTable & command)  {

		int top_left_x = local_coordinates.AdjustX(command.cmdxa);
		int top_left_y = local_coordinates.AdjustY(command.cmdya);

		int bottom_left_x = local_coordinates.AdjustX(command.cmdxd);
		int bottom_left_y = local_coordinates.AdjustY(command.cmdyd);

		int top_right_x = local_coordinates.AdjustX(command.cmdxb);
		int top_right_y = local_coordinates.AdjustY(command.cmdyb);

		int bottom_right_x = local_coordinates.AdjustX(command.cmdxc);
		int bottom_right_y = local_coordinates.AdjustY(command.cmdyc);

		StepLine(
			top_left_x, 
			top_left_y, 
			bottom_left_x, 
			bottom_left_y,
			top_right_x, 
			top_right_y,
			bottom_right_x, 
			bottom_right_y,
			command);
	}

	//texturing is simply skipped later
	void Polygon(const CommandTable & command)  {
		DistortedSprite(command);
	}

	void Polyline(const CommandTable & command)  {

		int x[4], y[4];

		x[0] = local_coordinates.AdjustX(command.cmdxa);
		y[0] = local_coordinates.AdjustY(command.cmdya);

		x[1] = local_coordinates.AdjustX(command.cmdxb);
		y[1] = local_coordinates.AdjustY(command.cmdyb);

		x[2] = local_coordinates.AdjustX(command.cmdxc);
		y[2] = local_coordinates.AdjustY(command.cmdyc);

		x[3] = local_coordinates.AdjustX(command.cmdxd);
		y[3] = local_coordinates.AdjustY(command.cmdyd);

		StepLine(x[0], y[0], x[1], y[1],x[0]+1, y[0]+1, x[1]+1, y[1]+1,command);
		StepLine(x[1], y[1], x[2], y[2],x[1]+1, y[1]+1, x[2]+1, y[2]+1,command);
		StepLine(x[2], y[2], x[3], y[3],x[2]+1, y[2]+1, x[3]+1, y[3]+1,command);
		StepLine(x[0], y[0], x[3], y[3],x[0]+1, y[0]+1, x[3]+1, y[3]+1,command);

	}
	void Line(const CommandTable & command)  {

		int x1 = local_coordinates.AdjustX(command.cmdxa);
		int y1 = local_coordinates.AdjustY(command.cmdya);
		int x2 = local_coordinates.AdjustX(command.cmdxb);
		int y2 = local_coordinates.AdjustY(command.cmdyb);

		StepLine(x1, y1, x2, y2,x1+1, y1+1, x2+1, y2+1,command);
	}
private:
	//	DISALLOW_COPY_AND_ASSIGN(Vdp1__);
};