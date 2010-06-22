#include "Platform/Error.h"

using namespace Platform;

u32 Platform::GetLastError()
{
    return 0xffffffff;
}

tstring Platform::GetErrorString( u32 errorOverride )
{
    return "Unknown";
}
