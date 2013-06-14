#pragma once

#include "Rendering/RendererTypes.h"

namespace Helium
{
    /// Renderer utility functions.
    class HELIUM_RENDERING_API RendererUtil
    {
    public:
        /// @name Pixel Format Support
        //@{
        static bool IsSrgbPixelFormat( ERendererPixelFormat format );
        static uint32_t PixelToBlockRowCount( uint32_t pixelRowCount, ERendererPixelFormat format );
        //@}

        /// @name Primitive Math
        //@{
        static uint32_t PrimitiveCountToIndexCount( ERendererPrimitiveType primitiveType, uint32_t primitiveCount );
        static uint32_t IndexCountToPrimitiveCount( ERendererPrimitiveType primitiveType, uint32_t indexCount );
        //@}
    };
}
