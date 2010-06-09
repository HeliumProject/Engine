#pragma once

#include "AssetClass.h"
#include "Asset/SceneAsset.h" 
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
  struct LevelWeatherAttributesChangedArgs
  {
    Asset::SceneAssetPtr  m_LevelClass; 
    u32                   m_Flags;
  };
  typedef Nocturnal::Signature< void, const LevelWeatherAttributesChangedArgs& > LevelWeatherAttributesChangedSignature;

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
    LevelWeatherAttributesChangedSignature::Event   m_LevelWeatherAttributesChanged;
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

    void AddLevelWeatherAttributesChangedListener( const LevelWeatherAttributesChangedSignature::Delegate& listener )
    {
      m_LevelWeatherAttributesChanged.Add( listener );
    }

    void RemoveLevelWeatherAttributesChangedListener( const LevelWeatherAttributesChangedSignature::Delegate& listener )
    {
      m_LevelWeatherAttributesChanged.Remove( listener );
    }

    void WeatherAttributeChanged(const Reflect::ElementChangeArgs& args);
    void OnAttributeAdded(const Attribute::AttributeCollectionChanged& args); 
    void OnAttributeRemoved(const Attribute::AttributeCollectionChanged& args); 
  };
}
