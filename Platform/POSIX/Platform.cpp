#include "Platform/Platform.h"

#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>

using namespace Platform;

Platform::Type Platform::GetType()
{
    return Types::PlayStation3;
}

void Platform::Print(const tchar* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

void Platform::Sleep(int millis)
{
    usleep( millis * 1000 );
}
