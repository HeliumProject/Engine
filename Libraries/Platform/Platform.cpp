#define PLATFORM_INCLUDE_SYSTEM
#include "Platform.h"

#include "Common/Assert.h"

using namespace Platform;

static const char* TypeNames[] = 
{
  "Windows",
  "PlayStation3",
};

NOC_COMPILE_ASSERT( Platform::Types::Count == sizeof(TypeNames) / sizeof(const char*) );

const char* Platform::GetTypeName(Type t)
{
  if (t >= 0 && t<Types::Count)
  {
    return TypeNames[t];
  }
  else
  {
    return TypeNames[0];
  }
}
