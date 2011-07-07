#include "PlatformPch.h"
#include "Print.h"

#include <stdlib.h>

void Helium::Print(const tchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    _vtprintf(fmt, args);
    va_end(args);
}
