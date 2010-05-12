#pragma once

#include "AssetClass.h"
#include "Asset/SkyAsset.h"
#include "Asset/LooseGraphShaderAsset.h"
#include "AssetBuilder/ISkyBuilder.h"

namespace Inspect
{
  class Control;
}

namespace Luna
{
  // Forwards
  struct SkyChangedArgs
  {
    Asset::SkyAssetPtr            m_SkyClass;
    AssetBuilder::ISkyBuilder*    m_SkyBuilder;
  };
  typedef Nocturnal::Signature< void, const SkyChangedArgs& > SkyChangedSignature;

  struct SkyLooseGraphShaderChangedArgs
  {
    Asset::SkyAssetPtr              m_SkyClass;
    Asset::LooseGraphShaderAssetPtr m_LooseGraphShaderClass;
    AssetBuilder::ISkyBuilder*      m_SkyBuilder;
  };
  typedef Nocturnal::Signature< void, const SkyLooseGraphShaderChangedArgs& > SkyLooseGraphShaderChangedSignature;

  struct SkyTextureChangedArgs
  {
    Asset::SkyAssetPtr            m_SkyClass;
    AssetBuilder::ISkyBuilder*    m_SkyBuilder;

    u32                           m_TextureType;
    std::string                   m_TexturePath;
  };
  typedef Nocturnal::Signature< void, const SkyTextureChangedArgs& > SkyTextureChangedSignature;

  /////////////////////////////////////////////////////////////////////////////
  // Overridden to provide real-time updating of skies.
  // 
  class SkyAsset : public Luna::AssetClass
  {
  private:
    AssetBuilder::ISkyBuilderPtr m_SkyBuilder;

  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::SkyAsset, Luna::AssetClass );
    static void InitializeType();
    static void CleanupType();
    static Luna::PersistentDataPtr Create( Reflect::Element* skyClass, Luna::AssetManager* manager );

  protected:
    SkyAsset( Asset::SkyAsset* skyClass, Luna::AssetManager* manager);
  public:
    virtual ~SkyAsset();

    virtual void Unpack() NOC_OVERRIDE;
    virtual void PopulateContextMenu( ContextMenuItemSet& menu ) NOC_OVERRIDE;

    void SyncShells();
    virtual void Changed( Inspect::Control* control ) NOC_OVERRIDE;

  protected:
    void OnSyncShells( const ContextMenuArgsPtr& args );

  protected:
    SkyChangedSignature::Event m_SkyChanged;
  public:
    void AddSkyChangedListener( const SkyChangedSignature::Delegate& listener )
    {
      m_SkyChanged.Add( listener );
    }

    void RemoveSkyChangedListener( const SkyChangedSignature::Delegate& listener )
    {
      m_SkyChanged.Remove( listener );
    }

  protected:
    SkyTextureChangedSignature::Event m_SkyTextureChanged;
  public:
    void AddSkyTextureChangedListener( const SkyTextureChangedSignature::Delegate& listener )
    {
      m_SkyTextureChanged.Add( listener );
    }

    void RemoveSkyTextureChangedListener( const SkyTextureChangedSignature::Delegate& listener )
    {
      m_SkyTextureChanged.Remove( listener );
    }

  protected:
    SkyLooseGraphShaderChangedSignature::Event m_SkyLooseGraphShaderChanged;
  public:
    void AddSkyLooseGraphShaderChangedListener( const SkyLooseGraphShaderChangedSignature::Delegate& listener )
    {
      m_SkyLooseGraphShaderChanged.Add( listener );
    }

    void RemoveSkyLooseGraphShaderChangedListener( const SkyLooseGraphShaderChangedSignature::Delegate& listener )
    {
      m_SkyLooseGraphShaderChanged.Remove( listener );
    }
  };
}
