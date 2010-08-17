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
            PropertiesGenerator* m_Generator;
            OS_SelectableDumbPtr m_Selection;
            Inspect::CheckBox* m_Solid;
            Inspect::CheckBox* m_SolidOverride;
            Inspect::CheckBox* m_Transparent;
            Inspect::CheckBox* m_TransparentOverride;

            bool                   m_EnableClassPicker; 
            bool                   m_EnableClassBrowser; 
            bool                   m_EnableSymbolInterpreter; 
            bool                   m_RecurseSelectableClasses; 

        public:
            InstancePanel(PropertiesGenerator* generator, const OS_SelectableDumbPtr& selection);
            virtual void Create() HELIUM_OVERRIDE;

        protected:
            void CreateAppearanceFlags();

            void OnSolidOverride( const Inspect::ControlChangedArgs& args );
            void OnTransparentOverride( const Inspect::ControlChangedArgs& args );

            // utility functions
            //
            static void Intersect(std::set< tstring >& intersection, const std::set< tstring >& classList);
            static void RecurseAddDerivedClasses( const tstring& baseClass, std::set< tstring >& classList );   
        };
    }
}