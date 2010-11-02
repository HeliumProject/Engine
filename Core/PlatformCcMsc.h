//----------------------------------------------------------------------------------------------------------------------
// PlatformCcMsc.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_PLATFORM_CC_MSC_H
#define LUNAR_CORE_PLATFORM_CC_MSC_H

// Template classes shouldn't be DLL exported, but the compiler warns us by default.
#pragma warning( disable : 4251 ) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
// Visual C++ does not support exception specifications at this time, but we still want to retain them for compilers
// that do support them.  This is harmless to ignore.
#pragma warning( disable : 4290 ) // C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
// This spuriously comes up on occasion with certain template class methods.
#pragma warning( disable : 4505 ) // 'function' : unreferenced local function has been removed

/// DLL export API declaration.
#define L_API_EXPORT __declspec( dllexport )
/// DLL import API declaration.
#define L_API_IMPORT __declspec( dllimport )

/// Attribute for forcing the compiler to inline a function.
#define L_FORCEINLINE __forceinline

/// Attribute for explicitly defining a pointer or reference as not being externally aliased.
#define L_RESTRICT __restrict

/// Prefix macro for declaring type or variable alignment.
///
/// @param[in] ALIGNMENT  Byte alignment (must be a power of two).
#define L_ALIGN_PRE( ALIGNMENT ) __declspec( align( ALIGNMENT ) )

/// Suffix macro for declaring type or variable alignment.
///
/// @param[in] ALIGNMENT  Byte alignment (must be a power of two).
#define L_ALIGN_POST( ALIGNMENT )

#endif  // LUNAR_CORE_PLATFORM_CC_MSC_H
