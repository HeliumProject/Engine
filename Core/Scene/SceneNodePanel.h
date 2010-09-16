#pragma once

#include "Foundation/Inspect/Data.h"
#include "Foundation/Inspect/Container.h"

#include "Core/API.h"
#include "Core/Scene/SceneNode.h"
#include "Core/Scene/PropertiesGenerator.h"

namespace Helium
{
    namespace Core
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