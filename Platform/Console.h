#pragma once

#include <string>
#include <stdio.h>

#include "Platform/API.h"
#include "Platform/Types.h"

namespace Helium
{
    namespace ConsoleColors
    {
        enum ConsoleColor
        {
            None   = 0xffffffff,

            // from wincon.h
            Red    = 0x0004,
            Green  = 0x0002,
            Blue   = 0x0001,

            Yellow = Red | Green,
            Aqua   = Green | Blue,
            Purple = Blue | Red,

            White  = Red | Green | Blue,
        };
    }
    typedef ConsoleColors::ConsoleColor ConsoleColor;    

    HELIUM_PLATFORM_API void Print(ConsoleColor color, FILE* stream, const tchar_t* fmt, ...);

    HELIUM_PLATFORM_API void PrintArgs(ConsoleColor color, FILE* stream, const tchar_t* fmt, va_list args);

    HELIUM_PLATFORM_API void PrintString(ConsoleColor color, FILE* stream, const tstring& tstring);
}