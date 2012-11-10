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

#pragma TODO("These char overloads are really ASCII, not UTF-8.  Track down some UTF-8 implementations.  -Geoff")

int Helium::Print(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int result = vprintf(fmt, args);
	va_end(args);
	return result;
}

int Helium::Print(const wchar_t* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int result = vwprintf(fmt, args);
	va_end(args);
	return result;
}

int Helium::PrintArgs(const char* fmt, va_list args)
{
	return vprintf(fmt, args);
}

int Helium::PrintArgs(const wchar_t* fmt, va_list args)
{
	return vwprintf(fmt, args);
}

int Helium::FilePrint(FILE* f, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int result = vfprintf(f, fmt, args);
	va_end(args);
	return result;
}

int Helium::FilePrint(FILE* f, const wchar_t* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int result = vfwprintf(f, fmt, args);
	va_end(args);
	return result;
}

int Helium::FilePrintArgs(FILE* f, const char* fmt, va_list args)
{
	return vfprintf(f, fmt, args);
}

int Helium::FilePrintArgs(FILE* f, const wchar_t* fmt, va_list args)
{
	return vfwprintf(f, fmt, args);
}

int Helium::StringPrint(char* dest, size_t destCount, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int result = vsnprintf(dest, destCount, fmt, args);
	va_end(args);
	return result;
}

int Helium::StringPrint(wchar_t* dest, size_t destCount, const wchar_t* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int result = _vsnwprintf(dest, destCount, fmt, args);
	va_end(args);
	return result;
}

int Helium::StringPrintArgs(char* dest, size_t destCount, const char* fmt, va_list args)
{
	return vsnprintf(dest, destCount, fmt, args);
}

int Helium::StringPrintArgs(wchar_t* dest, size_t destCount, const wchar_t* fmt, va_list args)
{
	return _vsnwprintf(dest, destCount, fmt, args);
}

template< class T >
int PrintArgs(ConsoleColor color, FILE* stream, const T* fmt, va_list args)
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

template< class T >
int PrintString(ConsoleColor color, FILE* stream, const std::basic_string< T >& tstring)
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

int Helium::Print(ConsoleColor color, FILE* stream, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int result = ::PrintArgs<char>( color, stream, fmt, args );
	va_end(args);
	return result;
}

int Helium::Print(ConsoleColor color, FILE* stream, const wchar_t* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int result = ::PrintArgs<wchar_t>( color, stream, fmt, args );
	va_end(args);
	return result;
}

int Helium::PrintArgs(ConsoleColor color, FILE* stream, const char* fmt, va_list args)
{
	return ::PrintArgs<char>( color, stream, fmt, args );
}

int Helium::PrintArgs(ConsoleColor color, FILE* stream, const wchar_t* fmt, va_list args)
{
	return ::PrintArgs<wchar_t>( color, stream, fmt, args );
}

int Helium::PrintString(ConsoleColor color, FILE* stream, const std::string& string)
{
	return ::PrintString<char>( color, stream, string );
}

int Helium::PrintString(ConsoleColor color, FILE* stream, const std::wstring& string)
{
	return ::PrintString<wchar_t>( color, stream, string );
}