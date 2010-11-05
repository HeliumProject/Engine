#include "Platform/Assert.h"
#include "Platform/Trace.h"

#if HELIUM_ASSERT_ENABLED

#include "Platform/Atomic.h"
#pragma TODO( "LUNAR MERGE - Update spin locking in Assert::Trigger() to use proper atomics once merged over from Lunar." )
#include "Platform/Windows/Windows.h"  // LUNAR MERGE - Remove this line once Lunar atomic variable support is merged.

#pragma TODO( "LUNAR MERGE - Remove HELIUM_ARRAY_COUNT() macro definition here once L_ARRAY_COUNT() is merged over." )
#define HELIUM_ARRAY_COUNT( ARRAY ) ( sizeof( ARRAY ) / sizeof( ARRAY[ 0 ] ) )

#if HELIUM_CC_MSC
# if HELIUM_UNICODE
#  define SNPRINTF( BUFFER, FORMAT, ... ) _snwprintf_s( BUFFER, _TRUNCATE, FORMAT, __VA_ARGS__ )
# else
#  define SNPRINTF( BUFFER, FORMAT, ... ) _snprintf_s( BUFFER, _TRUNCATE, FORMAT, __VA_ARGS__ )
# endif
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
    const tchar_t* pMessage,
    const char* pFunction,
    const char* pFile,
    int line )
{
    // Only allow one assert handler to be active at a time.
    // LUNAR MERGE - Update the following once Lunar atomic variable support is merged.
//    while( AtomicCompareExchangeAcquire( sm_active, 1, 0 ) != 0 )
    while( InterlockedCompareExchange( reinterpret_cast< volatile LONG* >( &sm_active ), 1, 0 ) )
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
            SNPRINTF(
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
            SNPRINTF(
                messageText,
                TXT( "Assertion failed in %s (%s, line %d): %s" ),
                functionName,
                fileName,
                line,
                pMessage );
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

    // LUNAR MERGE - Update the following once Lunar atomic variable support is merged.
//    AtomicExchangeRelease( sm_active, 0 );
    InterlockedExchange( reinterpret_cast< volatile LONG* >( &sm_active ), 0 );

    return result;
}

#endif  // HELIUM_ASSERT_ENABLED
