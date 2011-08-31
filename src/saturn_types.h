#pragma once
#include "util/varint.h"
#ifndef WIN32
#include <stdint.h>
#endif

//unsigned types
typedef types::uint_t<1> u1;
typedef types::uint_t<2> u2;
typedef types::uint_t<3> u3;
typedef types::uint_t<4> u4;
typedef types::uint_t<5> u5;
typedef types::uint_t<6> u6;
typedef types::uint_t<7> u7;
typedef unsigned char u8;
typedef types::uint_t<9> u9;
typedef types::uint_t<10> u10;
typedef types::uint_t<11> u11;
typedef types::uint_t<12> u12;
typedef types::uint_t<13> u13;
typedef types::uint_t<14> u14;
typedef types::uint_t<15> u15;
typedef unsigned short u16;
typedef types::uint_t<17> u17;
typedef types::uint_t<18> u18;
typedef types::uint_t<19> u19;
typedef types::uint_t<20> u20;
typedef types::uint_t<21> u21;
typedef types::uint_t<22> u22;
typedef types::uint_t<23> u23;
typedef types::uint_t<24> u24;
typedef types::uint_t<25> u25;
typedef types::uint_t<26> u26;
typedef types::uint_t<27> u27;
typedef types::uint_t<28> u28;
typedef types::uint_t<29> u29;
typedef types::uint_t<30> u30;
typedef types::uint_t<31> u31;
typedef unsigned long u32;
#ifdef WIN32
typedef unsigned __int64 u64;
#else
typedef uint64_t u64;
#endif
//signed types
typedef types::int_t<1> s1;
typedef types::int_t<2> s2;
typedef types::int_t<3> s3;
typedef types::int_t<4> s4;
typedef types::int_t<5> s5;
typedef types::int_t<6> s6;
typedef types::int_t<7> s7;
typedef signed char s8;
typedef types::int_t<9> s9;
typedef types::int_t<10> s10;
typedef types::int_t<11> s11;
typedef types::int_t<12> s12;
typedef types::int_t<13> s13;
typedef types::int_t<14> s14;
typedef types::int_t<15> s15;
typedef signed short s16;
typedef types::int_t<17> s17;
typedef types::int_t<18> s18;
typedef types::int_t<19> s19;
typedef types::int_t<20> s20;
typedef types::int_t<21> s21;
typedef types::int_t<22> s22;
typedef types::int_t<23> s23;
typedef types::int_t<24> s24;
typedef types::int_t<25> s25;
typedef types::int_t<26> s26;
typedef types::int_t<27> s27;
typedef types::int_t<28> s28;
typedef types::int_t<29> s29;
typedef types::int_t<30> s30;
typedef types::int_t<31> s31;
typedef signed long s32;
#ifdef WIN32
typedef signed __int64 s64;
#else
typedef int64_t s64;
#endif
