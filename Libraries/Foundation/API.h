#pragma once

#ifdef NOCTURNAL_STATIC
#define FOUNDATION_API
#else
# ifdef FOUNDATION_EXPORTS
#  define FOUNDATION_API __declspec(dllexport)
# else
#  define FOUNDATION_API __declspec(dllimport)
# endif
#endif