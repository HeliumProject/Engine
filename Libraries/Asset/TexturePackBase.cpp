#include "TexturePackBase.h"
#include "Finder/ExtensionSpecs.h"
#include "Finder/TexturePackSpecs.h"

using namespace Asset;

REFLECT_DEFINE_ABSTRACT( TexturePackBase );

void TexturePackBase::EnumerateClass( Reflect::Compositor<TexturePackBase>& comp )
{
  comp.GetComposite().m_UIName = "Texture Pack";
}


const Finder::FinderSpec& TextureEntryBase::s_FileFilter = FinderSpecs::Extension::TEXTUREMAP_FILTER;

const Finder::FileSpec& TexturePackBase::GetHeaderFileSpec() const
{
  return FinderSpecs::TexturePack::HEADER_FILE;
}

const Finder::FileSpec& TexturePackBase::GetTexelFileSpec() const
{
  return FinderSpecs::TexturePack::TEXEL_FILE;
}

const Finder::FileSpec& TexturePackBase::GetCombinedFileSpec() const
{
  return FinderSpecs::TexturePack::COMBINED_FILE;
}

bool TexturePackBase::UseCombinedOutputFile() const
{
  return false;
}

bool TexturePackBase::IsBuildable() const
{
  return true;
}