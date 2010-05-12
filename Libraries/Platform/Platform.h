#pragma once

#include "API.h"

namespace Platform
{
  namespace Types
  {
    enum Type
    {
      Windows,
      PlayStation3,
      Count,
    };
  }
  typedef Types::Type Type;

  PLATFORM_API Type GetType();
  PLATFORM_API const char* GetTypeName(Type t);

  PLATFORM_API void Print(const char* fmt, ...);

  PLATFORM_API void Sleep(int millis);
}
