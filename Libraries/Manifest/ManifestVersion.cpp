#include "ManifestVersion.h"

const char* Manifest::ManifestVersion::Manifest_VERSION = "5";

using namespace Reflect;
using namespace Manifest;

REFLECT_DEFINE_CLASS(ManifestVersion)

void ManifestVersion::EnumerateClass( Reflect::Compositor<ManifestVersion>& comp )
{
  Reflect::Field* fieldManifestVersion = comp.AddField( &ManifestVersion::m_ManifestVersion, "m_ManifestVersion", Reflect::FieldFlags::Force );
}


Manifest::ManifestVersion::ManifestVersion()
{

}

Manifest::ManifestVersion::ManifestVersion(const char* source, const char* sourceVersion, const char* sourceFile)
: Version (source, sourceVersion, sourceFile)
{
  m_ManifestVersion = Manifest_VERSION;
}

bool Manifest::ManifestVersion::IsCurrent()
{
  return __super::IsCurrent() && m_ManifestVersion == Manifest_VERSION;
}