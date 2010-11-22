//----------------------------------------------------------------------------------------------------------------------
// VertexTypes.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Serialize this vertex.
    ///
    /// @param[in] s  Serializer with which to serialize.
    template< size_t TexCoordSetCount >
    void StaticMeshVertex< TexCoordSetCount >::Serialize( Serializer& s )
    {
        s << Serializer::WrapArray( position );
        s << Serializer::WrapArray( normal );
        s << Serializer::WrapArray( tangent );
        s << color;

        for( size_t texCoordSetIndex = 0; texCoordSetIndex < HELIUM_ARRAY_COUNT( texCoords ); ++texCoordSetIndex )
        {
            s << Serializer::WrapArray( texCoords[ texCoordSetIndex ] );
        }
    }

    /// Serialize this vertex.
    ///
    /// @param[in] s  Serializer with which to serialize.
    void SkinnedMeshVertex::Serialize( Serializer& s )
    {
        s << Serializer::WrapArray( position );
        s << Serializer::WrapArray( blendWeights );
        s << Serializer::WrapArray( blendIndices );
        s << Serializer::WrapArray( normal );
        s << Serializer::WrapArray( tangent );
        s << Serializer::WrapArray( texCoords );
    }
}
