#pragma once

#include "Platform/Compiler.h"

#ifdef UITOOLKIT_DLL
# ifdef UITOOLKIT_EXPORTS
#  define UITOOLKIT_API __declspec(dllexport)
# else
#  define UITOOLKIT_API __declspec(dllimport)
# endif
#else
# define UITOOLKIT_API
#endif
