#pragma once

//
// Register types
//

#ifndef NULL
#define NULL (0)
#endif

#ifdef WIN32

typedef unsigned char           u8;
typedef signed char             i8;
typedef unsigned short          u16;
typedef signed short            i16;
typedef unsigned int            u32;
typedef signed int              i32;
typedef unsigned __int64        u64;
typedef signed __int64          i64;
typedef float                   f32;
typedef double                  f64;

typedef unsigned int            p32;
typedef unsigned __int64        p64;

#ifdef _WIN64
typedef unsigned __int64        uintptr;
typedef __int64                 intptr;
#else
typedef unsigned int            uintptr;
typedef int                     intptr;
#endif

#elif defined (__GCC__) || defined (__SNC__)

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

typedef unsigned int          uintptr;
typedef int                   intptr;

#endif

//
// String types
//

#include <string>

#ifdef _UNICODE
typedef wchar_t                 chr;
typedef std::wstring            str;
#define STR(s) L#s
#else
typedef char                    chr;
typedef std::string             str;
#define STR(s) s
#endif
