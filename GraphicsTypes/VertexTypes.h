//----------------------------------------------------------------------------------------------------------------------
// VertexTypes.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_GRAPHICS_TYPES_VERTEX_TYPES_H
#define LUNAR_GRAPHICS_TYPES_VERTEX_TYPES_H

#include "GraphicsTypes/GraphicsTypes.h"

#include "Platform/Math/Float16.h"
#include "Engine/Serializer.h"

namespace Lunar
{
    /// Maximum number of bones influencing vertices per skinned mesh (must match the same constant in
    /// Data/Shaders/Common.inl).
    static const size_t BONE_COUNT_MAX = 75;

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
        /// Packed color (see Color class for more information).
        uint32_t color;
        /// Texture coordinates.
        Float16 texCoords[ TexCoordSetCount ][ 2 ];

        /// @name Serialization
        //@{
        void Serialize( Serializer& s );
        //@}
    };

    /// Skinned mesh vertex type.
    ///
    /// Note that no vertex coloring and only one texture coordinate set are supported.  This is done in order to
    /// maintain a size no greater than 32 bytes.
    struct LUNAR_GRAPHICS_TYPES_API SkinnedMeshVertex
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
        inline void Serialize( Serializer& s );
        //@}
    };
}

#include "GraphicsTypes/VertexTypes.inl"

#endif  // LUNAR_GRAPHICS_TYPES_VERTEX_TYPES_H
