#include "Windows/Windows.h"
#include "Platform.h"

#include "Common/Assert.h"
#include "Windows/Error.h"
#include "Console/Console.h"

using namespace Platform;

Platform::Type Platform::GetType()
{
  return Types::Windows;
}

void Platform::Print(const char* fmt, ...)
{
  static char buff[Console::MAX_PRINT_SIZE];

  va_list args;
  va_start(args, fmt);
  _vsnprintf(buff, sizeof(buff), fmt, args);
  buff[ sizeof(buff) - 1] = 0; 
  va_end(args);

  Console::PrintString(buff, Console::Streams::Normal, Console::Levels::Default);
}

void Platform::Sleep(int millis)
{
  ::Sleep(millis);
}
