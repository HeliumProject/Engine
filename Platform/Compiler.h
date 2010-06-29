#pragma once

#include "Assert.h"

//
// Keywords
//

#ifdef _MSC_VER
# define NOC_OVERRIDE override
# define NOC_ABSTRACT abstract
#else
# define NOC_OVERRIDE
# define NOC_ABSTRACT
#endif

//
// Warnings
//

#ifdef _MSC_VER
# pragma warning(disable:4251) // disable DLL template code generation check, we use identical runtime libraries as checked below
# pragma warning(disable:4793) // disable native code generation warning (for .NET code)
# pragma warning(disable:4996) // disable deprication in general (secure CRT, etc...)
#endif

//
// Runtime Libraries
//

#ifdef _MSC_VER
# ifndef _MT
#  pragma message ("Danger: MultiThreaded CRT must be used to ensure proper DLL-exported template compatibility!")
# endif
# ifndef _DLL
#  pragma message ("Danger: DLL CRT must be used to ensure proper DLL-exported template compatibility!")
# endif
#endif

//
// Architecture
//

#ifdef _MSC_VER
# ifdef _M_X86
#  define X86
# endif
# ifdef _M_X64
#  define X64
# endif
#endif

//
// Byte Order
//

#ifdef _MSC_VER
# ifdef _M_IX86
#  define LITTLE_ENDIAN
# endif
# ifdef _M_X64
#  define LITTLE_ENDIAN
# endif
#endif