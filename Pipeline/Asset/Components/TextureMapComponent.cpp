#include "TextureMapComponent.h"
#include "Pipeline/Asset/Classes/StandardShaderAsset.h"

using namespace Asset;

REFLECT_DEFINE_ABSTRACT( TextureMapComponent );

void TextureMapComponent::EnumerateClass( Reflect::Compositor<TextureMapComponent>& comp )
{
  comp.GetComposite().m_UIName = "Texture Map";

  Reflect::EnumerationField* enumReductionRatio = comp.AddEnumerationField( &TextureMapComponent::m_ReductionRatio, "m_ReductionRatio", AssetFlags::RealTimeUpdateable );
  Reflect::EnumerationField* enumMipGenFilter = comp.AddEnumerationField( &TextureMapComponent::m_MipGenFilter, "m_MipGenFilter", AssetFlags::RealTimeUpdateable );
  Reflect::EnumerationField* enumPostMipFilter = comp.AddEnumerationField( &TextureMapComponent::m_PostMipFilter, "m_PostMipFilter", AssetFlags::RealTimeUpdateable );
  Reflect::EnumerationField* enumTexFilter = comp.AddEnumerationField( &TextureMapComponent::m_TexFilter, "m_TexFilter", AssetFlags::RealTimeUpdateable );

  Reflect::Field* fieldMipBias = comp.AddField( &TextureMapComponent::m_MipBias, "m_MipBias", AssetFlags::RealTimeUpdateable );

  Reflect::Field* fieldMipFilterPasses = comp.AddField( &TextureMapComponent::m_MipFilterPasses, "m_MipFilterPasses", Reflect::FieldFlags::Hide );
}


const std::string TextureMapComponent::s_FileFilter = "*.tga;*.png;*.bmp";

Component::ComponentCategoryType TextureMapComponent::GetCategoryType() const
{
  return Component::ComponentCategoryTypes::File;
}

bool TextureMapComponent::ShouldRebuildTexture( const TextureMapComponent* oldAttrib ) const
{
  if ( !oldAttrib )
  {
    return true;
  }

  if ( IsTextureDirty() )
  {
    return true;
  }

  return ( m_Path != oldAttrib->GetPath()
    || m_ReductionRatio != oldAttrib->m_ReductionRatio
    || m_MipGenFilter != oldAttrib->m_MipGenFilter
    || m_PostMipFilter != oldAttrib->m_PostMipFilter 
    || m_MipBias != oldAttrib->m_MipBias );
}

//void TextureMapComponent::SetFileID( const tuid& fileID )
//{
//  if ( m_FileID != fileID )
//  {
//    m_FileID = fileID;
//    RaiseChanged( GetClass()->FindField( &TextureMapComponent::m_FileID ) );
//  }
//}

const std::string& TextureMapComponent::GetFileFilter() const
{
  return s_FileFilter;
}

bool TextureMapComponent::IsTextureDirty() const
{
  return m_IsTextureDirty;
}

void TextureMapComponent::SetTextureDirty( bool dirty )
{
  m_IsTextureDirty = dirty;
}