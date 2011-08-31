#include "lib/lib.h"

u32 irq_mask = 0xBFFF;

namespace scu
{
	struct IrqMasks
	{
		enum 
		{
			kVblankIn = 1,
			kVblankOut = 2,
			kHblankIn = 4,
			kTimer0 = 8,
			kTimer1 = 0x10,
			kDspEnd = 0x20,
			kSoundRequest = 0x40,
			kSystemManager = 0x80,
			kPad = 0x100,
			kDmaLevel2End = 0x200,
			kDmaLevel1End = 0x400,
			kDmaLevel0End = 0x800,
			kDmaIllegal = 0x1000,
			kSpriteDrawEnd = 0x2000
		};
	};

	namespace regs
	{
		typedef vu32 * const Ptr;

#define declare_dma(offset, name)                           \
		namespace name                                      \
		{                                                   \
			Ptr read_address = (vu32*)offset;               \
			Ptr write_address = (vu32*)(offset + 4);        \
			Ptr transfer_byte_number = (vu32*)(offset + 8); \
			Ptr add_value = (vu32*)(offset + 0xC);          \
			Ptr enable = (vu32*)(offset + 0x10);            \
			Ptr mode = (vu32*)(offset + 0x14);              \
		}

		declare_dma(0x25fe0000,dma0);
		declare_dma(0x25fe0020,dma1);
		declare_dma(0x25fe0040,dma2);

#undef declare_dma

		Ptr dma_force_end = (vu32*)0x25FE0060;
	//	Ptr dma_force_end = (vu32*)0x25FE0060;

		Ptr dsp_program_control = (vu32*)0x25FE0080;
		Ptr dsp_program_ram = (vu32*)0x25FE0084;
		Ptr dsp_data_ram = (vu32*)0x25FE0088;
		Ptr dsp_ram_data = (vu32*)0x25FE008C;

		Ptr timer0_compare = (vu32*)0x25FE0090;
		Ptr timer1_set = (vu32*)0x25FE0094;
		Ptr timer1_mode = (vu32*)0x25FE0098;

		Ptr interrupt_mask = (vu32*)0x25FE00A0;
		Ptr interrupt_status = (vu32*)0x25FE00A4;

		Ptr abus_interrupt = (vu32*)0x25FE00A8;
		//abus set
		Ptr abus_refresh = (vu32*)0x25FE00B8;

		Ptr sdram_select = (vu32*)0x25FE00C4;
		Ptr version = (vu32*)0x25FE00C8;
	}

	void SetEnabledInterrupts(u32 which)
	{
		*regs::interrupt_mask = irq_mask = (0xbfff & ~(which));
	}
}


void Sh2EnableInterrupts()
{
	u32 sr = sh2::regs::GetSr();
	sh2::regs::SetSr(sr & 0xFFFFFF0F);
}


const u32 vbr_address = 0x002FFC00;


//repetitive yes, but this is more useful for later.

void ServiceInterruptDummy()
{
	*scu::regs::interrupt_mask = irq_mask;
	io::PrintString4bit(0,0,"Dummy!");
	Sh2EnableInterrupts();
}

void ServiceVblankIn()
{
	*scu::regs::interrupt_mask = irq_mask;
	io::PrintString4bit(0,1,"VblankIn!");
	Sh2EnableInterrupts();
}

void ServiceVblankOut()
{
	*scu::regs::interrupt_mask = irq_mask;
	io::PrintString4bit(0,2,"VblankOut!");
	Sh2EnableInterrupts();
}

//there's something weird about this interrupt?
//kills the system or at least yabause or something
//you probably have to handle it specially or turn it off at the right time or sommething
void ServiceHblankIn()
{
	*scu::regs::interrupt_mask = irq_mask;
	io::PrintString4bit(0,3,"HblankIn!");
	Sh2EnableInterrupts();
}

void ServiceTimer0()
{
	*scu::regs::interrupt_mask = irq_mask;
	io::PrintString4bit(0,4,"Timer0!");
	Sh2EnableInterrupts();
}

void ServiceTimer1()
{
	*scu::regs::interrupt_mask = irq_mask;
	io::PrintString4bit(0,5,"Timer1!");
	Sh2EnableInterrupts();
}

void ServiceDspEnd()
{
	*scu::regs::interrupt_mask = irq_mask;
	io::PrintString4bit(0,6,"DspEnd!");
	Sh2EnableInterrupts();
}

void ServiceSoundRequest()
{
	*scu::regs::interrupt_mask = irq_mask;
	io::PrintString4bit(0,7,"SoundRequest!");
	Sh2EnableInterrupts();
}

void ServiceSystemManager()
{
	*scu::regs::interrupt_mask = irq_mask;
	io::PrintString4bit(0,8,"SystemManager!");
	Sh2EnableInterrupts();
}

void ServicePad()
{
	*scu::regs::interrupt_mask = irq_mask;
	io::PrintString4bit(0,9,"Pad!");
	Sh2EnableInterrupts();
}

void ServiceDmaLevel2End()
{
	*scu::regs::interrupt_mask = irq_mask;
	io::PrintString4bit(0,10,"DmaLevel2End!");
	Sh2EnableInterrupts();
}

void ServiceDmaLevel1End()
{
	*scu::regs::interrupt_mask = irq_mask;
	io::PrintString4bit(0,11,"DmaLevel1End!");
	Sh2EnableInterrupts();
}

void ServiceDmaLevel0End()
{
	*scu::regs::interrupt_mask = irq_mask;
	io::PrintString4bit(0,12,"DmaLevel0End!");
	Sh2EnableInterrupts();
}

void ServiceDmaIllegal()
{
	*scu::regs::interrupt_mask = irq_mask;
	io::PrintString4bit(0,13,"DmaIllegal!");
	Sh2EnableInterrupts();
}

void ServiceSpriteDrawEnd()
{
	*scu::regs::interrupt_mask = irq_mask;
	io::PrintString4bit(0,14,"SpriteDrawEnd!");
	Sh2EnableInterrupts();
}

void ServiceExternal()
{
	*scu::regs::interrupt_mask = irq_mask;
	io::PrintString4bit(8,0,"External!");
	Sh2EnableInterrupts();
}

void InitializeInterrupts()
{
	u32 *vector = (u32 *)vbr_address;

	for(int i = 0; i < 0x100; i++)
		vector[i] = (u32)&ServiceInterruptDummy;

	vector[0x40] = (u32)&ServiceVblankIn;
	vector[0x41] = (u32)&ServiceVblankOut;
	vector[0x42] = (u32)&ServiceHblankIn;
	vector[0x43] = (u32)&ServiceTimer0;
	vector[0x44] = (u32)&ServiceTimer1;
	vector[0x45] = (u32)&ServiceDspEnd;
	vector[0x46] = (u32)&ServiceSoundRequest;
	vector[0x47] = (u32)&ServiceSystemManager;
	vector[0x48] = (u32)&ServicePad;
	vector[0x49] = (u32)&ServiceDmaLevel2End;
	vector[0x4A] = (u32)&ServiceDmaLevel1End;
	vector[0x4B] = (u32)&ServiceDmaLevel0End;
	vector[0x4C] = (u32)&ServiceDmaIllegal;
	vector[0x4D] = (u32)&ServiceSpriteDrawEnd;
	//unused?
	//unused?
	vector[0x50] = (u32)&ServiceExternal;
	vector[0x51] = (u32)&ServiceExternal;
	vector[0x52] = (u32)&ServiceExternal;
	vector[0x53] = (u32)&ServiceExternal;
	vector[0x54] = (u32)&ServiceExternal;
	vector[0x55] = (u32)&ServiceExternal;
	vector[0x56] = (u32)&ServiceExternal;
	vector[0x57] = (u32)&ServiceExternal;
	vector[0x58] = (u32)&ServiceExternal;
	vector[0x59] = (u32)&ServiceExternal;
	vector[0x5A] = (u32)&ServiceExternal;
	vector[0x5B] = (u32)&ServiceExternal;
	vector[0x5C] = (u32)&ServiceExternal;
	vector[0x5D] = (u32)&ServiceExternal;
	vector[0x5E] = (u32)&ServiceExternal;
	vector[0x5F] = (u32)&ServiceExternal;

	sh2::regs::SetVbr(vbr_address);
}

extern "C" void _main()
{
	InitializeInterrupts();

	//allow vblankin vblankout interrupts
	//disable the rest
	u16 interrupts_to_enable = 
		  scu::IrqMasks::kVblankIn 
		| scu::IrqMasks::kVblankOut;

	scu::SetEnabledInterrupts(interrupts_to_enable);

	//enable interrupts on the sh2 side
	Sh2EnableInterrupts();

	io::Init();
	
	while(1)
	{
		simple::DoNothing();
	}
}