struct Dsp
{
	Saturn &saturn;

	Dsp(Saturn & saturn_) : saturn(saturn_) {}

	template <typename T1, typename T2, typename T3>
	struct Reg48
	{
		T2 h;
		T3 l;

		operator T1()
		{
			return (h << 32) | l;
		}
		//	T operator ==(const T i)
		//	{
		//		return ((h << 32) | l) == i;
		//
		//	}
		T1 operator =(const T1 i)
		{
			h = i >> 32;
			l = i;
			return (h << 32) | l;
		}
	};

	struct Registers
	{
		array::Array<u32,256> program_ram_data;
		u6 data_ram_address;
		u32 data_ram_data;

		u2 data_ram_select;
		u1 program_execute_control;

		u8 program_ram_address;

		Reg48<u64,u16,u32> alu;

		//mul apparently needs the full sign extension as well
		Reg48<s64,s16,s32> mul;

		u8 top;
		u12 lop;
		u6 ct[4];
		//u32 md[4][64];
		array::Array<array::Array<u32,64>,4> md;
		u8 ra;
		u32 rx;
		u32 ry;

		//BEWARE THE SIGNEDNESS
		//so will using a signed value here sign extend the h part, even though it's supposed to be 16 bits?
		//let's see i guess....
		Reg48<s64,s16,s32> p;
		//types::uint_t<32> pl;

		//also used to have signedness issues
		Reg48<s64,s16,s32> ac;
		//types::uint_t<32> acl;
		//types::uint_t<32> d0_bus;

		struct External {
			u32 rao;
			u32 wao;
		};

		External external;

		struct Jump__
		{
			u8 address;
			u1 delayed;
			u1 need_jump;
		};

		Jump__ jump;

		u8 pc;

		u32 GetSourceRamValue(int selection)
		{
			switch(selection)
			{
			case 0:
			case 1:
			case 2:
			case 3:
				return md[selection][ct[selection]];
			case 4:
			case 5:
			case 6:
			case 7:
				return md[selection-4][ct[selection-4]++];
			case 9:
				return alu.l;
			case 0xA:
				return alu.h;
			default:
				assert(false);
			}
		assert(false);
		return 0;
		}
		enum WriteType
		{
			D1Bus,
			Immediate
		};

		template <WriteType type>
		void Write(int selection, int data)
		{
			switch(selection)
			{
			case 0:
			case 1:
			case 2:
			case 3:
				md[selection][ct[selection]++] = data;
				break;
			case 4:
				rx = data;
				break;
			case 5:
				p = data;
				break;
			case 6:
				external.rao = data;
				break;
			case 7:
				external.wao = data;
				break;
			case 0xA:
				lop = data;
				break;
			default:
				break;
				//				printf("unhandled");
				// assert(false);
			};
			if(selection > 0xA)
			{
				if(type == D1Bus)
				{
					switch(selection)
					{
					case 0xB:
						top = data;
						break;
					case 0xC:
					case 0xD:
					case 0xE:
					case 0xF:
						ct[selection-0xC] = data;
						break;
					}
				}
				else if(type == Immediate)
				{
					if(selection == 0xC)
					{
						top = pc;
						jump.address = data;
						jump.delayed = false;
						jump.need_jump = true;
					}
				}
			}
		}

		struct Flags{
			u1 sign;
			u1 zero;
			u1 carry;
			u1 overflow;
			u1 t0;

			//so, i was using unsigned long long before
			//but the sign extension blew up resulting in messed up
			//sign values ex 0x000000FFFFFE or whatever (aka not negative when it ought to be)
			//template <typename T>
			void SetSignFlag(signed long result)
			{
				if(result < 0)
					sign = 1;
				else
					sign = 0;
			}
			void SetZeroFlag(unsigned long long result)
			{
				if(result == 0)
					zero = 1;
				else
					zero = 0;
			}

			void SetSignZeroNoCarry(unsigned long long result)
			{
				SetSignFlag(result);
				SetZeroFlag(result);
				carry = 0;
			}

			void SetZeroSign(unsigned long long result)
			{
				SetZeroFlag(result);
				sign = result >> 31;
			}
		};
		Flags flags;
	};
public:
	Registers regs;
	struct Command
	{
		u32 all;

		u4 alu;
		struct XBus
		{
			u6 x_bus;
			u3 op;
			u3 selection;
		};
		XBus x_bus;
		u6 y_bus;

		struct D1Bus
		{
			u14 d1_bus;
			u4 selection;
		};

		D1Bus d1_bus;

		struct MoveImmediate
		{
			u4 storage_destination;
			//take care of the signed conv i think?
			u25 unconditional;
			u19 conditional;
			u6 status;
		};

		MoveImmediate move_immediate;

		struct Dma
		{
			u3 add_mode;
			u1 hold;
			u1 format;
			u1 direction;
			u3 ram;
			u8 immediate_data;
			u3 source;

			int GetAddMode()
			{
				switch(add_mode)
				{
				case 0: return 0;
				case 1: return 1;
				case 2: return 2;
				case 3: return 4;
				case 4: return 8;
				case 5: return 16;
				case 6: return 32;
				case 7: return 64;
				default:
					assert(false);
				}
				assert(false);
				return 0;
			}
		};

		struct Jump_
		{
			u7 status;
			u8 immediate;
		};

		Dma dma;

		Jump_ jump;

		Command(u32 command)
		{
			//compound stuff
			move_immediate.unconditional = command;
			move_immediate.conditional = command;
			move_immediate.storage_destination = command >> 26;
			move_immediate.status = command >> 19;

			all = command;
			d1_bus.d1_bus = command;
			d1_bus.selection = command >> 8;

			y_bus = command >> 14;
			x_bus.selection = command >> 20;
			x_bus.op = command >> 23;
			alu = command >> 26;

			dma.add_mode = command >> 15;
			dma.hold = command >> 14;
			dma.format = command >> 13;
			dma.direction = command >> 12;
			dma.ram = command >> 8;
			dma.immediate_data = command;
			dma.source = command;

			jump.status = command >> 19;
			jump.immediate = command;
		}

		//operator types::uint_t<32> ()
		//{
		// return d1_bus | (y_bus << 14) | (x_bus << 20) | (alu << 24);
		//}

		Command() {}
	};


	template <bool non_zero>
	bool compare(int input)
	{
		if(non_zero)
		{
			if(input) return true;
		}
		else
		{
			if(!input) return true;
		}
		return false;
	}

	template <bool non_zero, int a, int b, int c, int d, int e>
	void LoadImmediate(const Command &command)
	{
		switch(command.move_immediate.status)
		{
		case a:
			if(compare<non_zero>(regs.flags.zero))
				regs.Write<Registers::Immediate>(command.move_immediate.storage_destination,command.move_immediate.conditional);
			break;
		case b:
			if(compare<non_zero>(regs.flags.sign))
				regs.Write<Registers::Immediate>(command.move_immediate.storage_destination,command.move_immediate.conditional);
			break;
		case c:
			if(compare<non_zero>(regs.flags.zero) || compare<non_zero>(regs.flags.sign))
				regs.Write<Registers::Immediate>(command.move_immediate.storage_destination,command.move_immediate.conditional);
			break;
		case d:
			if(compare<non_zero>(regs.flags.carry))
				regs.Write<Registers::Immediate>(command.move_immediate.storage_destination,command.move_immediate.conditional);
			break;
		case e:
			if(compare<non_zero>(regs.flags.t0))
				regs.Write<Registers::Immediate>(command.move_immediate.storage_destination,command.move_immediate.conditional);
			break;
		};
	}

	void SetJump(int immediate)
	{
		regs.jump.address = immediate;
		regs.jump.delayed = false;
		regs.jump.need_jump = true;
	}

	template <bool non_zero, int a, int b, int c, int d, int e>
	void JumpImp(const Command &command)
	{
		switch(command.jump.status)
		{
		case a:
			if(compare<non_zero>(regs.flags.zero))
				SetJump(command.jump.immediate);
			break;
		case b:
			if(compare<non_zero>(regs.flags.sign))
				SetJump(command.jump.immediate);
			break;
		case c:
			if(compare<non_zero>(regs.flags.zero) || compare<non_zero>(regs.flags.sign))
				SetJump(command.jump.immediate);
			break;
		case d:
			if(compare<non_zero>(regs.flags.carry))
				SetJump(command.jump.immediate);
			break;
		case e:
			if(compare<non_zero>(regs.flags.t0))
				SetJump(command.jump.immediate);
			break;
		};
	}
#if 1
	void JumpCommand(const Command &command)
	{
		JumpImp<false,0x41,0x42,0x43,0x44,0x48>(command);
		JumpImp<true,0x61,0x62,0x63,0x64,0x68>(command);
		if(command.jump.status == 0)
			SetJump(command.jump.immediate);
	}
#endif
	void LoadImmediate(const Command &command)
	{
		LoadImmediate<false,1,2,3,4,8>(command);
		LoadImmediate<true,0x21,0x22,0x23,0x24,0x28>(command);
	}

	void AluControl(const Command &command)
	{
		switch(command.alu)
		{
		case 0:
			//nop
			regs.alu = 0;
			break;
		case 1:

			//and
			regs.alu.l = regs.ac.l & regs.p.l;
			regs.flags.SetSignZeroNoCarry(regs.alu.l);
			break;
		case 2:
			//or
			regs.alu.l = regs.ac.l | regs.p.l;
			regs.flags.SetSignZeroNoCarry(regs.alu.l);
			break;
		case 3:
			//exclusive or
			regs.alu.l = regs.ac.l ^ regs.p.l;
			regs.flags.SetSignZeroNoCarry(regs.alu.l);
			break;
		case 4:
			//add
			regs.alu.l = regs.ac.l + regs.p.l;
			regs.flags.SetSignFlag(regs.alu.l);
			regs.flags.SetZeroFlag(regs.alu.l);
			break;
		case 5:
			//sub
			regs.alu.l = regs.ac.l - regs.p.l;
			regs.flags.SetSignFlag(regs.alu.l);
			regs.flags.SetZeroFlag(regs.alu.l);
			break;
		case 6:
			//ad2
			regs.alu = regs.ac + regs.p;

			printf("almost guaranteed sign flag bug here\n");
			regs.flags.SetSignFlag(regs.alu);
			regs.flags.SetZeroFlag(regs.alu);

			//apparenlty some versions of the dsp have a bug that
			//breaks the overflow flag?  i suppose games don't rely on it then
			//apparently if you try to read the overflow flag
			//from the sh2s it gets set to zero.
			{
				u64 test = regs.ac + regs.p;

				if(test != regs.alu)
					//overflow
					regs.flags.overflow = 1;
				else
					regs.flags.overflow = 0;
			}
			break;
		case 8:
			//sr
			regs.flags.carry = regs.ac.l & 1;
			regs.alu.l = (regs.ac.l & (1 << 31)) | (regs.ac.l >> 1);
			regs.flags.SetZeroSign(regs.alu.l);
			break;
		case 9:
			//rr
			regs.flags.carry = regs.ac.l & 1;
			regs.alu.l = (regs.flags.carry << 31) | (regs.ac.l >> 1);
			regs.flags.SetZeroFlag(regs.alu.l);
			regs.flags.sign = regs.flags.carry;
			break;
		case 0xA:
			//sl
			regs.flags.carry = regs.ac.l >> 31;
			regs.alu.l = regs.ac.l << 1;
			regs.flags.SetZeroSign(regs.alu.l);
			break;
		case 0xB:
			//rl
			regs.flags.carry = regs.ac.l >> 31;
			regs.alu.l = (regs.ac.l << 1) | regs.flags.carry;
			regs.flags.SetZeroSign(regs.alu.l);
			break;
		case 0xF:
			//rl8
			regs.alu.l = (regs.ac.l << 8) | ((regs.ac.l >> 24) & 0xff);
			regs.flags.carry = regs.alu.l & 1;
			regs.flags.SetZeroSign(regs.alu.l);
			break;
		};
	}

	void XBusControl(const Command &command)
	{
		/*
		000 = 0
		100 = 4
		010 = 2
		011 = 3
		*/
		switch(command.x_bus.op)
		{
		case 0://nop
			break;
		case 2://mov mul,p
			regs.p = regs.mul;
			break;
		case 3://mov [s],p
			regs.p = regs.GetSourceRamValue(command.x_bus.selection);
			break;
		case 4://mov [s],x
			regs.rx = regs.GetSourceRamValue(command.x_bus.selection);
			break;
		default:
			//triggered by virtua fighter remix
			//	printf("odd x bus op\n");
			break;
			//assert(false);
		};
	}
	void YBusControl(const Command &command)
	{
		switch(command.y_bus >> 3)
		{
		case 0:
			//nop
			break;
		case 1:
			regs.ac = 0;
			break;
		case 2:
			regs.ac = regs.alu;
			break;
		case 3:
			//used to fail to sign extend
			regs.ac = (s64)(signed long)regs.GetSourceRamValue(command.y_bus & 7);
			break;
		case 4:
			regs.ry = regs.GetSourceRamValue(command.y_bus & 7);
			break;
		default:
			assert(false);
		};
	}

	void D1BusControl(const Command &command)
	{
		switch(command.d1_bus.d1_bus >> 12)
		{
		case 0:
			//nop
			break;
		case 1:
			//mov simm,[d]
			//we apparently have to sign extend the immediate value
			{
				int immediate_value = static_cast<signed char>(command.d1_bus.d1_bus & 0xff);
				immediate_value = static_cast<signed long>(immediate_value);
				regs.Write<Registers::D1Bus>(command.d1_bus.selection,immediate_value);
			}
			break;
		case 3:
			//mov [s],[d]
			regs.Write<Registers::D1Bus>(command.d1_bus.selection, regs.GetSourceRamValue(command.d1_bus.d1_bus & 0xf));
			break;
		};
	}

	//calling a method on an object makes it non const or something
	void Dma( Command&command)
	{

		int transfer_byte_number;
		int address_increment;

		//not using an immediate value
		if(command.dma.format)
		{
			transfer_byte_number = regs.GetSourceRamValue(command.dma.source);

			address_increment = (bool)command.dma.add_mode;

		}
		else//imm commands
		{
			transfer_byte_number = command.dma.immediate_data;
			address_increment = command.dma.GetAddMode();
		}
		if(command.dma.direction)
		{
			u32 original_wao = regs.external.wao;

			regs.external.wao &= 0x1FFFFFF;

			for(int i = 0; i < transfer_byte_number; i++)
			{
				unsigned int selection = command.dma.ram;
				unsigned int source = regs.md[selection][regs.ct[selection]++];
				saturn.CpuBusTransfer<kLong,kWrite>(regs.external.wao << 2,source);
				regs.external.wao += address_increment;
			}

			if(command.dma.hold)
				regs.external.wao = original_wao;
		}
		else
		{
			//used if hold is on
			u32 original_rao = regs.external.rao;

			regs.external.rao &= 0x1FFFFFF;

			//TODO dma timing
			for(int i = 0; i < transfer_byte_number; i++)
			{
				int selection = command.dma.ram;
				//fix me		
				int data = saturn.CpuBusTransfer<kLong,kRead>(regs.external.rao << 2);//MappedMemoryReadLong(regs.external.rao << 2);
				regs.md[selection][regs.ct[selection]++] = data;
				regs.external.rao += address_increment;
			}

			if(command.dma.hold)
				regs.external.rao = original_rao;
		}
	}

	void Loop(const Command&command)
	{
		//loop
		if(command.all & (1 << 27))
		{

			if(regs.lop)
			{
				regs.jump.address = regs.pc;
				regs.jump.need_jump = true;
				regs.jump.delayed = false;
				regs.lop--;
			}
		}
		else
		{
			if(regs.lop)
			{
				regs.jump.address = regs.top;
				regs.jump.need_jump = true;
				regs.jump.delayed = false;
				regs.lop--;
			}
		}
	}

	void Execute(Saturn&saturn)
	{

		if(!regs.program_execute_control)
			return;//dsp is not executing

		Command command(regs.program_ram_data[regs.pc]);
		if(command.all == 0x10003109)
			int zzz = 0;
		switch(command.all >> 30)
		{
		case 0:
			//operation
			AluControl(command);
			XBusControl(command);
			YBusControl(command);
			D1BusControl(command);
			break;
		case 2:
			//load immediate
			if ((command.all >> 25) & 1)
				LoadImmediate(command);
			else
				regs.Write<Registers::Immediate>(command.move_immediate.storage_destination,command.move_immediate.unconditional);
			break;
		case 3:
			//dma,jump,loop,end
			switch((command.all >> 28) & 3)
			{
			case 0:
				//dma
				Dma(command);
				break;
			case 1:
				//jump
				JumpCommand(command);
				break;
			case 2:
				//loop
				Loop(command);
				break;
			case 3:
				//end
				regs.program_execute_control = false;
				printf("-------NEW end\n");

				if (command.all & (1 << 27))
				{
					//regs.program_end_interrupt_flag = true;
					saturn.scu.SendInterruptRequest<kDspEnd>();
				}

				regs.program_ram_address = regs.pc + 1;
				break;
			}
			break;
		}
		//trying to sign extend correctly
		signed long long rx = (s64)(signed long)regs.rx;
		signed long long ry = (s64)(signed long)regs.ry;
		s64 res = rx * ry;
		s64 result = regs.rx * regs.ry;

		s64 resulltz = (signed)regs.rx * (signed)regs.ry;
		//regs.mul = static_cast<__int64>(regs.rx * regs.ry);
		regs.mul = resulltz;


		regs.pc++;

		if(regs.jump.need_jump)
		{
			if(regs.jump.delayed)
			{
				//				printf("-------NEw pc jmp\n");
				regs.pc = regs.jump.address;
				regs.jump.need_jump = false;
			}
			else
			{
				regs.jump.delayed = true;
			}
		}

		return;

	}
};
