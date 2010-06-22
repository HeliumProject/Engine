#pragma once

#include "Pipeline/API.h"

#include "Foundation/TUID.h"
#include "Foundation/Reflect/Serializers.h"

namespace Asset
{
  class PIPELINE_API AssetManifest : public Reflect::Element
  {
  public:
    Math::Vector3 m_BoundingBoxMin;
    Math::Vector3 m_BoundingBoxMax;

    REFLECT_DECLARE_ABSTRACT(AssetManifest, Reflect::Element);

    static void EnumerateClass( Reflect::Compositor<AssetManifest>& comp );
  };

  typedef Nocturnal::SmartPtr<AssetManifest> AssetManifestPtr;
  typedef std::vector<AssetManifestPtr> V_AssetManifest;
}