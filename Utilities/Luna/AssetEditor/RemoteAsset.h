#pragma once

#include "API.h"

#include "Live/RemoteEditor.h"

#include "rpc/interfaces/rpc_lunashader.h"
#include "rpc/interfaces/rpc_lunashader_host.h"
#include "rpc/interfaces/rpc_lunashader_tool.h"
#include "rpc/interfaces/rpc_lunaview.h"
#include "rpc/interfaces/rpc_lunaview_host.h"

#include "math/Matrix4.h"

#include "Asset/FoliageAsset.h"
#include "Asset/StandardShaderAsset.h"
#include "Asset/SkyAsset.h"
#include "Asset/LooseGraphShaderAsset.h"
#include "ShaderProcess/ShaderUtils.h"

#include "igCore/igHeaders/RSXTypes.h"

namespace AssetBuilder
{
  class ISkyBuilder;
}

namespace Luna
{
  class AssetEditor;
  struct ShaderChangedArgs;
  struct SkyLooseGraphShaderChangedArgs;
  struct SkyTextureChangedArgs;
  struct LevelWeatherAttributesChangedArgs;
  struct LevelChangedArgs;
  struct AssetLoadArgs;
  struct SkyChangedArgs;

  struct ShaderAttrTexSettings
  {
    IG::RSXTextureControl1  m_control1[MAX_TEXTURE_SLOTS];
    IG::RSXTextureControl2  m_control2[MAX_TEXTURE_SLOTS];
    IG::RSXTextureFilter    m_filter[MAX_TEXTURE_SLOTS];
  };

  class RemoteAsset : public Luna::RemoteEditor
  {
  public:
    RemoteAsset( AssetEditor* editor );
    virtual ~RemoteAsset();

    void TriggerAssetView(Asset::AssetClass* assetClass, bool showOptions); 

    Luna::AttachmentPtr TriggerMultiMoby(Asset::AssetClass* mainEntityClass, Asset::AssetClass* attachEntityClass); 

    void UpdateShader( const Asset::ShaderAssetPtr& shaderClass, const Asset::ShaderAssetPtr& oldStandardShaderClass, tuid refreshTextureId );
    void UpdateSky( const Asset::SkyAssetPtr& skyClass, AssetBuilder::ISkyBuilder* builder );
    void UpdateSkyLooseGraphShader( const Asset::SkyAssetPtr& skyClass, AssetBuilder::ISkyBuilder* builder, const Asset::LooseGraphShaderAssetPtr& lgs);
    void UpdateSkyTexture( const Asset::SkyAssetPtr& skyClass, AssetBuilder::ISkyBuilder* builder, RPC::SkyTextures::SkyTexture whichTex, const std::string& path );
    void TweakFoliageLod( const Asset::FoliageAsset* foliage );

  protected:

    bool UpdateSingleShaderTexture( const Asset::ShaderAssetPtr& shaderClass,  const Asset::ShaderAssetPtr& oldShaderClass, RPC::TextureTypes::TextureType texType, ShaderProcess::RuntimeTexture& rt );
    void UpdateShaderParams(  const Asset::ShaderAssetPtr&        shaderClass, const ShaderProcess::TextureParam* textures);
    void UpdateShaderAttrTexSettings( ShaderAttrTexSettings*      tex_settings,
                                      u32                        tex_index,
                                      Asset::WrapMode            wrap_u,
                                      Asset::WrapMode            wrap_v,
                                      Asset::RunTimeFilter       rt_filter,
                                      float                      mip_bias);

    void ShaderChanged( const ShaderChangedArgs& args );
    void SkyChanged( const SkyChangedArgs& args );
    void SkyTextureChanged( const SkyTextureChangedArgs& args );
    void SkyLooseGraphShaderChanged( const SkyLooseGraphShaderChangedArgs& args );
    void LevelWeatherAttributesChanged(const LevelWeatherAttributesChangedArgs& args);
    void LevelChanged(const LevelChangedArgs& args);
    void AssetLoaded( const AssetLoadArgs& args );
    virtual void AssetBuilt( const AssetBuilder::AssetBuiltArgsPtr& args ) NOC_OVERRIDE; 

  public: 
    AssetEditor*             m_AssetEditor;
    RPC::ILunaShaderHost*     m_LunaShaderHost;
    RPC::ILunaShaderTool*     m_LunaShaderTool;
    RPC::ILunaViewHost*       m_LunaViewHost;
  };
}