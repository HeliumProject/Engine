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

typedef unsigned char           u8;
typedef signed char             i8;
typedef unsigned short          u16;
typedef signed short            i16;
typedef unsigned int            u32;
typedef signed int              i32;
typedef unsigned long long      u64;
typedef long long               i64;
typedef float                   f32;
typedef double                  f64;

typedef unsigned int            p32;
typedef unsigned long long      p64;

typedef unsigned int            uintptr;
typedef int                     intptr;

#endif

//
// String types
//

#include <string>
#include <sstream>
#include <strstream>

#ifdef WIN32
# include <tchar.h>
#endif

#ifdef _UNICODE
# ifndef UNICODE
#  define UNICODE
# endif
#endif

#ifdef UNICODE
# ifndef _UNICODE
#  define _UNICODE
# endif
#endif

#ifdef UNICODE
typedef wchar_t                 tchar;
typedef std::wstring            tstring;
typedef std::wstringstream      tstringstream;
typedef std::wistringstream     tistringstream;
typedef std::wostringstream     tostringstream;
typedef std::wostream           tostream;
typedef std::wistream           tistream;
typedef std::wiostream          tiostream;
typedef std::wifstream          tifstream;
typedef std::wofstream          tofstream;
typedef std::wfstream           tfstream;
#define TXT(s) L##s
#else
typedef char                    tchar;
typedef std::string             tstring;
typedef std::stringstream       tstringstream;
typedef std::istringstream      tistringstream;
typedef std::ostringstream      tostringstream;
typedef std::ostream            tostream;
typedef std::istream            tistream;
typedef std::iostream           tiostream;
typedef std::ifstream           tifstream;
typedef std::ofstream           tofstream;
typedef std::fstream            tfstream;
#define TXT(s) s
#endif
