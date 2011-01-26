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
        class EditorSettings : public Settings
        {
        public:
            EditorSettings();

            std::vector< tstring >& GetMRUProjects();
            void SetMRUProjects( MRU< tstring >* mru );

            bool GetReopenLastProjectOnStartup() const;
            void SetReopenLastProjectOnStartup( bool value );

            bool GetShowFileExtensionsInProjectView() const;
            void SetShowFileExtensionsInProjectView( bool value );

            bool GetEnableAssetTracker() const;
            void SetEnableAssetTracker( bool value );

            REFLECT_DECLARE_OBJECT( EditorSettings, Settings );
            static void PopulateComposite( Reflect::Composite& comp );

        private:
            std::vector< tstring > m_MRUProjects;
            
            bool m_ReopenLastProjectOnStartup;
            bool m_ShowFileExtensionsInProjectView;
            bool m_EnableAssetTracker;
        };

        typedef Helium::StrongPtr< EditorSettings > GeneralSettingsPtr;

    }
}