#include "PlatformPch.h"
#include "Console.h"

#include "Platform/Assert.h"

#include <stdlib.h>

using namespace Helium;

struct ColorEntry
{
	int m_Key;
	int m_Value;
};

ColorEntry g_ColorTable[] =
{
	{ ConsoleColors::None, 0xffffffff },
	{ ConsoleColors::Red, FOREGROUND_RED },
	{ ConsoleColors::Green, FOREGROUND_GREEN },
	{ ConsoleColors::Blue, FOREGROUND_BLUE },
	{ ConsoleColors::Yellow, FOREGROUND_RED | FOREGROUND_GREEN, },
	{ ConsoleColors::Aqua, FOREGROUND_GREEN | FOREGROUND_BLUE, },
	{ ConsoleColors::Purple, FOREGROUND_BLUE | FOREGROUND_RED, },
	{ ConsoleColors::White, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, }
};

static inline int LookupColor( ConsoleColor color )
{
	for ( int i=0; i<sizeof(g_ColorTable)/sizeof(g_ColorTable[0]); i++ )
	{
		if ( g_ColorTable[i].m_Key == color )
		{
			return g_ColorTable[i].m_Value;
		}
	}

	HELIUM_ASSERT( false );
	return ConsoleColors::None;
}

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

int Helium::Print(ConsoleColor color, FILE* stream, const tchar_t* fmt, ...)
{
	CONSOLE_SCREEN_BUFFER_INFO info;

	if (color != ConsoleColors::None)
	{
		// retrieve settings
		GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &info);

		// extract background colors
		int background = info.wAttributes & ~(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

		// reset forground only to our desired color
		SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), color | FOREGROUND_INTENSITY | background);
	}

	va_list args;
	va_start(args, fmt); 
	int result = FilePrint(stream, fmt, args);
	va_end(args); 

	fflush(stream);

	if (color != ConsoleColors::None)
	{
		// restore previous settings
		SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), info.wAttributes);
	}

	return result;
}

int Helium::PrintArgs(ConsoleColor color, FILE* stream, const tchar_t* fmt, va_list args)
{
	CONSOLE_SCREEN_BUFFER_INFO info;

	if (color != ConsoleColors::None)
	{
		// retrieve settings
		GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &info);

		// extract background colors
		int background = info.wAttributes & ~(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

		// reset forground only to our desired color
		SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), color | FOREGROUND_INTENSITY | background);
	}

	int result = FilePrint(stream, fmt, args);

	fflush(stream);

	if (color != ConsoleColors::None)
	{
		// restore previous settings
		SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), info.wAttributes);
	}

	return result;
}

int Helium::PrintString(ConsoleColor color, FILE* stream, const tstring& tstring)
{
	CONSOLE_SCREEN_BUFFER_INFO info;

	if (color != ConsoleColors::None)
	{
		// retrieve settings
		GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &info);

		// extract background colors
		int background = info.wAttributes & ~(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

		// reset forground only to our desired color
		SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), color | FOREGROUND_INTENSITY | background);
	}

	int result = FilePrint(stream, TXT("%s"), tstring.c_str());

	fflush(stream);

	if (color != ConsoleColors::None)
	{
		// restore previous settings
		SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), info.wAttributes);
	}

	return result;
}