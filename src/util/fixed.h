namespace fixed
{
	//i don't want any non-explicit things happening so there are no automatic conversions
	template <typename T, int fractional_bits>
	struct FixedPoint
	{
		T data;
		inline operator FixedPoint() const { return *this; }
		inline FixedPoint operator =( const FixedPoint &i) { data = i.data; return *this;}
		inline FixedPoint operator +( const FixedPoint &i) { return FixedPoint(data + i.data);}
		inline FixedPoint operator -( const FixedPoint &i) { return FixedPoint(data - i.data);}
		inline FixedPoint operator *( const FixedPoint &i) { 
#if 1
			//well this messes up the generality
			//of the class....
			//beware if you want unsigned numbers

			//sign extend args
			s64 a = static_cast<s64>(i.data);
			s64 b = static_cast<s64>(data);
			s64 result = a * b;

			//why?
			result >>= fractional_bits;

			//mask out any remaining upper bits
			//is this necessary?
			T masked = static_cast<T>(result);
			return FixedPoint(masked);
#else
			return FixedPoint(data * i.data);
#endif
		}

		//	void ToFixed()
		//	{
		//		return i * (1 << fractional_bits);
		//	}
		void Multiply(const FixedPoint &i)
		{

		}

		FixedPoint() : data(0) {}

		FixedPoint(T fixed_point_data)//YOU MUST PROPERLY FORMAT YOUR DATA AS FIXED POINT
		{
			data = fixed_point_data;
		}

		FixedPoint IntToFixed(u32 i)
		{
			data = i * (1 << fractional_bits);
			//	data = input << fractional_bits;
			return *this;
		}

		u32 ToInt()
		{
			return data >> fractional_bits;
		}
	};
}