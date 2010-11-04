//----------------------------------------------------------------------------------------------------------------------
// AssertWin.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"

#if HELIUM_OS_WIN

#include "Core/Assert.h"
#include "Core/Atomic.h"

#ifndef NDEBUG

#if HELIUM_UNICODE
#define SPRINTF_S swprintf_s
#define MESSAGE_BOX MessageBoxW
#else
#define SPRINTF_S sprintf_s
#define MESSAGE_BOX MessageBoxA
#endif

namespace Lunar
{
    volatile int32_t Assert::sm_active = 0;

    /// Handle an assertion.
    ///
    /// @param[in] pExpression  String containing the expression that failed, or null if the assertion was
    ///                         unconditional.
    /// @param[in] pMessage     Message associated with the assertion, or null if no customized message was given.
    /// @param[in] pFunction    Function in which the assertion occurred.
    /// @param[in] pFile        File in which the assertion occurred.
    /// @param[in] line         Line number at which the assertion occurred.
    Assert::EResult Assert::Trigger(
        const tchar_t* pExpression,
        const tchar_t* pMessage,
        const char* pFunction,
        const char* pFile,
        int line )
    {
        // Only allow one assert handler to be active at a time.
        if( AtomicCompareExchangeAcquire( sm_active, 1, 0 ) != 0 )
        {
            return RESULT_BREAK;
        }

#if HELIUM_UNICODE
        wchar_t fileName[ MAX_PATH ];
        wchar_t functionName[ 1024 ];

        size_t charsConverted = 0;
        mbstowcs_s( &charsConverted, fileName, pFile, _TRUNCATE );
        fileName[ charsConverted ] = L'\0';
        mbstowcs_s( &charsConverted, functionName, pFunction, _TRUNCATE );
        functionName[ charsConverted ] = L'\0';
#else
        const char* fileName = pFile;
        const char* functionName = pFunction;
#endif

        tchar_t messageText[ 1024 ];

        if( pExpression )
        {
            if( pMessage )
            {
                SPRINTF_S(
                    messageText,
                    TXT( "Assertion failed in %s (%s, line %d): %s (%s)" ),
                    functionName,
                    fileName,
                    line,
                    pMessage,
                    pExpression );
            }
            else
            {
                SPRINTF_S(
                    messageText,
                    TXT( "Assertion failed in %s (%s, line %d): %s" ),
                    functionName,
                    fileName,
                    line,
                    pExpression );
            }
        }
        else
        {
            if( pMessage )
            {
                SPRINTF_S(
                    messageText,
                    TXT( "Assertion failed in %s (%s, line %d): %s" ),
                    functionName,
                    fileName,
                    line,
                    pMessage );
            }
            else
            {
                SPRINTF_S(
                    messageText,
                    TXT( "Assertion failed in %s (%s, line %d)" ),
                    functionName,
                    fileName,
                    line );
            }
        }

        L_LOG( LOG_ERROR, TXT( "%s\n" ), messageText );

        tchar_t messageBoxText[ 1024 ];
        SPRINTF_S(
            messageBoxText,
            ( TXT( "%s\n\nChoose \"Abort\" to terminate the program, \"Retry\" to debug the program (if a debugger " )
              TXT( "is attached), or \"Ignore\" to attempt to skip over the error." ) ),
            messageText );

        int result = MESSAGE_BOX( NULL, messageBoxText, TXT( "Assert" ), MB_ABORTRETRYIGNORE );

        AtomicExchangeRelease( sm_active, 0 );

        return ( result == IDABORT ? RESULT_ABORT : ( result == IDIGNORE ? RESULT_CONTINUE : RESULT_BREAK ) );
    }
}

#endif  // NDEBUG

#endif  // HELIUM_OS_WIN
