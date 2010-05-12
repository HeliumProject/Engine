#pragma once

#include "AssetClass.h"

#include "Asset/AnimationConfigAsset.h"

namespace Inspect
{ 
  class Button;
}

namespace Luna
{
  struct CreatePanelArgs; 

  /////////////////////////////////////////////////////////////////////////////
  // Wrapper for Asset::AnimationConfigAsset.
  // 
  class AnimationConfigAsset : public Luna::AssetClass
  {
  private:

  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::AnimationConfigAsset, Luna::AssetClass );
    static void InitializeType();
    static void CleanupType();
    static Luna::PersistentDataPtr Create( Reflect::Element* animConfig, Luna::AssetManager* manager );

  protected:
    AnimationConfigAsset( Asset::AnimationConfigAsset* animConfig, Luna::AssetManager* manager );
  public:
    virtual ~AnimationConfigAsset();

    const M_strbool& GetCategories() const;
    void SetCategories( const M_strbool& slots );

    const M_strbool& GetModifiers() const;
    void SetModifiers( const M_strbool& slots );

    const std::string& GetConfigurationFile() const;

    virtual bool ValidatePanel( const std::string& name ) NOC_OVERRIDE;

    // UI
  protected:
    static void CreatePanel( CreatePanelArgs& args );

    static void OnEditButton( Inspect::Button* button );
    static void OnReloadButton( Inspect::Button* button );
  };

  typedef Nocturnal::SmartPtr< Luna::AnimationConfigAsset > LAnimationConfigClassPtr;
}