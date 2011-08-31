//borrowed from bsnes http://byuu.org/

#ifndef NALL_VARINT_HPP
#define NALL_VARINT_HPP

//#include <type_traits>
#include "bit.h"
//#include <nall/static.hpp>

namespace types {
  template<unsigned bits> class uint_t {
  private:
    enum { bytes = (bits + 7) >> 3 };  //minimum number of bytes needed to store value
#if 0
    typedef typename static_if<
      sizeof(int) >= bytes,
      unsigned int,
      typename static_if<
        sizeof(long) >= bytes,
        unsigned long,
        typename static_if<
          sizeof(long long) >= bytes,
          unsigned long long,
          void
        >::type
      >::type
    >::type T;

    static_assert(!std::is_same<T, void>::value, "");
	#endif
	typedef unsigned long T;
public:
   T data;

  
    inline operator T() const { return data; }
    inline T operator ++(int) { T r = data; data = uclip<bits>(data + 1); return r; }
    inline T operator --(int) { T r = data; data = uclip<bits>(data - 1); return r; }
    inline T operator ++() { return data = uclip<bits>(data + 1); }
    inline T operator --() { return data = uclip<bits>(data - 1); }
    inline T operator  =(const T i) { return data = uclip<bits>(i); }
    inline T operator |=(const T i) { return data = uclip<bits>(data  | i); }
    inline T operator ^=(const T i) { return data = uclip<bits>(data  ^ i); }
    inline T operator &=(const T i) { return data = uclip<bits>(data  & i); }
    inline T operator<<=(const T i) { return data = uclip<bits>(data << i); }
    inline T operator>>=(const T i) { return data = uclip<bits>(data >> i); }
    inline T operator +=(const T i) { return data = uclip<bits>(data  + i); }
    inline T operator -=(const T i) { return data = uclip<bits>(data  - i); }
    inline T operator *=(const T i) { return data = uclip<bits>(data  * i); }
    inline T operator /=(const T i) { return data = uclip<bits>(data  / i); }
    inline T operator %=(const T i) { return data = uclip<bits>(data  % i); }

    inline uint_t() : data(0) {}
    inline uint_t(const T i) : data(uclip<bits>(i)) {}
  };

  template<unsigned bits> class int_t {
  private:
    enum { bytes = (bits + 7) >> 3 };  //minimum number of bytes needed to store value
#if 0
	typedef typename static_if<
      sizeof(int) >= bytes,
      signed int,
      typename static_if<
        sizeof(long) >= bytes,
        signed long,
        typename static_if<
          sizeof(long long) >= bytes,
          signed long long,
          void
        >::type
      >::type
    >::type T;
    static_assert(!std::is_same<T, void>::value, "");
#endif
	typedef signed long T;
    T data;

  public:
    inline operator T() const { return data; }
    inline T operator ++(int) { T r = data; data = sclip<bits>(data + 1); return r; }
    inline T operator --(int) { T r = data; data = sclip<bits>(data - 1); return r; }
    inline T operator ++() { return data = sclip<bits>(data + 1); }
    inline T operator --() { return data = sclip<bits>(data - 1); }
    inline T operator  =(const T i) { return data = sclip<bits>(i); }
    inline T operator |=(const T i) { return data = sclip<bits>(data  | i); }
    inline T operator ^=(const T i) { return data = sclip<bits>(data  ^ i); }
    inline T operator &=(const T i) { return data = sclip<bits>(data  & i); }
    inline T operator<<=(const T i) { return data = sclip<bits>(data << i); }
    inline T operator>>=(const T i) { return data = sclip<bits>(data >> i); }
    inline T operator +=(const T i) { return data = sclip<bits>(data  + i); }
    inline T operator -=(const T i) { return data = sclip<bits>(data  - i); }
    inline T operator *=(const T i) { return data = sclip<bits>(data  * i); }
    inline T operator /=(const T i) { return data = sclip<bits>(data  / i); }
    inline T operator %=(const T i) { return data = sclip<bits>(data  % i); }

    inline int_t() : data(0) {}
    inline int_t(const T i) : data(sclip<bits>(i)) {}
  };
}

#endif
