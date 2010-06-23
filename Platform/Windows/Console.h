#pragma once

#include <string>
#include <stdio.h>

#include "Platform/API.h"
#include "Platform/Types.h"

namespace Platform
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

    PLATFORM_API void Print(ConsoleColor color, FILE* stream, const tchar* fmt, ...);

    PLATFORM_API void PrintArgs(ConsoleColor color, FILE* stream, const tchar* fmt, va_list args);

    PLATFORM_API void PrintString(ConsoleColor color, FILE* stream, const tstring& tstring);
}