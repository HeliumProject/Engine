//----------------------------------------------------------------------------------------------------------------------
// PixelUtil.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "RenderingPch.h"
#include "Rendering/PixelUtil.h"

namespace Lunar
{
    /// Get whether a given pixel format is an sRGB pixel format.
    ///
    /// @return  True if the format is sampled in sRGB color space, false if it is sampled in linear space.
    bool PixelUtil::IsSrgbPixelFormat( ERendererPixelFormat format )
    {
        return ( format == RENDERER_PIXEL_FORMAT_R8G8B8A8_SRGB ||
                 format == RENDERER_PIXEL_FORMAT_BC1_SRGB ||
                 format == RENDERER_PIXEL_FORMAT_BC2_SRGB ||
                 format == RENDERER_PIXEL_FORMAT_BC3_SRGB );
    }

    /// Compute the number of block rows for the specified pixel format to accommodate the given number of rows of
    /// pixels.
    ///
    /// @param[in] pixelRowCount  Number of rows of pixels.
    /// @param[in] format         Pixel format.
    ///
    /// @return  Number of rows of compressed blocks if the format is a compressed block format, otherwise the number of
    ///          rows of pixels if the format is not compressed.
    uint32_t PixelUtil::PixelToBlockRowCount( uint32_t pixelRowCount, ERendererPixelFormat format )
    {
        HELIUM_ASSERT( static_cast< size_t >( format ) < static_cast< size_t >( RENDERER_PIXEL_FORMAT_MAX ) );

        static const uint32_t PIXEL_ROWS_PER_BLOCK[] =
        {
            1,  // RENDERER_PIXEL_FORMAT_R8G8B8A8
            1,  // RENDERER_PIXEL_FORMAT_R8G8B8A8_SRGB
            1,  // RENDERER_PIXEL_FORMAT_R8
            4,  // RENDERER_PIXEL_FORMAT_BC1
            4,  // RENDERER_PIXEL_FORMAT_BC1_SRGB
            4,  // RENDERER_PIXEL_FORMAT_BC2
            4,  // RENDERER_PIXEL_FORMAT_BC2_SRGB
            4,  // RENDERER_PIXEL_FORMAT_BC3
            4,  // RENDERER_PIXEL_FORMAT_BC3_SRGB
            1,  // RENDERER_PIXEL_FORMAT_R16G16B16A16_FLOAT
            1   // RENDERER_PIXEL_FORMAT_DEPTH
        };

        BOOST_STATIC_ASSERT( HELIUM_ARRAY_COUNT( PIXEL_ROWS_PER_BLOCK ) == RENDERER_PIXEL_FORMAT_MAX );

        uint32_t pixelRowsPerBlock = PIXEL_ROWS_PER_BLOCK[ format ];

        uint32_t blockRowCount = ( pixelRowCount + pixelRowsPerBlock - 1 ) / pixelRowsPerBlock;

        return blockRowCount;
    }
}
