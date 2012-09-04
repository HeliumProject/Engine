#include "PlatformPch.h"
#include "Console.h"

#include "Platform/Print.h"

using namespace Helium;

void Helium::Print(ConsoleColor color, FILE* stream, const tchar_t* fmt, ...)
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
    FilePrint(stream, fmt, args);
    va_end(args); 

    fflush(stream);

    if (color != ConsoleColors::None)
    {
        // restore previous settings
        SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), info.wAttributes);
    }
}

void Helium::PrintArgs(ConsoleColor color, FILE* stream, const tchar_t* fmt, va_list args)
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

    FilePrint(stream, fmt, args);

    fflush(stream);

    if (color != ConsoleColors::None)
    {
        // restore previous settings
        SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), info.wAttributes);
    }
}

void Helium::PrintString(ConsoleColor color, FILE* stream, const tstring& tstring)
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

    FilePrint(stream, TXT("%s"), tstring.c_str());

    fflush(stream);

    if (color != ConsoleColors::None)
    {
        // restore previous settings
        SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), info.wAttributes);
    }
}  