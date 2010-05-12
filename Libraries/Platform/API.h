#pragma once

#ifdef WIN32
# ifndef _WINDOWS_
#  ifdef PLATFORM_INCLUDE_SYSTEM
#   include "../Windows/Windows.h"
#  else
#   error Windows is required to consume this API, please include "Windows/Windows.h"
#  endif
# endif
# ifdef NOCTURNAL_STATIC
#  define PLATFORM_API
# else
#  ifdef PLATFORM_EXPORTS
#   define PLATFORM_API __declspec(dllexport)
#  else
#   define PLATFORM_API __declspec(dllimport)
#  endif
# endif
#else
# define PLATFORM_API
#endif
