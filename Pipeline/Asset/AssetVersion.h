#pragma once

#include "Pipeline/API.h"
#include "Foundation/Reflect/Version.h"
#include "Foundation/Reflect/Serializers.h"

namespace Asset
{
  //
  // Version information for scene classes
  //

  class PIPELINE_API AssetVersion : public Reflect::Version
  {
  public:
    tstring m_AssetVersion;
    static const tchar* ASSET_VERSION;

    REFLECT_DECLARE_CLASS(AssetVersion, Reflect::Version)

    static void EnumerateClass( Reflect::Compositor<AssetVersion>& comp );

    AssetVersion();
  };

  typedef Nocturnal::SmartPtr<AssetVersion> AssetVersionPtr;
  typedef std::vector<AssetVersionPtr> V_AssetVersion;
}