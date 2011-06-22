#include "PlatformPch.h"
#include "Platform/Assert.h"
#include "Platform/Trace.h"

#if HELIUM_ASSERT_ENABLED

#include "Platform/Atomic.h"

#if HELIUM_OS_WIN
#include "Platform/Windows/Windows.h"
#endif

#if HELIUM_CC_MSC
#  define SNPRINTF( BUFFER, FORMAT, ... )           _sntprintf_s( BUFFER, _TRUNCATE, FORMAT, __VA_ARGS__ )
#  define VSNPRINTF( BUFFER, FORMAT, FMT, ARGS )    _vsntprintf_s( BUFFER, _TRUNCATE, FORMAT, FMT, ARGS )
#else
# if HELIUM_UNICODE
#  define SNPRINTF( BUFFER, FORMAT, ... ) { swprintf( BUFFER, HELIUM_ARRAY_COUNT( BUFFER ) - 1, FORMAT, __VA_ARGS__ ); BUFFER[ HELIUM_BUFFER_ARRAY_COUNT( BUFFER ) - 1 ] = L'\0'; }
# else
#  define SNPRINTF( BUFFER, FORMAT, ... ) { snprintf( BUFFER, HELIUM_ARRAY_COUNT( BUFFER ) - 1, FORMAT, __VA_ARGS__ ); BUFFER[ HELIUM_BUFFER_ARRAY_COUNT( BUFFER ) - 1 ] = '\0'; }
# endif
#endif

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
    const tchar_t* pExpression,
    const char* pFunction,
    const char* pFile,
    int line,
    const tchar_t* pMessage,
    ... )
{
    // Only allow one assert handler to be active at a time.
    while( AtomicCompareExchangeAcquire( sm_active, 1, 0 ) != 0 )
    {
    }

    // Build the assert message string.
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
            va_list args;
            va_start(args, pMessage); 
            tchar_t message[1024];
            VSNPRINTF(message, sizeof(message) / sizeof(tchar_t), pMessage, args);
            va_end(args); 

            SNPRINTF(
                messageText,
                TXT( "%s\n\nAssertion failed in %s (%s, line %d): (%s)" ),
                message,
                functionName,
                fileName,
                line,
                pExpression );
        }
        else
        {
            SNPRINTF(
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
            va_list args;
            va_start(args, pMessage); 
            tchar_t message[1024];
            VSNPRINTF(message, sizeof(message) / sizeof(tchar_t), pMessage, args);
            va_end(args); 

            SNPRINTF(
                messageText,
                TXT( "%s\n\nAssertion failed in %s (%s, line %d)" ),
                message,
                functionName,
                fileName,
                line );
        }
        else
        {
            SNPRINTF(
                messageText,
                TXT( "Assertion failed in %s (%s, line %d)" ),
                functionName,
                fileName,
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
