#include "TexturePackAsset.h"

#include "File/Manager.h"
#include "Finder/AssetSpecs.h"
#include "Finder/TexturePackSpecs.h"

using namespace Asset;

REFLECT_DEFINE_CLASS( TexturePackAsset );

void TexturePackAsset::EnumerateClass( Reflect::Compositor<TexturePackAsset>& comp )
{
  comp.GetComposite().m_UIName = "Texture Pack";
  comp.GetComposite().SetProperty( AssetProperties::ModifierSpec, FinderSpecs::Asset::TEXTUREPACK_DECORATION.GetName() );
  comp.GetComposite().SetProperty( AssetProperties::RootFolderSpec, FinderSpecs::Asset::TEXTUREPACK_FOLDER.GetName() );
  comp.GetComposite().SetProperty( AssetProperties::SmallIcon, "enginetype_texturepack_16.png" );

  Reflect::Field* fieldEnumerationName = comp.AddField( &TexturePackAsset::m_EnumerationName, "m_EnumerationName" );
  fieldEnumerationName->m_UIName = "Enumeration Name (Optional)";

  Reflect::Field* fieldTextures = comp.AddField( &TexturePackAsset::m_Textures, "m_Textures", Asset::AssetFlags::Hierarchy | Reflect::FieldFlags::Hide );
  fieldTextures->m_UIName = "Textures";
  fieldTextures->m_Create = &Reflect::CreateObject< StandardTextureEntry >;
  fieldTextures->SetProperty( AssetProperties::SmallIcon, "texture_16.png" );
}

REFLECT_DEFINE_CLASS( StandardTextureEntry );

void StandardTextureEntry::EnumerateClass( Reflect::Compositor<StandardTextureEntry>& comp )
{
  comp.GetComposite().m_UIName = "Texture Entry";

  Reflect::Field* fieldEnumerationName = comp.AddField( &StandardTextureEntry::m_EnumerationName, "m_EnumerationName" );
  fieldEnumerationName->m_UIName = "Enumeration Name";

  Reflect::Field* fieldTextureFile = comp.AddField( &StandardTextureEntry::m_TextureFile, "m_TextureFile", Reflect::FieldFlags::FileID );
  fieldTextureFile->m_UIName = "Texture File";
  fieldTextureFile->SetProperty( Asset::AssetProperties::FilterSpec, s_FileFilter.GetName() );

  Reflect::EnumerationField* enumTextureFormat = comp.AddEnumerationField( &StandardTextureEntry::m_TextureFormat, "m_TextureFormat" );
  enumTextureFormat->m_UIName = "Output Format";

  Reflect::Field* fieldRelativeScaleX = comp.AddField( &StandardTextureEntry::m_RelativeScaleX, "m_RelativeScaleX" );
  fieldRelativeScaleX->m_UIName = "Relative Scale X";

  Reflect::Field* fieldRelativeScaleY = comp.AddField( &StandardTextureEntry::m_RelativeScaleY, "m_RelativeScaleY" );
  fieldRelativeScaleY->m_UIName = "Relative Scale Y";

  Reflect::Field* fieldForcePowerOf2 = comp.AddField( &StandardTextureEntry::m_ForcePowerOf2, "m_ForcePowerOf2" );
  fieldForcePowerOf2->m_UIName = "Force Power of 2";

  Reflect::EnumerationField* enumWrapU = comp.AddEnumerationField( &StandardTextureEntry::m_WrapU, "m_WrapU" );
  enumWrapU->m_UIName = "Wrap U";

  Reflect::EnumerationField* enumWrapV = comp.AddEnumerationField( &StandardTextureEntry::m_WrapV, "m_WrapV" );
  enumWrapV->m_UIName = "Wrap V";

  Reflect::EnumerationField* enumWrapW = comp.AddEnumerationField( &StandardTextureEntry::m_WrapW, "m_WrapW" );
  enumWrapW->m_UIName = "Wrap W";

  Reflect::Field* fieldTitle = comp.AddField( &StandardTextureEntry::m_Title, "m_Title", Reflect::FieldFlags::Discard | Reflect::FieldFlags::Hide );
}


void StandardTextureEntry::ToDefinition( const TextureProcess::DefinitionPtr& definition ) const
{
  definition->m_enum = m_EnumerationName;
  definition->m_texture_file = File::GlobalManager().GetPath( m_TextureFile );
  definition->m_output_format = (IG::OutputColorFormat)m_TextureFormat;
  definition->m_force_power_of_2 = m_ForcePowerOf2;
  definition->m_relscale_x = m_RelativeScaleX;
  definition->m_relscale_y = m_RelativeScaleY;
  definition->m_runtime.m_wrap_u = (IG::UVAddressMode)m_WrapU;
  definition->m_runtime.m_wrap_v = (IG::UVAddressMode)m_WrapV;
  definition->m_runtime.m_wrap_w = (IG::UVAddressMode)m_WrapW;
}

const std::string& StandardTextureEntry::GetTitle() const 
{
  if ( !m_Title.empty() )
  {
    return m_Title;
  }

  if ( m_TextureFile == TUID::Null )
  {
    return __super::GetTitle();
  }

  m_Title = GetFilePath();

  

  return m_Title;
}

std::string StandardTextureEntry::GetFilePath() const
{
  if ( m_TextureFile == TUID::Null )
  {
    return "";
  }

  try
  {
    return File::GlobalManager().GetPath( m_TextureFile );
  }
  catch ( const Nocturnal::Exception& )
  {
    Console::Warning( "Failed to find texture file with id "TUID_HEX_FORMAT"\n", m_TextureFile );
  }

  return "";
}

void TexturePackAsset::GetTextures( V_TextureEntryBase& textures )
{
  textures.insert( textures.end(), m_Textures.begin(), m_Textures.end() );
}
