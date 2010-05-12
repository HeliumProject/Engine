#include "TextureEntry.h"

using namespace Asset;

REFLECT_DEFINE_CLASS(TextureEntry);

void TextureEntry::EnumerateClass( Reflect::Compositor<TextureEntry>& comp )
{
  comp.GetComposite().m_UIName = "Texture Entry";

  Reflect::Field* fieldEnumerationName = comp.AddField( &TextureEntry::m_EnumerationName, "m_EnumerationName", Reflect::FieldFlags::Hide );

  Reflect::Field* fieldTextureFile = comp.AddField( &TextureEntry::m_TextureFile, "m_TextureFile", Reflect::FieldFlags::FilePath );
  fieldTextureFile->m_UIName = "Texture File";

  Reflect::EnumerationField* enumTextureFormat = comp.AddEnumerationField( &TextureEntry::m_TextureFormat, "m_TextureFormat" );
  enumTextureFormat->m_UIName = "Output Format";

  Reflect::Field* fieldRelativeScaleX = comp.AddField( &TextureEntry::m_RelativeScaleX, "m_RelativeScaleX" );
  fieldRelativeScaleX->m_UIName = "Relative Scale X";

  Reflect::Field* fieldRelativeScaleY = comp.AddField( &TextureEntry::m_RelativeScaleY, "m_RelativeScaleY" );
  fieldRelativeScaleY->m_UIName = "Relative Scale Y";

  Reflect::Field* fieldForcePowerOf2 = comp.AddField( &TextureEntry::m_ForcePowerOf2, "m_ForcePowerOf2" );
  fieldForcePowerOf2->m_UIName = "Force Power of 2";

  Reflect::EnumerationField* enumWrapU = comp.AddEnumerationField( &TextureEntry::m_WrapU, "m_WrapU" );
  enumWrapU->m_UIName = "Wrap U";

  Reflect::EnumerationField* enumWrapV = comp.AddEnumerationField( &TextureEntry::m_WrapV, "m_WrapV" );
  enumWrapV->m_UIName = "Wrap V";

  Reflect::EnumerationField* enumWrapW = comp.AddEnumerationField( &TextureEntry::m_WrapW, "m_WrapW" );
  enumWrapW->m_UIName = "Wrap W";
}