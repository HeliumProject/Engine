#pragma once

#ifdef WIN32
# ifdef PLATFORM_DLL
#  ifdef PLATFORM_EXPORTS
#   define PLATFORM_API __declspec(dllexport)
#  else
#   define PLATFORM_API __declspec(dllimport)
#  endif
# else
#  define PLATFORM_API
# endif
#else
# define PLATFORM_API
#endif
