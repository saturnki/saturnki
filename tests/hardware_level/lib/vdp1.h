namespace vdp1
{
	const u32 kVramStartAddress = 0x25C00000;
	 const u32 kFramebufferStartAddress = 0x25C80000;
	 const u32 kRegsStartAddress = 0x25D00000;

	 typedef vu16 * const Ptr;

	Ptr vram = (vu16 *)kVramStartAddress;
	 Ptr framebuffer = (vu16 *)kFramebufferStartAddress;
	 Ptr vdp1_regs = (vu16 *)kRegsStartAddress;

	 namespace command
	{
		u32 list_index = 0;

		 enum
		{
			kCmdctrl,
			kCmdlink,
			kCmdpmod,
			kCmdcolr,
			kCmdsrca,
			kCmdsize,
			kCmdxa,
			kCmdya,
			kCmdxb,
			kCmdyb,
			kCmdxc,
			kCmdyc,
			kCmdxd,
			kCmdyd,
			kCmdgrda
		};

		vu16* GetListPointer()
		 {
			return (u16 *)(kVramStartAddress + (list_index << 5));
		 }

		 void IncrementList()
		 {
			list_index++;
		 }

		 void ResetList()
		 {
			list_index = 0;
		 }

		 void SystemClipping(
			 u16 lower_right_x, 
			u16 lower_right_y)
		 {
			vu16 *p = GetListPointer();
			p[kCmdctrl] = 0x0009;//jmp not set
			p[kCmdlink] = 0x0000;
			p[kCmdxc] = lower_right_x;
			p[kCmdyc] = lower_right_y;
			IncrementList();
		 }


		 void UserClipping(
			 u16 upper_left_x, u16 upper_left_y,
			u16 lower_right_x, u16 lower_right_y)
		 {
			vu16 *p = GetListPointer();
			p[kCmdctrl] = 0x0008;//jmp not set
			p[kCmdlink] = 0x0000;
			p[kCmdxa]   = upper_left_x;
			p[kCmdya]   = upper_left_y;
			p[kCmdxc]   = lower_right_x;
			p[kCmdyc]   = lower_right_y;
			IncrementList();
		 }

		 void LocalCoordinate(u16 x, u16 y)
		 {
			vu16 *p = GetListPointer();
			p[kCmdctrl] = 0x000A;
			p[kCmdlink] = 0x0000;
			p[kCmdxa]   = x;
			p[kCmdya]   = y;
			IncrementList();
		 }

		 enum ColorModes
		{
			kColorBank16,
			kLookupTable16,
			kColorBank64,
			kColorBank128,
			kColorBank256,
			kRgb
		};

		enum ColorCalculationModes
		{
			kReplace,
			kShadow,
			kHalfLuminance,
			kHalfTransparency,
			kGouraudShading = 4,
			kGouraudShadingHalfLuminance,
			kGouruadShadingHalfTransparency
		};


#define do_four_points() \
	ptr[kCmdxa]   = p.xa;\
	ptr[kCmdya]   = p.ya;\
	ptr[kCmdxb]   = p.xb;\
	ptr[kCmdyb]   = p.yb;\
	ptr[kCmdxc]   = p.xc;\
	ptr[kCmdyc]   = p.yc;\
	ptr[kCmdxd]   = p.xd;\
	ptr[kCmdyd]   = p.yd;


		void Polygon(util::Points &p,
			 u16 color,
			u8 color_calculation,
			u32 gouraud_table_address,
			int msbon
			)
		 {
			vu16 *ptr = GetListPointer();

			ptr[kCmdctrl] = 0x0004;
			ptr[kCmdlink] = 0x0000;

			u32 color_mode = 0;//always 0 for nontextured i guess

			ptr[kCmdpmod] = msbon << 15 | 1 << 7 | 1 << 6 | color_mode << 3 | color_calculation;//kGouraudShading
			ptr[kCmdcolr] = color;// | 0x8000;//msb must be set

			do_four_points();

			ptr[kCmdgrda] = (u16)(gouraud_table_address >> 3);
			IncrementList();
		 }

		 /*
		D----A
		|    |
		|    |
		C----B
		*/
		 void Vdp1CommandDistortedSprite(util::Points &p,
			 u8 mesh_enabled,
			u8 color_mode,
			u8 color_calculation,
			u16 source_address,
			u8 width,
			u8 height)
		 {
			vu16 *ptr = GetListPointer();
			ptr[kCmdctrl] = 0x0002;
			ptr[kCmdlink] = 0x0000;

			ptr[kCmdpmod] = 
				0x0 
				| (mesh_enabled << 8) 
				| (color_mode << 3) 
				| color_calculation;

			ptr[kCmdcolr] = 0;//ignoring, rgb mode for now

			ptr[kCmdsrca] = source_address;
			ptr[kCmdsize] = (((width >> 3) & 0x3F) << 8) | (height & 0xFF);

			do_four_points();

			IncrementList();
		 }

		 void DrawEnd()
		 {
			vu16 *p = GetListPointer();
			p[kCmdctrl] = 0x8000;
			IncrementList();
		 }
	}

	namespace regs
	{
		Ptr tvmr = (vu16*)0x25D00000;
		Ptr fbcr = (vu16*)0x25D00002;
		 Ptr ptmr = (vu16*)0x25D00004;
		 Ptr ewdr = (vu16*)0x25D00006;
		 Ptr ewlr = (vu16*)0x25D00008;
		 Ptr ewrr = (vu16*)0x25D0000A;
		 Ptr endr = (vu16*)0x25D0000C;
		 //unused?

		 Ptr edsr = (vu16*)0x25D00010;
		 Ptr lopr = (vu16*)0x25D00012;
		 Ptr copr = (vu16*)0x25D00014;
		 Ptr modr = (vu16*)0x25D00016;
	}


	void Clear()
	 {
		int i;
		for(i = 0; i < 0x40000; i++)
			vram[i] = 0x0000;

		for(i = 0; i < 0x20000; i++)
			framebuffer[i] = 0x0000;

		for(i = 0; i < 0xb; i++)
			vdp1_regs[i] = 0x0000;
	 }
}
