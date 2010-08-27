#pragma once

#include "Core/API.h"
#include "SceneNodePanel.h"
#include "Core/Scene/Selectable.h"

namespace Helium
{
    namespace Core
    {
        class InstancePanel : public Inspect::Panel
        {
        protected:
            PropertiesGenerator*    m_Generator;
            OS_SelectableDumbPtr    m_Selection;
            Inspect::CheckBox*      m_Solid;
            Inspect::CheckBox*      m_SolidOverride;
            Inspect::CheckBox*      m_Transparent;
            Inspect::CheckBox*      m_TransparentOverride;

        public:
            InstancePanel(PropertiesGenerator* generator, const OS_SelectableDumbPtr& selection);

            void OnSolidOverride( const Inspect::ControlChangedArgs& args );
            void OnTransparentOverride( const Inspect::ControlChangedArgs& args );
        };
    }
}