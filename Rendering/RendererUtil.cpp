#include "RenderingPch.h"
#include "Rendering/RendererUtil.h"

using namespace Helium;


/// Get whether a given pixel format is an sRGB pixel format.
///
/// @return  True if the format is sampled in sRGB color space, false if it is sampled in linear space.
bool RendererUtil::IsSrgbPixelFormat( ERendererPixelFormat format )
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
uint32_t RendererUtil::PixelToBlockRowCount( uint32_t pixelRowCount, ERendererPixelFormat format )
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

    HELIUM_COMPILE_ASSERT( HELIUM_ARRAY_COUNT( PIXEL_ROWS_PER_BLOCK ) == RENDERER_PIXEL_FORMAT_MAX );

    uint32_t pixelRowsPerBlock = PIXEL_ROWS_PER_BLOCK[ format ];

    uint32_t blockRowCount = ( pixelRowCount + pixelRowsPerBlock - 1 ) / pixelRowsPerBlock;

    return blockRowCount;
}

/// Compute the number indices needed to render the number of primitives of a given type.
///
/// @param[in] primitiveType   Type of primitive.
/// @param[in] primitiveCount  Number of primitives.
///
/// @return  Number of indices needed to render the specified number of primitives.
///
/// @see IndexCountToPrimitiveCount()
uint32_t RendererUtil::PrimitiveCountToIndexCount( ERendererPrimitiveType primitiveType, uint32_t primitiveCount )
{
    HELIUM_ASSERT( static_cast< size_t >( primitiveType ) < static_cast< size_t >( RENDERER_PRIMITIVE_TYPE_MAX ) );

    if( primitiveCount == 0 )
    {
        return 0;
    }

    switch( primitiveType )
    {
        case RENDERER_PRIMITIVE_TYPE_POINT_LIST:
        {
            return primitiveCount;
        }

        case RENDERER_PRIMITIVE_TYPE_LINE_LIST:
        {
            return primitiveCount * 2;
        }

        case RENDERER_PRIMITIVE_TYPE_LINE_STRIP:
        {
            return primitiveCount + 1;
        }

        case RENDERER_PRIMITIVE_TYPE_TRIANGLE_LIST:
        {
            return primitiveCount * 3;
        }

        case RENDERER_PRIMITIVE_TYPE_TRIANGLE_STRIP:
        case RENDERER_PRIMITIVE_TYPE_TRIANGLE_FAN:
        {
            return primitiveCount + 2;
        }

        case RENDERER_PRIMITIVE_TYPE_MAX:
            break;
    }

    HELIUM_ASSERT_MSG_FALSE( TXT( "Unknown primitive type: %d" ), static_cast< int >( primitiveType ) );

    return 0;
}

/// Compute the number of primitives of a given type that would be rendered when given a number of indices.
///
/// @param[in] primitiveType  Type of primitive.
/// @param[in] indexCount     Number of indices.
///
/// @return  Number of primitives rendered with the specified number of indices.
///
/// @note  If @c indexCount does not specify the exact number of indices needed to render the given primitive type (i.e.
///        an index count that is not a multiple of three specified for a line list), the largest number of primitives
///        that would be rendered will be returned.
///
/// @see PrimitiveCountToIndexCount()
uint32_t RendererUtil::IndexCountToPrimitiveCount( ERendererPrimitiveType primitiveType, uint32_t indexCount )
{
    HELIUM_ASSERT( static_cast< size_t >( primitiveType ) < static_cast< size_t >( RENDERER_PRIMITIVE_TYPE_MAX ) );

    switch( indexCount )
    {
        case RENDERER_PRIMITIVE_TYPE_POINT_LIST:
        {
            return indexCount;
        }

        case RENDERER_PRIMITIVE_TYPE_LINE_LIST:
        {
            return indexCount / 2;
        }

        case RENDERER_PRIMITIVE_TYPE_LINE_STRIP:
        {
            return ( indexCount > 1 ? indexCount - 1 : 0 );
        }

        case RENDERER_PRIMITIVE_TYPE_TRIANGLE_LIST:
        {
            return indexCount / 3;
        }

        case RENDERER_PRIMITIVE_TYPE_TRIANGLE_STRIP:
        case RENDERER_PRIMITIVE_TYPE_TRIANGLE_FAN:
        {
            return ( indexCount > 2 ? indexCount - 2 : 0 );
        }
    }

    HELIUM_ASSERT_MSG_FALSE( TXT( "Unknown primitive type: %d" ), static_cast< int >( primitiveType ) );

    return 0;
}
