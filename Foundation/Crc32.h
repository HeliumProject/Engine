#pragma once

#include "Platform/Types.h"
#include "Platform/Utility.h"
#include "Foundation/API.h"

namespace Helium
{
    /// @defgroup crc32 CRC-32 Support
    //@{
    HELIUM_FOUNDATION_API inline uint32_t Crc32( const void* pData, size_t byteCount );
    HELIUM_FOUNDATION_API inline uint32_t Crc32( const char* pString );
    HELIUM_FOUNDATION_API inline uint32_t Crc32( const wchar_t* pString );

    HELIUM_FOUNDATION_API inline uint32_t BeginCrc32();
    HELIUM_FOUNDATION_API uint32_t UpdateCrc32( uint32_t runningCrc, const void* pData, size_t byteCount );
    HELIUM_FOUNDATION_API inline uint32_t FinishCrc32( uint32_t runningCrc );
    //@}
}

#include "Foundation/Crc32.inl"
