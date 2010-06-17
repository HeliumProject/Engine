#pragma once

#include "AssetClass.h"
#include "Pipeline/Asset/Classes/SceneAsset.h" 
#include "Reflect/Element.h"

namespace Inspect
{
  class Control;
}

namespace Luna
{
  enum WeatherOps
  {
    WEATHER_OP_NONE                      = 0x00,
    WEATHER_OP_TEXTURES2D_RELOAD         = 0x01,
    WEATHER_OP_CUBEMAP_RELOAD            = 0x02,
    WEATHER_OP_PARTICLE_TEXTURE_REBUILD  = 0x04,
    WEATHER_OP_DETAIL_NORMALMAP_REBUILD  = 0x08
  };

  // Forwards
  struct LevelChangedArgs
  {
    Asset::SceneAssetPtr  m_LevelClass; 
  };
  typedef Nocturnal::Signature< void, const LevelChangedArgs& > LevelChangedSignature;

  // Forwards
  struct LevelWeatherComponentsChangedArgs
  {
    Asset::SceneAssetPtr  m_LevelClass; 
    u32                   m_Flags;
  };
  typedef Nocturnal::Signature< void, const LevelWeatherComponentsChangedArgs& > LevelWeatherComponentsChangedSignature;

  // overridden to provide real-time update of Level Settings
  // 
  class SceneAsset : public Luna::AssetClass
  {
  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::SceneAsset, Luna::AssetClass );
    static void InitializeType();
    static void CleanupType();
    static Luna::PersistentDataPtr Create( Reflect::Element* level, Luna::AssetManager* manager );

  protected:
    SceneAsset( Asset::SceneAsset* level, Luna::AssetManager* manager ); 
  public:
    virtual ~SceneAsset(); 

  private: 
    void OnElementChanged(const Reflect::ElementChangeArgs& args); 

  protected:
    LevelWeatherComponentsChangedSignature::Event   m_LevelWeatherComponentsChanged;
    LevelChangedSignature::Event                    m_LevelChanged;

  public:
    void AddLevelChangedListener( const LevelChangedSignature::Delegate& listener )
    {
      m_LevelChanged.Add( listener );
    }

    void RemoveLevelChangedListener( const LevelChangedSignature::Delegate& listener )
    {
      m_LevelChanged.Remove( listener );
    }

    void AddLevelWeatherComponentsChangedListener( const LevelWeatherComponentsChangedSignature::Delegate& listener )
    {
      m_LevelWeatherComponentsChanged.Add( listener );
    }

    void RemoveLevelWeatherComponentsChangedListener( const LevelWeatherComponentsChangedSignature::Delegate& listener )
    {
      m_LevelWeatherComponentsChanged.Remove( listener );
    }

    void WeatherComponentChanged(const Reflect::ElementChangeArgs& args);
    void OnComponentAdded(const Component::ComponentCollectionChanged& args); 
    void OnComponentRemoved(const Component::ComponentCollectionChanged& args); 
  };
}
