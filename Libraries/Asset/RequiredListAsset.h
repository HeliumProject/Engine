#pragma once

#include "API.h"
#include "AssetClass.h"


#include "Finder/AssetSpecs.h"
#include "TUID/TUID.h"

namespace Asset
{
  class ASSET_API RequiredListAsset : public AssetClass
  {
  public:

    V_tuid  m_AssetIds;
    V_string  m_UpdateClasses;

    RequiredListAsset() {}
    virtual ~RequiredListAsset() {}

    REFLECT_DECLARE_CLASS( RequiredListAsset, AssetClass );

    static void EnumerateClass( Reflect::Compositor<RequiredListAsset>& comp );
  };

  typedef Nocturnal::SmartPtr< RequiredListAsset > RequiredListAssetPtr;
}