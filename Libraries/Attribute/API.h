#pragma once

#include "Platform/Compiler.h"

#ifdef ATTRIBUTE_DLL
# ifdef ATTRIBUTE_EXPORTS
#  define ATTRIBUTE_API __declspec(dllexport)
# else
#  define ATTRIBUTE_API __declspec(dllimport)
# endif
#else
# define ATTRIBUTE_API
#endif
