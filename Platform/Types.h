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
#include <fstream>
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
#define TXT(s) L##s
#else
typedef char                    tchar;
#define TXT(s) s
#endif

typedef std::basic_string<tchar> tstring;

typedef std::basic_istream<tchar, std::char_traits<tchar> > tistream;
typedef std::basic_ostream<tchar, std::char_traits<tchar> > tostream;
typedef std::basic_iostream<tchar, std::char_traits<tchar> > tiostream;

typedef std::basic_ifstream<tchar, std::char_traits<tchar> > tifstream;
typedef std::basic_ofstream<tchar, std::char_traits<tchar> > tofstream;
typedef std::basic_fstream<tchar, std::char_traits<tchar> > tfstream;

typedef std::basic_istringstream<tchar, std::char_traits<tchar>, std::allocator<tchar> > tistringstream;
typedef std::basic_ostringstream<tchar, std::char_traits<tchar>, std::allocator<tchar> > tostringstream;
typedef std::basic_stringstream<tchar, std::char_traits<tchar>, std::allocator<tchar> > tstringstream;
