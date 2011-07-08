#pragma once

#include "SceneGraph/API.h"

#include "Foundation/File/Path.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

namespace Helium
{
    namespace SceneGraph
    {
        class SCENE_GRAPH_API SceneManifest : public Reflect::Object
        {
        public:
            Vector3 m_BoundingBoxMin;
            Vector3 m_BoundingBoxMax;
            std::set< Helium::Path > m_Assets;

            REFLECT_DECLARE_OBJECT(SceneManifest, Reflect::Object);
			static void PopulateComposite( Reflect::Composite& comp );
        };

        typedef Helium::StrongPtr<SceneManifest> SceneManifestPtr;
        typedef std::vector<SceneManifestPtr> V_SceneManifest;
    }
}