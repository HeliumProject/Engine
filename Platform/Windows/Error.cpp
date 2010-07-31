#include "Platform/Error.h"

#include "Windows.h"

using namespace Helium;

u32 Helium::GetLastError()
{
    return ::GetLastError();
}

tstring Helium::GetErrorString( u32 errorOverride )
{
    // get the system error
    DWORD error = ( errorOverride != 0 ) ? errorOverride : ::GetLastError();

    LPVOID lpMsgBuf;
    ::FormatMessage( 
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL 
        );

    tstring result;

    if (lpMsgBuf)
    {
        result = (tchar*)lpMsgBuf;

        ::LocalFree( lpMsgBuf );
    }
    else
    {
        result = TXT("Unknown error (the error code could not be translated)");
    }

    // trim enter chracters from message
    while (!result.empty() && (*result.rbegin() == '\n' || *result.rbegin() == '\r'))
    {
        result.resize( result.size() - 1 );
    }

    return result;
}
