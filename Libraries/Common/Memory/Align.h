#pragma once

#include "../Types.h"

#define ALIGN_128(n)    ((((u32)(n)) + 127) & ~127)
#define ALIGN_64(n)     ((((u32)(n)) + 63 ) & ~63 )
#define ALIGN_32(n)     ((((u32)(n)) + 31 ) & ~31 )
#define ALIGN_16(n)     ((((u32)(n)) + 15 ) & ~15 )
#define ALIGN_8(n)      ((((u32)(n)) + 7  ) & ~7  )
#define ALIGN_4(n)      ((((u32)(n)) + 3  ) & ~3  )
#define ALIGN_2(n)      ((((u32)(n)) + 1  ) & ~1  )
#define ALIGN_ARB(n, a) ((((u32)(n)) + ((a)-1)) & ~((a)-1))  // 'a' needs to be a power of 2

