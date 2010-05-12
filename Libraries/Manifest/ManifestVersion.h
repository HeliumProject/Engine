#pragma once

#include "API.h"

#include "reflect/Version.h"
#include "reflect/Serializers.h"

namespace Manifest
{
  //
  // Version information for scene classes
  //

  class MANIFEST_API ManifestVersion : public Reflect::Version
  {
  public:
    std::string m_ManifestVersion;
    static const char* Manifest_VERSION;

    REFLECT_DECLARE_CLASS(ManifestVersion, Reflect::Version);

    static void EnumerateClass( Reflect::Compositor<ManifestVersion>& comp );

    ManifestVersion();

    ManifestVersion(const char* source, const char* sourceVersion, const char* sourceFile);

    virtual bool IsCurrent();
  };

  typedef Nocturnal::SmartPtr<ManifestVersion> ManifestVersionPtr;
  typedef std::vector<ManifestVersionPtr> V_ManifestVersion;
}