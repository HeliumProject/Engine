#pragma once

#include "Types.h"

//
// Setup state
//

// this is for cross-platform code that uses gcc's unused attribute to remove locals
#define HELIUM_ASSERT_ONLY

// this sets the master flag if we are going to compile in asserts or not
#if defined(_DEBUG) && !defined(HELIUM_ASSERT_ENABLED)
#define HELIUM_ASSERT_ENABLED
#endif


//
// Pull in prereqs
//

#ifdef HELIUM_ASSERT_ENABLED
# ifdef _MANAGED
#  using <System.dll>
# else // _MANAGED
#  include <stdio.h>
# endif // _MANAGED
#endif // HELIUM_ASSERT_ENABLED


//
// Define the main macros
//

#define HELIUM_DISABLEABLE_CODE_BLOCK(x) { static bool code_block_enabled = true; if (code_block_enabled) {x} }

#ifdef __GNUC__
# define HELIUM_ISSUE_BREAK() asm("int $3")
#elif defined( WIN32 )
# ifdef _MANAGED
#  define HELIUM_ISSUE_BREAK() System::Diagnostics::Debugger::Break()
# else //_MANAGED
#  define HELIUM_ISSUE_BREAK() __debugbreak()
# endif //_MANAGED
#elif defined (__SNC__)
# define HELIUM_ISSUE_BREAK() __builtin_snpause()
#else
# define HELIUM_ISSUE_BREAK() asm("int $3")
#endif

#define HELIUM_BREAK() HELIUM_DISABLEABLE_CODE_BLOCK( HELIUM_ISSUE_BREAK(); )

#ifdef HELIUM_ASSERT_ENABLED
# ifdef _MANAGED
#  ifdef __cplusplus_cli
#   define HELIUM_ASSERT(x) if (!(x)) HELIUM_DISABLEABLE_CODE_BLOCK( System::Log::Write(System::String::Format("ASSERT FAILED [expr='{0}']\n", gcnew System::String (#x))); HELIUM_ISSUE_BREAK(); )
#   define HELIUM_ASSERT_MSG(x, msg) if (!(x)) HELIUM_DISABLEABLE_CODE_BLOCK( System::Log::Write(System::String::Format("ASSERT FAILED [expr='{0}']\n", gcnew System::String (#x))); HELIUM_ISSUE_BREAK(); )
#  else //__cplusplus_cli
#   define HELIUM_ASSERT(x) if (!(x)) HELIUM_DISABLEABLE_CODE_BLOCK( System::Log::Write(System::String::Format("ASSERT FAILED [expr='{0}']\n", new System::String (#x))); HELIUM_ISSUE_BREAK(); )
#   define HELIUM_ASSERT_MSG(x, msg) if (!(x)) HELIUM_DISABLEABLE_CODE_BLOCK( System::Log::Write(System::String::Format("ASSERT FAILED [expr='{0}']\n", new System::String (#x))); HELIUM_ISSUE_BREAK(); )
#  endif //__cplusplus_cli
# else //_MANAGED
#  define HELIUM_ASSERT(x) if (!(x)) HELIUM_DISABLEABLE_CODE_BLOCK( ::printf("ASSERT FAILED [expr='%s', %s:%d]\n", #x, __FILE__, __LINE__); HELIUM_ISSUE_BREAK(); )
#  define HELIUM_ASSERT_MSG(x, msg) if (!(x)) HELIUM_DISABLEABLE_CODE_BLOCK( ::printf("ASSERT FAILED [expr='%s', %s:%d]\n", #x, __FILE__, __LINE__); ::printf("MESSAGE\n["); ::printf msg ; ::printf( "]\n" ); HELIUM_ISSUE_BREAK(); )
# endif
#else //HELIUM_ASSERT_ENABLED
# define HELIUM_ASSERT(x)
# define HELIUM_ASSERT_MSG(x, msg)
#endif

#ifdef HELIUM_ASSERT_ENABLED
# define HELIUM_VERIFY(x) HELIUM_ASSERT(x)
#else
# define HELIUM_VERIFY(x) x
#endif

//
// Compile time
//

#define HELIUM_COMPILE_ASSERT(exp) typedef tchar_t __HELIUM_COMPILE_ASSERT__[(exp)?1:-1]


//
// #pragma TODO("Do something!")
//

#ifdef _MSC_VER
#define TODO_STRING2(x) #x
#define TODO_STRING(x) TODO_STRING2(x)
#define TODO(msg) message (__FILE__ "(" TODO_STRING(__LINE__) ") : TODO: " msg)
#define NYI(msg) TODO(__FUNCTION__ " is not yet implemented... " msg)
#endif
