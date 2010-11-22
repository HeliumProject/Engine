#include "Platform/Platform.h"

#if HELIUM_OS_WIN

#include "Platform/Assert.h"

#include "Platform/Windows/Windows.h"

#ifdef _MANAGED
# using <System.dll>
#endif // _MANAGED

using namespace Helium;

/// Terminate the application on a fatal error.
///
/// @param[in] exitCode  Error code associated with the exit.
void Helium::FatalExit( int exitCode )
{
    ::FatalExit( exitCode );
}

#if HELIUM_ASSERT_ENABLED

#if HELIUM_UNICODE
#define SPRINTF_S swprintf_s
#define MESSAGE_BOX MessageBoxW
#else
#define SPRINTF_S sprintf_s
#define MESSAGE_BOX MessageBoxA
#endif

/// Handle an assertion.
///
/// @param[in] pMessageText  Assert message text.
Assert::EResult Assert::TriggerImplementation( const tchar_t* pMessageText )
{
#ifdef _MANAGED

#ifdef __cplusplus_cli
    System::Log::Write( gcnew System::String( messageText ) );
#else //__cplusplus_cli
    System::Log::Write( new System::String( messageText ) );
#endif //__cplusplus_cli

    return RESULT_BREAK;

#else  // _MANAGED

    tchar_t messageBoxText[ 1024 ];
    SPRINTF_S(
        messageBoxText,
        ( TXT( "%s\n\nChoose \"Abort\" to terminate the program, \"Retry\" to debug the program (if a debugger " )
          TXT( "is attached), or \"Ignore\" to attempt to skip over the error." ) ),
        pMessageText );

    int result = MESSAGE_BOX( NULL, messageBoxText, TXT( "Assert" ), MB_ABORTRETRYIGNORE );

    return ( result == IDABORT ? RESULT_ABORT : ( result == IDIGNORE ? RESULT_CONTINUE : RESULT_BREAK ) );

#endif  // _MANAGED
}

#endif  // HELIUM_OS_WIN

#endif  // HELIUM_ASSERT_ENABLED
