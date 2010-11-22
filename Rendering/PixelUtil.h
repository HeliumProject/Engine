//----------------------------------------------------------------------------------------------------------------------
// PixelUtil.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_PIXEL_UTIL_H
#define LUNAR_RENDERING_PIXEL_UTIL_H

#include "Rendering/RendererTypes.h"

namespace Lunar
{
    /// Pixel format utility functions.
    class LUNAR_RENDERING_API PixelUtil
    {
    public:
        /// @name Static Utility Functions
        //@{
        static bool IsSrgbPixelFormat( ERendererPixelFormat format );
        static uint32_t PixelToBlockRowCount( uint32_t pixelRowCount, ERendererPixelFormat format );
        //@}
    };
}

#endif  // LUNAR_RENDERING_PIXEL_UTIL_H
