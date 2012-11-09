#include "PlatformPch.h"
#include "Platform/Assert.h"
#include "Platform/Trace.h"
#include "Platform/Console.h"

#if HELIUM_ASSERT_ENABLED

#include "Platform/Atomic.h"

using namespace Helium;

volatile int32_t Assert::sm_active = 0;

/// Handle an assertion.
///
/// @param[in] pExpression  String containing the expression that failed, or null if the assertion was
///                         unconditional.
/// @param[in] pMessage     Message associated with the assertion, or null if no customized message was given.
/// @param[in] pFunction    Function in which the assertion occurred.
/// @param[in] pFile        File in which the assertion occurred.
/// @param[in] line         Line number at which the assertion occurred.
AssertResult Assert::Trigger(
    const tchar_t* pExpression,
    const tchar_t* pFunction,
    const tchar_t* pFile,
    int line,
    const tchar_t* pMessage,
    ... )
{
    // Only allow one assert handler to be active at a time.
    while( AtomicCompareExchangeAcquire( sm_active, 1, 0 ) != 0 )
    {
    }

    tchar_t messageText[ 1024 ];

    if( pExpression )
    {
        if( pMessage )
        {
            va_list args;
            va_start(args, pMessage); 
            tchar_t message[1024];
            StringPrint(message, pMessage, args);
            va_end(args); 

            StringPrint(
                messageText,
				TXT( "%s\n\nAssertion failed in %s (%s, line %d): (%s)" ),
                message,
                pFunction,
                pFile,
                line,
                pExpression );
        }
        else
        {
            StringPrint(
                messageText,
                TXT( "Assertion failed in %s (%s, line %d): %s" ),
                pFunction,
                pFile,
                line,
                pExpression );
        }
    }
    else
    {
        if( pMessage )
        {
            va_list args;
            va_start(args, pMessage); 
            tchar_t message[1024];
            StringPrint(message, pMessage, args);
            va_end(args); 

            StringPrint(
                messageText,
                TXT( "%s\n\nAssertion failed in %s (%s, line %d)" ),
                message,
                pFunction,
                pFile,
                line );
        }
        else
        {
            StringPrint(
                messageText,
                TXT( "Assertion failed in %s (%s, line %d)" ),
                pFunction,
                pFile,
                line );
        }
    }

    HELIUM_TRACE( TraceLevels::Error, TXT( "%s\n" ), messageText );

    // Present the assert message and get how we should proceed.
    AssertResult result = TriggerImplementation( messageText );

    AtomicExchangeRelease( sm_active, 0 );

    return result;
}

#endif  // HELIUM_ASSERT_ENABLED
