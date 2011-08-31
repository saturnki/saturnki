namespace vdp2
{
	struct ColorCount
	{
		enum
		{
			k16,
			k256,
			k2048,
			k32768,
			k16770000
		};
	};

	struct BitmapSize
	{
		enum
		{
			k512x256,
			k512x512,
			k1024x256,
			k1024x512
		};
	};

	struct ColorRamMode
	{
		enum
		{
			k5bit1024,
			k8bit2048,
			k8bit1024
		};
	};

	namespace regs
	{
		typedef vu16 * const Ptr;

		Ptr tvmd = (vu16*)0x25f80000;
		Ptr exten = (vu16*)0x25F80002;
		Ptr tvstat = (vu16*)0x25F80004;
		Ptr vrsize = (vu16*)0x25F80006;
		Ptr hcnt = (vu16*)0x25F80008;
		Ptr vcnt = (vu16*)0x25F8000A;
		//reserve
		Ptr ramctl = (vu16*)0x25F8000E;

		Ptr cyca0l = (vu16*)0x25F80010;
		Ptr cyca0u = (vu16*)0x25F80012;
		Ptr cyca1l = (vu16*)0x25F80014;
		Ptr cyca1u = (vu16*)0x25F80016;
		Ptr cycb0l = (vu16*)0x25F80018;
		Ptr cycb0u = (vu16*)0x25F8001A;
		Ptr cycb1l = (vu16*)0x25F8001C;
		Ptr cycb1u = (vu16*)0x25F8001E;

		Ptr bgon = (vu16*)0x25F80020;
		Ptr mzctl = (vu16*)0x25F80022;
		Ptr sfsel = (vu16*)0x25F80024;
		Ptr sfcode = (vu16*)0x25F80026;
		Ptr chctla = (vu16*)0x25F80028;
		Ptr chctlb = (vu16*)0x25F8002A;
		Ptr bmpna = (vu16*)0x25F8002C;
		Ptr bmpnb = (vu16*)0x25F8002E;

		Ptr pncn0 = (vu16*)0x25F80030;
		Ptr pncn1 = (vu16*)0x25F80032;
		Ptr pncn2 = (vu16*)0x25F80034;
		Ptr pncn3 = (vu16*)0x25F80036;
		Ptr pncr = (vu16*)0x25F80038;
		Ptr plsz = (vu16*)0x25F8003A;
		Ptr mpofn = (vu16*)0x25F8003C;
		Ptr mpofr = (vu16*)0x25F8003E;

		Ptr mpabn0 = (vu16*)0x25F80040;
		Ptr mpcdn0 = (vu16*)0x25F80042;
		Ptr mpabn1 = (vu16*)0x25F80044;
		Ptr mpcdn1 = (vu16*)0x25F80046;
		Ptr mpabn2 = (vu16*)0x25F80048;
		Ptr mpcdn2 = (vu16*)0x25F8004A;
		Ptr mpabn3 = (vu16*)0x25F8004C;
		Ptr mpcdn3 = (vu16*)0x25F8004E;

		Ptr mpabra = (vu16*)0x25F80050;
		Ptr mpcdra = (vu16*)0x25F80052;
		Ptr mpefra = (vu16*)0x25F80054;
		Ptr mpghra = (vu16*)0x25F80056;
		Ptr mpijra = (vu16*)0x25F80058;
		Ptr mpklra = (vu16*)0x25F8005A;
		Ptr mpmnra = (vu16*)0x25F8005C;
		Ptr mpopra = (vu16*)0x25F8005E;

		Ptr mpabrb = (vu16*)0x25F80060;
		Ptr mpcdrb = (vu16*)0x25F80062;
		Ptr mpefrb = (vu16*)0x25F80064;
		Ptr mpghrb = (vu16*)0x25F80066;
		Ptr mpijrb = (vu16*)0x25F80068;
		Ptr mpklrb = (vu16*)0x25F8006A;
		Ptr mpmnrb = (vu16*)0x25F8006C;
		Ptr mpoprb = (vu16*)0x25F8006E;

		Ptr scxin0 = (vu16*)0x25F80070;
		Ptr scxdn0 = (vu16*)0x25F80072;
		Ptr scyin0 = (vu16*)0x25F80074;
		Ptr scydn0 = (vu16*)0x25F80076;
		Ptr zmxin0 = (vu16*)0x25F80078;
		Ptr zmxdn0 = (vu16*)0x25F8007A;
		Ptr zmyin0 = (vu16*)0x25F8007C;
		Ptr zmydn0 = (vu16*)0x25F8007E;

		Ptr scxin1 = (vu16*)0x25F80080;
		Ptr scxdn1 = (vu16*)0x25F80082;
		Ptr scyin1 = (vu16*)0x25F80084;
		Ptr scydn1 = (vu16*)0x25F80086;
		Ptr zmxin1 = (vu16*)0x25F80088;
		Ptr zmxdn1 = (vu16*)0x25F8008A;
		Ptr zmyin1 = (vu16*)0x25F8008C;
		Ptr zmydn1 = (vu16*)0x25F8008E;

		Ptr scxn2 = (vu16*)0x25F80090;
		Ptr scyn2 = (vu16*)0x25F80092;
		Ptr scxn3 = (vu16*)0x25F80094;
		Ptr scyn3 = (vu16*)0x25F80096;
		Ptr zmctl = (vu16*)0x25F80098;
		Ptr scrctl = (vu16*)0x25F8009A;
		Ptr vcstau = (vu16*)0x25F8009C;
		Ptr vcstal = (vu16*)0x25F8009E;

		Ptr lsta0u = (vu16*)0x25F800A0;
		Ptr lsta0l = (vu16*)0x25F800A2;
		Ptr lsta1u = (vu16*)0x25F800A4;
		Ptr lsta1l = (vu16*)0x25F800A6;
		Ptr lctau = (vu16*)0x25F800A8;
		Ptr lctal = (vu16*)0x25F800AA;
		Ptr bktau = (vu16*)0x25F800AC;
		Ptr bktal = (vu16*)0x25F800AE;

		Ptr rpmd = (vu16*)0x25F800B0;
		Ptr rprctl = (vu16*)0x25F800B2;
		Ptr ktctl = (vu16*)0x25F800B4;
		Ptr ktaof = (vu16*)0x25F800B6;
		Ptr ovpnra = (vu16*)0x25F800B8;
		Ptr ovpnrb = (vu16*)0x25F800BA;
		Ptr rptau = (vu16*)0x25F800BC;
		Ptr rptal = (vu16*)0x25F800BE;

		Ptr wpsx0 = (vu16*)0x25F800C0;
		Ptr wpsy0 = (vu16*)0x25F800C2;
		Ptr wpex0 = (vu16*)0x25F800C4;
		Ptr wpey0 = (vu16*)0x25F800C6;
		Ptr wpsx1 = (vu16*)0x25F800C8;
		Ptr wpsy1 = (vu16*)0x25F800CA;
		Ptr wpex1 = (vu16*)0x25F800CC;
		Ptr wpey1 = (vu16*)0x25F800CE;

		Ptr wctla = (vu16*)0x25F800D0;
		Ptr wctlb = (vu16*)0x25F800D2;
		Ptr wctlc = (vu16*)0x25F800D4;
		Ptr wctld = (vu16*)0x25F800D6;
		Ptr lwta0u = (vu16*)0x25F800D8;
		Ptr lwta0l = (vu16*)0x25F800DA;
		Ptr lwta1u = (vu16*)0x25F800DC;
		Ptr lwta1l = (vu16*)0x25F800DE;

		Ptr spctl = (vu16*)0x25F800E0;
		Ptr sdctl = (vu16*)0x25F800E2;
		Ptr craofa = (vu16*)0x25F800E4;
		Ptr craofb = (vu16*)0x25F800E6;
		Ptr lnclen = (vu16*)0x25F800E8;
		Ptr sfprmd = (vu16*)0x25F800EA;
		Ptr ccctl = (vu16*)0x25F800EC;
		Ptr sfccmd = (vu16*)0x25F800EE;

		Ptr prisa = (vu16*)0x25F800F0;
		Ptr prisb = (vu16*)0x25F800F2;
		Ptr prisc = (vu16*)0x25F800F4;
		Ptr prisd = (vu16*)0x25F800F6;
		Ptr prina = (vu16*)0x25F800F8;
		Ptr prinb = (vu16*)0x25F800FA;
		Ptr prir = (vu16*)0x25F800FC;
		//reserve

		Ptr ccrsa = (vu16*)0x25F80100;
		Ptr ccrsb = (vu16*)0x25F80102;
		Ptr ccrsc = (vu16*)0x25F80104;
		Ptr ccrsd = (vu16*)0x25F80106;
		Ptr ccrna = (vu16*)0x25F80108;
		Ptr ccrnb = (vu16*)0x25F8010A;
		Ptr ccrr = (vu16*)0x25F8010C;
		Ptr ccrlb = (vu16*)0x25F8010E;

		Ptr clofen = (vu16*)0x25F80110;
		Ptr clofsl = (vu16*)0x25F80112;
		Ptr coar = (vu16*)0x25F80114;
		Ptr coag = (vu16*)0x25F80116;
		Ptr coab = (vu16*)0x25F80118;
		Ptr cobr = (vu16*)0x25F8011A;
		Ptr cobg = (vu16*)0x25F8011C;
		Ptr cobb = (vu16*)0x25F8011E;
	}

	const u32 kVramBaseAddress = 0x25E00000;
	const u32 kCramBaseAddress = 0x25F00000;
	const u32 kRegsBaseAddress = 0x25F80000;

	vu16 * const vram = (vu16 *)kVramBaseAddress;
	vu16 * const cram = (vu16 *)kCramBaseAddress;
	vu16 * const vdp2_regs = (vu16 *)kRegsBaseAddress;

	void Clear()
	{
		int i;
		for(i = 0; i < 0x40000; i++)//0x80000 total
			vram[i] = 0x0000;

		for(i = 0; i < 0x0800; i++)//0x1000 total
			cram[i] = 0x0000;

		for(i = 0; i < 0x90; i++)//0x120 total
			vdp2_regs[i] = 0x0000;
	}

	void WaitForVblank()
	{
		while((*regs::tvstat & 8) == 0);
		while((*regs::tvstat & 8) == 8);
	}
}
