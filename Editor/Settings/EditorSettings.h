#pragma once

#include "Editor/API.h"
#include "Editor/MRU/MRU.h"
#include "Pipeline/Settings.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

namespace Helium
{
    namespace Editor
    {
        class GeneralSettings : public Settings
        {
        public:
            GeneralSettings();

            std::vector< tstring >& GetMRUProjects();
            void SetMRUProjects( MRU< tstring >* mru );

            bool GetLoadLastOpenedProjectOnStartup() const;
            void SetLoadLastOpenedProjectOnStartup( bool value );

            REFLECT_DECLARE_CLASS( GeneralSettings, Settings );
            static void AcceptCompositeVisitor( Reflect::Composite& comp );

        private:
            std::vector< tstring > m_MRUProjects;
            bool m_LoadLastOpenedProjectOnStartup;
        };

        typedef Helium::StrongPtr< GeneralSettings > GeneralSettingsPtr;

    }
}