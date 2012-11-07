#include "PlatformPch.h"
#include "Print.h"
#include "Assert.h"

#include <stdlib.h>

#pragma TODO("These are really ASCII, not UTF-8.  Track down some UTF-8 implementations.  -Geoff")

int Helium::Print(const tchar_t* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
#if HELIUM_WCHAR_T
	int result = vwprintf(fmt, args);
#else
	int result = vprintf(fmt, args);
#endif
	va_end(args);
	return result;
}

int Helium::PrintArgs(const tchar_t* fmt, va_list args)
{
#if HELIUM_WCHAR_T
	return vwprintf(fmt, args);
#else
	return vprintf(fmt, args);
#endif
}

int Helium::FilePrint(FILE* f, const tchar_t* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
#if HELIUM_WCHAR_T
	int result = vfwprintf(f, fmt, args);
#else
	int result = vfprintf(f, fmt, args);
#endif
	va_end(args);
	return result;
}

int Helium::FilePrintArgs(FILE* f, const tchar_t* fmt, va_list args)
{
#if HELIUM_WCHAR_T
	return vfwprintf(f, fmt, args);
#else
	return vfprintf(f, fmt, args);
#endif
}

int Helium::StringPrint(tchar_t* dest, size_t destCount, const tchar_t* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
#if HELIUM_WCHAR_T
	int result = _vsnwprintf(dest, destCount, fmt, args);
#else
	int result = vsnprintf(dest, destCount, fmt, args);
#endif
	va_end(args);
	return result;
}

int Helium::StringPrintArgs(tchar_t* dest, size_t destCount, const tchar_t* fmt, va_list args)
{
#if HELIUM_WCHAR_T
	return _vsnwprintf(dest, destCount, fmt, args);
#else
	return vsnprintf(dest, destCount, fmt, args);
#endif
}