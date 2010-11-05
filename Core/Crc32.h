//----------------------------------------------------------------------------------------------------------------------
// Crc32.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_CRC_32_H
#define LUNAR_CORE_CRC_32_H

#include "Core/Core.h"

namespace Lunar
{
    /// @defgroup crc32 CRC-32 Support
    //@{
    LUNAR_CORE_API inline uint32_t Crc32( const void* pData, size_t byteCount );

    LUNAR_CORE_API inline uint32_t BeginCrc32();
    LUNAR_CORE_API uint32_t UpdateCrc32( uint32_t runningCrc, const void* pData, size_t byteCount );
    LUNAR_CORE_API inline uint32_t FinishCrc32( uint32_t runningCrc );
    //@}
}

#include "Core/Crc32.inl"

#endif  // LUNAR_CORE_CRC_32_H
