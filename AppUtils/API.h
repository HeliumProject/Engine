#pragma once

#ifdef NOCTURNAL_STATIC
# define APPUTILS_API
#else
# ifdef APPUTILS_EXPORTS
#  define APPUTILS_API __declspec (dllexport)
# else
#  define APPUTILS_API __declspec (dllimport)
# endif
#endif