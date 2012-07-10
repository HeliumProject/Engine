#include "PlatformPch.h"
#include "Platform/Assert.h"
#include "Platform/Trace.h"
#include "Platform/Print.h"

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
Assert::EResult Assert::Trigger(
    const char* pExpression,
    const char* pFunction,
    const char* pFile,
    int line,
    const char* pMessage,
    ... )
{
    // Only allow one assert handler to be active at a time.
    while( AtomicCompareExchangeAcquire( sm_active, 1, 0 ) != 0 )
    {
    }

    char messageText[ 1024 ];

    if( pExpression )
    {
        if( pMessage )
        {
            va_list args;
            va_start(args, pMessage); 
            char message[1024];
            vsnprintf(message, sizeof(message) / sizeof(char), pMessage, args);
            va_end(args); 

            snprintf(
                messageText,
				sizeof( messageText ) / sizeof( messageText[0] ),
                TXT( "%s\n\nAssertion failed in %s (%s, line %d): (%s)" ),
                message,
                pFunction,
                pFile,
                line,
                pExpression );
        }
        else
        {
            snprintf(
                messageText,
				sizeof( messageText ) / sizeof( messageText[0] ),
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
            char message[1024];
            vsnprintf(message, sizeof(message) / sizeof(char), pMessage, args);
            va_end(args); 

            snprintf(
                messageText,
				sizeof( messageText ) / sizeof( messageText[0] ),
                TXT( "%s\n\nAssertion failed in %s (%s, line %d)" ),
                message,
                pFunction,
                pFile,
                line );
        }
        else
        {
            snprintf(
                messageText,
				sizeof( messageText ) / sizeof( messageText[0] ),
                TXT( "Assertion failed in %s (%s, line %d)" ),
                pFunction,
                pFile,
                line );
        }
    }

    HELIUM_TRACE( TRACE_ERROR, TXT( "%s\n" ), messageText );

    // Present the assert message and get how we should proceed.
    EResult result = TriggerImplementation( messageText );

    AtomicExchangeRelease( sm_active, 0 );

    return result;
}

#endif  // HELIUM_ASSERT_ENABLED
