#pragma once

#include <xmmintrin.h>

/// Defined as 1 for little-endian platforms.
#define HELIUM_ENDIAN_LITTLE 1

#ifdef HELIUM_ENDIAN_BIG
#undef HELIUM_ENDIAN_BIG
#endif

#if HELIUM_CPU_X86_64
/// Number of bits in a pointer/size_t/ptrdiff_t/etc.
#define HELIUM_WORDSIZE 64
#else
/// Number of bits in a pointer/size_t/ptrdiff_t/etc.
#define HELIUM_WORDSIZE 32
#endif

/// Cache line prefetch size, in bytes (general case).
#define HELIUM_PREFETCH_CACHE_SIZE 32

namespace Helium
{
    /// Prefetch a cache line of data from the specified address.
    ///
    /// @param[in] pData  Address to prefetch.
    inline void Prefetch( void* pData )
    {
        _mm_prefetch( static_cast< char* >( pData ), _MM_HINT_T0 );
    }
}
