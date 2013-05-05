//----------------------------------------------------------------------------------------------------------------------
// VertexTypes.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_GRAPHICS_TYPES_VERTEX_TYPES_H
#define HELIUM_GRAPHICS_TYPES_VERTEX_TYPES_H

#include "GraphicsTypes/GraphicsTypes.h"

#include "Math/Float16.h"
#include "MathSimd/Vector2.h"
#include "MathSimd/Vector3.h"
#include "Rendering/Color.h"

namespace Helium
{
    /// Maximum number of bones influencing vertices per skinned mesh (must match the same constant in
    /// Data/Shaders/Common.inl).
    static const size_t BONE_COUNT_MAX = 75;

    /// Simple vertex type (position and color only).
    struct HELIUM_GRAPHICS_TYPES_API SimpleVertex
    {
        /// Position.
        float32_t position[ 3 ];
        /// Color.
        uint8_t color[ 4 ];

        /// @name Construction/Destruction
        //@{
        inline SimpleVertex();
        inline SimpleVertex(
            float32_t positionX, float32_t positionY, float32_t positionZ, uint8_t colorRed = 0xff,
            uint8_t colorGreen = 0xff, uint8_t colorBlue = 0xff, uint8_t colorAlpha = 0xff );
        inline SimpleVertex( const Simd::Vector3& rPosition, const Color& rColor = Color( 0xffffffff ) );
        //@}

        /// @name Serialization
        //@{
        //inline void Serialize( Serializer& s );
        //@}
    };

    /// Simple textured vertex type (position, color, and single texture coordinate set only).
    struct HELIUM_GRAPHICS_TYPES_API SimpleTexturedVertex
    {
        /// Position.
        float32_t position[ 3 ];
        /// Color.
        uint8_t color[ 4 ];
        /// Texture coordinates.
        Float16 texCoords[ 2 ];

        /// @name Construction/Destruction
        //@{
        inline SimpleTexturedVertex();
        inline SimpleTexturedVertex(
            float32_t positionX, float32_t positionY, float32_t positionZ, Float16 texCoordU, Float16 texCoordV,
            uint8_t colorRed = 0xff, uint8_t colorGreen = 0xff, uint8_t colorBlue = 0xff, uint8_t colorAlpha = 0xff );
        inline SimpleTexturedVertex(
            const Simd::Vector3& rPosition, const Simd::Vector2& rTexCoords,
            const Color& rColor = Color( 0xffffffff ) );
        //@}

        /// @name Serialization
        //@{
        //inline void Serialize( Serializer& s );
        //@}
    };

    /// Vertex with position values specified as 2D screen coordinates.
    struct HELIUM_GRAPHICS_TYPES_API ScreenVertex
    {
        /// Position.
        float32_t position[ 2 ];
        /// Color.
        uint8_t color[ 4 ];
        /// Texture coordinates.
        Float16 texCoords[ 2 ];

        /// @name Construction/Destruction
        //@{
        inline ScreenVertex();
        inline ScreenVertex(
            float32_t positionX, float32_t positionY, Float16 texCoordU, Float16 texCoordV, uint8_t colorRed = 0xff,
            uint8_t colorGreen = 0xff, uint8_t colorBlue = 0xff, uint8_t colorAlpha = 0xff );
        inline ScreenVertex(
            const Simd::Vector2& rPosition, const Simd::Vector2& rTexCoords,
            const Color& rColor = Color( 0xffffffff ) );
        //@}

        /// @name Serialization
        //@{
        //inline void Serialize( Serializer& s );
        //@}
    };

    /// Vertex with world position and screen coordinate offset values.
    struct HELIUM_GRAPHICS_TYPES_API ProjectedVertex
    {
        /// World position.
        float32_t position[ 3 ];
        /// Color.
        uint8_t color[ 4 ];
        /// Texture coordinates.
        Float16 texCoords[ 2 ];
        /// Screen offset.
        float32_t screenOffset[ 2 ];

        /// @name Construction/Destruction
        //@{
        inline ProjectedVertex();
        inline ProjectedVertex(
            float32_t positionX, float32_t positionY, float32_t positionZ, float32_t screenOffsetX,
            float32_t screenOffsetY, Float16 texCoordU, Float16 texCoordV, uint8_t colorRed = 0xff,
            uint8_t colorGreen = 0xff, uint8_t colorBlue = 0xff, uint8_t colorAlpha = 0xff );
        inline ProjectedVertex(
            const Simd::Vector3& rPosition, const Simd::Vector2& rScreenOffset, const Simd::Vector2& rTexCoords,
            const Color& rColor = Color( 0xffffffff ) );
        //@}

        /// @name Serialization
        //@{
        //inline void Serialize( Serializer& s );
        //@}
    };

    /// Basic static mesh vertex type.
    template< size_t TexCoordSetCount >
    struct StaticMeshVertex
    {
        /// Position.
        float32_t position[ 3 ];
        /// Normal.
        uint8_t normal[ 4 ];
        /// Tangent.
        uint8_t tangent[ 4 ];
        /// Color.
        uint8_t color[ 4 ];
        /// Texture coordinates.
        Float16 texCoords[ TexCoordSetCount ][ 2 ];

        /// @name Serialization
        //@{
        //void Serialize( Serializer& s );
        //@}
    };

    /// Skinned mesh vertex type.
    ///
    /// Note that no vertex coloring and only one texture coordinate set are supported.  This is done in order to
    /// maintain a size no greater than 32 bytes.
    struct HELIUM_GRAPHICS_TYPES_API SkinnedMeshVertex
    {
        /// Position.
        float32_t position[ 3 ];
        /// Blend weights.
        uint8_t blendWeights[ 4 ];
        /// Blend indices.
        uint8_t blendIndices[ 4 ];
        /// Normal.
        uint8_t normal[ 4 ];
        /// Tangent.
        uint8_t tangent[ 4 ];
        /// Texture coordinates.
        Float16 texCoords[ 2 ];

        /// @name Serialization
        //@{
        //inline void Serialize( Serializer& s );
        //@}
    };
}

#include "GraphicsTypes/VertexTypes.inl"

#endif  // HELIUM_GRAPHICS_TYPES_VERTEX_TYPES_H
