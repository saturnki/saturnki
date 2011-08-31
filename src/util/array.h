//bounds checked array classes
namespace array
{
	template <typename T, int size>
	struct Array
	{
		T& operator[] (int offset) {
			assert(offset >= 0);
			assert(offset < size);
			return data_[offset];
		}
		T data_[size];
	};

	template <int array_size>
	struct RamArray
	{
		unsigned char data_[array_size];

		RamArray()
		{
			//is this necessary?
			for(int i = 0; i < array_size; i++)
				data_[i] = 0;
		}

		template <OperationSize size>
		void Write(int address, int value)
		{
			assert(address >= 0);

			if(size == kByte)
			{
				assert(address < array_size);
				data_[address] = value;
			}
			else if(size == kWord)
			{
				assert(address < (array_size - 1));
				data_[address] = value >> 8;
				data_[address + 1] = value & 0xFF;
			}
			else
			{
				assert(address < (array_size - 3));
				data_[address] = (value >> 24) & 0xFF;
				data_[address + 1] = (value >> 16) & 0xFF;
				data_[address + 2] = (value >> 8) & 0xFF;
				data_[address + 3] = value & 0xFF;
			}
		}

		template <OperationSize size>
		unsigned int Read(int address)
		{
			assert(address >= 0);

			if(size == kByte)
			{
				assert(address < array_size);
				return data_[address];
			}
			else if(size == kWord)
			{
				assert(address < (array_size - 1));
				return (data_[address] << 8) | data_[address + 1];
			}
			else
			{
				assert(address < (array_size - 3));
				return (data_[address] << 24 
					| data_[address + 1] << 16 
					| data_[address + 2] << 8 
					| data_[address + 3]);
			}
		}
	};
}