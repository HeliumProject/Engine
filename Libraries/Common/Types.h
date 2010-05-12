#pragma once

#include "Config.h"

#ifdef WIN32

#include <string>
#include <vector>
#include <set>
#include <map>


//
// Fundamental types
//

typedef unsigned char         u8;
typedef signed char           i8;
typedef unsigned short        u16;
typedef signed short          i16;
typedef unsigned int          u32;
typedef signed int            i32;
typedef unsigned __int64      u64;
typedef signed __int64        i64;
typedef float                 f32;
typedef double                f64;

typedef unsigned int          p32;
typedef unsigned __int64      p64;

#ifdef _WIN64
  typedef unsigned __int64    PointerSizedUInt;
  typedef __int64             PointerSizedInt;
#else
  typedef __w64 unsigned int  PointerSizedUInt;
  typedef __w64 int           PointerSizedInt;
#endif

//
// Permute the intantiation of some basic containers
//

typedef std::vector<std::string>                V_string;
typedef std::set<std::string>                   S_string;
typedef std::map<std::string, std::string>      M_string;

#define STD_TYPES( __Type ) \
typedef std::vector<__Type>         V_##__Type; \
typedef std::set<__Type>            S_##__Type; \
typedef std::map<__Type, __Type>    M_##__Type;

STD_TYPES( u8 );
STD_TYPES( i8 );
STD_TYPES( u16 );
STD_TYPES( i16 );
STD_TYPES( u32 );
STD_TYPES( i32 );
STD_TYPES( u64 );
STD_TYPES( i64 );
STD_TYPES( f32 );
STD_TYPES( f64 );

#elif defined (__GCC__) || defined (__SNC__)

#ifndef NULL
#define NULL (0)
#endif

typedef unsigned char         u8;
typedef signed char           i8;
typedef unsigned short        u16;
typedef signed short          i16;
typedef unsigned int          u32;
typedef signed int            i32;
typedef unsigned long long    u64;
typedef long long             i64;
typedef float                 f32;
typedef double                f64;

typedef unsigned int          p32;
typedef unsigned long long    p64;

typedef unsigned int          PointerSizedUInt;
typedef int                   PointerSizedInt;

#endif
