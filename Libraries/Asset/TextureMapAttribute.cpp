#include "TextureMapAttribute.h"
#include "StandardShaderAsset.h"

#include "Finder/ExtensionSpecs.h"

using namespace Asset;

REFLECT_DEFINE_ABSTRACT( TextureMapAttribute );

void TextureMapAttribute::EnumerateClass( Reflect::Compositor<TextureMapAttribute>& comp )
{
  comp.GetComposite().m_UIName = "Texture Map";

  //Reflect::Field* fieldFileID = comp.AddField( &TextureMapAttribute::m_FileID, "m_FileID", Reflect::FieldFlags::FileID | AssetFlags::RealTimeUpdateable | Asset::AssetFlags::ManageField );
  //fieldFileID->SetProperty( Asset::AssetProperties::FilterSpec, s_FileFilter.GetName() );

  Reflect::EnumerationField* enumReductionRatio = comp.AddEnumerationField( &TextureMapAttribute::m_ReductionRatio, "m_ReductionRatio", AssetFlags::RealTimeUpdateable );
  Reflect::EnumerationField* enumMipGenFilter = comp.AddEnumerationField( &TextureMapAttribute::m_MipGenFilter, "m_MipGenFilter", AssetFlags::RealTimeUpdateable );
  Reflect::EnumerationField* enumPostMipFilter = comp.AddEnumerationField( &TextureMapAttribute::m_PostMipFilter, "m_PostMipFilter", AssetFlags::RealTimeUpdateable );
  Reflect::EnumerationField* enumTexFilter = comp.AddEnumerationField( &TextureMapAttribute::m_TexFilter, "m_TexFilter", AssetFlags::RealTimeUpdateable );

  Reflect::Field* fieldMipBias = comp.AddField( &TextureMapAttribute::m_MipBias, "m_MipBias", AssetFlags::RealTimeUpdateable );

  Reflect::Field* fieldMipFilterPasses = comp.AddField( &TextureMapAttribute::m_MipFilterPasses, "m_MipFilterPasses", Reflect::FieldFlags::Hide );
}


const Finder::FinderSpec& TextureMapAttribute::s_FileFilter = FinderSpecs::Extension::TEXTUREMAP_FILTER;

Attribute::AttributeCategoryType TextureMapAttribute::GetCategoryType() const
{
  return Attribute::AttributeCategoryTypes::File;
}

bool TextureMapAttribute::ShouldRebuildTexture( const TextureMapAttribute* oldAttrib ) const
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

//void TextureMapAttribute::SetFileID( const tuid& fileID )
//{
//  if ( m_FileID != fileID )
//  {
//    m_FileID = fileID;
//    RaiseChanged( GetClass()->FindField( &TextureMapAttribute::m_FileID ) );
//  }
//}

const Finder::FinderSpec* TextureMapAttribute::GetFileFilter() const
{
  return &s_FileFilter;
}

bool TextureMapAttribute::IsTextureDirty() const
{
  return m_IsTextureDirty;
}

void TextureMapAttribute::SetTextureDirty( bool dirty )
{
  m_IsTextureDirty = dirty;
}