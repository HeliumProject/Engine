#pragma once

#include "Editor/API.h"
#include "Editor/MRU/MRU.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

namespace Helium
{
    namespace Editor
    {
        class GeneralSettings;
        typedef Helium::StrongPtr< GeneralSettings > GeneralSettingsPtr;

        class GeneralSettings : public Reflect::Element
        {
        public:
            GeneralSettings();

            std::vector< tstring >& GetMRUProjects();
            void SetMRUProjects( MRU< tstring >* mru );

        public:
            REFLECT_DECLARE_CLASS( GeneralSettings, Reflect::Element );
            static void AcceptCompositeVisitor( Reflect::Composite& comp )
            {
                comp.AddField( &GeneralSettings::m_MRUProjects, "m_MRUProjects" );
            }

        private:
            std::vector< tstring > m_MRUProjects;
        };
    }
}