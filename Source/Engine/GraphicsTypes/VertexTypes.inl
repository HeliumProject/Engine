//PMDTODO: Serialization of vertex types

namespace Helium
{
    /// Constructor.
    SimpleVertex::SimpleVertex()
    {
    }

    /// Constructor.
    ///
    /// @param[in] positionX   Vertex position x-coordinate.
    /// @param[in] positionY   Vertex position y-coordinate.
    /// @param[in] positionZ   Vertex position z-coordinate.
    /// @param[in] colorRed    Red component value of the vertex color.
    /// @param[in] colorGreen  Green component value of the vertex color.
    /// @param[in] colorBlue   Blue component value of the vertex color.
    /// @param[in] colorAlpha  Alpha component value of the vertex color.
    SimpleVertex::SimpleVertex(
        float32_t positionX,
        float32_t positionY,
        float32_t positionZ,
        uint8_t colorRed,
        uint8_t colorGreen,
        uint8_t colorBlue,
        uint8_t colorAlpha )
    {
        position[ 0 ] = positionX;
        position[ 1 ] = positionY;
        position[ 2 ] = positionZ;
        color[ 0 ] = colorRed;
        color[ 1 ] = colorGreen;
        color[ 2 ] = colorBlue;
        color[ 3 ] = colorAlpha;
    }

    /// Constructor.
    ///
    /// @param[in] rPosition  Vertex position.
    /// @param[in] rColor     Vertex color.
    SimpleVertex::SimpleVertex( const Simd::Vector3& rPosition, const Color& rColor )
    {
        position[ 0 ] = rPosition.GetElement( 0 );
        position[ 1 ] = rPosition.GetElement( 1 );
        position[ 2 ] = rPosition.GetElement( 2 );
        color[ 0 ] = rColor.GetR();
        color[ 1 ] = rColor.GetG();
        color[ 2 ] = rColor.GetB();
        color[ 3 ] = rColor.GetA();
    }

    /// Constructor.
    SimpleTexturedVertex::SimpleTexturedVertex()
    {
    }

    /// Constructor.
    ///
    /// @param[in] positionX   Vertex position x-coordinate.
    /// @param[in] positionY   Vertex position y-coordinate.
    /// @param[in] positionZ   Vertex position z-coordinate.
    /// @param[in] texCoordU   Vertex texture u-coordinate.
    /// @param[in] texCoordV   Vertex texture v-coordinate.
    /// @param[in] colorRed    Red component value of the vertex color.
    /// @param[in] colorGreen  Green component value of the vertex color.
    /// @param[in] colorBlue   Blue component value of the vertex color.
    /// @param[in] colorAlpha  Alpha component value of the vertex color.
    SimpleTexturedVertex::SimpleTexturedVertex(
        float32_t positionX,
        float32_t positionY,
        float32_t positionZ,
        Float16 texCoordU,
        Float16 texCoordV,
        uint8_t colorRed,
        uint8_t colorGreen,
        uint8_t colorBlue,
        uint8_t colorAlpha )
    {
        position[ 0 ] = positionX;
        position[ 1 ] = positionY;
        position[ 2 ] = positionZ;
        color[ 0 ] = colorRed;
        color[ 1 ] = colorGreen;
        color[ 2 ] = colorBlue;
        color[ 3 ] = colorAlpha;
        texCoords[ 0 ] = texCoordU;
        texCoords[ 1 ] = texCoordV;
    }

    /// Constructor.
    ///
    /// @param[in] rPosition   Vertex position.
    /// @param[in] rTexCoords  Vertex texture coordinates.
    /// @param[in] rColor      Vertex color.
    SimpleTexturedVertex::SimpleTexturedVertex(
        const Simd::Vector3& rPosition,
        const Simd::Vector2& rTexCoords,
        const Color& rColor )
    {
        position[ 0 ] = rPosition.GetElement( 0 );
        position[ 1 ] = rPosition.GetElement( 1 );
        position[ 2 ] = rPosition.GetElement( 2 );
        color[ 0 ] = rColor.GetR();
        color[ 1 ] = rColor.GetG();
        color[ 2 ] = rColor.GetB();
        color[ 3 ] = rColor.GetA();

        Float32 floatPacker;
        floatPacker.value = rTexCoords.GetElement( 0 );
        texCoords[ 0 ] = Float32To16( floatPacker );
        floatPacker.value = rTexCoords.GetElement( 1 );
        texCoords[ 1 ] = Float32To16( floatPacker );
    }

    /// Constructor.
    ScreenVertex::ScreenVertex()
    {
    }

    /// Constructor.
    ///
    /// @param[in] positionX   Vertex position x-coordinate.
    /// @param[in] positionY   Vertex position y-coordinate.
    /// @param[in] texCoordU   Vertex texture u-coordinate.
    /// @param[in] texCoordV   Vertex texture v-coordinate.
    /// @param[in] colorRed    Red component value of the vertex color.
    /// @param[in] colorGreen  Green component value of the vertex color.
    /// @param[in] colorBlue   Blue component value of the vertex color.
    /// @param[in] colorAlpha  Alpha component value of the vertex color.
    ScreenVertex::ScreenVertex(
        float32_t positionX,
        float32_t positionY,
        Float16 texCoordU,
        Float16 texCoordV,
        uint8_t colorRed,
        uint8_t colorGreen,
        uint8_t colorBlue,
        uint8_t colorAlpha )
    {
        position[ 0 ] = positionX;
        position[ 1 ] = positionY;
        color[ 0 ] = colorRed;
        color[ 1 ] = colorGreen;
        color[ 2 ] = colorBlue;
        color[ 3 ] = colorAlpha;
        texCoords[ 0 ] = texCoordU;
        texCoords[ 1 ] = texCoordV;
    }

    /// Constructor.
    ///
    /// @param[in] rPosition   Vertex position.
    /// @param[in] rTexCoords  Vertex texture coordinates.
    /// @param[in] rColor      Vertex color.
    ScreenVertex::ScreenVertex( const Simd::Vector2& rPosition, const Simd::Vector2& rTexCoords, const Color& rColor )
    {
        position[ 0 ] = rPosition.GetElement( 0 );
        position[ 1 ] = rPosition.GetElement( 1 );
        color[ 0 ] = rColor.GetR();
        color[ 1 ] = rColor.GetG();
        color[ 2 ] = rColor.GetB();
        color[ 3 ] = rColor.GetA();

        Float32 floatPacker;
        floatPacker.value = rTexCoords.GetElement( 0 );
        texCoords[ 0 ] = Float32To16( floatPacker );
        floatPacker.value = rTexCoords.GetElement( 1 );
        texCoords[ 1 ] = Float32To16( floatPacker );
    }

    /// Constructor.
    ProjectedVertex::ProjectedVertex()
    {
    }

    /// Constructor.
    ///
    /// @param[in] positionX      Vertex world position x-coordinate.
    /// @param[in] positionY      Vertex world position y-coordinate.
    /// @param[in] positionZ      Vertex world position z-coordinate.
    /// @param[in] screenOffsetX  Vertex horizontal pixel offset.
    /// @param[in] screenOffsetY  Vertex vertical pixel offset.
    /// @param[in] texCoordU      Vertex texture u-coordinate.
    /// @param[in] texCoordV      Vertex texture v-coordinate.
    /// @param[in] colorRed       Red component value of the vertex color.
    /// @param[in] colorGreen     Green component value of the vertex color.
    /// @param[in] colorBlue      Blue component value of the vertex color.
    /// @param[in] colorAlpha     Alpha component value of the vertex color.
    ProjectedVertex::ProjectedVertex(
        float32_t positionX,
        float32_t positionY,
        float32_t positionZ,
        float32_t screenOffsetX,
        float32_t screenOffsetY,
        Float16 texCoordU,
        Float16 texCoordV,
        uint8_t colorRed,
        uint8_t colorGreen,
        uint8_t colorBlue,
        uint8_t colorAlpha )
    {
        position[ 0 ] = positionX;
        position[ 1 ] = positionY;
        position[ 2 ] = positionZ;
        color[ 0 ] = colorRed;
        color[ 1 ] = colorGreen;
        color[ 2 ] = colorBlue;
        color[ 3 ] = colorAlpha;
        texCoords[ 0 ] = texCoordU;
        texCoords[ 1 ] = texCoordV;
        screenOffset[ 0 ] = screenOffsetX;
        screenOffset[ 1 ] = screenOffsetY;
    }

    /// Constructor.
    ///
    /// @param[in] rPosition      Vertex world position.
    /// @param[in] rScreenOffset  Vertex screen-space pixel offset.
    /// @param[in] rTexCoords     Vertex texture coordinates.
    /// @param[in] rColor         Vertex color.
    ProjectedVertex::ProjectedVertex(
        const Simd::Vector3& rPosition,
        const Simd::Vector2& rScreenOffset,
        const Simd::Vector2& rTexCoords,
        const Color& rColor )
    {
        position[ 0 ] = rPosition.GetElement( 0 );
        position[ 1 ] = rPosition.GetElement( 1 );
        position[ 2 ] = rPosition.GetElement( 2 );
        color[ 0 ] = rColor.GetR();
        color[ 1 ] = rColor.GetG();
        color[ 2 ] = rColor.GetB();
        color[ 3 ] = rColor.GetA();

        Float32 floatPacker;
        floatPacker.value = rTexCoords.GetElement( 0 );
        texCoords[ 0 ] = Float32To16( floatPacker );
        floatPacker.value = rTexCoords.GetElement( 1 );
        texCoords[ 1 ] = Float32To16( floatPacker );

        screenOffset[ 0 ] = rScreenOffset.GetElement( 0 );
        screenOffset[ 1 ] = rScreenOffset.GetElement( 1 );
    }
}
