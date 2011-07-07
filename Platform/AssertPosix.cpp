#include "Platform/Assert.h"

#include <cstdio>

using namespace Helium;

/// Terminate the application on a fatal error.
///
/// @param[in] exitCode  Error code associated with the exit.
void Helium::FatalExit( int /*exitCode*/ )
{
    abort();
}

#if HELIUM_ASSERT_ENABLED

#if HELIUM_UNICODE
#define PRINTF printf
#else
#define PRINTF wprintf
#endif

/// Handle an assertion.
///
/// @param[in] pMessageText  Assert message text.
Assert::EResult Assert::TriggerImplementation( const tchar_t* pMessageText )
{
    PRINTF( "%s\n", pMessageText );

    return RESULT_BREAK;
}

#endif  // HELIUM_ASSERT_ENABLED
