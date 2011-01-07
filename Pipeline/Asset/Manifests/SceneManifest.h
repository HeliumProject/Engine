#pragma once

#include "Pipeline/API.h"

#include "Foundation/File/Path.h"

#include "AssetManifest.h"

namespace Helium
{
    namespace Asset
    {
        class PIPELINE_API SceneManifest : public AssetManifest
        {
        public:
            std::set< Helium::Path > m_Assets;

            REFLECT_DECLARE_CLASS(SceneManifest, AssetManifest);

            static void AcceptCompositeVisitor( Reflect::Composite& comp );
        };

        typedef Helium::StrongPtr<SceneManifest> SceneManifestPtr;
        typedef std::vector<SceneManifestPtr> V_SceneManifest;
    }
}