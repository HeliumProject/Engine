#include "ManifestInit.h"
#include "reflect/Registry.h"

#include "ManifestVersion.h"
#include "AssetManifest.h"

namespace Manifest
{
  i32 g_InitCount = 0;

  bool Initialize()
  {
    if ( ++g_InitCount == 1 )
    {
      REFLECT_REGISTER_CLASS(ManifestVersion);
      REFLECT_REGISTER_CLASS(AssetManifest);
    }

    return true;
  }

  void Cleanup()
  {
    g_InitCount--;
  }

  REFLECT_DEFINE_ENTRY_POINTS(Initialize, Cleanup);
}