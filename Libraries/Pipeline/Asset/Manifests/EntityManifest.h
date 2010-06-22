#pragma once

#include "Pipeline/API.h"

#include "Foundation/File/Path.h"

#include "AssetManifest.h"

namespace Asset
{
  class PIPELINE_API EntityManifest : public AssetManifest
  {
  public:
      std::set< Nocturnal::Path > m_Shaders;
      std::set< Nocturnal::Path > m_BlendTextures;
      std::set< Nocturnal::Path > m_LooseTextures;

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
