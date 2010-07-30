#include "Platform/Error.h"

using namespace Helium;

u32 Helium::GetLastError()
{
    return 0xffffffff;
}

tstring Helium::GetErrorString( u32 errorOverride )
{
    return "Unknown";
}
