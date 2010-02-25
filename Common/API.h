#pragma once

#include "Common/Compiler.h"

#ifdef NOCTURNAL_STATIC
#define COMMON_API
#else
# ifdef COMMON_EXPORTS
#  define COMMON_API __declspec(dllexport)
# else
#  define COMMON_API __declspec(dllimport)
# endif
#endif