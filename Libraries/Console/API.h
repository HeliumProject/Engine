#pragma once

#ifdef NOCTURNAL_STATIC
# define CONSOLE_API
#else
# ifdef CONSOLE_EXPORTS
#  define CONSOLE_API __declspec(dllexport)
# else
#  define CONSOLE_API __declspec(dllimport)
# endif
#endif