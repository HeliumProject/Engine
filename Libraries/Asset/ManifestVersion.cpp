#include "ManifestVersion.h"

using namespace Reflect;
using namespace Asset;

REFLECT_DEFINE_CLASS(ManifestVersion)

void ManifestVersion::EnumerateClass( Reflect::Compositor<ManifestVersion>& comp )
{
  Reflect::Field* fieldManifestVersion = comp.AddField( &ManifestVersion::m_ManifestVersion, "m_ManifestVersion", Reflect::FieldFlags::Force );
}


const char* ManifestVersion::MANIFEST_VERSION = "5";

ManifestVersion::ManifestVersion()
{

}

ManifestVersion::ManifestVersion(const char* source, const char* sourceVersion)
: Version (source, sourceVersion)
{
  m_ManifestVersion = MANIFEST_VERSION;
}

bool ManifestVersion::IsCurrent()
{
  return __super::IsCurrent() && m_ManifestVersion == MANIFEST_VERSION;
}