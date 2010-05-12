#pragma once

#ifdef REQUIRES_WINDOWS_H
#ifndef _WINDOWS_
#error You need to #include <windows.h>
#endif
#endif

#ifdef NOCTURNAL_STATIC
# define WINDOWS_API
#else
# ifdef WINDOWSDLL_EXPORTS
#  define WINDOWS_API __declspec(dllexport)
# else
#  define WINDOWS_API __declspec(dllimport)
# endif
#endif
