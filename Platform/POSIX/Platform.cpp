#include "Platform/Platform.h"

#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>

using namespace Helium;

Platform::Type Helium::GetType()
{
    return Types::PlayStation3;
}

void Helium::Print(const tchar* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

void Helium::Sleep(int millis)
{
    usleep( millis * 1000 );
}
