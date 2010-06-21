#include "Platform/Error.h"

#include "Windows.h"

using namespace Platform;

u32 Platform::GetLastError()
{
    return ::GetLastError();
}

std::string Platform::GetErrorString( u32 errorOverride )
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

    std::string result;

    if (lpMsgBuf)
    {
        result = (char*)lpMsgBuf;

        ::LocalFree( lpMsgBuf );
    }
    else
    {
        result = "Unknown error (the error code could not be translated)";
    }

    // trim enter characters from message
    while (!result.empty() && (*result.rbegin() == '\n' || *result.rbegin() == '\r'))
    {
        result.resize( result.size() - 1 );
    }

    return result;
}
