#pragma once

#include "AssetClass.h"

#include "Pipeline/Asset/Classes/StandardShaderAsset.h"
#include "Editor/ContextMenuGenerator.h"

namespace Luna
{
  struct ShaderChangedArgs
  {
    Asset::ShaderAssetPtr m_ShaderClass;
    Asset::ShaderAssetPtr m_OldShaderClass;
    Nocturnal::Path       m_TexturePath;
  };
  typedef Nocturnal::Signature< void, const ShaderChangedArgs& > ShaderChangedSignature;

  class ShaderAsset : public Luna::AssetClass
  {
  private:
    Asset::ShaderAssetPtr m_OldShaderClass;
    ContextMenuItemSet m_CustomShaders;

  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::ShaderAsset, Luna::AssetClass );
    static void InitializeType();
    static void CleanupType();
    static Luna::PersistentDataPtr Create( Reflect::Element* shaderClass, Luna::AssetManager* manager );

  protected:
    ShaderAsset( Asset::ShaderAsset* shaderClass, Luna::AssetManager* manager );
  public:
    virtual ~ShaderAsset();
    std::string GetIcon() const NOC_OVERRIDE;
    virtual void PopulateContextMenu( ContextMenuItemSet& menu );
    virtual void Changed( Inspect::Control* control ) NOC_OVERRIDE;
    virtual void Unpack() NOC_OVERRIDE;
  protected:
    void InitializeContextMenu();
    static void ConvertShader( const ContextMenuArgsPtr& args );
    void ReloadAllTextures( const ContextMenuArgsPtr& args );
    void CheckShaderChanged( Nocturnal::Path texturePath );

  protected:
    ShaderChangedSignature::Event m_ShaderChanged;
  public:
    void AddShaderChangedListener( const ShaderChangedSignature::Delegate& listener )
    {
      m_ShaderChanged.Add( listener );
    }

    void RemoveShaderChangedListener( const ShaderChangedSignature::Delegate& listener )
    {
      m_ShaderChanged.Remove( listener );
    }
  };

  typedef Nocturnal::SmartPtr< Luna::ShaderAsset > ShaderAssetPtr;
}