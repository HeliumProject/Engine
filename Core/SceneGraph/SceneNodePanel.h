#pragma once

#include "Foundation/Inspect/Data.h"
#include "Foundation/Inspect/Container.h"

#include "Core/API.h"
#include "Core/SceneGraph/SceneNode.h"
#include "Core/SceneGraph/PropertiesGenerator.h"

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