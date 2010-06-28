#pragma once

#include "Pipeline/API.h"

#include <hash_map>

#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/Asset/Manifests/EntityManifest.h"

namespace Asset
{
  //
  // The Definition of an Entity Class
  //

  
  class EntityAsset;
  typedef Nocturnal::SmartPtr< EntityAsset > EntityAssetPtr;
  typedef std::vector< EntityAssetPtr > V_EntityAsset;
  typedef stdext::hash_map< tuid, EntityAssetPtr > M_EntityAssetPtr; 

  class PIPELINE_API EntityAsset : public AssetClass
  {
  private:

    // manifest information exported from the content
    Asset::EntityManifestPtr m_Manifest;

  public:
    //
    // RTTI
    //

    REFLECT_DECLARE_CLASS(EntityAsset, AssetClass);

    static void EnumerateClass( Reflect::Compositor<EntityAsset>& comp );


    //
    // Member functions
    // 

    EntityAsset();

    virtual bool ValidateCompatible( const Component::ComponentPtr& attr, tstring& error ) const NOC_OVERRIDE;
    void MakeDefault();

    virtual bool IsBuildable() const NOC_OVERRIDE;
    virtual bool IsViewable() const NOC_OVERRIDE;

    const Asset::EntityManifestPtr GetManifest();
  }; 
};