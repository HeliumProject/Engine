#pragma once

// Operating system (HELIUM_OS_*) macros:
// - HELIUM_OS_PC: PC operating system (any)
//   - HELIUM_OS_WIN: Windows (any architecture/version)
//     - HELIUM_OS_WIN32: 32-bit Windows
//     - HELIUM_OS_WIN64: 64-bit Windows

// CPU architecture (HELIUM_CPU_*) macros:
// - HELIUM_CPU_X86: Intel x86-based (any architecture)
//   - HELIUM_CPU_X86_32: 32-bit Intel x86
//   - HELIUM_CPU_X86_64: 64-bit Intel x86

// Compiler (HELIUM_CC_*) macros:
// - HELIUM_CC_MSC: Microsoft Visual C++
// - HELIUM_CC_GCC: GCC
// - HELIUM_CC_SNC: SNC (PS3)

#if defined( _WIN64 )
# define HELIUM_OS_WIN 1
# define HELIUM_OS_WIN64 1
# define HELIUM_CPU_X86 1
# define HELIUM_CPU_X86_64 1
#elif defined( _WIN32 )
# define HELIUM_OS_WIN 1
# define HELIUM_OS_WIN32 1
# define HELIUM_CPU_X86 1
# define HELIUM_CPU_X86_32 1
#else
# error Unsupported platform.
#endif

#if HELIUM_OS_WIN
# define HELIUM_OS_PC 1
#endif

#if defined( _MSC_VER )
# define HELIUM_CC_MSC 1
#elif defined( __GNUC__ )
# define HELIUM_CC_GCC 1
#elif defined( __SNC__ )
# define HELIUM_CC_SNC 1
#else
# error Unsupported compiler.
#endif

#if HELIUM_CC_MSC
# include "Platform/Msc/Compiler.h"
#elif HELIUM_CC_GCC
# include "Platform/Gcc/Compiler.h"
#endif

#if HELIUM_CPU_X86
# include "Platform/X86/Cpu.h"
#endif

#if HELIUM_OS_WIN
# include "Platform/Windows/Platform.h"
#endif

#if HELIUM_CC_MSC
# pragma message ( "TODO: Remove this once Lunar/Helium integration is done -Geoff" )
#endif
namespace Lunar
{
    using namespace Helium;
}
