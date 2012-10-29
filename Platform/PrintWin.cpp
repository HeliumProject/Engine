#include "PlatformPch.h"
#include "Print.h"
#include "Assert.h"

#include <stdlib.h>

#pragma TODO("These are really ASCII, not UTF-8.  Track down some UTF-8 implementations.  -Geoff")

int Helium::Print(const tchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int result = vprintf(fmt, args);
    va_end(args);
	return result;
}

int Helium::PrintArgs(const tchar_t* fmt, va_list args)
{
    return vprintf(fmt, args);
}

int Helium::FilePrint(FILE* f, const tchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int result = vfprintf(f, fmt, args);
    va_end(args);
	return result;
}

int Helium::FilePrintArgs(FILE* f, const tchar_t* fmt, va_list args)
{
    return vfprintf(f, fmt, args);
}

int Helium::StringPrint(tchar_t* dest, size_t destCount, const tchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int result = _vsnprintf(dest, destCount, fmt, args);
    va_end(args);
	return result;
}

int Helium::StringPrintArgs(tchar_t* dest, size_t destCount, const tchar_t* fmt, va_list args)
{
    return _vsnprintf(dest, destCount, fmt, args);
}