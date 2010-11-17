//----------------------------------------------------------------------------------------------------------------------
// RendererTypes.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_RENDERER_TYPES_H
#define LUNAR_RENDERING_RENDERER_TYPES_H

#include "Rendering/Rendering.h"

namespace Lunar
{
    /// Maximum simultaneous render targets supported by the engine (note that the render device may support less).
    static const size_t SIMULTANEOUS_RENDER_TARGET_COUNT_MAX = 16;

    /// Renderer feature support flags.
    enum ERendererFeatureFlag
    {
        /// Depth texture support (for shadow mapping and depth-based post effects).
        RENDERER_FEATURE_FLAG_DEPTH_TEXTURE = ( 1 << 0 )
    };

    /// Triangle fill modes.
    enum ERendererFillMode
    {
        RENDERER_FILL_MODE_FIRST   =  0,
        RENDERER_FILL_MODE_INVALID = -1,

        /// Do not fill triangles (only draw lines connecting each triangle vertex).
        RENDERER_FILL_MODE_WIREFRAME,
        /// Fill all triangles.
        RENDERER_FILL_MODE_SOLID,

        RENDERER_FILL_MODE_MAX,
        RENDERER_FILL_MODE_LAST = RENDERER_FILL_MODE_MAX - 1
    };

    /// Triangle cull modes.
    enum ERendererCullMode
    {
        RENDERER_CULL_MODE_FIRST   =  0,
        RENDERER_CULL_MODE_INVALID = -1,

        /// Do not cull triangles based on the direction in which they are facing.
        RENDERER_CULL_MODE_NONE,
        /// Cull out front-facing triangles.
        RENDERER_CULL_MODE_FRONT,
        /// Cull out back-facing triangles.
        RENDERER_CULL_MODE_BACK,

        RENDERER_CULL_MODE_MAX,
        RENDERER_CULL_MODE_LAST = RENDERER_CULL_MODE_MAX - 1
    };

    /// Triangle winding.
    enum ERendererWinding
    {
        RENDERER_WINDING_FIRST   =  0,
        RENDERER_WINDING_INVALID = -1,

        /// Triangles are front-facing if their vertices are ordered counter-clockwise on screen.
        RENDERER_WINDING_COUNTER_CLOCKWISE,
        /// Triangles are front-facing if their vertices are ordered clockwise on screen.
        RENDERER_WINDING_CLOCKWISE,

        RENDERER_WINDING_MAX,
        RENDERER_WINDING_LAST = RENDERER_WINDING_MAX - 1
    };

    /// Blending factors.
    enum ERendererBlendFactor
    {
        RENDERER_BLEND_FACTOR_FIRST   =  0,
        RENDERER_BLEND_FACTOR_INVALID = -1,

        /// Input data is scaled by zero.
        RENDERER_BLEND_FACTOR_ZERO,
        /// Input data is scaled by one.
        RENDERER_BLEND_FACTOR_ONE,
        /// Input data is scaled by the source pixel color.
        RENDERER_BLEND_FACTOR_SRC_COLOR,
        /// Input data is scaled by one minus the source pixel color.
        RENDERER_BLEND_FACTOR_INV_SRC_COLOR,
        /// Input data is scaled by the source pixel alpha.
        RENDERER_BLEND_FACTOR_SRC_ALPHA,
        /// Input data is scaled by one minus the source pixel alpha.
        RENDERER_BLEND_FACTOR_INV_SRC_ALPHA,
        /// Input data is scaled by the destination pixel color.
        RENDERER_BLEND_FACTOR_DEST_COLOR,
        /// Input data is scaled by one minus the destination pixel color.
        RENDERER_BLEND_FACTOR_INV_DEST_COLOR,
        /// Input data is scaled by the destination alpha.
        RENDERER_BLEND_FACTOR_DEST_ALPHA,
        /// Input data is scaled by one minus the destination alpha.
        RENDERER_BLEND_FACTOR_INV_DEST_ALPHA,
        /// Input data is scaled by the minimum of the source alpha and one minuse the destination alpha.
        RENDERER_BLEND_FACTOR_SRC_ALPHA_SATURATE,

        RENDERER_BLEND_FACTOR_MAX,
        RENDERER_BLEND_FACTOR_LAST = RENDERER_BLEND_FACTOR_MAX - 1
    };

    /// Blending function operations.
    enum ERendererBlendFunction
    {
        RENDERER_BLEND_FUNCTION_FIRST   =  0,
        RENDERER_BLEND_FUNCTION_INVALID = -1,

        /// Add the source and destination component values.
        RENDERER_BLEND_FUNCTION_ADD,
        /// Subtract the destination component values from the source component values.
        RENDERER_BLEND_FUNCTION_SUBTRACT,
        /// Subtract the source component values from the destination component values.
        RENDERER_BLEND_FUNCTION_REVERSE_SUBTRACT,
        /// Select the minimum of each component value.
        RENDERER_BLEND_FUNCTION_MINIMUM,
        /// Select the maximum of each component value.
        RENDERER_BLEND_FUNCTION_MAXIMUM,

        RENDERER_BLEND_FUNCTION_MAX,
        RENDERER_BLEND_FUNCTION_LAST = RENDERER_BLEND_FUNCTION_MAX - 1
    };

    /// Depth/stencil comparison functions.
    enum ERendererCompareFunction
    {
        RENDERER_COMPARE_FUNCTION_FIRST   =  0,
        RENDERER_COMPARE_FUNCTION_INVALID = -1,

        /// Never pass.
        RENDERER_COMPARE_FUNCTION_NEVER,
        /// Pass if the incoming depth value is less than the stored depth value.
        RENDERER_COMPARE_FUNCTION_LESS,
        /// Pass if the incoming depth value is equal to the stored depth value.
        RENDERER_COMPARE_FUNCTION_EQUAL,
        /// Pass if the incoming depth value is less than or equal to the stored depth value.
        RENDERER_COMPARE_FUNCTION_LESS_EQUAL,
        /// Pass if the incoming depth value is greater than the stored depth value.
        RENDERER_COMPARE_FUNCTION_GREATER,
        /// Pass if the incoming depth value is not equal to the stored depth value.
        RENDERER_COMPARE_FUNCTION_NOT_EQUAL,
        /// Pass if the incoming depth value is greater than or equal to the stored depth value.
        RENDERER_COMPARE_FUNCTION_GREATER_EQUAL,
        /// Always pass.
        RENDERER_COMPARE_FUNCTION_ALWAYS,

        RENDERER_COMPARE_FUNCTION_MAX,
        RENDERER_COMPARE_FUNCTION_LAST = RENDERER_COMPARE_FUNCTION_MAX - 1
    };

    /// Stencil operations.
    enum ERendererStencilOperation
    {
        RENDERER_STENCIL_OPERATION_FIRST   =  0,
        RENDERER_STENCIL_OPERATION_INVALID = -1,

        /// Keep the current stencil value.
        RENDERER_STENCIL_OPERATION_KEEP,
        /// Set the stencil value to zero.
        RENDERER_STENCIL_OPERATION_ZERO,
        /// Replace the stencil value with the reference value.
        RENDERER_STENCIL_OPERATION_REPLACE,
        /// Increment the stencil value by one, clamping the result to the maximum possible value.
        RENDERER_STENCIL_OPERATION_INCREMENT,
        /// Increment the stencil value by one, wrapping the result to zero when the maximum possible value is
        /// exceeded.
        RENDERER_STENCIL_OPERATION_INCREMENT_WRAP,
        /// Decrement the stencil value by one, clamping the result to zero.
        RENDERER_STENCIL_OPERATION_DECREMENT,
        /// Decrement the stencil value by one, wrapping the result to the maximum possible value when decrementing
        /// from zero.
        RENDERER_STENCIL_OPERATION_DECREMENT_WRAP,
        /// Invert the bits of the current stencil value.
        RENDERER_STENCIL_OPERATION_INVERT,

        RENDERER_STENCIL_OPERATION_MAX,
        RENDERER_STENCIL_OPERATION_LAST = RENDERER_STENCIL_OPERATION_MAX - 1
    };

    /// Texture sampling filter modes.
    enum ERendererTextureFilter
    {
        RENDERER_TEXTURE_FILTER_FIRST   =  0,
        RENDERER_TEXTURE_FILTER_INVALID = -1,

        /// Point sampling for minification, point sampling for magnification, point sampling for mip-level sampling.
        RENDERER_TEXTURE_FILTER_MIN_POINT_MAG_POINT_MIP_POINT,
        /// Point sampling for minification, point sampling for magnification, linear interpolation for mip-level
        /// sampling.
        RENDERER_TEXTURE_FILTER_MIN_POINT_MAG_POINT_MIP_LINEAR,
        /// Point sampling for minification, linear interpolation for magnification, point sampling for mip-level
        /// sampling.
        RENDERER_TEXTURE_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
        /// Point sampling for minification, linear interpolation for magnification, linear interpolation for mip-level
        /// sampling.
        RENDERER_TEXTURE_FILTER_MIN_POINT_MAG_LINEAR_MIP_LINEAR,
        /// Linear interpolation for minification, point sampling for magnification, point sampling for mip-level
        /// sampling.
        RENDERER_TEXTURE_FILTER_MIN_LINEAR_MAG_POINT_MIP_POINT,
        /// Linear interpolation for minification, point sampling for magnification, linear interpolation for mip-level
        /// sampling.
        RENDERER_TEXTURE_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
        /// Linear interpolation for minification, linear interpolation for magnification, point sampling for mip-level
        /// sampling.
        RENDERER_TEXTURE_FILTER_MIN_LINEAR_MAG_LINEAR_MIP_POINT,
        /// Linear interpolation for minification, linear interpolation for magnification, linear interpolation for
        /// mip-level sampling.
        RENDERER_TEXTURE_FILTER_MIN_LINEAR_MAG_LINEAR_MIP_LINEAR,
        /// Anisotropic interpolation for minification, magnification, and mip-level sampling.
        RENDERER_TEXTURE_FILTER_ANISOTROPIC,

        RENDERER_TEXTURE_FILTER_MAX,
        RENDERER_TEXTURE_FILTER_LAST = RENDERER_TEXTURE_FILTER_MAX - 1
    };

    /// Texture addressing modes.
    enum ERendererTextureAddressMode
    {
        RENDERER_TEXTURE_ADDRESS_MODE_FIRST   =  0,
        RENDERER_TEXTURE_ADDRESS_MODE_INVALID = -1,

        /// Wrap values outside the 0 to 1 range.
        RENDERER_TEXTURE_ADDRESS_MODE_WRAP,
        /// Clamp values to the 0 to 1 range.
        RENDERER_TEXTURE_ADDRESS_MODE_CLAMP,

        RENDERER_TEXTURE_ADDRESS_MODE_MAX,
        RENDERER_TEXTURE_ADDRESS_MODE_LAST = RENDERER_TEXTURE_ADDRESS_MODE_MAX - 1
    };

    /// Resource buffer (vertex buffer, index buffer, texture, etc.) usage.
    enum ERendererBufferUsage
    {
        RENDERER_BUFFER_USAGE_FIRST   =  0,
        RENDERER_BUFFER_USAGE_INVALID = -1,

        /// Static buffer/texture.
        RENDERER_BUFFER_USAGE_STATIC,
        /// Dynamic buffer/texture.
        RENDERER_BUFFER_USAGE_DYNAMIC,

        /// Render target.
        RENDERER_BUFFER_USAGE_RENDER_TARGET,
        /// Depth-stencil buffer.
        RENDERER_BUFFER_USAGE_DEPTH_STENCIL,

        RENDERER_BUFFER_USAGE_MAX,
        RENDERER_BUFFER_USAGE_LAST = RENDERER_BUFFER_USAGE_MAX - 1
    };

    /// Surface format type.
    enum ERendererSurfaceFormat
    {
        RENDERER_SURFACE_FORMAT_FIRST   =  0,
        RENDERER_SURFACE_FORMAT_INVALID = -1,

        /// Depth-only.
        RENDERER_SURFACE_FORMAT_DEPTH_ONLY,
        /// Depth-stencil (8-bit stencil guaranteed).
        RENDERER_SURFACE_FORMAT_DEPTH_STENCIL,

        RENDERER_SURFACE_FORMAT_MAX,
        RENDERER_SURFACE_FORMAT_LAST = RENDERER_SURFACE_FORMAT_MAX - 1
    };

    /// Index buffer format.
    enum ERendererIndexFormat
    {
        RENDERER_INDEX_FORMAT_FIRST   =  0,
        RENDERER_INDEX_FORMAT_INVALID = -1,

        /// Unsigned 16-bit integer.
        RENDERER_INDEX_FORMAT_UINT16,
        /// Unsigned 32-bit integer.
        RENDERER_INDEX_FORMAT_UINT32,

        RENDERER_INDEX_FORMAT_MAX,
        RENDERER_INDEX_FORMAT_LAST = RENDERER_INDEX_FORMAT_MAX - 1
    };

    /// Vertex input semantic.
    enum ERendererVertexSemantic
    {
        RENDERER_VERTEX_SEMANTIC_FIRST   =  0,
        RENDERER_VERTEX_SEMANTIC_INVALID = -1,

        /// Position.
        RENDERER_VERTEX_SEMANTIC_POSITION,
        /// Blend weights.
        RENDERER_VERTEX_SEMANTIC_BLENDWEIGHT,
        /// Blend indices.
        RENDERER_VERTEX_SEMANTIC_BLENDINDICES,
        /// Normal.
        RENDERER_VERTEX_SEMANTIC_NORMAL,
        /// Point size.
        RENDERER_VERTEX_SEMANTIC_PSIZE,
        /// Texture coordinates.
        RENDERER_VERTEX_SEMANTIC_TEXCOORD,
        /// Tangent.
        RENDERER_VERTEX_SEMANTIC_TANGENT,
        /// Binormal.
        RENDERER_VERTEX_SEMANTIC_BINORMAL,
        /// Color.
        RENDERER_VERTEX_SEMANTIC_COLOR,

        RENDERER_VERTEX_SEMANTIC_MAX,
        RENDERER_VERTEX_SEMANTIC_LAST = RENDERER_VERTEX_SEMANTIC_MAX - 1
    };

    /// Vertex input data type.
    enum ERendererVertexDataType
    {
        RENDERER_VERTEX_DATA_TYPE_FIRST   =  0,
        RENDERER_VERTEX_DATA_TYPE_INVALID = -1,

        /// 2-component, single-precision float.
        RENDERER_VERTEX_DATA_TYPE_FLOAT32_2,
        /// 3-component, single-precision float.
        RENDERER_VERTEX_DATA_TYPE_FLOAT32_3,
        /// 4-component, single-precision float.
        RENDERER_VERTEX_DATA_TYPE_FLOAT32_4,
        /// 4-component, unsigned byte, normalized by dividing by 255.
        RENDERER_VERTEX_DATA_TYPE_UINT8_4_NORM,
        /// 4-component, unsigned byte, not normalized.
        RENDERER_VERTEX_DATA_TYPE_UINT8_4,
        /// 2-component, half-precision float.
        RENDERER_VERTEX_DATA_TYPE_FLOAT16_2,
        /// 4-component, half-precision float.
        RENDERER_VERTEX_DATA_TYPE_FLOAT16_4,

        RENDERER_VERTEX_DATA_TYPE_MAX,
        RENDERER_VERTEX_DATA_TYPE_LAST = RENDERER_VERTEX_DATA_TYPE_MAX - 1
    };

    /// Surface pixel formats.
    ///
    /// When adding, removing, or modifying formats, make sure to update the PixelUtil functions as well as the texture
    /// creation functions (Renderer::CreateTexture2d()) as appropriate.
    enum ERendererPixelFormat
    {
        RENDERER_PIXEL_FORMAT_FIRST   =  0,
        RENDERER_PIXEL_FORMAT_INVALID = -1,

        /// Uncompressed, 32-bit RGB pixel with alpha (8-bit normalized integer per channel).
        RENDERER_PIXEL_FORMAT_R8G8B8A8,
        /// Uncompressed, 32-bit standard RGB pixel with alpha (8-bit normalized integer per channel).
        RENDERER_PIXEL_FORMAT_R8G8B8A8_SRGB,
        /// Uncompressed, 8-bit single-channel texel (8-bit normalized integer per pixel).
        RENDERER_PIXEL_FORMAT_R8,
        /// BC1/BC1a (DXT1/DXT1a) compressed.
        /// - Compressed RGB with pre-modulated alpha mask (1-bit alpha).
        /// - 4x4 texel compressed blocks.
        /// - 64 bits per compressed block (4 bits per texel within each block).
        RENDERER_PIXEL_FORMAT_BC1,
        /// BC1 (DXT1/DXT1a) compressed in sRGB color space.
        RENDERER_PIXEL_FORMAT_BC1_SRGB,
        /// BC2 (DXT3) compressed.
        /// - Compressed RGB with separate, uncompressed 4-bit alpha channel.
        /// - 4x4 texel compressed blocks.
        /// - 128 bits per compressed block (8 bits per texel within each block).
        RENDERER_PIXEL_FORMAT_BC2,
        /// BC2 (DXT3) compressed in sRGB color space.
        RENDERER_PIXEL_FORMAT_BC2_SRGB,
        /// BC3 (DXT5) compressed.
        /// - Compressed RGB with separate, compressed 8-bit alpha channel.
        /// - 4x4 texel compressed blocks.
        /// - 128 bits per compressed block (8 bits per texel within each block).
        RENDERER_PIXEL_FORMAT_BC3,
        /// BC3 (DXT5) compressed in sRGB color space.
        RENDERER_PIXEL_FORMAT_BC3_SRGB,

        /// Uncompressed, 64-bit floating-point RGB pixel with alpha (16-bit floating-point value per channel).
        RENDERER_PIXEL_FORMAT_R16G16B16A16_FLOAT,

        /// Depth texture format.
        RENDERER_PIXEL_FORMAT_DEPTH,

        RENDERER_PIXEL_FORMAT_MAX,
        RENDERER_PIXEL_FORMAT_LAST = RENDERER_PIXEL_FORMAT_MAX - 1
    };

    /// Color write mask flags.
    enum ERendererColorWriteMaskFlag
    {
        /// Write to the red channel.
        RENDERER_COLOR_WRITE_MASK_FLAG_RED = ( 1 << 0 ),
        /// Write to the green channel.
        RENDERER_COLOR_WRITE_MASK_FLAG_GREEN = ( 1 << 1 ),
        /// Write to the blue channel.
        RENDERER_COLOR_WRITE_MASK_FLAG_BLUE = ( 1 << 2 ),
        /// Write to the alpha channel.
        RENDERER_COLOR_WRITE_MASK_FLAG_ALPHA = ( 1 << 3 ),

        /// Write to all channels.
        RENDERER_COLOR_WRITE_MASK_FLAG_ALL =
            ( RENDERER_COLOR_WRITE_MASK_FLAG_RED |
              RENDERER_COLOR_WRITE_MASK_FLAG_GREEN |
              RENDERER_COLOR_WRITE_MASK_FLAG_BLUE |
              RENDERER_COLOR_WRITE_MASK_FLAG_ALPHA )
    };

    /// Render surface clear flags.
    enum ERendererClearFlag
    {
        /// Clear the render target.
        RENDERER_CLEAR_FLAG_TARGET = ( 1 << 0 ),
        /// Clear the depth buffer.
        RENDERER_CLEAR_FLAG_DEPTH = ( 1 << 1 ),
        /// Clear the stencil buffer.
        RENDERER_CLEAR_FLAG_STENCIL = ( 1 << 2 ),

        /// Clear all render surfaces.
        RENDERER_CLEAR_FLAG_ALL =
            ( RENDERER_CLEAR_FLAG_TARGET | RENDERER_CLEAR_FLAG_DEPTH | RENDERER_CLEAR_FLAG_STENCIL )
    };

    /// Render buffer mapping hints.
    enum ERendererBufferMapHint
    {
        RENDERER_BUFFER_MAP_HINT_FIRST   =  0,
        RENDERER_BUFFER_MAP_HINT_INVALID = -1,

        /// No special considerations.
        RENDERER_BUFFER_MAP_HINT_NONE,
        /// All existing contents will be overwritten.
        RENDERER_BUFFER_MAP_HINT_DISCARD,
        /// Buffer contents that may be in use by the renderer will not be overwritten (caller must still enforce this
        /// as well).
        RENDERER_BUFFER_MAP_HINT_NO_OVERWRITE,

        RENDERER_BUFFER_MAP_HINT_MAX,
        RENDERER_BUFFER_MAP_HINT_LAST = RENDERER_BUFFER_MAP_HINT_MAX - 1
    };

    /// Render primitive types.
    enum ERendererPrimitiveType
    {
        RENDERER_PRIMITIVE_TYPE_FIRST   =  0,
        RENDERER_PRIMITIVE_TYPE_INVALID = -1,

        /// Line list.
        RENDERER_PRIMITIVE_TYPE_LINE_LIST,
        /// Line strip.
        RENDERER_PRIMITIVE_TYPE_LINE_STRIP,
        /// Triangle list.
        RENDERER_PRIMITIVE_TYPE_TRIANGLE_LIST,
        /// Triangle strip.
        RENDERER_PRIMITIVE_TYPE_TRIANGLE_STRIP,

        RENDERER_PRIMITIVE_TYPE_MAX,
        RENDERER_PRIMITIVE_TYPE_LAST = RENDERER_PRIMITIVE_TYPE_MAX - 1
    };
}

#endif  // LUNAR_RENDERING_RENDERER_TYPES_H
