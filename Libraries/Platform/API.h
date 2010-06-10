#pragma once

#ifdef WIN32
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
