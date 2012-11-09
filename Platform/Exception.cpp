#include "PlatformPch.h"
#include "Exception.h"

using namespace Helium;

Helium::Exception::Exception()
{

}

const tchar_t* Helium::Exception::What() const
{
    return m_Message.c_str();
}

Exception::Exception( const tchar_t *msgFormat, ... )
{
    va_list msgArgs;
    va_start( msgArgs, msgFormat );
    SetMessage( msgFormat, msgArgs );
    va_end( msgArgs );
}

void Exception::SetMessage( const tchar_t* msgFormat, ... )
{
    va_list msgArgs;
    va_start( msgArgs, msgFormat );
    SetMessage( msgFormat, msgArgs );
    va_end( msgArgs );
}

void Exception::SetMessage( const tchar_t* msgFormat, va_list msgArgs )
{
    tchar_t msgBuffer[ERROR_STRING_BUF_SIZE];
    StringPrint( msgBuffer, sizeof(msgBuffer) / sizeof( tchar_t ), msgFormat, msgArgs );
    m_Message = msgBuffer;
}