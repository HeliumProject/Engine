#pragma once

#include "Foundation/Inspect/DataBinding.h"
#include "Foundation/Inspect/Container.h"

#include "Pipeline/API.h"
#include "Pipeline/SceneGraph/SceneNode.h"
#include "Pipeline/SceneGraph/PropertiesGenerator.h"

namespace Helium
{
    namespace SceneGraph
    {
        class SceneNodePanel : public Inspect::Container
        {
        public:
            SceneNodePanel(PropertiesGenerator* generator, const OS_SceneNodeDumbPtr& selection);

        protected:
            PropertiesGenerator*          m_Generator;
            OS_SceneNodeDumbPtr  m_Selection;
        };
    }
}