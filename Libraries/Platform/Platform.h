#pragma once

#include "API.h"
#include "Assert.h"

namespace Platform
{
  PLATFORM_API void Initialize();
  PLATFORM_API void Cleanup();

  namespace Types
  {
    enum Type
    {
      Windows,
      PlayStation3,
      Count,
    };

    static const char* Strings[] = 
    {
      "Windows",
      "PlayStation3",
    };

    NOC_COMPILE_ASSERT( Platform::Types::Count == sizeof(Strings) / sizeof(const char*) );
  }
  typedef Types::Type Type;

  inline const char* GetTypeName(Type t)
  {
    if (t >= 0 && t<Types::Count)
    {
      return Types::Strings[t];
    }
    else
    {
      return Types::Strings[0];
    }
  }

  PLATFORM_API Type GetType();
  PLATFORM_API void Print(const char* fmt, ...);
  PLATFORM_API void Sleep(int millis);
}
