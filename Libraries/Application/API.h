#pragma once

#ifdef NOCTURNAL_STATIC
# define APPLICATION_API
#else
# ifdef APPLICATION_EXPORTS
#  define APPLICATION_API __declspec (dllexport)
# else
#  define APPLICATION_API __declspec (dllimport)
# endif
#endif