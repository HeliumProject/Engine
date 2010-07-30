#include "TextureProcessingComponent.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_CLASS( TextureProcessingComponent );

void TextureProcessingComponent::EnumerateClass( Reflect::Compositor< TextureProcessingComponent >& comp )
{
  comp.GetComposite().m_UIName = TXT( "Texture Processing" );

  comp.AddField( &TextureProcessingComponent::m_MaxTextureResolution, "m_MaxTextureResolution" );
  comp.AddField( &TextureProcessingComponent::m_TextureDataFormat, "m_TextureDataFormat" );
  comp.AddField( &TextureProcessingComponent::m_GenerateMipMaps, "m_GenerateMipMaps" );
  comp.AddField( &TextureProcessingComponent::m_GenerateBumpMap, "m_GenerateBumpMap" );
}

Component::ComponentUsage TextureProcessingComponent::GetComponentUsage() const
{
    return Component::ComponentUsages::Class;
}


