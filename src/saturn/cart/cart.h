//todo add some of the various kinds of carts you can plug into a saturn

struct CartridgeInterface
{
	template <int size>
	void Write(int address, int data)
	{
		//do nothing?
	}
	template <int size>
	unsigned int Read(int address)
	{
		return 0xFFFFFFFFF;//todo is this the actual return value?
	}
};