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
        class IconSize
        {
        public:
            enum Enum
            {
                Small,
                Medium,
                Large,
            };

            REFLECT_DECLARE_ENUMERATION( IconSize );

            static void EnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement( Small, TXT( "Small" ) );
                info.AddElement( Medium, TXT( "Medium" ) );
                info.AddElement( Large, TXT( "Large" ) );
            }
        };

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

        public:
            std::vector< tstring > m_MRUProjects;
            
            bool m_ReopenLastProjectOnStartup;
            bool m_ShowFileExtensionsInProjectView;
            bool m_EnableAssetTracker;
            bool m_ShowTextOnButtons;
            bool m_ShowIconsOnButtons;
            IconSize m_IconSizeOnButtons;
        };

        typedef Helium::StrongPtr< EditorSettings > GeneralSettingsPtr;

    }
}