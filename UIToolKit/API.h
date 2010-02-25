#pragma once

#include "Common/Compiler.h"

#ifdef NOCTURNAL_STATIC
# define UITOOLKIT_API
#else
# ifdef UITOOLKITDLL_EXPORTS
#  define UITOOLKIT_API __declspec(dllexport)
# else
#  define UITOOLKIT_API __declspec(dllimport)
# endif
#endif
