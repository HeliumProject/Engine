namespace Helium
{
    /// Constructor.
    RVertexDescription::Element::Element()
        : type( RENDERER_VERTEX_DATA_TYPE_FIRST )
        , semantic( RENDERER_VERTEX_SEMANTIC_FIRST )
        , semanticIndex( 0 )
        , bufferIndex( 0 )
    {
    }
}
