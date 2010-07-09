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
  class PIPELINE_API Entity : public AssetClass
  {
  private:

  public:
    //
    // RTTI
    //

    REFLECT_DECLARE_CLASS(Entity, AssetClass);

    static void EnumerateClass( Reflect::Compositor<Entity>& comp );


  public:

    Entity()
    {
    }

    void GatherIndexData( AssetIndexData& indexData );

  }; 

  typedef Nocturnal::SmartPtr< Entity > EntityPtr;
  typedef std::vector< EntityPtr > V_Entity;

};