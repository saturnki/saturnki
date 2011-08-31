/*
the cd block has an sh1 with an internal rom
and lle emulation is impossible
until it is decapped and read
so we are using yabause's hle
for the moment

what this file will do someday:
emulate an sh1 running its embedded rom
*/

struct CdControllerHle
{
	Cs2Hle cs2_hle;
	SmpcHle& smpc_hle;

	CdControllerHle(SmpcHle&smpc_hle_,Saturn&saturn___) : smpc_hle(smpc_hle_),cs2_hle(saturn___) {}

	void LoadImageAndInitialize()
	{
		int cart_type = 0;
#define CDCORE_ISO 1

		cs2_hle.Cs2Init(
			cart_type,
			CDCORE_ISO,
			0,
			"",
			"",smpc_hle);

		cs2_hle.Cs2ChangeCDCore(CDCORE_ISO, NULL,smpc_hle);

	}

	void Execute(int microseconds)
	{
		cs2_hle.Cs2Exec(microseconds);
	}

	template <OperationSize size, typename T>
	T Read(T address)
	{
		if(size == kByte)
			return cs2_hle.Cs2ReadByte(address);
		if(size == kWord)
			return cs2_hle.Cs2ReadWord(address);
		if(size == kLong)
			return cs2_hle.Cs2ReadLong(address);
	}
	template <OperationSize size, typename T>
	void Write(T address, T data)
	{
		if(size == kByte)
			return cs2_hle.Cs2WriteByte(address,data);
		if(size == kWord)
			return cs2_hle.Cs2WriteWord(address,data);
		if(size == kLong)
			return cs2_hle.Cs2WriteLong(address,data);
	}
};