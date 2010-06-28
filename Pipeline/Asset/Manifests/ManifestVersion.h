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
    tstring m_ManifestVersion;
    static const tchar* MANIFEST_VERSION;

    REFLECT_DECLARE_CLASS(ManifestVersion, Reflect::Version);

    static void EnumerateClass( Reflect::Compositor<ManifestVersion>& comp );

    ManifestVersion();

    ManifestVersion(const tchar* source, const tchar* sourceVersion);

    virtual bool IsCurrent();
  };

  typedef Nocturnal::SmartPtr<ManifestVersion> ManifestVersionPtr;
  typedef std::vector<ManifestVersionPtr> V_ManifestVersion;
}