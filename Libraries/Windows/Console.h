#pragma once

#include <string>

#ifndef REQUIRES_WINDOWS_H
#define REQUIRES_WINDOWS_H
#endif
#include "API.h"

namespace Windows
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

  WINDOWS_API void Print(ConsoleColor color, FILE* stream, const char* fmt, ...);

  WINDOWS_API void PrintArgs(ConsoleColor color, FILE* stream, const char* fmt, va_list args);

  WINDOWS_API void PrintString(ConsoleColor color, FILE* stream, const std::string& str);
}