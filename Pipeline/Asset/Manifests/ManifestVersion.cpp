#include "ManifestVersion.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_CLASS(ManifestVersion)

void ManifestVersion::EnumerateClass( Reflect::Compositor<ManifestVersion>& comp )
{
  Reflect::Field* fieldManifestVersion = comp.AddField( &ManifestVersion::m_ManifestVersion, "m_ManifestVersion", Reflect::FieldFlags::Force );
}


const tchar_t* ManifestVersion::MANIFEST_VERSION = TXT( "5" );

ManifestVersion::ManifestVersion()
{

}

ManifestVersion::ManifestVersion(const tchar_t* source, const tchar_t* sourceVersion)
: Version (source, sourceVersion)
{
  m_ManifestVersion = MANIFEST_VERSION;
}

bool ManifestVersion::IsCurrent()
{
  return __super::IsCurrent() && m_ManifestVersion == MANIFEST_VERSION;
}