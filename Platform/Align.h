#pragma once

#include "Platform/Types.h"

//
// n is BYTES
//

#define HELIUM_ALIGN_128(n)    ((((uintptr)(n)) + 127) & ~127)
#define HELIUM_ALIGN_64(n)     ((((uintptr)(n)) + 63 ) & ~63 )
#define HELIUM_ALIGN_32(n)     ((((uintptr)(n)) + 31 ) & ~31 )
#define HELIUM_ALIGN_16(n)     ((((uintptr)(n)) + 15 ) & ~15 )
#define HELIUM_ALIGN_8(n)      ((((uintptr)(n)) + 7  ) & ~7  )
#define HELIUM_ALIGN_4(n)      ((((uintptr)(n)) + 3  ) & ~3  )
#define HELIUM_ALIGN_2(n)      ((((uintptr)(n)) + 1  ) & ~1  )
#define HELIUM_ALIGN_ARB(n, a) ((((uintptr)(n)) + ((a)-1)) & ~((a)-1))  // 'a' needs to be a power of 2

#ifdef _MSC_VER
# define HELIUM_PRE_STRUCT_ALIGN( bits ) __declspec( align( bits ) )
# define HELIUM_POST_STRUCT_ALIGN( bits )
#else
# define HELIUM_PRE_STRUCT_ALIGN( bits )
# define HELIUM_POST_STRUCT_ALIGN( bits ) __attribute__ (( aligned ( bits ) ))
#endif
