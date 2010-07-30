#pragma once

#include "Application/Inspect/Controls/InspectPanel.h"
#include "Application/Inspect/Interpreters/Reflect/ReflectInterpreter.h"
#include "PropertiesGenerator.h"

namespace Helium
{
    namespace Editor
    {
        /////////////////////////////////////////////////////////////////////////////
        // UI panel containing controls pertaining to lights
        // 
        class LightPanel : public Inspect::Panel
        {
        private:
            PropertiesGenerator* m_Generator;
            OS_SelectableDumbPtr m_Selection;
            Inspect::ReflectInterpreterPtr  m_ReflectInterpreter;

        public:
            LightPanel( PropertiesGenerator* generator, const OS_SelectableDumbPtr& selection );
            virtual void Create() HELIUM_OVERRIDE;
        };
    }
}