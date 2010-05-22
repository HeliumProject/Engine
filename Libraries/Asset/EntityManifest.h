#pragma once

#include "API.h"

#include "AssetManifest.h"

#include "File/Reference.h"

namespace Asset
{
  class ASSET_API EntityManifest : public AssetManifest
  {
  public:
      File::V_Reference m_Shaders;
      File::V_Reference m_BlendTextures;
      File::V_Reference m_LooseTextures;

    u32 m_TriangleCount;

    u32 m_MentalRayShaderCount;

    bool m_LightMapped;

    EntityManifest()
      : m_TriangleCount(0)
      , m_MentalRayShaderCount(0)
      , m_LightMapped(false)
    {
      
    }

    REFLECT_DECLARE_CLASS(EntityManifest, AssetManifest);

    static void EnumerateClass( Reflect::Compositor<EntityManifest>& comp );
  };

  typedef Nocturnal::SmartPtr<EntityManifest> EntityManifestPtr;
  typedef std::vector<EntityManifestPtr> V_EntityManifest;
}
