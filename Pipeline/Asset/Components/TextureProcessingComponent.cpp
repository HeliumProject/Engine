#include "TextureProcessingComponent.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_ENUMERATION( TextureDataFormat );
REFLECT_DEFINE_OBJECT( TextureProcessingComponent );

void TextureProcessingComponent::AcceptCompositeVisitor( Reflect::Composite& comp )
{
  comp.AddField( &TextureProcessingComponent::m_MaxTextureResolution, TXT( "m_MaxTextureResolution" ) );
  comp.AddEnumerationField( &TextureProcessingComponent::m_TextureDataFormat, TXT( "m_TextureDataFormat" ) );
  comp.AddField( &TextureProcessingComponent::m_GenerateMipMaps, TXT( "m_GenerateMipMaps" ) );
  comp.AddField( &TextureProcessingComponent::m_GenerateBumpMap, TXT( "m_GenerateBumpMap" ) );
}

Component::ComponentUsage TextureProcessingComponent::GetComponentUsage() const
{
    return Component::ComponentUsages::Class;
}
