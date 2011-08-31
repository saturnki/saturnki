struct FetchResult
{
	u32 data;
	u32 address;
};

template <OperationSize size,OperationSize which_op>
FetchResult AbsoluteDataAddressingFetch()
{
	FetchResult f = {0};
	f.address = FetchImmediate<which_op>(regs.pc);
	f.data = Read<size>(f.address);
	return f;
}

template <OperationSize size,OperationSize which_op>
void AbsoluteDataAddressingSet(const u32&input_address, const bool &update_before_write, const u32 &write_value)
{
	u32 address = input_address;
	//we have the same sort of overfetching problem with the write here too
	if(update_before_write)
		address = FetchImmediate<which_op>(regs.pc);
	Write<size>(address,write_value);
}

template <OperationSize size, bool predecrement>
FetchResult EffectiveAddressFetch(const u32 &mode, const u32 &reg)
{
	FetchResult f = {0};
	switch(mode)
	{
	case 0:
		f.data = SizeMask<size>(regs.d[reg]);
		break;
	case 1:
		f.data = SizeMask<size>(regs.a[reg]);
		break;
	case 2:
		f.address = regs.a[reg];
		f.data = Read<size>(f.address);
		break;
	case 3:
		f.address = regs.a[reg];
		f.data = Read<size>(f.address);
		regs.a[reg] += size;
		break;
	case 4:		//Address Register Indirect with Predecremen
		if(predecrement)//so, movem does NOT predecrement apparently
		{
			regs.a[reg]-=size;
			AddCycles(2);//apparently doing the decrement costs 2 cycles
		}
		f.address = regs.a[reg];
		f.data = Read<size>(f.address);
		break;
	case 5:
		f.address = regs.a[reg] + static_cast<s16>(FetchImmediate<kWord>(regs.pc));//sign extend
		f.data = Read<size>(f.address);
		break;
	case 6:
		f.address = Index(regs.a[reg]);
		f.data = Read<size>(f.address);
		break;
	case 7:
		switch(reg)
		{
		case 0://Absolute Data Addressing Word (xxx).W
			f = AbsoluteDataAddressingFetch<size,kWord>();
			break;
		case 1://Absolute Data Addressing Long (xxx).L
			f = AbsoluteDataAddressingFetch<size,kLong>();
			break;
		case 2:
			{
			u32 old_pc = regs.pc;//otherwise it increments too much
			f.address = old_pc + static_cast<signed short>(FetchImmediate<kWord>(regs.pc));
			}
			break;
		case 3:
			{
			u32 pc = regs.pc;
			f.address = pc + Index(0);
			f.data = Read<size>(f.address);
			}
			break;
		case 4:
			//Immediate #<xxx>
			f.data = FetchImmediate<size>(regs.pc);
			break;
		}
		break;
	}

	return f;
}

template <OperationSize size, bool update_before_write>
void EffectiveAddressSet(
	const u32&mode, const u32&reg,
	const u32& address, const u32&data)
{

	u32 addr = address;
	switch(mode)
	{
	case 0:
		regs.d[reg] = MaskedCombine<size>(regs.d[reg], data);
		break;
	case 1:
		regs.a[reg] = SizeMask<size>(data);
		break;
	case 2:
		Write<size>(regs.a[reg],data);
		break;
	case 3:
		//for a load and store using this addressing mode,
		//it uses the original address to write to,
		//and it also doesn't increment again
		if(update_before_write)
			addr = regs.a[reg];
		Write<size>(addr,data);
		if(update_before_write)
			regs.a[reg] += size;
		break;
	case 4:
		if(update_before_write)
			regs.a[reg]-=size;
		Write<size>(regs.a[reg],data);
		break;
	case 5:
		if(update_before_write)
			addr = regs.a[reg] + FetchImmediate<kWord>(regs.pc);
		Write<size>(addr,data);
		break;
	case 6:
		if(update_before_write)
			addr = Index(regs.a[reg]);
		Write<size>(addr,data);
		break;
	case 7:
		switch(reg)
		{
		case 0:
			AbsoluteDataAddressingSet<size,kWord>(address,update_before_write,data);
			break;
		case 1:
			AbsoluteDataAddressingSet<size,kLong>(address,update_before_write,data);
			break;
		default:
			assert(false);
			break;
		}
	}
}