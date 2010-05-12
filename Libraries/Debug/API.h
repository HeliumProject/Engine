#pragma once

#ifdef NOCTURNAL_STATIC
# define DEBUG_API
#else
# ifdef DEBUGDLL_EXPORTS
#  define DEBUG_API __declspec(dllexport)
# else
#  define DEBUG_API __declspec(dllimport)
# endif
#endif
