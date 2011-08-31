
/*
the smpc has an internal rom
and can't be emulated properly
until it is decapped and read out

lle is impossible 
so we are using yabause's hle instead

what this file ought to be doing:
emulating the 4 bit cpu on the smpc
and having that cpu execute the internal
firmware
*/

struct SmpcHle
{
	struct Cs2Hle;

	SmpcHle__ smpc_hle_impl;

	void Init(int region_id)
	{
		smpc_hle_impl.SmpcInit(region_id);

		printf("fix portdata");
		smpc_hle_impl.PORTDATA1.data[0]=0xf1;
		smpc_hle_impl.PORTDATA1.data[1]=2;
		smpc_hle_impl.PORTDATA1.data[2]=0xff;
		smpc_hle_impl.PORTDATA1.data[3]=0xff;
		smpc_hle_impl.PORTDATA1.size=4;

		smpc_hle_impl.PORTDATA2.data[0]=0xf0;
		smpc_hle_impl.PORTDATA2.data[1]=0;
		smpc_hle_impl.PORTDATA2.data[2]=0;
		smpc_hle_impl.PORTDATA2.data[3]=0;
		smpc_hle_impl.PORTDATA2.size=1;
	}

	void Execute(int microseconds,Saturn &saturn)
	{
		smpc_hle_impl.SmpcExec(microseconds,saturn);
	}

	void StopIntback()
	{
		smpc_hle_impl.SmpcINTBACKEnd();
	}

	template <OperationSize size>
	int Read(int address)
	{
		if(size == kByte)
			return smpc_hle_impl.SmpcReadByte(address);

		return 0;
	}

	template <OperationSize size>
	void Write(int address, int data)
	{
		if(size == kByte)
			smpc_hle_impl.SmpcWriteByte(address,data);
	}
};