#pragma once

#include "Foundation/FilePath.h"
#include "Reflect/Data/DataDeduction.h"
#include "Math/Vector3.h"

#include "SceneGraph/API.h"

namespace Helium
{
    namespace SceneGraph
    {
        class HELIUM_SCENE_GRAPH_API SceneManifest : public Reflect::Object
        {
        public:
            Vector3 m_BoundingBoxMin;
            Vector3 m_BoundingBoxMax;
            std::set< Helium::FilePath > m_Assets;

            REFLECT_DECLARE_OBJECT(SceneManifest, Reflect::Object);
			static void PopulateComposite( Reflect::Composite& comp );
        };

        typedef Helium::StrongPtr<SceneManifest> SceneManifestPtr;
        typedef std::vector<SceneManifestPtr> V_SceneManifest;
    }
}