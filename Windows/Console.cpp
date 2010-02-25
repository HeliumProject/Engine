#include "Windows.h"
#include "Console.h"

using namespace Windows;

void Windows::Print(ConsoleColor color, FILE* stream, const char* fmt, ...)
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
  vfprintf(stream, fmt, args);
  va_end(args); 

  fflush(stream);

  if (color != ConsoleColors::None)
  {
    // restore previous settings
    SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), info.wAttributes);
  }
}

void Windows::PrintArgs(ConsoleColor color, FILE* stream, const char* fmt, va_list args)
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

  vfprintf(stream, fmt, args);

  fflush(stream);

  if (color != ConsoleColors::None)
  {
    // restore previous settings
    SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), info.wAttributes);
  }
}

void Windows::PrintString(ConsoleColor color, FILE* stream, const std::string& str)
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

  fprintf(stream, "%s", str.c_str());

  fflush(stream);

  if (color != ConsoleColors::None)
  {
    // restore previous settings
    SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), info.wAttributes);
  }
}  