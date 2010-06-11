#include "Platform/Error.h"

using namespace Platform;

u32 Platform::GetLastError()
{
  return 0xffffffff;
}

std::string Platform::GetErrorString( u32 errorOverride )
{
  return "Unknown";
}
