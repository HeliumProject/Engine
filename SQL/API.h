#pragma once

#ifdef NOCTURNAL_STATIC
# define SQL_API
#else
# ifdef SQL_EXPORTS
#  define SQL_API __declspec(dllexport)
# else
#  define SQL_API __declspec(dllimport)
# endif
#endif