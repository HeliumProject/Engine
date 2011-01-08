#pragma once

#include "Editor/API.h"
#include "Editor/MRU/MRU.h"
#include "Pipeline/Settings.h"
#include "Foundation/Reflect/Object.h"
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

            bool GetReopenLastProjectOnStartup() const;
            void SetReopenLastProjectOnStartup( bool value );

            bool GetShowFileExtensionsInProjectView() const;
            void SetShowFileExtensionsInProjectView( bool value );

            REFLECT_DECLARE_CLASS( GeneralSettings, Settings );
            static void AcceptCompositeVisitor( Reflect::Composite& comp );

        private:
            std::vector< tstring > m_MRUProjects;
            
            bool m_ReopenLastProjectOnStartup;
            bool m_ShowFileExtensionsInProjectView;
        };

        typedef Helium::StrongPtr< GeneralSettings > GeneralSettingsPtr;

    }
}