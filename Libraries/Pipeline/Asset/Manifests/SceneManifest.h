#pragma once

#include "Pipeline/API.h"

#include "Foundation/File/Path.h"

#include "AssetManifest.h"

namespace Asset
{
  class PIPELINE_API SceneManifest : public AssetManifest
  {
  public:
      Nocturnal::S_Path m_Assets;

    REFLECT_DECLARE_CLASS(SceneManifest, AssetManifest);

    static void EnumerateClass( Reflect::Compositor<SceneManifest>& comp );
  };

  typedef Nocturnal::SmartPtr<SceneManifest> SceneManifestPtr;
  typedef std::vector<SceneManifestPtr> V_SceneManifest;
}