#pragma once

#include "Common/Compiler.h"

#ifdef FILEUI_DLL
# ifdef FILEUI_EXPORTS
#  define FILEUI_API __declspec(dllexport)
# else
#  define FILEUI_API __declspec(dllimport)
# endif
#else
# define FILEUI_API
#endif

// profiling for header systems
//#define FILEUI_PROFILE

#if defined(PROFILEUI_INSTRUMENT_ALL) || defined (FILEUI_PROFILE)
# define FILEUI_SCOPE_TIMER(__Str) PROFILEUI_SCOPE_TIMER(__Str)
#else
# define FILEUI_SCOPE_TIMER(__Str)
#endif