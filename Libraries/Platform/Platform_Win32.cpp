#include "Windows/Windows.h"
#include "Platform.h"

#include "Common/Assert.h"

using namespace Platform;

Platform::Type Platform::GetType()
{
  return Types::Windows;
}

void Platform::Print(const char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}

void Platform::Sleep(int millis)
{
  ::Sleep(millis);
}
