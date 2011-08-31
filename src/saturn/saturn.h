#pragma once
#pragma warning(push, 0)

#include "../standard_headers.h"

//internal headers
#include "../globals.h"
#include "../saturn_types.h"
#include "../saturn_input.h"

#include "../util/array.h"
#include "../util/varint.h"
#include "../util/fixed.h"

namespace saturn
{
	//scsp stuff
	const float kDecayChangeTimeInMilliseconds[64] =//taken from aica, but almost certainly the same for scsp, from 0 to -96db
	{
		1000000,//arbitrary, represents infinity
		1000000,118200,101300,88600,70900,59100,50700,
		44300,35500,29600,25300,22200,17700,14800,12700,
		11100,8900,7400,6300,5500,4400,3700,3200,
		2800,2200,1800,1600,1400,1100,920,
		790,
		690,550,460,390,340,270,230,
		200,170,140,110,98,85,68,57,
		49,43,34,28,25,22,18,14,
		12,11,8.5,7.1,6.1,5.4,4.3,3.6,3.1
	};
	const float kAttackChangeTimeInMilliseconds[64] =
	{
		1000000,//arbitrary, represents infinity
		1000000,8100,6900,6000,4800,4000,3400,
		3000,2400,2000,1700,1500,1200,1000,860,
		760,600,500,430,380,300,250,220,
		190,150,130,110,95,76,63,55,47,
		38,31,27,24,19,15,13,12,9.4,
		7.9,6.8,6.0,4.7,3.8,3.4,
		3.0,2.4,2.0,1.8,1.6,1.3,1.1,
		.93,.85,.65,.53,.44,.40,.35,0,0
	};
	const float OscillationFrequencyTable[32] =
	{
		0.17,0.19,0.23,0.27,0.34,0.39,0.45,0.55,
		0.68,0.78,0.92,1.10,1.39,1.60,1.87,2.27,
		2.87,3.31,3.92,4.79,6.15,7.18,8.60,10.8,
		14.4,17.2,21.5,28.7,43.1,57.4,86.1,172.3
	};	

	//hle bios stuff, this is stateful!
	static u8 sh2masklist[0x20] = {
		0xF0, 0xE0, 0xD0, 0xC0, 0xB0, 0xA0, 0x90, 0x80,
		0x80, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 
		0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 
		0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70
	};

	static u32 scumasklist[0x20] = {
		0xFFFFFFFF, 0xFFFFFFFE, 0xFFFFFFFC, 0xFFFFFFF8,
		0xFFFFFFF0, 0xFFFFFFE0, 0xFFFFFFC0, 0xFFFFFF80,
		0xFFFFFF80, 0xFFFFFE00, 0xFFFFFE00, 0xFFFFFE00,
		0xFFFFFE00, 0xFFFFFE00, 0xFFFFFE00, 0xFFFFFE00,
		0xFFFFFE00, 0xFFFFFE00, 0xFFFFFE00, 0xFFFFFE00,
		0xFFFFFE00, 0xFFFFFE00, 0xFFFFFE00, 0xFFFFFE00,
		0xFFFFFE00, 0xFFFFFE00, 0xFFFFFE00, 0xFFFFFE00,
		0xFFFFFE00, 0xFFFFFE00, 0xFFFFFE00, 0xFFFFFE00
	};

	struct Saturn
	{
		struct Configuration
		{
			bool use_hle_bios;
		} config;

		Saturn() : master(*this),slave(*this),scu(*this), vdp2(vdp1), cd_controller_hle(smpc_hle,*this),bios_hle(*this), scsp(*this), peripherals(*this)
		{
			config.use_hle_bios = true;
		}

		//our basic 3 callbacks for the gui, audio, video, gamepad input
		virtual void InterfaceUpdateAudioSample(const s16 &l, const s16& r){assert(false);}
		virtual void InterfaceUpdateFramebuffer(const u32 *data, const u32&width, const u32&height){assert(false);}
		virtual  SaturnStandardPadInputType InterfaceUpdateInput(){SaturnStandardPadInputType s = {0}; assert(false); return s;}

		//cd interface callbacks
		virtual int InterfaceCdGetPhysicalStatus(){assert(false);return 0;}
		virtual s32 InterfaceCdReadToc(u32* toc){assert(false);return 0;}
		virtual int InterfaceCdReadSectorAtFad(u32 fad, void *buffer){assert(false);return 0;}

		void InterfacePowerOn() 
		{
			Power();
		}

		//lle components
#include "scu/scu.h"
#include "scsp/scsp.h"
#include "vdp1/vdp1.h"
#include "vdp2/vdp2.h"
#include "sh2/sh2.h"
#include "cart/cart.h"

		//hle components
		struct SmpcHle;
		struct Cs2Hle;
#include "../hle/smpc_hle.h"
#include "../hle/cs2_hle.h"
#include "../hle/bios_hle.h"
#include "../hle/load_hle.h"

		//hle component wrappers
#include "cd/cd.h"
#include "smpc/smpc.h"

#include "peripheral/peripheral.h"

		//bbus memory and devices
		Scsp scsp;

		Vdp1 vdp1;
		Vdp2 vdp2;

		//cpu bus memory and devices
		array::RamArray<0x80000> bios_rom;
		SmpcHle smpc_hle;
		array::RamArray<0x10000> backup_ram;
		array::RamArray<0x100000> low_work_ram;
		array::RamArray<0x100000> high_work_ram;

		Sh2 master;
		Sh2 slave;

		BiosHle bios_hle;

		//a bus memory and devices
		CartridgeInterface cartridge;//cs0
		//cs1?
		CdControllerHle cd_controller_hle;//cs2
		//cs3?

		//multiple busses
		Scu scu;

		template <OperationSize size, TransferType type>
		u32 ABusTransfer(u32 address, u32 data)
		{
			if(address >= 0x02000000 && address <= 0x03FFFFFF
				|| address >= 0x22000000 && address <= 0x23FFFFFF )
			{
				if(type == kRead)
				{
					unsigned int result = cartridge.Read<size>(address);
					return result;
				}
				else
				{
					cartridge.Write<size>(address,data);
					return 0;
				}
			}
			else if(address >= 0x04000000 && address <=0x04FFFFFF
				||  address >= 0x24000000 && address <=0x24FFFFFF)
			{
				return 0xFFFFFFFFFF;
				assert(false);
			}
			else if(address >= 0x05800000 && address <= 0x058FFFFF
				||      address >= 0x25800000 && address <= 0x258FFFFF)
			{
				if(type == kRead)
				{
					unsigned int result = cd_controller_hle.Read<size>(address);
					return result;
				}
				else
				{
					cd_controller_hle.Write<size>(address,data);
					return 0;
				}
			}
			else
			{
				//if we've made it all the way here it's an
				//unhandled write
				return 0;
			}
		}

		template <OperationSize size, TransferType type>
		u32 BBusTransfer(u32 address, u32 data)
		{
			if(address >= 0x05A00000 && address <= 0x05AFFFFF ||
				address >= 0x25A00000 && address <= 0x25AFFFFF)
			{
				if(type == kRead)
				{

					unsigned int result = scsp.sound_ram.Read<size>(address & 0xFFFFF);
					return result;
				}
				else
				{
					scsp.sound_ram.Write<size>(address & 0xFFFFF,data);
					return 0;
				}
			}
			else if(address >= 0x05B00000 && address <= 0x05BFFFFF)
			{
				if(type == kRead)
				{
					unsigned int result = scsp.Read<size>(address);
					return result;
				}
				else
				{
					scsp.Write<size>(address,data);
					return 0;
				}
			}
			else if(address >= 0x05C00000 && address <= 0x05C7FFFF)
			{
				if(type == kRead)
				{
					unsigned int result = vdp1.ram.Read<size>(address & 0x7FFFF);
					return result;
				}
				else
				{
					vdp1.ram.Write<size>(address & 0x7FFFF,data);
					return 0;
				}
			}
			else if(address >= 0x05C80000 && address <= 0x05CFFFFF)
			{
				if(type == kRead)
				{
					unsigned int result = vdp1.framebuffer.back.Read<size>(address);
					return result;
				}
				else
				{
					vdp1.framebuffer.back.Write<size>(address,data);
					return 0;
				}
			}
			else if(address >= 0x05D00000 && address <= 0x05D7FFFF)
			{
				if(type == kRead)
				{
					unsigned int result = vdp1.Read<size>(address);
					return result;
				}
				else
				{
					vdp1.Write<size>(address,data);
					return 0;
				}
			}
			else if(address >= 0x05E00000 && address <= 0x05EFFFFF)
			{
				if(type == kRead)
				{
					unsigned int result = vdp2.vdp2_ram.Read<size>(address & 0x7FFFF);
					return result;
				}
				else
				{
					vdp2.vdp2_ram.Write<size>(address & 0x7FFFF,data);
					return 0;
				}
			}
			else if(address >= 0x05F00000 && address <= 0x05F7FFFF)
			{
				if(type == kRead)
				{
					unsigned int result = vdp2.vdp2_color_ram.Read<size>(address & 0xFFF);
					return result;
				}
				else
				{
					vdp2.vdp2_color_ram.Write<size>(address & 0xFFF,data);
					return 0;
				}
			}
			else if(address >= 0x05F80000 && address <=0x05FBFFFF
				|| address >= 0x25F80000 && address <= 0x25FBFFFF)
			{
				if(type == kRead)
				{
					unsigned int result = vdp2.regs.Read(address);
					return result;
				}
				else
				{
					if(size == kWord)
						vdp2.regs.Write(address,data);
					else if(size == kLong)
					{
						vdp2.regs.Write(address,(data >> 16) & 0xFFFF);
						vdp2.regs.Write(address+2,data & 0xFFFF);
					}
					return 0;
				}
			}
			else if(address >= 0x05FE0000 && address <= 0x05FEFFFF
				|| address >= 0x25fe0000 && address <= 0x25feffff)
			{
				if(type == kRead)
				{
					unsigned int result = scu.regs.Read(address,scu.dsp);
					return result;
				}
				else
				{
					scu.regs.Write(address,data,scu.dsp);
					return 0;
				}
			}
			else
			{
				return ABusTransfer<size,type>(address,data);
			}
		}


		template <OperationSize size, TransferType type>
		unsigned int CpuBusTransfer(unsigned int address, unsigned int data = 0)
		{
			//address&=0x1FFFFFFF;
			if(    
				//mirrored every 512 kb
				address >= 0x00000000 && address <= 0x000FFFFF
				||address >= 0x20000000 && address <= 0x200FFFFF
				)
			{
				if(type == kRead)
				{
					unsigned int result = bios_rom.Read<size>(address);
					return result;
				}
				else
				{
					//read only
					return 0;
				}
			}
			else if(
				address >= 0x00100000 && address <= 0x0017FFFF
				|| address >= 0x20100000 && address <= 0x2017FFFF)
			{
				if(type == kRead)
				{
					unsigned int result=smpc_hle.Read<size>(address);
					return result;
				}
				else
				{
					smpc_hle.Write<size>(address,data);
					return 0;
				}
			}
			else if(address >= 0x00018000 && address <= 0x001FFFFF)
			{
				if(type == kRead)
				{
					unsigned int result = backup_ram.Read<size>(address);
					return result;
				}
				else
				{
					backup_ram.Write<size>(address,data);
					return 0;
				}
			}
			else if(address >= 0x00020000 && address <= 0x002FFFFF)
			{
				if(type == kRead)
				{

					unsigned int result = low_work_ram.Read<size>(address & 0xFFFFF);
					return result;
				}
				else
				{
					low_work_ram.Write<size>(address & 0xFFFFF,data);
					return 0;
				}
			}
			else if(address >= 0x06000000 && address <= 0x07FFFFFF)
			{
				if(type == kRead)
				{
					unsigned result = high_work_ram.Read<size>(address & 0xFFFFF);
					return result;
				}
				else
				{
					high_work_ram.Write<size>(address & 0xFFFFF,data);
					return 0;
				}
			}
			else 
				return BBusTransfer<size,type>(address,data);
		}

		void Power()
		{
			smpc_hle.Init(0);

			if(config.use_hle_bios)
				bios_hle.BiosInit();

			master.Reset();
			master.PowerOn();

			cd_controller_hle.LoadImageAndInitialize();

			smpc_hle.smpc_hle_impl.SmpcReset(cd_controller_hle.cs2_hle);
			smpc_hle.smpc_hle_impl.SmpcRecheckRegion(cd_controller_hle.cs2_hle);

			if(config.use_hle_bios)
				YabauseQuickLoadGame();

		}

		//sh2s, vdps and scu are on the same clock
		//vdp2 takes system clock / 4 per pixel

		//scu dsp is 1/2 speed

		//smpc is on 4 mhz clock, rtc chip is something else

		//cd block sh1 is on it's own clock

		//scsp is 2x the m68k speed

		struct TimingState
		{
			u64 master_clock_count;//counts up forever
			int sh2_master_clock_count;
			int sh2_slave_clock_count;
			int m68k_clock_count;//11mhz m68k clock
			int scsp_clocks;//ratio between main cpu and sound clock
			int microseconds;//used for hle parts
			int microsecond_clocks;

			bool execute_m68k;
			bool in_26_mhz_mode;
			bool m68k_is_running;
			bool slave_sh2_is_running;
			int frames_executed;

			static const int scsp_clock_speed = 22579200;
			static const int microsecond_clock_speed = 1000000;

			//the system clock, either 26 or 28mhz
			int master_clock;

			struct
			{
				int hcnt;//current horizontal pixel
				int vcnt;//current vertical line

				bool is_pal;
				
				int horizontal_resolution;
				int lines_per_frame;
				int pixels_per_line;
			}video;

			TimingState()
			{
				master_clock_count = 0;//counts up forever
				sh2_master_clock_count = 0;
				sh2_slave_clock_count = 0;
				m68k_clock_count = 0;
				scsp_clocks = 0;
				microseconds = 0;
				microsecond_clocks = 0;
				video.hcnt = 0;
				video.vcnt = 0;
				execute_m68k = true;//when true, add one 68k cycle
				in_26_mhz_mode = true;
				m68k_is_running = false;
				slave_sh2_is_running = false;
				frames_executed = 0;

				video.is_pal = false;
			}
		}timing;

		u32 display_frame_buffer[704*526*2];

		void HblankIn()
		{
			vdp2.regs.tvstat.hblank = 1;
			scu.SendInterruptRequest<Scu::kHBlankIn>();
		}
		void HblankOut()
		{
			vdp2.regs.tvstat.hblank = 0;
		}
		void VblankIn()
		{
			vdp2.regs.tvstat.vblank = 1;
			scu.SendInterruptRequest<Scu::kVBlankIn>();
		}
		void VblankOut()
		{
			vdp2.regs.tvstat.vblank = 0;
			vdp2.regs.tvstat.odd = 1;

			vdp1.ParseCommands();
			scu.SendInterruptRequest<Scu::kVBlankOut>();
		}

		void NewExecute()
		{
			bool frame_executed = false;

			if(timing.video.is_pal)
				timing.video.lines_per_frame = 313;
			else
				timing.video.lines_per_frame = 262;

			while(!frame_executed)
			{
				if(timing.in_26_mhz_mode)
				{
					timing.master_clock = 26846587;
					timing.video.pixels_per_line = 426-1;//-1 since we start counting from zero
					timing.video.horizontal_resolution = 320;
				}
				else
				{
					timing.master_clock = 28636360;
					timing.video.pixels_per_line = 454-1;
					timing.video.horizontal_resolution = 352;
				}

				timing.master_clock_count++;
				timing.sh2_master_clock_count++;

				if(timing.slave_sh2_is_running)
					timing.sh2_slave_clock_count++;
				else
					timing.sh2_slave_clock_count = 0;

				//sh2s are instruction accurate, not cycle accurate
				if(timing.sh2_master_clock_count > 0)
				{
					master.CheckInterruptRequests();
					timing.sh2_master_clock_count -= master.Execute();
				}

				if(timing.sh2_slave_clock_count > 0)
						timing.sh2_slave_clock_count -= slave.Execute();

				//we don't need to count scu cycles
				//since the dsp always does 1 cycle
				if((timing.master_clock_count & 1 ) == 0)
					scu.dsp.Execute(*this);

				//sound system is on its own clock
				timing.scsp_clocks += 1 * timing.scsp_clock_speed;

				if(timing.scsp_clocks > timing.master_clock)//this is apparently equivalent and saves a division
				{
					//m68k runs at 1/2 the scsp speed
					if(timing.execute_m68k)
						timing.m68k_clock_count++;

					timing.execute_m68k = !timing.execute_m68k;

					if(timing.m68k_clock_count > 0)
						timing.m68k_clock_count -= scsp.m68k.Execute();

					scsp.Execute();
					timing.scsp_clocks -= 1 * timing.master_clock;
				}

				//these are hle and can't be timed correctly
				//1 second == 1000000 usec
				//thus 28636360 sh2 cycles == 1000000 usec
				//thus 28.63636 sh2 cycles == 1 usec
				//so every 28 or so cycles, we tick a usec
				timing.microsecond_clocks += 1 * timing.microsecond_clock_speed;

				if(timing.microsecond_clocks > timing.master_clock)
				{
					timing.microseconds++;

					cd_controller_hle.Execute(timing.microseconds);
					smpc_hle.Execute(timing.microseconds,*this);
					timing.microsecond_clocks -= 1 * timing.master_clock;
				}

				//the vdp2 takes 4 main clocks to write a pixel
				//apparently in 2x horizontal res it writes at 2x the speed
				//the reason it can do that is because it combines 2 layers, thus it has 2x the output
				//we currently don't emulate that, fixme
				if((timing.master_clock_count & 3)==0)
				{
					int start_of_display = 53;//number of "pixels" before active display starts horizontally
					int vblank_in_starting_line = 224;

					int x = timing.video.hcnt - start_of_display;
					int y = timing.video.vcnt;

					if(x > 0 && y > 0)
						if(x < timing.video.horizontal_resolution && y < 240)
							display_frame_buffer[x+(y*704)] = vdp2.GeneratePixel(x,y,vdp1);

					if(timing.video.hcnt == 0)
					{
						if(timing.video.vcnt == 0)
						{
							VblankOut();
						}
						timing.video.hcnt++;
					}
					else if(timing.video.hcnt == start_of_display)
					{
						HblankOut();
						timing.video.hcnt++;
					}
					else if(timing.video.hcnt == timing.video.horizontal_resolution)
					{
						HblankIn();
						timing.video.hcnt++;
					}
					else if(timing.video.hcnt == timing.video.pixels_per_line)
					{
						//move to the next line
						timing.video.hcnt = 0;
						timing.video.vcnt++;
					}
					else
						timing.video.hcnt++;

					if(timing.video.vcnt == vblank_in_starting_line)
					{
						smpc_hle.StopIntback();
						VblankIn();
					}
					else if(timing.video.vcnt == timing.video.lines_per_frame)
					{

						timing.video.vcnt = 0;
						frame_executed = true;
						InterfaceUpdateFramebuffer(display_frame_buffer,704,524);
						printf("timing.frames_executed %d\n",timing.frames_executed);
						timing.frames_executed++;
					}
				}
			}
		}
	};
}