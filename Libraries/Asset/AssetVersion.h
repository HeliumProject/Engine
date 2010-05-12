#pragma once

#include "API.h"
#include "Reflect/Version.h"
#include "Reflect/Serializers.h"

namespace Asset
{
  //
  // Version information for scene classes
  //

  class ASSET_API AssetVersion : public Reflect::Version
  {
  public:
    std::string m_AssetVersion;
    static const char* ASSET_VERSION;

    REFLECT_DECLARE_CLASS(AssetVersion, Reflect::Version)

    static void EnumerateClass( Reflect::Compositor<AssetVersion>& comp );

    AssetVersion();
  };

  typedef Nocturnal::SmartPtr<AssetVersion> AssetVersionPtr;
  typedef std::vector<AssetVersionPtr> V_AssetVersion;
}