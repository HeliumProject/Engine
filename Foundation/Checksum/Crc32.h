#pragma once

#include "Platform/Types.h"
#include "Platform/Utility.h"
#include "Foundation/API.h"

namespace Helium
{
    /// @defgroup crc32 CRC-32 Support
    //@{
    FOUNDATION_API inline uint32_t Crc32( const void* pData, size_t byteCount );
    FOUNDATION_API inline uint32_t Crc32( const char* pString );
    FOUNDATION_API inline uint32_t Crc32( const wchar_t* pString );

    FOUNDATION_API inline uint32_t BeginCrc32();
    FOUNDATION_API uint32_t UpdateCrc32( uint32_t runningCrc, const void* pData, size_t byteCount );
    FOUNDATION_API inline uint32_t FinishCrc32( uint32_t runningCrc );
    //@}
}

#include "Foundation/Checksum/Crc32.inl"
