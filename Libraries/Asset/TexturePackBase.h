#pragma once

#include "API.h"
#include "AssetClass.h"

#include "TextureProcess/TextureProcess.h"

namespace Finder
{
  class FinderSpec;
  class FileSpec;
}

namespace Asset
{
  class ASSET_API TextureEntryBase NOC_ABSTRACT : public Reflect::Element
  {
  protected:
    static const Finder::FinderSpec& s_FileFilter;

  public:

    virtual void ToDefinition(const TextureProcess::DefinitionPtr& definition) const = 0;
    virtual std::string GetFilePath() const = 0;

    REFLECT_DECLARE_ABSTRACT(TextureEntryBase, Reflect::Element);

    static void EnumerateClass( Reflect::Compositor<TextureEntryBase>& comp )
    {
    }
  };
  typedef Nocturnal::SmartPtr<TextureEntryBase> TextureEntryBasePtr;
  typedef std::vector<TextureEntryBasePtr> V_TextureEntryBase;

  // Base class for all texture pack types
  class ASSET_API TexturePackBase NOC_ABSTRACT : public AssetClass
  {
  public:

    TexturePackBase()
    {
    }

    //
    // There is an important abstraction here that allows different texture pack types to have
    // different format versions.  otherwise a logic change that could only require a small
    // subset of the texture packs to rebuild would force *all* the texture packs to rebuild
    //
    virtual const Finder::FileSpec& GetHeaderFileSpec() const;
    virtual const Finder::FileSpec& GetTexelFileSpec() const;
    virtual const Finder::FileSpec& GetCombinedFileSpec() const;

    virtual void GetTextures( V_TextureEntryBase& textures ) = 0;
    virtual std::string GetEnumName() { return ""; }

    virtual bool UseCombinedOutputFile() const;

    virtual bool IsBuildable() const NOC_OVERRIDE;

    REFLECT_DECLARE_ABSTRACT( TexturePackBase, AssetClass );

    static void EnumerateClass( Reflect::Compositor<TexturePackBase>& comp );
  };

  typedef Nocturnal::SmartPtr< TexturePackBase > TexturePackBasePtr;
  typedef std::vector< TexturePackBasePtr > V_TexturePackBase;
}