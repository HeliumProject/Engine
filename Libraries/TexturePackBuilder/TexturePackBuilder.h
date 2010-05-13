#pragma once
#include "API.h"

#include "Asset/TexturePackBase.h"

#include "AssetBuilder/BuilderInterface.h"

#include "Dependencies/FileInfo.h"
#include "Dependencies/Dependencies.h"

#include "AssetBuilder/BuilderOptions.h"

#include "TextureProcess/TextureProcess.h"

namespace Symbol
{
  class Enum;
  typedef Nocturnal::SmartPtr<Enum> EnumPtr;
}

namespace AssetBuilder
{
  class TEXPACKBUILDER_API TexturePack : public IBuilder 
  {
  public:
    ///////////////////////////////////////////
    // members

    Asset::TexturePackBasePtr        m_TexturePackAsset;

    TexturePackBuilderOptionsPtr      m_BuilderOptions;

    std::string                  m_HeaderFilename; 
    std::string                  m_TexelFilename;
    std::string                  m_DebugFilename;

    Dependencies::V_DependencyInfo     m_OutputFiles;
    Dependencies::V_DependencyInfo     m_LeafInputFiles;

    TextureProcess::Bank         m_Bank;

    ///////////////////////////////////////////
    // methods

    static void Initialize();
    static void Cleanup();

    TexturePack();
    ~TexturePack();

    virtual AssetBuilder::BuilderOptionsPtr ParseOptions( const V_string& options ) NOC_OVERRIDE;

    virtual Asset::AssetClass* GetAssetClass() NOC_OVERRIDE;
    virtual const std::string GetBuildString() NOC_OVERRIDE;

    virtual void Initialize( const Asset::AssetClassPtr& assetClass, BuilderOptionsPtr options = NULL ) NOC_OVERRIDE;
    virtual void GatherJobs( V_BuildJob& jobs ) NOC_OVERRIDE;
    virtual bool IsUpToDate() NOC_OVERRIDE;
    virtual void RegisterInputs( Dependencies::V_DependencyInfo& outputFiles ) NOC_OVERRIDE;

    virtual bool Build() NOC_OVERRIDE;

  protected:
    void ProcessTexturePack();
  };
}