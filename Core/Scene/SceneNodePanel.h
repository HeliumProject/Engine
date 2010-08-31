#pragma once

#include "Core/API.h"
#include "Foundation/Inspect/Data.h"
#include "Foundation/Inspect/Container.h"
#include "Core/Scene/SceneNode.h"

namespace Helium
{
    namespace Core
    {
        class SceneNodePanel : public Inspect::Container
        {
        public:
            SceneNodePanel(PropertiesGenerator* generator, const OS_SelectableDumbPtr& selection);

        protected:
            PropertiesGenerator*          m_Generator;
            OS_SelectableDumbPtr  m_Selection;
        };
    }
}