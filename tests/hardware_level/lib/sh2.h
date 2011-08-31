namespace sh2
{
	namespace regs
	{
#if 1
		void SetSr(u32 sr)
		{
			asm volatile ("ldc\t%0,sr"::"r"(sr));
		}

		u32 GetSr()
		{
			u32 sr;
			asm volatile ("stc\tsr,%0":"=r"(sr));
			return sr;
		}

		void SetVbr(u32 vbr)
		{
			asm volatile ("ldc\t%0,vbr"::"r"(vbr));
		}

		u32 GetVbr()
		{
			u32 vbr;
			asm volatile ("stc\tvbr,%0":"=r"(vbr));
			return vbr;
		}
#endif
	}
}