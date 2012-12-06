#pragma once

#include "Platform/System.h"

#include "Foundation/Profile.h"

#if HELIUM_SHARED
# ifdef HELIUM_MATH_SIMD_EXPORTS
#  define HELIUM_MATH_SIMD_API HELIUM_API_EXPORT
# else
#  define HELIUM_MATH_SIMD_API HELIUM_API_IMPORT
# endif
#else
#define HELIUM_MATH_SIMD_API
#endif
