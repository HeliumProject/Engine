#pragma once

#include "Platform/Types.h"

namespace Helium
{
    tstring MD5(const void* data, uint32_t count);
    tstring MD5(const tstring& data);
    tstring FileMD5(const tstring& filePath, uint32_t packetSize = 4096);
}
