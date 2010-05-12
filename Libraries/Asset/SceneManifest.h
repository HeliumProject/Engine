#pragma once

#include "API.h"

#include "AssetManifest.h"

namespace Asset
{
  class ASSET_API SceneManifest : public AssetManifest
  {
  public:
    S_tuid m_Assets;

    REFLECT_DECLARE_CLASS(SceneManifest, AssetManifest);

    static void EnumerateClass( Reflect::Compositor<SceneManifest>& comp );
  };

  typedef Nocturnal::SmartPtr<SceneManifest> SceneManifestPtr;
  typedef std::vector<SceneManifestPtr> V_SceneManifest;
}