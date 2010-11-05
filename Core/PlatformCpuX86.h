//----------------------------------------------------------------------------------------------------------------------
// PlatformCpuX86.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_PLATFORM_CPU_X86_H
#define LUNAR_CORE_PLATFORM_CPU_X86_H

#include <xmmintrin.h>

/// Defined as 1 for little-endian platforms.
#define L_ENDIAN_LITTLE 1

#ifdef L_ENDIAN_BIG
#undef L_ENDIAN_BIG
#endif

#if L_CPU_X86_64
/// Number of bits in a pointer/size_t/ptrdiff_t/etc.
#define L_WORDSIZE 64
#else
/// Number of bits in a pointer/size_t/ptrdiff_t/etc.
#define L_WORDSIZE 32
#endif

/// Cache line prefetch size, in bytes (general case).
#define L_PREFETCH_CACHE_SIZE 32

namespace Lunar
{
    /// Prefetch a cache line of data from the specified address.
    ///
    /// @param[in] pData  Address to prefetch.
    inline void Prefetch( void* pData )
    {
        _mm_prefetch( static_cast< char* >( pData ), _MM_HINT_T0 );
    }
}

#endif  // LUNAR_CORE_PLATFORM_CPU_X86_H
