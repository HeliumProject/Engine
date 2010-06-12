#pragma once

#include "API.h"
#include "AssetClass.h"


#include "Foundation/TUID.h"
#include "Reflect/Registry.h"

#include "Finder/AssetSpecs.h"
#include "Finder/ExtensionSpecs.h"

namespace Asset
{
  class ASSET_API SceneAsset : public AssetClass
  {
  public:
    float       m_NearClipDist;
    float       m_FarClipDist;
    float       m_SpatialGridSize;
    bool        m_OcclTestDownwardColl;
    bool        m_OcclTestDownwardVis;
    float       m_OcclVisDistAdjust;
    float       m_DecalGeomMem;

    Nocturnal::S_Path m_Zones;

    SceneAsset()
      : m_NearClipDist( 0.06f )
      , m_FarClipDist( 1800.0f )
      , m_SpatialGridSize( 2.0f )
      , m_OcclTestDownwardColl( true )
      , m_OcclTestDownwardVis( true )
      , m_OcclVisDistAdjust( 0.0f )
      , m_DecalGeomMem( 1024.0f )
    {
    }

    virtual bool ProcessComponent(Reflect::ElementPtr element, const std::string& fieldName) NOC_OVERRIDE;

    virtual bool ValidateCompatible( const Attribute::AttributePtr& attr, std::string& error ) const NOC_OVERRIDE;

    virtual bool IsBuildable() const NOC_OVERRIDE;
    virtual bool IsViewable() const NOC_OVERRIDE;

    void MakeDefault();

    REFLECT_DECLARE_CLASS( SceneAsset, AssetClass );

    static void EnumerateClass( Reflect::Compositor<SceneAsset>& comp );
  };

  typedef Nocturnal::SmartPtr< SceneAsset > SceneAssetPtr;
  typedef std::vector< SceneAsset > V_SceneAsset;
}
