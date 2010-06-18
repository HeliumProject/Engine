#pragma once

#include "Pipeline/API.h"

#include "Foundation/Reflect/Version.h"
#include "Foundation/Reflect/Serializers.h"

namespace Asset
{
  //
  // Version information for scene classes
  //

  class PIPELINE_API ManifestVersion : public Reflect::Version
  {
  public:
    std::string m_ManifestVersion;
    static const char* MANIFEST_VERSION;

    REFLECT_DECLARE_CLASS(ManifestVersion, Reflect::Version);

    static void EnumerateClass( Reflect::Compositor<ManifestVersion>& comp );

    ManifestVersion();

    ManifestVersion(const char* source, const char* sourceVersion);

    virtual bool IsCurrent();
  };

  typedef Nocturnal::SmartPtr<ManifestVersion> ManifestVersionPtr;
  typedef std::vector<ManifestVersionPtr> V_ManifestVersion;
}