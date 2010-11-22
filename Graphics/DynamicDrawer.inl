//----------------------------------------------------------------------------------------------------------------------
// DynamicDrawer.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Constructor.
    DynamicDrawer::Vertex::Vertex()
    {
    }

    /// Constructor.
    ///
    /// @param[in] positionX    Vertex position x-coordinate.
    /// @param[in] positionY    Vertex position y-coordinate.
    /// @param[in] positionZ    Vertex position z-coordinate.
    /// @param[in] packedColor  Packed vertex color value.
    DynamicDrawer::Vertex::Vertex( float32_t positionX, float32_t positionY, float32_t positionZ, uint32_t packedColor )
    {
        position[ 0 ] = positionX;
        position[ 1 ] = positionY;
        position[ 2 ] = positionZ;
        color = packedColor;
    }

    /// Constructor.
    ///
    /// @param[in] rPosition  Vertex position.
    /// @param[in] rColor     Vertex color.
    DynamicDrawer::Vertex::Vertex( const Simd::Vector3& rPosition, const Color& rColor )
    {
        position[ 0 ] = rPosition.GetElement( 0 );
        position[ 1 ] = rPosition.GetElement( 1 );
        position[ 2 ] = rPosition.GetElement( 2 );
        color = rColor.GetArgb();
    }

    /// Constructor.
    DynamicDrawer::VertexTextured::VertexTextured()
    {
    }

    /// Constructor.
    ///
    /// @param[in] positionX    Vertex position x-coordinate.
    /// @param[in] positionY    Vertex position y-coordinate.
    /// @param[in] positionZ    Vertex position z-coordinate.
    /// @param[in] texCoordU    Vertex texture u-coordinate.
    /// @param[in] texCoordV    Vertex texture v-coordinate.
    /// @param[in] packedColor  Packed vertex color value.
    DynamicDrawer::VertexTextured::VertexTextured(
        float32_t positionX,
        float32_t positionY,
        float32_t positionZ,
        Float16 texCoordU,
        Float16 texCoordV,
        uint32_t packedColor )
    {
        position[ 0 ] = positionX;
        position[ 1 ] = positionY;
        position[ 2 ] = positionZ;
        color = packedColor;
        texCoords[ 0 ] = texCoordU;
        texCoords[ 1 ] = texCoordV;
    }

    /// Constructor.
    ///
    /// @param[in] rPosition   Vertex position.
    /// @param[in] rTexCoords  Vertex texture coordinates.
    /// @param[in] rColor      Vertex color.
    DynamicDrawer::VertexTextured::VertexTextured(
        const Simd::Vector3& rPosition,
        const Simd::Vector2& rTexCoords,
        const Color& rColor )
    {
        position[ 0 ] = rPosition.GetElement( 0 );
        position[ 1 ] = rPosition.GetElement( 1 );
        position[ 2 ] = rPosition.GetElement( 2 );
        color = rColor.GetArgb();

        Float32 floatPacker;
        floatPacker.value = rTexCoords.GetElement( 0 );
        texCoords[ 0 ] = Float32To16( floatPacker );
        floatPacker.value = rTexCoords.GetElement( 1 );
        texCoords[ 1 ] = Float32To16( floatPacker );
    }
}
