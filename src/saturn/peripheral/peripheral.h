/*
while it just increases complexity for no reason at the moment,
	i'm doing the multiplexed input because if the smpc ever
gets decapped i think i'd have to rewrite it to be like that anyway
*/

struct PolledData
{
	bool d0;
	bool d1;
	bool d2;
	bool d3;
};

struct DeviceType
{
	virtual PolledData Poll(const bool &select1, const bool &select2) = 0;
};

struct StandardPadType : public DeviceType
{
	Saturn& saturn__;
	StandardPadType(Saturn &saturn___) : saturn__(saturn___) {}

	PolledData Poll(const bool &select1, const bool &select2)
	{
		PolledData d;

		SaturnStandardPadInputType input = saturn__.InterfaceUpdateInput();

		//4 cases
		if(!select1 && !select2)
		{
			d.d0 = input.z;
			d.d1 = input.y;
			d.d2 = input.x;
			d.d3 = input.right;
		}
		else if(!select1 && select2)
		{
			d.d0 = input.b;
			d.d1 = input.c;
			d.d2 = input.a;
			d.d3 = input.start;
		}
		else if(select1 && !select2)
		{
			d.d0 = input.up;
			d.d1 = input.down;
			d.d2 = input.l;
			d.d3 = input.r;
		}
		else
		{
			//d.d0 = ???;
			//d.d1 = ???;
			//d.d2 = ???;
			d.d3 = input.l;
		}

		return d;
	}
};
#if 0
struct MultiTapType : public DeviceType
{
	MultiTapType(Saturn& saturn___) : which_pad(0) ,
pads(saturn___)
{}

	//it's probably multiplexed, not sure how
	int which_pad;
	static const int number_of_pads = 6;
	StandardPadType pads[number_of_pads];

	PolledData Poll(const bool &select1, const bool &select2)
	{
		return pads[which_pad].Poll(select1,select2);
	}
};
#endif
struct Peripherals
{
	Saturn& saturn__;

	Peripherals(Saturn &saturn___) : saturn__(saturn___) 
	{
		//this is the only place in the emu where we use new i guess
		//not sure if there's a better way to do this or not
		port[0] = new StandardPadType(saturn___);
	}

	DeviceType* port[2];

}peripherals;


#if 0
struct Port
{
//	Devices connected_device;
	
	PolledData ReadPort(const bool &select1, const bool &select2)
	{
		return connected_device->Poll(select1,select2);
#if 0
		switch(connected_device)
		{
		case StandardPad: return standard_pad.Poll(select1,select2);
		case MultiTap:
			break;
		case Mouse:
			break;
		case AnalogPad:
			break;
		}
#endif
	}
};
#endif


