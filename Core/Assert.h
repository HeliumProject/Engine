//----------------------------------------------------------------------------------------------------------------------
// Assert.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_ASSERT_H
#define LUNAR_CORE_ASSERT_H

#include "Core/Core.h"

#include "Platform/Types.h"

/// @defgroup assert Assertion Support
//@{

#if HELIUM_CC_MSC

/// Trigger a debug breakpoint.
#define L_BREAKPOINT __debugbreak()

/// Abort program execution immediately.
///
/// @param[in] EXIT_CODE  Error code associated with the program termination.
#define L_FATAL_EXIT( EXIT_CODE ) FatalExit( EXIT_CODE )

/// Preprocessor token alias for the current function name string.
#define L_FUNCTION_NAME __FUNCSIG__

#elif HELIUM_CC_GCC

/// Trigger a debug breakpoint.
#define L_BREAKPOINT __builtin_trap()

/// Abort program execution immediately.
///
/// @param[in] EXIT_CODE  Error code associated with the program termination.
#define L_FATAL_EXIT( EXIT_CODE ) abort()

/// Preprocessor token alias for the current function name string.
#define L_FUNCTION_NAME __PRETTY_FUNCTION__

#endif

#ifdef NDEBUG

/// Trigger a debug breakpoint if the result of an expression is false in non-release builds.
///
/// @param[in] EXP  Expression to evaluate.
///
/// @see L_ASSERT_MESSAGE(), L_ASSERT_FALSE(), L_ASSERT_MESSAGE_FALSE(), L_VERIFY(), L_VERIFY_MESSAGE()
#define L_ASSERT( EXP )

/// Trigger a debug breakpoint with a customized message if the result of an expression is false in non-release builds.
///
/// @param[in] EXP      Expression to evaluate.
/// @param[in] MESSAGE  Message to display if the assertion is triggered.
///
/// @see L_ASSERT(), L_ASSERT_FALSE(), L_ASSERT_MESSAGE_FALSE() L_VERIFY(), L_VERIFY_MESSAGE()
#define L_ASSERT_MESSAGE( EXP, MESSAGE )

/// Trigger a debug breakpoint unconditionally in non-release builds.
///
/// @see L_ASSERT(), L_ASSERT_MESSAGE(), L_ASSERT_MESSAGE_FALSE(), L_VERIFY(), L_VERIFY_MESSAGE()
#define L_ASSERT_FALSE()

/// Trigger a debug breakpoint with a customized message unconditionally in non-release builds.
///
/// @param[in] MESSAGE  Message to display if the assertion is triggered.
///
/// @see L_ASSERT(), L_ASSERT_MESSAGE(), L_ASSERT_FALSE(), L_VERIFY(), L_VERIFY_MESSAGE()
#define L_ASSERT_MESSAGE_FALSE( MESSAGE )

/// Trigger a debug breakpoint if the result of an expression is false in non-release builds while still evaluating the
/// expression in release builds.
///
/// @param[in] EXP  Expression to evaluate.
///
/// @see L_ASSERT(), L_ASSERT_MESSAGE(), L_ASSERT_FALSE(), L_ASSERT_MESSAGE_FALSE(), L_VERIFY_MESSAGE()
#define L_VERIFY( EXP ) EXP

/// Trigger a debug breakpoint with a customized message if the result of an expression is false in non-release builds
/// while still evaluating the expression in release builds.
///
/// @param[in] EXP      Expression to evaluate.
/// @param[in] MESSAGE  Message to display if the assertion is triggered.
///
/// @see L_ASSERT(), L_ASSERT_MESSAGE(), L_ASSERT_FALSE(), L_ASSERT_MESSAGE_FALSE(), L_VERIFY()
#define L_VERIFY_MESSAGE( EXP, MESSAGE ) EXP

#else

namespace Lunar
{
    /// Assert utility functions.
    class LUNAR_CORE_API Assert
    {
    public:
        /// Assertion handler result.
        enum EResult
        {
            RESULT_FIRST   =  0,
            RESULT_INVALID = -1,

            RESULT_BREAK,     ///< Break execution.
            RESULT_ABORT,     ///< Terminate the program.
            RESULT_CONTINUE,  ///< Continue execution.

            RESULT_MAX,
            RESULT_LAST = RESULT_MAX - 1
        };

        /// @name Static Utility Functions
        //@{
        static EResult Trigger(
            const tchar_t* pExpression, const tchar_t* pMessage, const char* pFunction, const char* pFile, int line );
        //@}

    private:
        /// Non-zero if the assert handler is currently active, zero if not.
        static volatile int32_t sm_active;
    };
}

/// Trigger and handle a Lunar::Assert::EResult code.
///
/// @param[in] EXP      Expression string.
/// @param[in] MESSAGE  Message string.
#define L_TRIGGER_ASSERT_HANDLER( EXP, MESSAGE ) \
    { \
        Lunar::Assert::EResult result = Lunar::Assert::Trigger( EXP, MESSAGE, L_FUNCTION_NAME, __FILE__, __LINE__ ); \
        if( result == Lunar::Assert::RESULT_BREAK ) \
        { \
            L_BREAKPOINT; \
        } \
        else if( result == Lunar::Assert::RESULT_ABORT ) \
        { \
            L_FATAL_EXIT( -1 ); \
        } \
    }

/// Trigger a debug breakpoint if the result of an expression is false in non-release builds.
///
/// @param[in] EXP  Expression to evaluate.
///
/// @see L_ASSERT_MESSAGE(), L_ASSERT_FALSE(), L_ASSERT_MESSAGE_FALSE(), L_VERIFY(), L_VERIFY_MESSAGE()
#define L_ASSERT( EXP ) { if( !( EXP ) ) L_TRIGGER_ASSERT_HANDLER( TXT( #EXP ), NULL ) }

/// Trigger a debug breakpoint with a customized message if the result of an expression is false in non-release builds.
///
/// @param[in] EXP      Expression to evaluate.
/// @param[in] MESSAGE  Message to display if the assertion is triggered.
///
/// @see L_ASSERT(), L_ASSERT_FALSE(), L_ASSERT_MESSAGE_FALSE() L_VERIFY(), L_VERIFY_MESSAGE()
#define L_ASSERT_MESSAGE( EXP, MESSAGE ) { if( !( EXP ) ) L_TRIGGER_ASSERT_HANDLER( TXT( #EXP ), MESSAGE ) }

/// Trigger a debug breakpoint unconditionally in non-release builds.
///
/// @see L_ASSERT(), L_ASSERT_MESSAGE(), L_ASSERT_MESSAGE_FALSE(), L_VERIFY(), L_VERIFY_MESSAGE()
#define L_ASSERT_FALSE() L_TRIGGER_ASSERT_HANDLER( NULL, NULL )

/// Trigger a debug breakpoint with a customized message unconditionally in non-release builds.
///
/// @param[in] MESSAGE  Message to display if the assertion is triggered.
///
/// @see L_ASSERT(), L_ASSERT_MESSAGE(), L_ASSERT_FALSE(), L_VERIFY(), L_VERIFY_MESSAGE()
#define L_ASSERT_MESSAGE_FALSE( MESSAGE ) L_TRIGGER_ASSERT_HANDLER( NULL, MESSAGE )

/// Trigger a debug breakpoint if the result of an expression is false in non-release builds while still evaluating the
/// expression in release builds.
///
/// @param[in] EXP  Expression to evaluate.
///
/// @see L_ASSERT(), L_ASSERT_MESSAGE(), L_ASSERT_FALSE(), L_ASSERT_MESSAGE_FALSE(), L_VERIFY_MESSAGE()
#define L_VERIFY( EXP ) L_ASSERT( EXP )

/// Trigger a debug breakpoint with a customized message if the result of an expression is false in non-release builds
/// while still evaluating the expression in release builds.
///
/// @param[in] EXP      Expression to evaluate.
/// @param[in] MESSAGE  Message to display if the assertion is triggered.
///
/// @see L_ASSERT(), L_ASSERT_MESSAGE(), L_ASSERT_FALSE(), L_ASSERT_MESSAGE_FALSE(), L_VERIFY()
#define L_VERIFY_MESSAGE( EXP, MESSAGE ) L_ASSERT_MESSAGE( EXP, MESSAGE )

#endif

//@}

#endif  // LUNAR_CORE_ASSERT_H
