#pragma once

#include "AssetClass.h"
#include "Asset/FoliageAsset.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // Class wrapping Asset::FoliageAsset.  Handles real-time update of foliage
  // properties.
  // 
  class FoliageAsset : public Luna::AssetClass
  {
  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::FoliageAsset, Luna::AssetClass );
    static void InitializeType();
    static void CleanupType();
    static Luna::PersistentDataPtr Create( Reflect::Element* foliageClass, Luna::AssetManager* manager );

  protected:
    FoliageAsset( Asset::FoliageAsset* foliageClass, Luna::AssetManager* manager);
  public:
    virtual ~FoliageAsset();

  protected:
    void OnPackageDataChanged( const Reflect::ElementChangeArgs& args );
  };
}
