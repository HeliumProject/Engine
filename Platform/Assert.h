#pragma once

#include "Platform/API.h"
#include "Platform/Types.h"

//
// Setup state
//

// this is for cross-platform code that uses gcc's unused attribute to remove locals
#define HELIUM_ASSERT_ONLY

// this sets the master flag if we are going to compile in asserts or not
#if !defined( NDEBUG ) && !defined( HELIUM_ASSERT_ENABLED )
#define HELIUM_ASSERT_ENABLED 1
#endif


//
// Define the main macros
//

#define HELIUM_DISABLEABLE_CODE_BLOCK(x) { static bool code_block_enabled = true; if (code_block_enabled) {x} }

#if HELIUM_CC_CL
# ifdef _MANAGED
#  define HELIUM_ISSUE_BREAK() System::Diagnostics::Debugger::Break()
# else //_MANAGED
#  define HELIUM_ISSUE_BREAK() __debugbreak()
# endif //_MANAGED
# define HELIUM_FUNCTION_NAME __FUNCSIG__
#else
# if HELIUM_CC_GCC
#  if HELIUM_CPU_X86_32
#   define HELIUM_ISSUE_BREAK() __asm__( "int $3" )
#  else
#   define HELIUM_ISSUE_BREAK() __builtin_trap()
#  endif
# elif HELIUM_CC_SNC
#  define HELIUM_ISSUE_BREAK() __builtin_snpause()
# else
#  define HELIUM_ISSUE_BREAK() __asm__( "int $3" )
# endif
# define HELIUM_FUNCTION_NAME __PRETTY_FUNCTION__
#endif

#define HELIUM_BREAK() HELIUM_DISABLEABLE_CODE_BLOCK( HELIUM_ISSUE_BREAK(); )

namespace Helium
{
	HELIUM_PLATFORM_API void FatalExit( int exitCode );
}

#if HELIUM_ASSERT_ENABLED

namespace Helium
{
	namespace AssertResults
	{
		/// Assertion handler result.
		enum Type
		{
			Break,		///< Break execution.
			Abort,		///< Terminate the program.
			Continue,	///< Continue execution.
		};
	}
	typedef AssertResults::Type AssertResult;

	/// Assert utility functions.
	class HELIUM_PLATFORM_API Assert
	{
	public:
		/// @name Static Utility Functions
		//@{
		static AssertResult Trigger( const tchar_t* pExpression, const tchar_t* pFunction, const tchar_t* pFile, int line, const tchar_t* pMessage, ... );
		//@}

	private:
		/// Non-zero if the assert handler is currently active, zero if not.
		static volatile int32_t sm_active;

		/// @name Private Static Utility Functions
		//@{
		static AssertResult TriggerImplementation( const tchar_t* pMessageText );
		//@}
	};
}

/// Trigger and handle a Helium::AssertResult code.
///
/// @param[in] EXP      Expression string.
/// @param[in] MESSAGE  Message string.
#define HELIUM_TRIGGER_ASSERT_HANDLER( EXP, ... ) \
	{ \
		HELIUM_DISABLEABLE_CODE_BLOCK( \
			Helium::AssertResult _result = Helium::Assert::Trigger( \
				EXP, \
				TXT(HELIUM_FUNCTION_NAME), \
				TXT(__FILE__), \
				__LINE__, \
				__VA_ARGS__ ); \
			if( _result == Helium::AssertResults::Break ) \
			{ \
				HELIUM_ISSUE_BREAK(); \
			} \
			else if( _result == Helium::AssertResults::Abort ) \
			{ \
				Helium::FatalExit( -1 ); \
			} ) \
	}

/// Trigger a debug breakpoint if the result of an expression is false in non-release builds.
///
/// @param[in] EXP  Expression to evaluate.
///
/// @see HELIUM_ASSERT_MSG(), HELIUM_ASSERT_FALSE(), HELIUM_ASSERT_MSG_FALSE(), HELIUM_VERIFY(), HELIUM_VERIFY_MSG()
#define HELIUM_ASSERT( EXP ) { if( !( EXP ) ) HELIUM_TRIGGER_ASSERT_HANDLER( TXT( #EXP ), NULL ) }

/// Trigger a debug breakpoint with a customized message if the result of an expression is false in non-release builds.
///
/// @param[in] EXP      Expression to evaluate.
/// @param[in] ...      Message to display if the assertion is triggered.
///
/// @see HELIUM_ASSERT(), HELIUM_ASSERT_FALSE(), HELIUM_ASSERT_MSG_FALSE() HELIUM_VERIFY(), HELIUM_VERIFY_MSG()
#define HELIUM_ASSERT_MSG( EXP, ... ) { if( !( EXP ) ) HELIUM_TRIGGER_ASSERT_HANDLER( TXT( #EXP ), __VA_ARGS__ ) }

/// Trigger a debug breakpoint unconditionally in non-release builds.
///
/// @see HELIUM_ASSERT(), HELIUM_ASSERT_MSG(), HELIUM_ASSERT_MSG_FALSE(), HELIUM_VERIFY(), HELIUM_VERIFY_MSG()
#define HELIUM_ASSERT_FALSE() HELIUM_TRIGGER_ASSERT_HANDLER( NULL, NULL )

/// Trigger a debug breakpoint with a customized message unconditionally in non-release builds.
///
/// @param[in] ...  Message to display if the assertion is triggered.
///
/// @see HELIUM_ASSERT(), HELIUM_ASSERT_MSG(), HELIUM_ASSERT_FALSE(), HELIUM_VERIFY(), HELIUM_VERIFY_MSG()
#define HELIUM_ASSERT_MSG_FALSE( ... ) HELIUM_TRIGGER_ASSERT_HANDLER( NULL, __VA_ARGS__ )

/// Trigger a debug breakpoint if the result of an expression is false in non-release builds while still evaluating the
/// expression in release builds.
///
/// @param[in] EXP  Expression to evaluate.
///
/// @see HELIUM_ASSERT(), HELIUM_ASSERT_MSG(), HELIUM_ASSERT_FALSE(), HELIUM_ASSERT_MSG_FALSE(), HELIUM_VERIFY_MSG()
#define HELIUM_VERIFY( EXP ) HELIUM_ASSERT( EXP )

/// Trigger a debug breakpoint with a customized message if the result of an expression is false in non-release builds
/// while still evaluating the expression in release builds.
///
/// @param[in] EXP      Expression to evaluate.
/// @param[in] ...      Message to display if the assertion is triggered.
///
/// @see HELIUM_ASSERT(), HELIUM_ASSERT_MSG(), HELIUM_ASSERT_FALSE(), HELIUM_ASSERT_MSG_FALSE(), HELIUM_VERIFY()
#define HELIUM_VERIFY_MSG( EXP, ... ) HELIUM_ASSERT_MSG( EXP, __VA_ARGS__ )

#else  // HELIUM_ASSERT_ENABLED

/// Trigger a debug breakpoint if the result of an expression is false in non-release builds.
///
/// @param[in] EXP  Expression to evaluate.
///
/// @see HELIUM_ASSERT_MSG(), HELIUM_ASSERT_FALSE(), HELIUM_ASSERT_MSG_FALSE(), HELIUM_VERIFY(), HELIUM_VERIFY_MSG()
#define HELIUM_ASSERT( EXP )

/// Trigger a debug breakpoint with a customized message if the result of an expression is false in non-release builds.
///
/// @param[in] EXP      Expression to evaluate.
/// @param[in] ...      Message to display if the assertion is triggered.
///
/// @see HELIUM_ASSERT(), HELIUM_ASSERT_FALSE(), HELIUM_ASSERT_MSG_FALSE() HELIUM_VERIFY(), HELIUM_VERIFY_MSG()
#define HELIUM_ASSERT_MSG( EXP, ... )

/// Trigger a debug breakpoint unconditionally in non-release builds.
///
/// @see HELIUM_ASSERT(), HELIUM_ASSERT_MSG(), HELIUM_ASSERT_MSG_FALSE(), HELIUM_VERIFY(), HELIUM_VERIFY_MSG()
#define HELIUM_ASSERT_FALSE()

/// Trigger a debug breakpoint with a customized message unconditionally in non-release builds.
///
/// @param[in] ...  Message to display if the assertion is triggered.
///
/// @see HELIUM_ASSERT(), HELIUM_ASSERT_MSG(), HELIUM_ASSERT_FALSE(), HELIUM_VERIFY(), HELIUM_VERIFY_MSG()
#define HELIUM_ASSERT_MSG_FALSE( ... )

/// Trigger a debug breakpoint if the result of an expression is false in non-release builds while still evaluating the
/// expression in release builds.
///
/// @param[in] EXP  Expression to evaluate.
///
/// @see HELIUM_ASSERT(), HELIUM_ASSERT_MSG(), HELIUM_ASSERT_FALSE(), HELIUM_ASSERT_MSG_FALSE(), HELIUM_VERIFY_MSG()
#define HELIUM_VERIFY( EXP ) EXP

/// Trigger a debug breakpoint with a customized message if the result of an expression is false in non-release builds
/// while still evaluating the expression in release builds.
///
/// @param[in] EXP      Expression to evaluate.
/// @param[in] ...  Message to display if the assertion is triggered.
///
/// @see HELIUM_ASSERT(), HELIUM_ASSERT_MSG(), HELIUM_ASSERT_FALSE(), HELIUM_ASSERT_MSG_FALSE(), HELIUM_VERIFY()
#define HELIUM_VERIFY_MSG( EXP, ... ) EXP

#endif

//
// Compile time
//

#if HELIUM_CC_CL
# if _MSC_VER >= 1600
#  define HELIUM_COMPILE_ASSERT(exp) static_assert(exp, #exp)
# endif
#endif

#ifndef HELIUM_COMPILE_ASSERT
# define HELIUM_COMPILE_ASSERT(exp) typedef char __HELIUM_COMPILE_ASSERT__[(exp)?1:-1]
#endif

//
// #pragma TODO("Do something!")
//

#if HELIUM_CC_CL
#define TODO_STRING2(x) #x
#define TODO_STRING(x) TODO_STRING2(x)
#define TODO(msg) message (__FILE__ "(" TODO_STRING(__LINE__) ") : TODO: " msg)
#define NYI(msg) TODO(__FUNCTION__ " is not yet implemented... " msg)
#endif
