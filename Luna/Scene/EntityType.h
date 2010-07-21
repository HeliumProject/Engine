#pragma once

#include "InstanceType.h"

namespace Luna
{
  class EntityType : public Luna::InstanceType
  {
    //
    // Members
    //

  private:
    // materials
    D3DMATERIAL9 m_Material;


    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::EntityType, Luna::InstanceType );
    static void InitializeType();
    static void CleanupType();


    //
    // Implementation
    //

  public:
    EntityType( Luna::Scene* scene, i32 instanceType );

    virtual ~EntityType();

    virtual void Reset() NOC_OVERRIDE;
    virtual void Create() NOC_OVERRIDE;
    virtual void Delete() NOC_OVERRIDE;

  public:
    virtual void PopulateManifest( Asset::SceneManifest* manifest ) const NOC_OVERRIDE;
  };
}
