
/*
This core is fairly complete
but still has bugs
needs:
dma
midi
call address
volume fix bugs
interrupts/timers probably have bugs
fm is not emulated
*/

#define USINGOLDTIMING
#define USINGOLDTIMING

struct Scsp
	//	: Processor
{

	array::RamArray<0x80000> sound_ram;

#include "../m68k/m68k.h"
	M68k m68k;//m68k is only hooked up to the scsp, so we might as well put it here

	Saturn&saturn__;

	//Scsp() : {}
	Scsp(Saturn&saturni) : saturn__(saturni), interrupt_controller(*this), m68k(*this), dsp(sound_ram){}

	#include "scsp_dsp.h"
	ScspDsp dsp;

	enum EnvelopeState{
		kAttack,
		kDecay1,
		kDecay2,
		kRelease,
		kNull
	};

	//used in a few different classes
	//todo move me somewhere better
	static float DecibelsToPercentage(int decibels)
	{

		//return pow(10.0,decibels/20.0);


		static float cache[2010];
		static bool cached = false;
		if(!cached)
		{
			for(int i = -1001; i < 1001; i++)
				cache[i+1002] = pow(10.0,i/20.0);

			cached = true;
			return  cache[decibels+1002];
		}
		else
		{
			//looks like this is correct
			//todo verify infinite and zero values
#if 0
			float result = pow(10.0,decibels/20.0);
			float val1 = cache[decibels+1002]-1;
			float val2 = cache[decibels+1002]+1;
			assert(result >= val1);
			assert(result <= val2);
#endif
			//caching for speed purposes
			return cache[decibels+1002];
		}
#if 0
		return pow(10.0,decibels/20.0);
#endif
		assert(false);
		return 0;
	}

	struct Sample{
		int l;
		int r;

		Sample() : l(0), r(0) {}
	};

	struct Slot
	{
		struct Lfo
		{
			float phase_;
			enum LfoWaveforms{
				kSaw = 0,
				kSquare = 1,
				kTriangle = 2,
				kNoise = 3
			};

			enum LfoSelections{
				kAmplitude = true,
				kPitch = false
			};

			int SawOscillator(LfoSelections is_amplitude)
			{
				if(is_amplitude)
					return 0xFF - phase_;
				else
				{
					//this is wrong todo
					return 0xFF - phase_;
				}
			}

			int SquareOscillator(LfoSelections is_amplitude)
			{
				if(is_amplitude) {
					if(phase_ < 0x7F)
						return 0;
					else
						return 0xFF;
				}
				else {
					if(phase_ < 0x7F)
						return 0x7F;
					else
						return 0x0;
				}
			}

			int TriangleOscillator(LfoSelections is_amplitude)
			{
				if(is_amplitude) {
					if(phase_ < (0xFF/2))
						return 0xFF - (phase_ * 2);
					else
						return (phase_ - (0xFF / 2)) * 2;
				}
				else {
					if(phase_<0x40)
						return phase_*2;
					else if(phase_<0x80)
						return 0xFF-phase_*2;
					else if(phase_<0xC0)
						return 0x100-phase_*2;
					else
						return phase_*2-0x1FF;
				}
			}

			int NoiseOscillator(LfoSelections is_amplitude)
			{
				//todo seed rand()
				//or get the actual data off the chip
				if(is_amplitude)
					return rand()&0xFF;
				else
					return (rand()& 0xFF)- 0x7F;
			}

			bool active;

			//this needs to be verified
			void Reset(int lfore)
			{
				// ScspLog("lfo reset\n");
				if(lfore == 0)
					active = true;
				else
					active = false;
			}

			Lfo(): phase_(0), lfof_table_value_(0), active(false) {}

			int GetLfoValue(LfoWaveforms waveform, LfoSelections lfo_select){

				if(!active)
					return 0;

				int result;
				if(waveform == kSaw)
					result = SawOscillator(lfo_select);
				if(waveform == kSquare)
					result = SquareOscillator(lfo_select);
				if(waveform == kTriangle)
					result = TriangleOscillator(lfo_select);
				if(waveform == kNoise)
					result = NoiseOscillator(lfo_select);

				return result;
			}

			void Update()
			{
				phase_+= lfo_step_speed;

				if(phase_ > 0xFF)
					phase_ = 0;
			}

			void SetOscillationFrequency(int lfof){
				//page 77
				const float kLfoSteps = 256;
				//how quickly we are oscillating
				lfo_step_speed = OscillationFrequencyTable[lfof]*kLfoSteps/44100.0;
			}

			float AmplitudeLfo(Slot &slot)
			{
				SetOscillationFrequency(slot.regs.lfof);
				Update();
				int alfows = slot.regs.alfows;
				int current_lfo_value = GetLfoValue((LfoWaveforms)alfows,kAmplitude);
				const float kAmplitudeDisplacementTableInDecibels[8] = {0,.4,.8,1.5,3,6,12,24};
				float displacement_limit = kAmplitudeDisplacementTableInDecibels[slot.regs.alfos];
				float value = DecibelsToPercentage((displacement_limit*current_lfo_value)/256.0);
				return value;
			}
			//should be cached
			static float CentsToSpeedIncrement(int cents)
			{
				return pow(2.0,cents/1200.0);
			}
			float PitchLfo(const Slot &slot)
			{
				//yeah not correct
				SetOscillationFrequency(slot.regs.lfof);
				Update();
				int plfows = slot.regs.plfows;
				int current_lfo_value = GetLfoValue((LfoWaveforms)plfows,kPitch);

				//maximum displacements i guess, so we need to scale the waveform to this
				const float kPitchLfoDisplacementTableInCents[8] = {0,7,13.5,27,55,112,230,494};
				float displacement_limit = kPitchLfoDisplacementTableInCents[slot.regs.plfos];
				return CentsToSpeedIncrement(((displacement_limit*(float) current_lfo_value)/128.0));
			}
		private:
			float lfo_step_speed;
			float lfof_table_value_;
		};

		struct SlotRegisters
		{
			u1 keyonex;
			u1 kyonb;
			u2 sbctl;
			u2 ssctl;
			u2 lpctl;
			u1 pcm8b;
			u20 sa;
			u16 lsa;
			u16 lea;
			u5 d2r;
			u5 d1r;
			u1 eghold;
			u5 ar;
			u1 lpslnk;
			u4 krs;
			u5 dl;
			u5 rr;
			u1 stwinh;
			u1 sdir;
			u8 tl;
			u4 mdl;
			u6 mdxsl;
			u6 mdysl;
			u4 oct;
			u10 fns;
			u1 lfore;
			u5 lfof;
			u2 plfows;
			u3 plfos;
			u2 alfows;
			u3 alfos;
			u4 isel;
			u3 imxl;
			u3 disdl;
			u5 dipan;
			u3 efsdl;
			u5 efpan;

			//address is bytes relative to the slot
			//our types take care of masking off upper bits
			//in assignment
			void Write(int address, u8 data){
				switch(address)
				{
				case 0:
					kyonb = data >> 3;
					sbctl = data >> 1;
					ssctl = (ssctl & 1) | (data & 1) << 1; //upper bit
					break;
				case 1:
					ssctl = (ssctl & 2) + ((data >> 7) & 1); //lower bit
					lpctl = data >> 5;
					pcm8b = data >> 4;
					sa = data << 16 | (sa & 0xFFFF);//upper bits
					break;
				case 2:
					sa = (sa & 0xF00FF) | (data << 8);//upper byte
					break;
				case 3:
					sa = (sa & 0xFFF00) | data;//lower byte
					break;
				case 4:
					lsa = (lsa & 0xFF) | data << 8;
					break;
				case 5:
					lsa = (lsa & 0xFF00) | data;
					break;
				case 6:
					lea = (lea & 0xFF) | data << 8;
					break;
				case 7:
					lea = (lea & 0xFF00) | data;
					break;
				case 8:
					d2r = data >> 3;
					d1r = (d1r & 3) | (data << 2); //upper
					break;
				case 9:
					d1r = (d1r & 0x1C) | data >> 6;
					eghold = data >> 5;
					ar = data;
					break;
				case 10:
					lpslnk = data >> 6;
					krs = data >> 2;
					dl = (dl & 7) | (data << 3);
					break;
				case 11:
					dl = (dl & 0x18) | data >> 5;
					rr = data;
					break;
				case 12:
					stwinh = data >> 1;
					sdir = data;
					break;
				case 13:
					tl = data;
					break;
				case 14:
					mdl = data >> 4;
					mdxsl = (mdxsl & 0x3 ) | data << 2;
					break;
				case 15:
					mdxsl = (mdxsl & 0x3C) | data >> 6;
					mdysl = data;
					break;
				case 16:
					oct = data >> 3;
					fns = (fns & 0xFF) | (data << 8);
					break;
				case 17:
					fns = (fns & 0x300) | data;
					break;
				case 18:
					lfore = data >> 7;
					lfof = data >> 2;
					plfows = data;
					break;
				case 19:
					plfos = data >> 5;
					alfows = data >> 3;
					alfos = data;
					break;
				case 21:
					isel = data >> 3;
					imxl = data;
					break;
				case 22:
					disdl = data >> 5;
					dipan = data;
					break;
				case 23:
					efsdl = data >> 5;
					efpan = data;
					break;
				default:
					// ScspLog("unhandled scsp slot write\n");
					//assert(false);
					break;
				}
			}
			u8 Read(int address){

				u8 result = 0;
				switch(address) 
				{
				case 0:
					result |= keyonex << 4;
					result |= kyonb << 3;
					result |= sbctl << 1;
					result |= (ssctl >> 1) & 1;//upper bit
					break;

				case 1:
					result |= ssctl << 7;//lower bit
					result |= lpctl << 5;
					result |= pcm8b << 4;
					result |= (sa >> 16) & 0xF;//upper bits
					break;

				case 2:
					result |= (sa >> 8) & 0xFF;
					break;
				case 3:
					result |= sa & 0xFF;
					break;
				case 4:
					result |= lsa >> 8;
					break;
				case 5:
					result |= lsa & 0xFF;
					break;
				case 6:
					result |= lea >> 8;
					break;
				case 7:
					result |= lea & 0xFF;
					break;
				case 8:
					result |= d2r << 3;
					result |= d1r >> 2;//upper bits
					break;

				case 9:
					result |= d1r << 6; //lower bits
					result |= eghold << 5;
					result |= ar;
					break;

				case 10:
					result |= lpslnk << 6;
					result |= krs << 2;
					result |= dl >> 3;
					break;

				case 11:
					result |= dl << 4;
					result |= rr;
					break;

				case 12:
					result |= stwinh << 1;
					result |= sdir;
					break;
				case 13:
					result = tl;
					break;

				case 14:
					result |= mdl << 3;
					result |= mdxsl >> 2;
					break;

				case 15:
					result |= mdxsl << 6;
					result |= mdysl;
					break;

				case 16:
					result |= oct << 3;
					result |= fns >> 8;
					break;

				case 17:
					result = fns;
					break;
				case 18:
					result |= lfore << 7;
					result |= lfof << 2;
					result |= plfows;
					break;

				case 19:
					result |= plfos << 5;
					result |= alfows << 3;
					result |= alfos;
					break;
				case 21:
					result |= isel << 3;
					result |= imxl;
					break;
				case 22:
					result |= disdl << 5;
					result |= dipan;
					break;

				case 23:
					result |= efsdl << 5;
					result |= efpan;
					break;

				default:
					break;
					// assert(false);
					// ScspLog("unhandled read\n");
				}

				return result;

			}
		};

		struct SlotState{
			struct PhaseInfo
			{
				float pos;
				bool backwards;
			}phase;

			struct{
				EnvelopeState mode;
				float volume;
				float percentage;//from the envelope generator
			}envelope;

			s16 waveform_data_buffer;
			s16 output_register;
			Pair<int,int> address_pointer;
			Pair<s16,s16> interpolator_input;

			bool active;
		};

		SlotRegisters regs;
		SlotState state;
		Lfo lfo;
	};


	template <int vector>//vector level
	struct Timer
	{
		int value;
		int ctl;
#ifdef USINGOLDTIMING
		Timer(): value(0xFFFF), ctl(0) {}
#else
		Timer(): value(0xFF00), ctl(0) {}
#endif
		void Tick(u11 &scipd)
		{
			if(value<=0xff00)
			{
				int shift = 8-(ctl&0x7);
				value += 1 << shift;
				if (value > 0xFF00)
				{
					value = 0xFFFF;
					scipd|=vector;
				}
			}
		}
	};

	struct ControlRegisters
	{
		ControlRegisters(){}

		u1 mem4mb;
		u1 dac18b;
		u4 mvol;
		u2 rbl;
		u7 rbp;
		u1 mofull;
		u1 moemp;
		u1 miovf;
		u1 mifull;
		u1 miemp;
		u8 mibuf;
		u8 mobuf;
		u5 mslc;
		u4 ca;

		u20 dmea;//check this, byte 1 is not present
		u12 drga;//check this, byte 1 is not present
		u1 dgate;
		u1 ddir;
		u1 dexe;
		u12 dtlg;//check this, byte 1 is not present
		Timer<0x40> tima;
		Timer<0x80> timb;
		Timer<0x100> timc;
		u11 scieb;
		u11 scipd;
		u11 scire;
		u8 scilv0;
		u8 scilv1;
		u8 scilv2;
		u11 mcieb;
		u11 mcipd;
		u11 mcire;

		void Write(int address, u8 data){
			switch(address)
			{
			case 0:
				mem4mb = data >> 1;
				dac18b = data;
				break;
			case 1:
				mvol = data;
				break;
			case 2:
				rbl = (rbl&1) | data<< 1;
				break;
			case 3:
				rbl = (rbl & 2) | (data >> 7) & 1;
				rbp = data;
				break;
			case 4:
				mofull = data >> 4;
				moemp = data >> 3;
				miovf = data >> 2;
				mifull = data >> 1;
				miemp = data;
				break;
			case 5:
				mibuf = data;
				break;
			case 7:
				mobuf = data;
				break;
			case 8:
				mslc = data >> 3;
				printf("fix write");
				// ScspLog("control reg write");
				//ca = (ca &1) | (data & 7) << 1;
				break;
			case 9:
				printf("fix writesss");
				//ScspLog("control reg write");
				//ca = (ca & 0xE) | (data >> 7) & 1;
				break;

				//i'm going to leave the low byte for the moment
				//dmea is 19 bit
			case 0x12:
				dmea = (dmea & 0x780FF) | data << 8;//maybe 9
				break;
			case 0x13:
				dmea = (dmea & 0x78000) | data << 1;
				break;
			case 0x14:
				dmea = (dmea & 0x7FFF) | data << 16;
				drga = (drga & 0x7F) | data << 9;
				break;
			case 0x15:
				drga = (drga & 0x780) | data << 1;
				break;
				//todo get rid of the value shifting << 8 stuff
			case 0x18:
				tima.ctl = data & 7;
				break;
			case 0x19:
				tima.value = data << 8;
				break;
			case 0x1A:
				//				#ifdef USINGOLDTIMING
				//				timb.ctl = 1 << (data & 7);
				//			#else
				timb.ctl = data & 7;
				//				#endif
				break;
			case 0x1B:
				timb.value = data << 8;
				break;
			case 0x1C:
				timc.ctl = data & 7;
				break;
			case 0x1D:
				timc.value = data << 8;
				break;
			case 0x1E:
#ifdef USINGOLDTIMING
				scieb = (scieb & 0xFF) | data << 8;
#else
				int i;
				scieb = (scieb & 0xFF) + (data << 8);

				for (i = 0; i < 3; i++)
				{
					if (scieb & (1 << i) && scipd & (1 << i))
						assert(false);
					//scspdebugging::TriggerSoundInterrupt((1 << (i+8)));
				}
#endif
				break;
			case 0x1F:
#ifdef USINGOLDTIMING
				scieb = (scieb & 0x700) | data;
#else
				{
					scieb = (scieb & 0x700) + data;

					for (int i = 0; i < 8; i++)
					{
						if (scieb & (1 << i) && scipd & (1 << i))
							//scsp_trigger_sound_interrupt((1 << i));
							//scspdebugging::TriggerSoundInterrupt(1 << i);
							assert(false);
					}
				}
#endif
				break;
			case 0x20:
#ifdef USINGOLDTIMING
				scipd = (scipd & 0xFF) | data << 8;
#endif
				break;
			case 0x21:
#ifdef USINGOLDTIMING
				scipd = (scipd & 0x700) | data;
#else
				//if (data & 0x20) scspdebugging::TriggerSoundInterrupt(0x20);
				assert(false);
#endif
				break;
			case 0x22:
#ifdef USINGOLDTIMING
				printf("fix scire");
				scire = (scire & 0xFF) | data << 8;
#else
				scipd &= ~(data << 8);
#endif
				break;
			case 0x23:
#ifdef USINGOLDTIMING
				printf("fix scire");
				scire = (scire & 0x700) | data;
#else
				scipd &= ~(u32)data;
#endif
				break;
			case 0x25:
				scilv0 = data;
				break;
			case 0x27:
				scilv1 = data;
				break;
			case 0x29:
				scilv2 = data;
				break;
			case 0x2A:
				mcieb = (mcieb & 0xFF) | data << 8;
				break;
			case 0x2B:
				mcieb = (mcieb & 0x700) | data;
				break;
			case 0x2C:
				mcipd = (mcipd & 0xFF) | data << 8;
				break;
			case 0x2D:
				mcipd = (mcipd & 0x700) | data;
				break;
			case 0x2E:
				mcire = (mcire & 0xFF) | data << 8;
				break;
			case 0x2F:
				mcire = (mcire & 0x700) | data;
				break;
			default:
				break;
				//assert(false);
				// ScspLog("unhandled control reg write");
			}
		}
		u8 Read(int address){
			u8 result;

			switch(address)
			{
			case 0:
				result = mem4mb << 1;
				result |= dac18b;
				break;

			case 1:
				result = mvol;
				break;
			case 2:
				result = rbl >> 1;
				break;
			case 3:
				result = rbl << 7;
				result |= rbp;
				break;
			case 4:
				result = mofull << 4;
				result |= moemp << 3;
				result |= miovf << 2;
				result |= mifull << 1;
				result |= miemp;
				break;
			case 5:
				result = mibuf;
				break;
			case 6:
				result = 0;
				//nothing here
				break;
			case 7:
				result = mobuf;
				break;
			case 8:
				result = mslc << 3;
				result |= ca >> 1;
				break;

			case 9:
				//rockin b minesweeper relies on this
				//wrong wrong wrong fix this
				printf("fix ca");
				result = ca << 7;
				break;

			case 0x20:

				result = scipd >> 8;
				break;

			case 0x21:
				result = scipd;
				break;

				// case 0x20:
				// result = timb.ctl;
				// break;
				// case 0x21:
				// result = timb.value >> 8;
				// break;

			default:
				result = 0;
				//nothing here
				//assert(false);
				// ScspLog("unhandled byte read");
				break;

			}

			return result;
		}
	};

	ControlRegisters control_registers;
	array::Array<Slot,32> slots;

	void KeyOn(int data, Scsp::Slot *slots)
	{
		//keyonex
		if((data >> 4) & 1) {
			for(int i = 0; i < 32; i++) {
				//key_on
				if(slots[i].regs.kyonb) {
					if(slots[i].state.envelope.mode == kRelease)
					{
						slots[i].state.envelope.mode = kAttack;
						slots[i].state.envelope.volume = 0;//more likely//0x17F;//verify this
						slots[i].state.phase.pos = 0;
						//this is only here as an optimization
						slots[i].state.active = true;

						printf("key_on");
						//ScspLog("keyon\n");
					}
					//else
					//ScspLog("rejected keyon\n");
				}
				//key_off
				else {
					slots[i].state.envelope.mode = kRelease;
					//ScspLog("key_off\n");
				}
			}
		}
	}

	void WriteByte(const int &address_, const u8 & data)
	{
		int address = address_ & 0xFFF;
		//slots
		if(address >= 0 && address < 0x400) {
			slots[address >> 5].regs.Write(address & 0x1f, data);
			if(address % 0x20 == 0)
				KeyOn(data,slots.data_);
			if(address % 0x20 == 18)
				slots[address >> 5].lfo.Reset(data >> 7);
		}
		//control register
		else if(address >= 0x400 && address <= 0x42F) {
			control_registers.Write(address-0x400,data);
			if(address == 0x403)
			{
				dsp.regs.ring_buffer_pointer = data & 0x7f;
			}
			switch(control_registers.rbl)
			{
			case 0:
				dsp.regs.ring_buffer_length = 8*1024;
				break;
			case 1:
				dsp.regs.ring_buffer_length = 16*1024;
				break;
			case 2:
				dsp.regs.ring_buffer_length = 32*1024;
				break;
			case 3:
				dsp.regs.ring_buffer_length = 64*1024;
				break;
			}
		}
		//dsp stuff, but it only is word acess right?
		else if(address >= 0x700 && address <= 0xEE3)
		{
		}
	}
	u8 ReadByte(const int &address_)
	{
		int address = address_ & 0xFFF;

		//slots
		if(address >= 0 && address < 0x400) {
			return slots[address >> 5].regs.Read(address & 0x1f);
		}
		//control register
		else if(address >= 0x400 && address <= 0x42F) {
			if(address == 0x409)
				return static_cast<u16>((slots[control_registers.mslc].state.phase.pos / 4096.0));
			return control_registers.Read(address-0x400);
		}
		//dsp stuff
		else if(address >= 0x700 && address <= 0xEE3)
		{
			return 0;
			//	assert(false);
		}
		assert(false);
		return 0;
	}

	//ring buffer class
	template <typename T, int size>
	struct SoundStack
	{
		//i guess we have to init to -6 to get the slots to write to the correct positions
		SoundStack(): current_position(size-6), sound_stack(size), size_mask(size-1) {}

		void Write(T sample)
		{
			sound_stack[current_position] = sample;
			Increment();
		}

		T Read(int position)
		{
			return sound_stack[position & size_mask];
		}

		void Increment()
		{
			current_position++;
			current_position&=size_mask;
		}

		//	DISALLOW_COPY_AND_ASSIGN(SoundStack);
		int current_position;
		std::vector<T> sound_stack;
		int size_mask;
	private:
	};

	//64 word ring buffer
	SoundStack<s16, 64> sound_stack;

	struct Operations
	{
		struct Op1{

			static float log_base_2(float n)
			{
				return log(n)/log(2.0);
			}
			static float ConvertFns(int fns)
			{
				static bool cached = false;
				static u32 cache[0x400];

				if(!cached)
				{
					for(int i = 0; i < 0x400; i++) {
						float fcent=1200.0*log_base_2((1024.0+(double)i)/1024.0);
						fcent=44100.0*pow(2.0,fcent/1200.0);
						cache[i] = fcent*1;
					}
					cached = true;
				}

				return cache[fns];
			}

			static float Frequency(int oct, int fns)
			{
				unsigned long long freq;

				freq = ConvertFns(fns);
				if(oct&8)
					freq>>=(16-oct);
				else
					freq<<=oct;

				return freq/44100.0;
			}

			static float NoLoop(const float pos, const float speed){
				return pos+speed;
			}
			static float NormalLoop(const float pos, const float speed, const int loop_start_address, const int loop_end_address){
				float new_pos = pos+speed;
				if(new_pos > loop_end_address)
					new_pos = loop_start_address;
				return new_pos;
			}
			//this function makes an assignment to backwards
			static float ReverseLoop(
				const float pos,
				const float speed,
				const int loop_start_address,
				const int loop_end_address,
				bool &backwards)
			{
				int new_pos;

				//todo off by a sample
				if(pos > loop_start_address && !backwards) {
					backwards = true;
					new_pos = loop_end_address;
				}
				else if(pos < loop_start_address && backwards)
				{
					new_pos = loop_end_address;
				}
				if(!backwards)
					return new_pos+speed;
				else
					return new_pos-speed;
			}
			//this function makes an assignment to backwards
			static float PingPongLoop(
				const float pos,
				const float speed,
				const int loop_start_address,
				const int loop_end_address,
				bool &backwards){
					//todo off by a sample
					if(pos >= loop_end_address)
						backwards = true;

					else if(pos <= loop_start_address)
						backwards = false;

					if(!backwards)
						return pos+speed;
					else
						return pos-speed;
			}

			//////////////////////////////////////////////////////////////////////////////////////////

			int FNS_Table[0x400];

			void MakeTable()
			{
				for(int i=0;i<0x400;++i)
				{
					float fcent=(double) 1200.0*log_base_2((double)(((double) 1024.0+(double)i)/(double)1024.0));
					fcent=(double) 44100.0*pow(2.0,fcent/1200.0);
					FNS_Table[i]=(float) fcent;//(1<<SHIFT) *
				}
			}

			float GetFreq(Slot &slot)
			{
				static bool initalized = false;
				if(!initalized)
				{
					MakeTable();
					initalized = true;
				}
				int octave=slot.regs.oct;
				u64 Fn;

				Fn=(FNS_Table[slot.regs.fns]);	//24.8
				if(octave&8)
					Fn>>=(16-octave);
				else
					Fn<<=octave;

				//slot->Fn = Fn;

				return Fn/(44100.0);
				return 0;
			}

			//////////////////////////////////////////////////////////////////////////////////////////

			//phase generator, pitch lfo
			void Exec(Slot &slot)
			{
				if(!slot.state.active)//only for optimization purposes
					return;

				//	printf("you are bugged");
				//float speed = GetFreq(slot);
				float speed = Frequency(slot.regs.oct,slot.regs.fns);

				speed *= slot.lfo.PitchLfo(slot);

				//speed += FrequencyModulation(slot);

				enum LoopTypes{
					kNone = 0,
					kNormal = 1,
					kReverse = 2,
					kAlternative = 3//Ping Pong
				};

				//slot.state.phase.pos+=speed;
#if 1
				switch(slot.regs.lpctl)
				{
				case kNone:
					slot.state.phase.pos = NoLoop(slot.state.phase.pos,speed);
					if(slot.state.phase.pos >= slot.regs.lea) {
						//ScspLog("hit end of sample, stopping\n");
						//probably wrong
						slot.state.active = false;
						//slot.state.envelope.mode = kRelease;
					}
					break;
				case kNormal:
					slot.state.phase.pos = NormalLoop(slot.state.phase.pos,speed,slot.regs.lsa,slot.regs.lea);
					break;
				case kReverse:
					slot.state.phase.pos = ReverseLoop(slot.state.phase.pos,speed,slot.regs.lsa,slot.regs.lea,slot.state.phase.backwards);
					break;
				case kAlternative:
					slot.state.phase.pos = PingPongLoop(slot.state.phase.pos,speed,slot.regs.lsa,slot.regs.lea,slot.state.phase.backwards);
					break;
				default:
					assert(false);
				}

				//slot.state.phase.pos += FrequencyModulation(slot);
#endif
			}

		};

		//address ptr, sound stack read
		static void op2(Slot &slot)
		{
			if(!slot.state.active)//only for optimization purposes
				return;

			int phase = slot.state.phase.pos;

			int increment = 1;

			//16 bit sounds take up 2x the space
			if(!slot.regs.pcm8b) {
				increment = 2;
				phase*=2;
			}

			slot.state.address_pointer.first = slot.regs.sa + phase;
			slot.state.address_pointer.second = slot.regs.sa + phase + increment;
		}
		//waveform dram fetch
		static void op3(Slot &slot, array::RamArray<0x80000> &sound_ram)
		{

			if(!slot.state.active)//only for optimization purposes
				return;
			if(slot.regs.pcm8b)
			{
				//we have to double the volume since it's 8 bit
				slot.state.interpolator_input.first = sound_ram.Read<kByte>(slot.state.address_pointer.first & 0x7FFFE) << 8;
				slot.state.interpolator_input.second = sound_ram.Read<kByte>(slot.state.address_pointer.second & 0x7FFFE) << 8;
				//		MemoryCycle();
			}
			else
			{
//WRONG BYTE ORDER
//WHY IS THE BYTE ORDER WRONG?
//IF YOU ARE HEARING STATIC THIS IS PROBABLY WHY
//FIX ME FIX ME FIX ME
				slot.state.interpolator_input.first = sound_ram.Read<kWord>(slot.state.address_pointer.first & 0x7FFFE);
				//	MemoryCycle();
				slot.state.interpolator_input.second = sound_ram.Read<kWord>(slot.state.address_pointer.second & 0x7FFFE);
				//	MemoryCycle();

				s16 i = slot.state.interpolator_input.first;
				i = ((i & 0xFF00) >> 8) | ((i & 0xFF) << 8);
				slot.state.interpolator_input.first = i;

				s16 j = slot.state.interpolator_input.second;
				j = ((j & 0xFF00) >> 8) | ((j & 0xFF) << 8);
				slot.state.interpolator_input.second = j;

			}
			//slot.state.interpolator_input = MemoryController(slot.state.address_pointer,slot);

			//inverter, verify me

			if(slot.regs.sbctl & 1)
				slot.state.waveform_data_buffer ^= 0x7FFF;
			if(slot.regs.sbctl & 2)
				slot.state.waveform_data_buffer = static_cast<s16>(slot.state.waveform_data_buffer^0x8000);

		}

		struct Op4{

			struct EnvelopeGenerator
			{
				float kMaxEnvelopeGain;// = 0x3FF;

				static int ScaleRate(const int rate_register, const Scsp::Slot &slot)
				{
					//handle key rate scaling
					int rate = slot.regs.krs + (slot.regs.fns >> 9) + rate_register * 2;

					if(slot.regs.krs == 0xF)
						rate -=0xF;

					if(slot.regs.oct & 8)
						rate -=(16 - slot.regs.oct) * 2;
					else
						rate += slot.regs.oct * 2;

					if(rate < 0)
						rate = 0;
					if(rate > 63)
						rate = 63;

					return rate;
				}
				//convert the rate to a percentage change
				static float GetEnvelopeRate(float table_value, const Scsp::Slot &slot)
				{
					const float constant = 10000;

					//i guess we add/subtract nothing
					if(table_value == 0)
						return 0;
					else
						return ((1023.0*constant)/(44100*table_value));
				}

				void Attack(Slot &slot){

					//is the attack rate scaled? it screws up for the fm demo with test_dl
					int scaled_rate = slot.regs.ar;//ScaleRate(slot.regs.ar,slot);
					float millisecond_rate = kAttackChangeTimeInMilliseconds[scaled_rate];
					float envelope_rate = GetEnvelopeRate(millisecond_rate,slot);

					slot.state.envelope.volume += envelope_rate;
					if(slot.regs.lpslnk) {
						if(slot.state.phase.pos > slot.regs.lsa) {
							//if(envelope_gain >= kMaxEnvelopeGain) {
							slot.state.envelope.mode = kDecay1;
							//ScspLog("lpslnk switch\n");
							//}
						}
					}
					else if(slot.state.envelope.volume >= kMaxEnvelopeGain) {
						slot.state.envelope.mode = kDecay1;
						//ScspLog("switching to decay 1\n");
					}
				}

				static void DecayCommon(Slot &slot, const int &rate)
				{
					int scaled_rate = ScaleRate(rate,slot);
					float millisecond_rate = kDecayChangeTimeInMilliseconds[scaled_rate];
					float envelope_rate = GetEnvelopeRate(millisecond_rate,slot);
					slot.state.envelope.volume -= envelope_rate;
				}

				void Decay1(Slot &slot){
					//it's supposed to match (==), but this may require
					//more exactness than we have in order to work correctly
					if((static_cast<int>(slot.state.envelope.volume)/32) <= (0x1f - slot.regs.dl)) {//0x1f -
						slot.state.envelope.mode = kDecay2;
						//ScspLog("switching to decay 2\n");
					}

					DecayCommon(slot,slot.regs.d1r);
				}

				void Decay2(Slot &slot)
				{
					DecayCommon(slot,slot.regs.d2r);
				}

				void Release(Slot &slot)
				{
					DecayCommon(slot,slot.regs.rr);

					//hmm
					if(slot.state.envelope.volume <= 0)
						slot.state.active = false;
				}

				EnvelopeGenerator(): kMaxEnvelopeGain(0x3FF){}

				float Exec(Scsp::Slot &slot)
				{
					//float envelope_gain = slot.state.envelope.volume;

					switch(slot.state.envelope.mode)
					{
					case kAttack:
						Attack(slot);
						//we don't want this to be an assignment because
						//it will break the envelope timing
						if(slot.regs.eghold)
							return kMaxEnvelopeGain / 1024.0;
						break;
					case kDecay1:
						Decay1(slot);
						break;
					case kDecay2:
						Decay2(slot);
						break;
					case kRelease:
						Release(slot);
						break;
					}

					//clamp
					if(slot.state.envelope.volume > kMaxEnvelopeGain)
						slot.state.envelope.volume = kMaxEnvelopeGain;
					if(slot.state.envelope.volume < 0)
						slot.state.envelope.volume = 0;

					float bleh = slot.lfo.AmplitudeLfo(slot);

					return slot.state.envelope.volume / 1024.0;
				}
			};

			enum InterpolationTypes
			{
				kNone,
				kLinear,
				kCosine
			};

			//requires 2 samples
			template <InterpolationTypes type>
			struct Interpolator{

				static int None(int a, int b, float mu)
				{
					return a;
				}
				static int Linear(int a, int b, float mu)
				{
					return (a*(1-mu)+b*mu);
				}

				static int Cosine(int a, int b, float mu)
				{
					const float pi = 3.14159265;
					double mu2 = (1-cos(mu*pi))/2;
					return(a*(1-mu2)+b*mu2);
				}

				static int Interpolate(int a, int b, float mu)
				{
					if(type == kCosine)
						return Cosine(a,b,mu);
					else if(type == kLinear)
						return Linear(a,b,mu);
					else
						return None(a,b,mu);
				}
			};

			Interpolator<kCosine> interpolator;
			EnvelopeGenerator envelope_generator;

			//interpolation, eg, alfo
			void Exec(Slot &slot)
			{
				if(!slot.state.active)//only for optimization purposes
					return;
				int a = slot.state.interpolator_input.first;
				int b = slot.state.interpolator_input.second;
				double int_part = 0.0;
				float mu = modf(slot.state.phase.pos,&int_part);

				slot.state.waveform_data_buffer = interpolator.Interpolate(a,b,mu);

				//slot.state.amplitude_lfo_percentage = AmplitudeLfo(slot);
				slot.state.envelope.percentage = envelope_generator.Exec(slot);//envelope::EnvelopeGenerator(slot);
			}
		};
		//level calc
		static void op5(Slot &slot)
		{
			//level calc takes 2 cycles but
			//i don't know the timing
			//also it almost certainly doesn't matter
		}

		struct Op6{

			//cached for performance
			static float TotalLevel(Scsp::Slot &slot)
			{
				static bool cached = false;
				static float tl_cache[0xFF];
				if(!cached)
				{
					for(int tl_value = 0; tl_value <= 0xFF; tl_value++) {
						const float kAttenuationTable[8] = {-.4,-.8,-1.5,-3,-6,-12,-24,-48};
						float attenuation = 0;

						for (int i = 0; i < 8; i++)
						{
							if(tl_value & (1 << i))
								attenuation += kAttenuationTable[i];
						}

						tl_cache[tl_value] = DecibelsToPercentage(attenuation);
					}

					tl_cache[0] = 1;
					cached = true;

					return tl_cache[slot.regs.tl];
				}
				else
					return tl_cache[slot.regs.tl];
			}

			//page 36 2 to 1 selector
			//and level calculator block
			static int SoundSourceControl(Scsp::Slot &slot)
			{
				switch(slot.regs.ssctl)
				{
				case 0:
					//external dram data
					return slot.state.waveform_data_buffer;
					break;
				case 1:
					//noise
					//	assert(false);
					return slot.lfo.NoiseOscillator(Scsp::Slot::Lfo::kAmplitude);
					break;
				case 2:
					//zero
					return 0;
					break;
				default:
					assert(false);
					//ScspLog("invalid ssctl value");
				}
				assert(false);
				return 0;
			}

			//level calc 2
			void Exec(Slot &slot)
			{
				if(!slot.state.active)//only for optimization purposes
					return;
				if(slot.regs.sdir)
					//direct output without eg, tl, alfo
					return;

				slot.state.waveform_data_buffer = SoundSourceControl(slot);
				//slot.state.waveform_data_buffer *= AmplitudeLfo();
				float attenuation = TotalLevel(slot);
				slot.state.waveform_data_buffer *= attenuation;
				slot.state.waveform_data_buffer *= slot.state.envelope.percentage;
			}
		};
		//sound stack write
		void op7(Slot &slot, SoundStack<s16,64> &sound_stack)
		{
			if(!slot.state.active) return;//only for optimization purposes

			slot.state.output_register = slot.state.waveform_data_buffer;

			//stack write inhibit
			if(!slot.regs.stwinh) {
				//assert(false);
				sound_stack.Write(slot.state.output_register);
			}
		}

		Op1 op1;
		Op4 op4;
		Op6 op6;
	};
	Operations ops;

	struct Mixer
	{
		static float ApplySendVolume(int register_value, float input_sample)
		{
			//disdl , efsdl, imxl, and mvol
			const float kSendLevelTableInDecibels[8] = {-1000,-36,-30,-24,-18,-12,-6,0};
			float decibel_volume = kSendLevelTableInDecibels[register_value];
			float percentage_volume = DecibelsToPercentage(decibel_volume);
			return input_sample * percentage_volume;
		}
		static Sample Pan(const int &register_value, const int &input_sample)
		{
			Sample output;

			//efpan, i think each table is the same though
			const float kEffectPanningTableInDecibels[16] = {-0,-3,-6,-9,-12,-15,-18,-21,-24,-27,-30,-33,-36,-39,-42,-1000};//no send

			if(register_value == 0)
			{
				output.l += input_sample;
				output.r += input_sample;
			}
			else if(register_value >= 0x10)
			{
				//subtract on right side
				float panning_value = kEffectPanningTableInDecibels[register_value-0x10];
				output.r += input_sample * DecibelsToPercentage(panning_value);
				output.l += input_sample;//left is unchanged
			}
			else
			{
				//subtact on left side
				float panning_value = kEffectPanningTableInDecibels[register_value];
				output.l += input_sample * DecibelsToPercentage(panning_value);
				output.r += input_sample;//right is unchanged
			}
			return output;
		}

		static void DspInputMixer(Slot* slots, ScspDsp &dsp)
		{
			// [0] does not send
			//dunno if it's actually 32
			//pg 83
			for(int i=0;i<32;i++) {
				//imxl volume adjustment

				//if(!slots[i].state.active) continue;//only for optimization purposes

				int volume_adjusted_sample = ApplySendVolume(
					slots[i].regs.imxl,
					slots[i].state.output_register);

				dsp.SetSample(volume_adjusted_sample,slots[i].regs.isel);
			}
		}

		//efreg efsdl and efpan handled here
		Sample DspOutputMixer(Slot* slots, ScspDsp &dsp)
		{
			Sample dsp_output_sample;
#if 0
			//todo pointers suck
			s16* efreg = dsp.Exec();

			//so we have to go 0 -> slot 17
			//the last two are exts, cd audio in
			for(int i=0;i<18;i++)
			{
				//so apparently we can input 32 channels
				//but there's only 16 outputs
				if(slots[i].regs.efsdl)
				{

					//if(!slots[i].state.active) continue;//only for optimization purposes

					//each one needs to be panned before adding
					int volume_adjusted_sample = ApplySendVolume(slots[i].regs.efsdl,efreg[i]);
					Scsp::Sample effected_sample = Pan(slots[i].regs.efpan,volume_adjusted_sample);

					//mixing is messed up
					//todo fixme
					dsp_output_sample.l += effected_sample.l;
					dsp_output_sample.r += effected_sample.r;
				}
			}
#endif
			return dsp_output_sample;
		}

		Sample Exec(Slot* slots, const int &mvol, ScspDsp &dsp)
		{
			DspInputMixer(slots,dsp);
			Sample dsp_output_sample = DspOutputMixer(slots,dsp);//the input sample used to be cd audio

			Sample final_output;
			//disdl, dipan
			for(int i = 0; i < 32; i++)
			{
				//if(!slots[i].state.active) continue;//only for optimization purposes

				//		if(slots[i].state.output_register)
				//			printf("output!");

				//direct send level, direct pan
				int volume_adjusted_sample = ApplySendVolume(slots[i].regs.disdl,slots[i].state.output_register);
				Sample panned_sample = Pan(slots[i].regs.dipan,volume_adjusted_sample);

				//the /3 is a hack, not sure what is wrong with the volume levels
				final_output.l += dsp_output_sample.l + panned_sample.l/3;
				final_output.r += dsp_output_sample.r + panned_sample.r/3;
			}
			ApplySendVolume(mvol,final_output.l);
			ApplySendVolume(mvol,final_output.r);



			//HAX
			final_output.l = final_output.r = 0;
			for (int i = 0; i < 32; i++)
			{
//if(slots[i].state.output_register)
//printf("output!");
				final_output.l += slots[i].state.output_register >> 1;
				final_output.r += slots[i].state.output_register >> 1;
			}
			return final_output;
		}
	};

	Mixer mixer;

	struct InterruptController
	{
		Scsp &self;

		//it doesn't make sense to put the control reg class
		//inside the interrupt controller
		InterruptController(Scsp &scsp): self(scsp) {}

		struct Interrupts{
			enum Vectors{
				kExternal0 = 1 << 0,
				kExternal1 = 1 << 1,
				kExternal2 = 1 << 2,
				kMidiInput = 1 << 3,
				kDmaTransferEnd = 1 << 4,
				kCpu = 1 << 5,
				kTimerA = 1 << 6,
				kTimerB = 1 << 7,
				kTimerC = 1 << 8,
				kMidiOutput = 1 << 9,
				k1Sample = 1 << 10,
			};
		};

		void ResetInterrupts()
		{
			if (self.control_registers.scire & 0x40)
				self.m68k.SetInterruptRequest(-InterruptRequestTimerA);
			if (self.control_registers.scire & 0x180)
				self.m68k.SetInterruptRequest(-InterruptRequestTimerBC);
		}
		unsigned int Decode(unsigned char irq)
		{
			unsigned char result = 0;
			if(self.control_registers.scilv0&(1<<irq))
				result|=1;
			if(self.control_registers.scilv1&(1<<irq))
				result|=1<<1;
			if(self.control_registers.scilv2&(1<<irq))
				result|=1<<2;
			return result;
		}
		void CheckPendingInterruptRequests()
		{
			//todo midi interrupt etc

			InterruptRequestTimerA=Decode(6);
			InterruptRequestTimerBC=Decode(7);

			if(self.control_registers.scieb & Interrupts::kTimerA) {
				if(self.control_registers.scipd & Interrupts::kTimerA) {
					self.m68k.SetInterruptRequest(InterruptRequestTimerA);
					return;
				}
			}

			if(self.control_registers.scieb & Interrupts::kTimerB) {
				if(self.control_registers.scipd & Interrupts::kTimerB) {
					self.m68k.SetInterruptRequest(InterruptRequestTimerBC);
					return;
				}
			}

			if(self.control_registers.scieb & Interrupts::kTimerC) {
				if(self.control_registers.scipd & Interrupts::kTimerC) {
					self.m68k.SetInterruptRequest(InterruptRequestTimerBC);
					return;
				}
			}
		}
		int InterruptRequestTimerA, InterruptRequestTimerBC;
	};

	InterruptController interrupt_controller;

	//run one cycle
	void Tick(const int &current_cycle)
	{
		ops.op1.Exec(slots[current_cycle]);//phase generator, pitch lfo
		ops.op2(slots[(current_cycle-1)&0x1F]);//address ptr, sound stack read
		ops.op3(slots[(current_cycle-2)&0x1F],sound_ram);//waveform dram fetch
		ops.op4.Exec(slots[(current_cycle-3)&0x1F]);//interpolation, eg, alfo
		ops.op5(slots[(current_cycle-4)&0x1F]);//level calc
		ops.op6.Exec(slots[(current_cycle-5)&0x1F]);//level calc 2
		ops.op7(slots[(current_cycle-6)&0x1F],sound_stack);//sound stack write
	}

	struct Timing
	{
		int stage_clock;//curent stage 0->31 of scsp generation
		int dsp_clock;
		int sample_clock;
		int current_stage;
		int clock;
		static const int frequency = 22579200;

		Timing() : stage_clock(0),dsp_clock(0),sample_clock(0) {}
	}timing;

	
	

	void Execute()
	{
//each memory access is 4 cycles
//		if((clock % 15) == 0)
		
		if(timing.stage_clock == 0)
		{
			timing.stage_clock = 16;
			Tick(timing.current_stage++);//every tick takes 16 cycles
		}

		timing.stage_clock--;

		//execute dsp every 4 cycles
		//if((clock % 4) == 0)
		if(timing.dsp_clock == 0)
		{
//			dsp.Exec((clock%128) & 1);
			timing.dsp_clock = 4;
		}

		timing.dsp_clock--;

		//we have a new sample finished every 512 cycles
//		if((clock % 511)==0)
		if(timing.sample_clock == 0)
		{
			//dec is decremented every sample
		//	dsp.regs.dec--;

			timing.sample_clock = 512;

#ifdef USINGOLDTIMING
			control_registers.tima.Tick(control_registers.scipd);
			control_registers.timb.Tick(control_registers.scipd);
			control_registers.timc.Tick(control_registers.scipd);

			//not sure when interrupts are checked honestly
			interrupt_controller.CheckPendingInterruptRequests();
#endif
			//i have no idea about the mixer timing related to the ops
			//i don't think it's done in parallel during op7
			Sample sample = mixer.Exec(slots.data_,control_registers.mvol,dsp);

			//new sample is complete
			saturn__.InterfaceUpdateAudioSample(sample.l,sample.r);
		}

		timing.sample_clock--;

		if(timing.current_stage > 31)
		{
			timing.current_stage = 0;
		}

//		if(clock == frequency)
	//		memory_cycles = 0;//each memory cycle is equivalent to 4 main cycles

		//one cycle completed
		timing.clock++;
	}

	void GenerateSample(int* left_sample, int* right_sample)
	{
		for(int current_cycle = 0; current_cycle < 32; current_cycle++)
		{
			Tick(current_cycle);
		}
#ifdef USINGOLDTIMING
		control_registers.tima.Tick(control_registers.scipd);
		control_registers.timb.Tick(control_registers.scipd);
		control_registers.timc.Tick(control_registers.scipd);

		interrupt_controller.CheckPendingInterruptRequests();
#endif
		//i have no idea about the mixer timing related to the ops
		//i don't think it's done in parallel during op7
		Sample sample = mixer.Exec(slots.data_,control_registers.mvol,dsp);

		*left_sample = sample.l;
		*right_sample = sample.r;
	}

	//these have the potential for trouble since we are reusing the byte funct
	//also the main cpu can't write bytes
	void WriteWord(const int &address, const u16 &data)
	{
		if(address < 0x700)
{

#if 1
		WriteByte(address+1,data&0xFF);
		WriteByte(address, data >> 8);
#endif
}
else
		dsp.WriteWord(address,data);

#ifndef USINGOLDTIMING
		return;
#endif
	}

	u16 ReadWord(const int &address)
	{
		u16 result;
		result = ReadByte(address+1);
		result |= ReadByte(address) << 8;
#if 0
		if(address == 0x408)
		{
			//doesn't work i think
			int slot = control_registers.ca >> 11;
			int ca = static_cast<int>(slots[slot&0x1f].state.phase.pos)>>12;
			control_registers.ca &-~(0x780);
			control_registers.ca |=ca << 7;
		}
#endif
		return result;
	}

	template <OperationSize size>
	unsigned int Read(unsigned int address)
	{
		if(address == 0x00100408 || address == 0x00100409)
			int zzz = 0;
		if(address & 0xFFF == 0x408)
			int zzz = 0;
		//without the correct return value from the scsp
		//it dies. looks like we are forced to fix this.
		return ReadWord(address);
	}

	template <OperationSize size>
	void Write(unsigned int address, unsigned int data)
	{
		address &= 0xffff;
		if(size == kByte)
		{
#ifdef USINGOLDTIMING
			//this needs to be fixed
			if(address == 0x422 || address == 0x423) {
				control_registers.scipd&=~control_registers.scire;
				interrupt_controller.ResetInterrupts();

				//verify this stuff
				if (control_registers.tima.value >= 0xff00)
				{
					control_registers.scipd |= 0x40;
				}
				if (control_registers.timb.value >= 0xff00)
				{
					control_registers.scipd |= 0x80;
				}
				if (control_registers.timc.value >= 0xff00)
				{
					control_registers.scipd |= 0x100;
				}
				return;
			}
#endif
			return WriteByte(address,data);
		}
		else if(size == kWord)
			return WriteWord(address,data);
		else if(size == kLong)
		{

			WriteWord(address, data >> 16);
			WriteWord(address+2, data & 0xFFFF);
		}

	}
};