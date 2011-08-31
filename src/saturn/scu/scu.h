/*
todo, fix all the sign extension in the dsp and do it by hand
all this casting is really unintuitive and error prone

todo everything here ought to be checked against hardware

in general this thing needs some sort of test written for it
with as much coverage as possible
*/

struct Scu
{
	Saturn &saturn;

	Scu(Saturn &saturn_) : saturn(saturn_), dsp(saturn_), regs(saturn_) {
	regs.ims = 0xBFFF;
	}
	enum InterruptType//doubles as the status bit mask i guess
	{
		kVBlankIn = 1 << 0,
		kVBlankOut = 1 << 1,
		kHBlankIn = 1 << 2,
		kTimer0 = 1 << 3,
		kTimer1 = 1 << 4,
		kDspEnd = 1 << 5,
		kSoundRequest = 1 << 6,
		kSystemManager = 1 << 7,
		kPadInterrupt = 1 << 8,
		kLevel2DmaEnd = 1 << 9,
		kLevel1DmaEnd = 1 << 10,
		kLevel0DmaEnd = 1 << 11,
		kDmaIllegal = 1 << 12,
		kSpriteDrawEnd = 1 << 13,
		kExternalInterrupt = 1 << 16//these aren't used are they?
	};
	//Scu(bbus::BBus &bbus_, cpubus::CpuBus &cpu_bus_) : bbus(bbus_), cpu_bus(cpu_bus_) {}

#if 0
	struct StartingFactors
	{
		enum _
		{
			VBlankIn,
			VBlankOut,
			HBlankIn,
			Timer0,
			Timer1,
			SoundRequest,
			SpriteDrawEnd
		};
	};

	template <Interrupts interrupt, StartingFactors::_ starting_factor>
	void StartingFactorDma()
	{
		if(dma.level[0].start_factor == starting_factor)
			dma.Execute<dma.level[0],interrupt,0>();
		if(dma.level[1].start_factor == starting_factor)
			dma.Execute<dma.level[1],interrupt,1>();
		if(dma.level[2].start_factor == starting_factor)
			dma.Execute<dma.level[2],interrupt,2>();
	}
#endif

#include "scu_dsp.h"

	Dsp dsp;

	struct Registers
	{
		Saturn &saturn;

		Registers(Saturn &saturn_) : saturn(saturn_) {}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		struct Dma
		{
			struct DmaLevel
			{
				u27 read_address;
				u27 write_address;
				u32 transfer_byte_number;
				u1 read_address_add;
				u3 write_address_add;
				u1 enable;
				u1 starting;
				u1 mode;
				u1 read_address_update;
				u1 write_address_update;
				u3 start_factor;
			};

			array::Array<DmaLevel,3> level;

			template <int lev>
			void LevelWrite(int address, int data,Saturn&saturn)
			{
				switch(address)
				{
				case 0:
					level[lev].read_address = data;
					break;
				case 4:
					level[lev].write_address = data;
					break;
				case 8:
					level[lev].transfer_byte_number = data;
					break;
				case 0xC:
					level[lev].read_address_add = data >> 8;
					level[lev].write_address_add = data;
					break;
				case 0x10:
					level[lev].enable = data >> 8;
					level[lev].starting = data;

					if(level[lev].starting)
						DmaExecute<lev>(
						level[lev].read_address_add,
						level[lev].write_address_add,
						level[lev].transfer_byte_number,
						level[lev].write_address,
						level[lev].read_address,
						level[lev].mode,
						saturn);
					break;
				case 0x14:
					level[lev].mode = data >> 24;
					level[lev].read_address_update = data >> 16;
					level[lev].write_address_update = data >> 8;
					level[lev].start_factor = data;
					break;
				default:
					break;
				}
			}

			template <int lev>

			u32 LevelRead(int address)
			{
				switch(address)
				{
				case 0:
					return level[lev].read_address;
					break;
				case 4:
					return level[lev].write_address;
				case 8:
					return level[lev].transfer_byte_number;
					break;
				case 0xC:
					{
						int result = 0;
						result |= level[lev].read_address_add << 8;
						result |= level[lev].write_address_add;
						return result;
					}
					break;
				case 0x10:
					{
						int result = 0;
						result |= level[lev].enable << 8;
						result |= level[lev].starting;
						return result;
					}
					break;
				case 0x14:
					{
						int result = 0;
						result |= level[lev].mode << 24;
						result |= level[lev].read_address_update << 16;
						result |= level[lev].write_address_update << 8;
						result |= level[lev].start_factor;
						return result;
					}
					break;
				default:
					break;
				}
		assert(false);
		return 0;
			}
			u1 force_stop;

			struct Status
			{
				u1 dsp_bus_access;
				u1 b_bus_access;
				u1 a_bus_access;
				u1 level_1_interrupt;
				u1 level_0_interrupt;
				u1 level_2_standby;
				u1 level_2_in_operation;
				u1 level_1_standby;
				u1 level_1_in_operation;
				u1 level_0_standby;
				u1 level_0_in_operation;
				u1 dsp_side_standby;
				u1 dsp_side_in_operation;
			};
			Status status;

			//scu dma write to a bus is prohibited

			//scu dma read from vdp2 area 
			//( i guess vdp2, vdp2ram, vdp2cram) is  prohibited
			
			//scu dma cannot read or write to low work ram

			//the sh2s cannot access a or b bus
			//when a->b dma or b->a dma is in effect


			void ExecuteIndirect()
			{
			}
				template <int level_number>
			void ExecuteDirect(
				const u32 read_address_add_value, 
				const u32 write_address_add_value,
				u32 transfer_byte_number,
				u32 write_address,
				u32 read_address,
				Saturn &saturn
				)
			{
				//direct mode
				if(level_number > 0)
				{
					transfer_byte_number &= 0xfff;

					if(transfer_byte_number == 0)
						transfer_byte_number = 0x1000;
				}
				else
				{
					if(transfer_byte_number == 0)
						transfer_byte_number = 0x100000;
				}

				if((write_address >= 0x5A00000) && (write_address < 0x5FF0000))
				{
					u32 bytes_written = 0;

					while(bytes_written < transfer_byte_number)
					{
						//dma to b bus
						//but the source can be on the cpu bus, aka high w ram
						u32 data = saturn.CpuBusTransfer<kLong,kRead>(read_address,0);

						//our endianness is messed up apparently?
						saturn.BBusTransfer<kLong,kWrite>(write_address,data);
						write_address+=write_address_add_value*2;

						read_address+=read_address_add_value;
						bytes_written+=4;
					}

				//	busdebugging::CheckVdp1RamAll();
				}
				else if((write_address >= 0x02000000) && (write_address< 0x058FFFFF))
				{
					printf("scu dma to a-bus devices is prohibited");
				}
				else if(
					(write_address >= 0x00020000 && write_address <= 0x002FFFFF)
					|| (read_address >= 0x00020000 && read_address <= 0x002FFFFF))
				{
					printf("scu dma to or from low work ram is prohibited");
				}
				else
					printf("unhandled dma");
			}			

			u32 GetReadAddressAddValue(u32 input)
			{
				switch(input)
				{
				case 0: return 0;
				case 1: return 4;
				}
				assert(false);
				return 0;
			}
			u32 GetWriteAddressAddValue(u32 input)
			{
				switch(input)
				{
				case 0: return 0;
				case 1: return 2;
				case 2: return 4;
				case 3: return 8;
				case 4: return 16;
				case 5: return 32;
				case 6: return 64;
				case 7: return 128;
				}
				assert(false);
				return 0;
			}

			template <int level_number>
			void DmaExecute(
				const u32 read_address_add, 
				const u32 write_address_add,
				const u32 transfer_byte_number,
				const u32 write_address,
				const u32 read_address,
				const u32 mode,
				Saturn &saturn
				)
			{
				const u32 read_address_add_value = GetReadAddressAddValue(read_address_add);
				const u32 write_address_add_value = GetWriteAddressAddValue(write_address_add);

				if(mode)
					ExecuteIndirect();
				else
					ExecuteDirect<level_number>(
					read_address_add_value, 
					write_address_add_value,
					transfer_byte_number,
					write_address,
					read_address,
					saturn);

				if(level_number == 0) saturn.scu.SendInterruptRequest<kLevel0DmaEnd>();
				if(level_number == 1) saturn.scu.SendInterruptRequest<kLevel1DmaEnd>();
				if(level_number == 2) saturn.scu.SendInterruptRequest<kLevel2DmaEnd>();
			}
		};

		Dma dma;


		struct ProgramControl
		{
			u1 execute_pause_reset;//pr
			u1 execute_pause; //ep
			u1 transfer_0; //t0
			//sign
			//zero
			//carry
			//overflow
			u1 end;//e
			u1 step_execute_control;//es
			u1 program_execute;//ex
			u1 load_enable;//le
		//	u8> program_ram_address;
		};

		ProgramControl dsp_program_control;

		u10 timer_0_compare;
		u9 timer_1_set_data;

		struct Timer1Mode
		{
			u1 mode;
			u1 operation_enable;
		};
		Timer1Mode timer_1_mode;
		//nothing
		//InterruptMask;
		//InterruptStatus;

		//it's actually easier to not break these
		//up into structs
		u32 ims;
		u32 isr;

		//this makes no sense to me
		struct ABusSet
		{
			struct CsSpace
			{
				u1 pre_read_significant_bit;
				u1 precharge_insertion_bit_after_write;
				u1 precharge_insertion_bit_after_read;
				u1 external_wait_significant_bit;
				u4 burst_cycle_wait_no_set;
				u4 single_cycle_wait_no_set;
				u2 burst_length_set;
				u1 bus_size_set;
			};

			CsSpace cs0_space;
			CsSpace cs1_space;
			CsSpace cs2_space;//burst,single do not exist for this!!!!
			CsSpace spare_space;
		};

		u1 sdram_select;
		u4 version;

		//write long for now i guess
		void Write(int address, int data, Dsp & dsp)
		{
			address &= 0xff;

			switch(address)
			{
			case 0:
			case 4:
			case 8:
			case 0xC:
			case 0x10:
			case 0x14:
				dma.LevelWrite<0>(address,data,saturn);
				break;
			case 0x20:
			case 0x24:
			case 0x28:
			case 0x2C:
			case 0x30:
			case 0x34:
				dma.LevelWrite<1>(address-0x20,data,saturn);
				break;
			case 0x40:
			case 0x44:
			case 0x48:
			case 0x4C:
			case 0x50:
			case 0x54:
				dma.LevelWrite<2>(address-0x40,data,saturn);
				break;
			case 0x60:
				dma.force_stop = data;
				break;
			case 0x7C:
				//read only
				break;
			case 0x80:
				dsp_program_control.execute_pause_reset = data >> 26;
				dsp_program_control.execute_pause = data >> 25;
				//t0 read only
				//sign read only
				//zero read only
				//carry read only
				//overflow read only
				//end read only
				dsp_program_control.step_execute_control = data >> 17;
				dsp_program_control.program_execute = data >> 16;
				dsp_program_control.load_enable = data >> 15;
				dsp.regs.program_ram_address = data;

				dsp.regs.program_execute_control = data >> 16;
				printf("-------NEW write\n");


				//i'm guessing that if you simultaneously
				//write program ram addr and load enable it uses the new addr

				//program ram addr
				dsp.regs.program_ram_address = data;

				//load enable
				if((data >> 15) & 1)
				{
					printf("-------NEW pc set 80 write\n");
					dsp.regs.pc = dsp.regs.program_ram_address;
				}
				break;
			case 0x84:
				printf("-------NEW pc inc 84 write\n");
				dsp.regs.program_ram_data[dsp.regs.pc++] = data;
				dsp.regs.program_ram_address = dsp.regs.pc;
				break;
			case 0x88:
				//if(!dsp_program_control.program_execute_control)
				dsp.regs.data_ram_select = data >> 6;
				dsp.regs.data_ram_address = data ;

				break;
			case 0x8c:
				if(!dsp.regs.program_execute_control)
					dsp.regs.md[dsp.regs.data_ram_select][dsp.regs.data_ram_address++] = data;
				break;
			case 0x90:
				timer_0_compare = data;
				break;
			case 0x94:
				timer_1_set_data = data;
				break;
			case 0x98:
				timer_1_mode.mode = data >> 8;
				timer_1_mode.operation_enable = data;
				break;
			case 0xA0:
				ims = data;
				break;
			default:
				break;
				//	assert(false);
			}
		}

		u32 Read(int address, const Dsp & dsp)
		{
			address &= 0xff;
			switch(address)
			{
			case 0:
			case 4:
			case 8:
			case 0xC:
			case 0x10:
			case 0x14:
				return dma.LevelRead<0>(address);	
				break;
			case 0x20:
			case 0x24:
			case 0x28:
			case 0x2C:
			case 0x30:
			case 0x34:
				return dma.LevelRead<1>(address-0x20);	
				break;
			case 0x40:
			case 0x44:
			case 0x48:
			case 0x4C:
			case 0x50:
			case 0x54:
				return dma.LevelRead<2>(address-0x40);	
				break;
				//0x60 write only
			case 0x7c:
				{
					int result = 0;
					result |= dma.status.dsp_bus_access << 22;
					result |= dma.status.b_bus_access << 21;
					result |= dma.status.a_bus_access << 20;
					result |= dma.status.level_1_interrupt << 17;
					result |= dma.status.level_0_interrupt << 16;
					result |= dma.status.level_2_standby << 13;
					result |= dma.status.level_2_in_operation << 12;
					result |= dma.status.level_1_standby << 9;
					result |= dma.status.level_1_in_operation << 8;
					result |= dma.status.level_0_standby << 5;
					result |= dma.status.level_0_in_operation << 4;
					result |= dma.status.dsp_side_standby << 1;
					result |= dma.status.dsp_side_in_operation;
					return result;
				}
				break;
			case 0x80:
				{
					int result = 0;
					result |= dsp.regs.flags.t0 << 23;
					result |= dsp.regs.flags.sign << 22;
					result |= dsp.regs.flags.zero << 21;
					result |= dsp.regs.flags.carry << 20;
					result |= dsp.regs.flags.overflow << 19;
					result |= dsp_program_control.end << 18;
					result |= dsp.regs.program_execute_control << 16;
					result |= dsp.regs.program_ram_address;
					printf("-------NEW read\n");
					return result;
				}
				//assert(false);
				break;
			case 0x8c:
				//				return dsp.regs.md[dsp.regs.data_ram_select][dsp.regs.data_ram_address++];
				break;
			default:
				assert(false);
			}

			return 0;
		}

	}regs;

	//////////////////////////////////////////////
	//////////////////////////////////////////////
	//////////////////////////////////////////////

	//starting on interrupts again from scratch


	struct InterruptInfo
	{
		int bit_number;
		InterruptType type;
		int vector;
		int level;
		int mask;//which bit in the ims register
	};

	///this ought to be shared with the sh2 i guess but our structs are
	//different therefore no-go i guess
	std::vector<InterruptInfo> interrupts_pending;

	//boilerplate
	struct InterruptSortingPredicate {
		bool operator()(const InterruptInfo &left, const InterruptInfo &right) 
		{
			return left.level < right.level;
		}
	};

	void QueueInterrupt(const InterruptInfo &i)
	{
		int end = interrupts_pending.size();

		for(int j = 0; j < end; j++)
		{
			if(interrupts_pending[j].level == i.level)
				return;
		}

		interrupts_pending.push_back(i);
		std::sort(interrupts_pending.begin(), interrupts_pending.end(),InterruptSortingPredicate());//alas no lambdas
	}

	void SendInterruptRequest(const InterruptInfo &i)
	{
		u32 mask = i.type;
		if(!(regs.ims & mask))//type doubles as the mask
			saturn.master.SendInterruptRequest(i.vector,i.level);
		else
		{
			QueueInterrupt(i);
			//note that this is wrong if you
			//add in the external interrupts
			regs.isr |= i.type;
		}
	}

	template <InterruptType type>
	void SendInterruptRequest()
	{
		InterruptInfo i;

		i.type = type;//doubles as status bit mask

		switch(type)
		{
		case kVBlankIn:
			i.vector = 0x40;
			i.level = 0xF;
			break;
		case  kVBlankOut:
			i.vector = 0x41;
			i.level = 0xE;
			break;
		case kHBlankIn:
			i.vector = 0x42;
			i.level = 0xD;
			break;
		case kTimer0:
			i.vector = 0x43;
			i.level = 0xC;
			break;
		case kTimer1:
			i.vector = 0x44;
			i.level = 0xB;
			break;
		case kDspEnd:
			i.vector = 0x45;
			i.level = 0xA;
			break;
		case kSoundRequest:
			i.vector = 0x46;
			i.level = 0x9;
			break;
		case kSystemManager:
			i.vector = 0x47;
			i.level = 0x8;
			break;
		case kPadInterrupt:
			i.vector = 0x48;
			i.level = 0x8;
			break;
		case kLevel2DmaEnd:
			i.vector = 0x49;
			i.level = 0x6;
			break;
		case kLevel1DmaEnd:
			i.vector = 0x4A;
			i.level = 0x6;
			break;
		case kLevel0DmaEnd:
			i.vector = 0x4B;
			i.level = 0x5;
			break;
		case kDmaIllegal:
			i.vector = 0x4C;
			i.level = 0x3;
			break;
		case kSpriteDrawEnd:
			i.vector = 0x4D;
			i.level = 0x2;
			break;
		case kExternalInterrupt:
			assert(false);
			break;
		}
		SendInterruptRequest(i);
	}
};