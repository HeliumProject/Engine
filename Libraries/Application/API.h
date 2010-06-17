#pragma once

#ifdef APPLICATION_DLL
# ifdef APPLICATION_EXPORTS
#  define APPLICATION_API __declspec (dllexport)
# else
#  define APPLICATION_API __declspec (dllimport)
# endif
#else
# define APPLICATION_API
#endif