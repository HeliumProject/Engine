//----------------------------------------------------------------------------------------------------------------------
// PlatformCcGcc.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_PLATFORM_CC_GCC_H
#define LUNAR_CORE_PLATFORM_CC_GCC_H

/// DLL export API declaration.
#define L_API_EXPORT
/// DLL import API declaration.
#define L_API_IMPORT

/// Attribute for forcing the compiler to inline a function.
#if __GNUC__ < 4
#define L_FORCEINLINE inline  // GCC 3 support for "always_inline" is somewhat bugged, so fall back to just "inline".
#else
#define L_FORCEINLINE __attribute__( ( always_inline ) )
#endif

/// Attribute for explicitly defining a pointer or reference as not being externally aliased.
#define L_RESTRICT __restrict

/// Prefix macro for declaring type or variable alignment.
///
/// @param[in] ALIGNMENT  Byte alignment (must be a power of two).
#define L_ALIGN_PRE( ALIGNMENT )

/// Suffix macro for declaring type or variable alignment.
///
/// @param[in] ALIGNMENT  Byte alignment (must be a power of two).
#define L_ALIGN_POST( ALIGNMENT ) __attribute__( ( aligned( ALIGNMENT ) ) )

#endif  // LUNAR_CORE_PLATFORM_CC_GCC_H
