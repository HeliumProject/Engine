#pragma once

#include "Core/API.h"
#include "Core/Scene/SceneNode.h"
#include "Core/Scene/SceneNodePanel.h"

namespace Helium
{
    namespace Core
    {
        class InstancePanel : public Inspect::Container
        {
        protected:
            PropertiesGenerator*    m_Generator;
            OS_SceneNodeDumbPtr    m_Selection;
            Inspect::CheckBox*      m_Solid;
            Inspect::CheckBox*      m_SolidOverride;
            Inspect::CheckBox*      m_Transparent;
            Inspect::CheckBox*      m_TransparentOverride;

        public:
            InstancePanel(PropertiesGenerator* generator, const OS_SceneNodeDumbPtr& selection);

            void OnSolidOverride( const Inspect::ControlChangedArgs& args );
            void OnTransparentOverride( const Inspect::ControlChangedArgs& args );
        };
    }
}