#pragma once

#include "API.h"

#include "TUID/tuid.h"
#include "reflect/Serializers.h"

namespace Manifest
{
  class MANIFEST_API AssetManifest : public Reflect::Element
  {
  public:
    V_tuid m_Shaders;
    S_tuid m_BlendTextures;

    u32 m_TriangleCount;

    u32 m_MentalRayShaderCount;

    bool m_LightMapped;

    Math::Vector3 m_BoundingBoxMin;
    Math::Vector3 m_BoundingBoxMax;

    AssetManifest() :
      m_TriangleCount(0),
      m_MentalRayShaderCount(0),
      m_LightMapped(false)
    {}

    REFLECT_DECLARE_CLASS(AssetManifest, Reflect::Element);

    static void EnumerateClass( Reflect::Compositor<AssetManifest>& comp );
  };

  typedef Nocturnal::SmartPtr<AssetManifest> AssetManifestPtr;
  typedef std::vector<AssetManifestPtr> V_AssetManifest;
}