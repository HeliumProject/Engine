//----------------------------------------------------------------------------------------------------------------------
// Platform.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_PLATFORM_H
#define LUNAR_CORE_PLATFORM_H

// Operating system (L_OS_*) macros:
// - L_OS_PC: PC operating system (any)
//   - L_OS_WIN: Windows (any architecture/version)
//     - L_OS_WIN32: 32-bit Windows
//     - L_OS_WIN64: 64-bit Windows

// CPU architecture (L_CPU_*) macros:
// - L_CPU_X86: Intel x86-based (any architecture)
//   - L_CPU_X86_32: 32-bit Intel x86
//   - L_CPU_X86_64: 64-bit Intel x86

// Compiler (L_CC_*) macros:
// - L_CC_MSC: Microsoft Visual C++
// - L_CC_GCC: GCC

#if defined( _WIN64 )
#define L_OS_WIN 1
#define L_OS_WIN64 1
#define L_CPU_X86 1
#define L_CPU_X86_64 1
#elif defined( _WIN32 )
#define L_OS_WIN 1
#define L_OS_WIN32 1
#define L_CPU_X86 1
#define L_CPU_X86_32 1
#else
#error Unsupported platform.
#endif

#if L_OS_WIN
#define L_OS_PC 1
#endif

#if defined( _MSC_VER )
#define L_CC_MSC 1
#elif defined( __GNUC__ )
#define L_CC_GCC 1
#else
#error Unsupported compiler.
#endif

#if L_OS_WIN
#include "Core/PlatformOsWin.h"
#endif

#if L_CC_MSC
#include "Core/PlatformCcMsc.h"
#elif L_CC_GCC
#include "Core/PlatformCcGcc.h"
#endif

#if L_CPU_X86
#include "Core/PlatformCpuX86.h"
#endif

#endif  // LUNAR_CORE_PLATFORM_H
