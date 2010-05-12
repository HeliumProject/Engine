#pragma once


//
// Setup state
//

// this is for cross-platform code that uses gcc's unused attribute to remove locals
#define NOC_ASSERT_ONLY

// this sets the master flag if we are going to compile in asserts or not
#if defined(_DEBUG) && !defined(NOC_ASSERT_ENABLED)
#define NOC_ASSERT_ENABLED
#endif


//
// Pull in prereqs
//

#ifdef NOC_ASSERT_ENABLED
# ifdef _MANAGED
#  using <System.dll>
# else // _MANAGED
#  include <stdio.h>
# endif // _MANAGED
#endif // NOC_ASSERT_ENABLED


//
// Define the main macros
//

#define NOC_DISABLEABLE_CODE_BLOCK(x) { static bool code_block_enabled = true; if (code_block_enabled) {x} }

#ifdef WIN32
# ifdef _MANAGED
#  define NOC_ISSUE_BREAK() System::Diagnostics::Debugger::Break()
# else //_MANAGED
#  define NOC_ISSUE_BREAK() __debugbreak()
# endif //_MANAGED
#elif defined (__GCC__)
# define NOC_ISSUE_BREAK() asm volatile ("tw 31,1,1")
#elif defined (__SNC__)
# define NOC_ISSUE_BREAK() __builtin_snpause()
#else
# define NOC_ISSUE_BREAK() asm("int $3")
#endif

#define NOC_BREAK() NOC_DISABLEABLE_CODE_BLOCK( NOC_ISSUE_BREAK(); )

#ifdef NOC_ASSERT_ENABLED
# ifdef _MANAGED
#  ifdef __cplusplus_cli
#   define NOC_ASSERT(x) if (!(x)) NOC_DISABLEABLE_CODE_BLOCK( System::Console::Write(System::String::Format("ASSERT FAILED [expr='{0}']\n", gcnew System::String (#x))); NOC_ISSUE_BREAK(); )
#   define NOC_ASSERT_MSG(x, msg) if (!(x)) NOC_DISABLEABLE_CODE_BLOCK( System::Console::Write(System::String::Format("ASSERT FAILED [expr='{0}']\n", gcnew System::String (#x))); NOC_ISSUE_BREAK(); )
#  else //__cplusplus_cli
#   define NOC_ASSERT(x) if (!(x)) NOC_DISABLEABLE_CODE_BLOCK( System::Console::Write(System::String::Format("ASSERT FAILED [expr='{0}']\n", new System::String (#x))); NOC_ISSUE_BREAK(); )
#   define NOC_ASSERT_MSG(x, msg) if (!(x)) NOC_DISABLEABLE_CODE_BLOCK( System::Console::Write(System::String::Format("ASSERT FAILED [expr='{0}']\n", new System::String (#x))); NOC_ISSUE_BREAK(); )
#  endif //__cplusplus_cli
# else //_MANAGED
#  define NOC_ASSERT(x) if (!(x)) NOC_DISABLEABLE_CODE_BLOCK( ::printf("ASSERT FAILED [expr='%s', %s:%d]\n", #x, __FILE__, __LINE__); NOC_ISSUE_BREAK(); )
#  define NOC_ASSERT_MSG(x, msg) if (!(x)) NOC_DISABLEABLE_CODE_BLOCK( ::printf("ASSERT FAILED [expr='%s', %s:%d]\n", #x, __FILE__, __LINE__); ::printf("MESSAGE\n["); ::printf msg ; ::printf( "]\n" ); NOC_ISSUE_BREAK(); )
# endif 
#else //NOC_ASSERT_ENABLED
# define NOC_ASSERT(x)
# define NOC_ASSERT_MSG(x, msg)
#endif


//
// Compile time
//

#define NOC_COMPILE_ASSERT(exp) typedef char __NOC_COMPILE_ASSERT__[(exp)?1:-1] 


//
// #pragma TODO("Do something!")
//

#ifdef _MSC_VER
#define TODO_STRING2(x) #x
#define TODO_STRING(x) TODO_STRING2(x)
#define TODO(msg) message (__FILE__ "(" TODO_STRING(__LINE__) ") : TODO: " msg)
#endif
