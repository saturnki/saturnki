struct OnChipPeripherals
{
	Sh2 &sh2;
	OnChipPeripherals(Sh2 &sh2_) : sh2(sh2_), regs(sh2_){}
	struct Registers
	{
		Sh2 &sh2;
		Registers(Sh2 &sh2_) : sh2(sh2_) {}

		struct Smr
		{
			u1 communication_mode;
			u1 character_length;
			u1 parity_enable;
			u1 parity_mode;
			u1 stop_bit_length;
			u1 multiprocessor_mode;
			u2 clock_select;
			operator int() const {
				return 
					(communication_mode << 7)
					|(character_length << 6)
					|(parity_enable << 5)
					|(parity_mode << 4)
					|(stop_bit_length << 3)
					|(multiprocessor_mode << 2)
					|(clock_select);
			}
		}smr;

		u8 brr;

		struct Scr
		{
			u1 transmit_interrupt_enable;
			u1 receive_interrupt_enable;
			u1 transmit_enable;
			u1 receive_enable;
			u1 multiprocessor_interrupt_enable;
			u1 transmit_end_interrupt_enable;
			u2 clock_enable;
			operator int() const {
				return 
					(transmit_interrupt_enable << 7)
					|(receive_interrupt_enable << 6)
					|(transmit_enable << 5)
					|(receive_enable << 4)
					|(multiprocessor_interrupt_enable << 3)
					|(transmit_end_interrupt_enable << 2)
					|(clock_enable);
			}
		}scr;

		u8 tdr;

		struct Ssr
		{	
			u1 transmit_data_register_empty;
			u1 receive_data_register_full;
			u1 overrun_error;
			u1 framing_error;
			u1 parity_error;
			u1 transmit_end;
			u1 multiprocessor_bit;
			u1 multiprocessor_bit_transfer;
			operator int() const {
				return 
					(transmit_data_register_empty << 7)
					|(receive_data_register_full << 6)
					|(overrun_error << 5)
					|(framing_error << 4)
					|(parity_error << 3)
					|(transmit_end << 2)
					|(multiprocessor_bit << 1)
					|(multiprocessor_bit_transfer);
			}
			int operator =(int data)
			{
				transmit_data_register_empty  = data >> 7;
				receive_data_register_full  = data >> 6;
				overrun_error  = data >> 5;
				framing_error  = data >> 4;
				parity_error  = data >> 3;
				transmit_end = data  >> 2;
				multiprocessor_bit = data >> 1;
				multiprocessor_bit_transfer = data;
				return data;//uhh probably a bug
			}
		}ssr;

		u8 rdr;

		struct Tier
		{
			u1 input_capture_interrupt_enable;
			u1 output_compare_interrupt_a_enable;
			u1 output_compare_interrupt_b_enable;
			u1 timer_overflow_interrupt_enable;
		}tier;

		struct Ftcsr
		{
			u1 input_capture_flag;
			u1 output_compare_a_flag;
			u1 output_compare_b_flag;
			u1 timer_overflow_flag;
			u1 counter_clear_a;

			operator u8() const
			{
				u8 result = 0;
				result |= input_capture_flag << 7;
				result |= output_compare_a_flag << 3;
				result |= output_compare_b_flag << 2;
				result |= timer_overflow_flag << 1;
				result |= counter_clear_a;
				return result;
			}
		}ftcsr;

		u16 frc;
		
		u16 ocra;
		u16 ocrb;

		struct Tcr
		{
			u1 input_edge_select;
			u2 clock_selects;
		}timer_control_register;//too similar to tcr0/1

		struct Tocr
		{
			u1 output_compare_register_select;
			u1 output_level_a;
			u1 output_level_b;
		}tocr;

		u16 icr;

		struct Ipra
		{
			u4 division_unit_interrupt_priority_level;
			u4 dma_controller_interrupt_priority_level;
			u4 watchdog_timer_interrupt_priority_level;
		}ipra;

		struct Iprb
		{
			u4 serial_commmunication_interface_interrupt_priority_level;
			u4 free_running_timer_interrupt_priority_level;
		}iprb;

		struct Vcra
		{
			u8 sci_receive_error_interrupt_vector;
			u8 sci_receive_data_full_interrupt_vector;
		}vcra;

		struct Vcrb
		{
			u8 sci_transmit_data_empty_interrupt_vector;
			u8 sci_transmit_end_interrupt_vector;
		}vcrb;

		struct Vcrc
		{
			u8 frt_input_capture_interrupt_vector;
			u8 frt_output_compare_interrupt_vector;
		}vcrc;

		struct Vcrd
		{
			u8 frt_overflow_interrupt_vector;
		}vcrd;

		struct Vcrwdt
		{
			u8 wdt_interval_interrupt_vector;
			u8 bsc_compare_match_interrupt_vector;
		}vcrwdt;

		u16 vcrdiv;

		u8 vcrdma0;
		u8 vcrdma1;

		//there's two regs with the same name, yay
		struct Icr
		{
			u1 nmi_input_level;
			u1 nmi_edge_select;
			u1 rl_interrupt_vector;
		}interrupt_control_register;

		struct Wtcsr
		{
			u1 overflow_flag;
			u1 timer_mode_select;
			u1 timer_enable;
			u3 clock_select;
		}wtcsr;

		u7 wtcnt;

		struct Rstcsr
		{
			u1 watchdog_timer_overflow_flag;
			u1 reset_enable;
			u1 reset_select;
		}rstcsr;

		u32 dvsr;

		u32 dvnt;

		struct Dvcr
		{
			u1 ovf_interrupt_enable;
			u1 overflow_flag;
		}dvcr;

		u32 dvdnth;
		u32 dvdntl;

		//break address goes here
#if 1
		u32 sar0;
		u32 sar1;
		u32 dar0;
		u32 dar1;

		u24 tcr0;
		u24 tcr1;

		struct Chcr
		{
			u2 destination_address_mode;
			u2 source_address_mode;
			u2 transfer_size;
			u1 auto_request_mode;
			u1 acknowledge_transfer_mode;
			u1 acknowledge_level;
			u1 dreq_select;
			u1 dreq_level;
			u1 transfer_bus_mode;
			u1 transfer_address_mode;
			u1 interrupt_enable;
			u1 transfer_end;
			u1 dma_enable;

			int operator =(int data)
			{
				destination_address_mode = data >> 14;
				source_address_mode = data >> 13;
				transfer_size = data >> 11;
				auto_request_mode = data >> 9;
				acknowledge_transfer_mode = data >> 8;
				acknowledge_level = data >> 7;
				dreq_select = data >> 6;
				dreq_level = data >> 5;
				transfer_bus_mode = data >> 4;
				transfer_address_mode = data >> 3;
				interrupt_enable = data >> 2;
				transfer_end = data >> 1;
				dma_enable = data;

				return data;//uhh probably a bug
			}
			operator u32 const()
			{
				u32 result = 0;
				result |= destination_address_mode << 14;
				result |= source_address_mode << 13;
				result |= transfer_size << 11;
				result |= auto_request_mode << 9;
				result |= acknowledge_transfer_mode << 8;
				result |= acknowledge_level << 7;
				result |= dreq_select << 6;
				result |= dreq_level << 5;
				result |= transfer_bus_mode << 4;
				result |= transfer_address_mode << 3;
				result |= interrupt_enable << 2;
				result |= transfer_end << 1;
				result |= dma_enable;
				return result;
			}
		};

		Chcr chcr0,chcr1;

		u2 drcr0;
		u2 drcr1;
#else
		struct DmaChannel
		{
			u32> sar;
			u32> dar;
			u24> tcr;

			struct Chcr
			{
				u2> destination_address_mode;
				u2> source_address_mode;
				u2> transfer_size;
				u1> auto_request_mode;
				u1> acknowledge_transfer_mode;
				u1> acknowledge_level;
				u1> dreq_select;
				u1> dreq_level;
				u1> transfer_bus_mode;
				u1> transfer_address_mode;
				u1> interrupt_enable;
				u1> transfer_end;
				u1> dma_enable;

				int operator =(int data)
				{
					destination_address_mode = data >> 14;
					source_address_mode = data >> 13;
					transfer_size = data >> 11;
					auto_request_mode = data >> 9;
					acknowledge_transfer_mode = data >> 8;
					acknowledge_level = data >> 7;
					dreq_select = data >> 6;
					dreq_level = data >> 5;
					transfer_bus_mode = data >> 4;
					transfer_address_mode = data >> 3;
					interrupt_enable = data >> 2;
					transfer_end = data >> 1;
					dma_enable = data;

					return data;//uhh probably a bug
				}
			}chcr;
			u2> drcr;
		}dma_channel[2];
#endif
		struct Dmaor
		{
			u1 priority_mode;
			u1 address_error_flag;
			u1 nmi_flag;
			u1 dma_master_enable;

			operator u32() const
			{
				u32 result = 0;
				result |= priority_mode << 3;
				result |= address_error_flag << 2;
				result |= nmi_flag << 1;
				result |= dma_master_enable;
				return result;
			}
		}dmaor;

		struct Bcr1
		{
			u1 bus_arbitration_mode;
			u1 endian_specification;
			u1 area_0_burst_rom_enable;
			u1 partial_space_share;
			u2 long_wait_for_areas_2_3;
			u2 long_wait_for_area_1;
			u2 long_wait_for_area_0;
			u3 enable_for_dram_and_other;

			operator int() const {
				return 
					(bus_arbitration_mode << 15)
					| (endian_specification << 12)
					| (area_0_burst_rom_enable << 11)
					| (partial_space_share << 10)
					| (long_wait_for_areas_2_3 << 8)
					| (long_wait_for_area_1 << 6)
					| (long_wait_for_area_0 << 4)
					| enable_for_dram_and_other;
			}
		}bcr1;

		struct Bcr2
		{
			u2 bus_size_area_3;
			u2 bus_size_area_2;
			u2 bus_size_area_1;
		}bcr2;

		struct Wcr
		{
			//implement me
			u8 idles_between_cycles;
			u8 wait_control;
		}wcr;

		struct Mcr
		{
			u1 ras_precharge_time;
			u1 ras_cas_delay;
			u1 write_precharge_delay;
			u2 cas_before_ras_refresh_ras_assert_time;
			u1 burst_enable;
			u1 bank_active_mode ;
			//what an odd reg, bits 7,5,4
			u3 address_multiplex;
			u1 memory_data_size;
			u1 refresh_control;
			u1 refresh_mode;
		}mcr;

		struct Rtscr
		{
			u1 compare_match_flag ;
			u1 compare_match_interrupt_enable;
			u3 clock_select_bits ;
		}rtcsr;

		u8 rtcnt;
		u8 rtcor;

		struct Ccr
		{
			u2 way;
			u1 cache_purge;
			u1 two_way_mode;
			u1 data_replacement_disable;
			u1 instruction_replacement_disable;
			u1 cache_enable;
		}ccr;

		struct Sbycr
		{
			//implement me
		}sbycr;

		//what are these?
		u32 dvdntul;
		u32 dvdntuh;

		void StartDmaThingy(Chcr chcr)
		{
			if(dmaor.dma_master_enable && !dmaor.address_error_flag && !dmaor.nmi_flag)
				if(!chcr.transfer_end && chcr.dma_enable)
					sh2.DmaChannelPriority();
		}

		template <OperationSize size>
		void Write(unsigned int address, unsigned int data)
		{
			address &= 0x1FF;
			switch(address)
			{
			case 0x10:
				tier.input_capture_interrupt_enable = data >> 7;
				tier.output_compare_interrupt_a_enable = data >> 3;
				tier.output_compare_interrupt_b_enable = data >> 2;
				tier.timer_overflow_interrupt_enable = data >> 1;
				return;
			case 0x11:
				ftcsr.input_capture_flag = data >> 7;
				ftcsr.output_compare_a_flag = data >> 3;
				ftcsr.output_compare_b_flag = data >> 2;
				ftcsr.timer_overflow_flag = data >> 1;
				ftcsr.counter_clear_a = data;
				break;
			case 0x17:
				assert(false);
				return;
			case 0x88:

				return;
			case 0x92:
				ccr.way = data >> 6;
				ccr.cache_purge = data >> 4;
				ccr.two_way_mode = data >> 3;
				ccr.data_replacement_disable = data >> 2;
				ccr.instruction_replacement_disable = data >> 1;
				ccr.cache_enable = data;
				return;
			case 0x100:
				dvsr = data;
				return;
			case 0x104:
				{
					//todo division timing
					signed int dividend = static_cast<signed int>(data);
					signed int divisor = static_cast<signed int>(dvsr);
					signed int quotient = dividend / divisor;
					signed int remainder = dividend % divisor;
					dvdntl = quotient;
					dvdntul = quotient;
					dvdnth = remainder;
					dvdntuh = remainder;
				}
				return;
			case 0x10c:
				vcrdiv = data;
				return;
			case 0x110:
				dvdnth = data;
				return;
			case 0x114:
				{
					//rewrite all of this from scratch
					//with proper timing
					s32 divisor = static_cast<s32>(dvsr);
					s64 dividend = dvdnth;
					dividend <<= 32;
					dividend |= data;
					if(divisor == 0)
					{
						if(dvdnth & 0x80000000)
						{
							dvdntl = 0x80000000;
							dvdnth <<= 3;
						}
						else
						{
							dvdntl = 0x7FFFFFFF;
							dvdnth <<= 3;
						}

						dvdntul = dvdntl;
						dvdntuh = dvdnth;

						dvcr.overflow_flag = 1;
					}
					else
					{
						s64 quotient = dividend / divisor;
						s32 remainder = dividend % divisor;

						if (quotient > 0x7FFFFFFF)
						{
							dvcr.overflow_flag = 1;
							//this stuff is not correct
							dvdntl = 0x7FFFFFFF;
							dvdnth = 0xFFFFFFFE;
						}
						else if ((s32)(quotient >> 32) < -1)
						{
							dvcr.overflow_flag = 1;
							//this stuff is not correct
							dvdntl = 0x80000000;
							dvdnth = 0xFFFFFFFE;
						}
						else
						{
							dvdntl = quotient;
							dvdnth = remainder;
						}

						dvdntul = dvdntl;
						dvdntuh = dvdnth;
					}
				}
				return;
			case 0x18c:
				chcr0 = data;
				StartDmaThingy(chcr0);
				return;
			case 0x19c:
				chcr1 = data;
				StartDmaThingy(chcr1);
				return;
			case 0x1a0:
				vcrdma0 = data;
				return;
			case 0x1b0:
				dmaor.priority_mode = data >> 3;
				dmaor.address_error_flag = data >> 2;
				dmaor.nmi_flag = data >> 1;
				dmaor.dma_master_enable = data;

				if(dmaor.dma_master_enable && dmaor.address_error_flag == 0 && dmaor.nmi_flag == 0)
					sh2.DmaChannelPriority();
			//	assert(false);
				return;
			case 0x1a8:
				vcrdma1 = data;
				return;
			case 0x1e0:
				//	(bus_arbitration_mode << 15) //read only
				bcr1.endian_specification = data >> 12;
				bcr1.area_0_burst_rom_enable = data  >> 11;
				bcr1.partial_space_share = data  >> 10;
				bcr1.long_wait_for_areas_2_3 = data  >> 8;
				bcr1.long_wait_for_area_1 = data >> 6;
				bcr1.long_wait_for_area_0 = data >> 4;
				bcr1.enable_for_dram_and_other = data;
				return;
			case 0x1e4:
				bcr2.bus_size_area_3 = data >> 6;
				bcr2.bus_size_area_2 = data >> 4;
				bcr2.bus_size_area_1 = data >> 2;
				return;
			case 0x1e8:
				//the mapping of this
				//reg makes no sense to me
				//	wcr = data;
				wcr.idles_between_cycles = data >> 8;
				wcr.wait_control = data;
				//assert(false);
				return;
			case 0x1ec:
				mcr.ras_precharge_time = data >> 15;
				mcr.ras_cas_delay = data >> 14;
				mcr.write_precharge_delay = data >> 13;
				mcr.cas_before_ras_refresh_ras_assert_time = data >> 11;
				mcr.burst_enable = data >> 10;
				mcr.bank_active_mode = data >> 9;
				//what an odd reg, bits 7,5,4
				mcr.address_multiplex = ((data >> 4)&3) | ((data >> 4) & 0x4);
				mcr.memory_data_size = data >> 6;
				mcr.refresh_control = data >> 3;
				mcr.refresh_mode = data >> 2;
				return;
			case 0x1f0:
				rtcsr.compare_match_flag = data >> 7;
				rtcsr.compare_match_interrupt_enable = data >> 6;
				rtcsr.clock_select_bits = data >> 3;
				return;
			case 0x1f8:
				rtcor = data;
				return;
			}
			//assert(false);
		}
		template <OperationSize size>
		unsigned int Read(unsigned int address)
		{
			address &= 0x1FF;

			if(size == kByte)
			{
				switch(address)
				{
				case 0x11:
					return ftcsr;
					break;
				}
			}
			if(size == kWord)
			{

				switch(address)
				{
				case 0x1e0:
					return bcr1;
				}
			}
			else if(size == kLong)
			{
				switch(address)
				{
				case 0x104:
					return dvdntl;
				case 0x1b0:
					return dmaor;
				case 0x1e0:
					return bcr1;
				case 0x114:
					return dvdntl;
				case 0x18c:
					return chcr0;
				case 0x19c:
					return chcr1;
				}
			}
			assert(false);
			return 0;
		}

		void Reset()
		{


			//	bcr1.bus_arbitration_mode = 
			bcr1.endian_specification = 0;
			bcr1.area_0_burst_rom_enable = 0;
			bcr1.partial_space_share = 0;
			bcr1.long_wait_for_areas_2_3 = 3;
			bcr1.long_wait_for_area_1 = 3;
			bcr1.long_wait_for_area_0 = 3;
			bcr1.enable_for_dram_and_other = 0;
		}
	}regs;

	void ExecuteFreeRunningTimer(u64 sh2_cycles)
	{
		u32 divisor = 0;
		switch(regs.timer_control_register.clock_selects)
		{
		case 0:
			divisor = 8;
			break;
		case 1:
			divisor = 32;
			break;
		case 2:
			divisor = 128;
			break;
		}

		//>> 3
		//>> 5
		//>> 7

		//this division seems to be slow eliminate me
		if(sh2_cycles % divisor == 0)
			regs.frc++;

		if(regs.frc >= regs.ocra)
		{
			if(regs.tier.output_compare_interrupt_a_enable)
				sh2.SendInterruptRequest(regs.vcrc.frt_output_compare_interrupt_vector,regs.iprb.free_running_timer_interrupt_priority_level);

			if(regs.ftcsr.counter_clear_a)
				regs.frc = 0;

			regs.ftcsr.output_compare_a_flag = true;
		}

		if(regs.frc >= regs.ocrb)
		{
			if(regs.tier.output_compare_interrupt_b_enable)
				sh2.SendInterruptRequest(regs.vcrc.frt_output_compare_interrupt_vector,regs.iprb.free_running_timer_interrupt_priority_level);

			regs.ftcsr.output_compare_b_flag = true;
		}

		if(regs.frc > 0xFFFF)
		{
			if(regs.tier.timer_overflow_interrupt_enable)
				sh2.SendInterruptRequest(regs.vcrd.frt_overflow_interrupt_vector,regs.iprb.free_running_timer_interrupt_priority_level);

			regs.ftcsr.timer_overflow_flag = true;
		}
	}
}peripherals;