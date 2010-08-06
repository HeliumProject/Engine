#pragma once

#include "Editor/API.h"
#include "Application/Inspect/Data/Data.h"
#include "Application/Inspect/Controls/InspectPanel.h"
#include "Editor/Scene/SceneNode.h"

namespace Helium
{
    namespace Editor
    {
        class SceneNodePanel : public Inspect::Panel
        {
        public:
            SceneNodePanel(PropertiesGenerator* generator, const OS_SelectableDumbPtr& selection);
            virtual ~SceneNodePanel();
            virtual void Create() HELIUM_OVERRIDE;

        protected:
            PropertiesGenerator*          m_Generator;
            OS_SelectableDumbPtr  m_Selection;
        };
    }
}