#pragma once

#ifdef DEBUG_DLL
# ifdef DEBUGDLL_EXPORTS
#  define DEBUG_API __declspec(dllexport)
# else
#  define DEBUG_API __declspec(dllimport)
# endif
#else
# define DEBUG_API
#endif
