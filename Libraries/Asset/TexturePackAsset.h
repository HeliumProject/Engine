#pragma once

#include "API.h"
#include "TextureEnums.h"
#include "TexturePackBase.h"


namespace Finder
{
  class FileSpec;
}

namespace Asset
{
  class ASSET_API StandardTextureEntry : public TextureEntryBase
  {
  public:
    // name of the enumeration value (optional)
    std::string m_EnumerationName;

    // the texture file path to the data
    tuid m_TextureFile;

    // the output color format for the packer
    TextureColorFormat m_TextureFormat;

    // scaling options for the packer
    float m_RelativeScaleX;
    float m_RelativeScaleY;
    bool m_ForcePowerOf2;

    // how to setup the runtime register values
    TextureWrapMode m_WrapU;
    TextureWrapMode m_WrapV;
    TextureWrapMode m_WrapW;

    // not serialized
    mutable std::string m_Title;

    StandardTextureEntry()
      : m_TextureFormat ( TextureColorFormats::DXT5 )
      , m_TextureFile( TUID::Null )
      , m_RelativeScaleX (1.f)
      , m_RelativeScaleY (1.f)
      , m_ForcePowerOf2 (false)
      , m_WrapU (TextureWrapModes::Clamp)
      , m_WrapV (TextureWrapModes::Clamp)
      , m_WrapW (TextureWrapModes::Clamp)
    {

    }

    REFLECT_DECLARE_CLASS(StandardTextureEntry, TextureEntryBase);

    static void EnumerateClass( Reflect::Compositor<StandardTextureEntry>& comp );

    virtual void ToDefinition(const TextureProcess::DefinitionPtr& definition) const NOC_OVERRIDE;
    virtual const std::string& GetTitle() const NOC_OVERRIDE;

    virtual std::string GetFilePath() const NOC_OVERRIDE;
  };

  typedef Nocturnal::SmartPtr<StandardTextureEntry> StandardTextureEntryPtr;
  typedef std::vector<StandardTextureEntryPtr> V_StandardTextureEntry;

  // Base class for all shader types
  class ASSET_API TexturePackAsset : public TexturePackBase
  {
  public:
    std::string m_EnumerationName;
    V_StandardTextureEntry m_Textures;

    TexturePackAsset()
    {
    }

    virtual void GetTextures( V_TextureEntryBase& textures ) NOC_OVERRIDE;
    virtual std::string GetEnumName() NOC_OVERRIDE { return m_EnumerationName; }

    REFLECT_DECLARE_CLASS( TexturePackAsset, TexturePackBase );

    static void EnumerateClass( Reflect::Compositor<TexturePackAsset>& comp );
  };

  typedef Nocturnal::SmartPtr< TexturePackAsset > TexturePackAssetPtr;
  typedef std::vector< TexturePackAssetPtr > V_TexturePackAsset;
}