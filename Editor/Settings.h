#pragma once

#include "Core/API.h"

#include "Core/Scene/SceneSettings.h"
#include "Core/Scene/GridSettings.h"
#include "Core/Scene/ViewportSettings.h"
#include "Editor/Vault/VaultSettings.h"
#include "Editor/WindowSettings.h"

namespace Helium
{
    namespace Editor
    {
        /////////////////////////////////////////////////////////////////////////////
        // Base class for preferenced within Editor.  Provides convenience functions
        // for saving and loading.
        // 
        class Settings : public Reflect::ConcreteInheritor< Settings, Reflect::Element >
        {
        public:
            Settings();

            Core::SceneSettings* GetSceneSettings()
            {
                return m_SceneSettings;
            }

            Core::ViewportSettings* GetViewportSettings()
            {
                return m_ViewportSettings;
            }

            Core::GridSettings* GetGridSettings()
            {
                return m_GridSettings;
            }

            VaultSettings* GetVaultSettings()
            {
                return m_VaultSettings;
            }

            WindowSettings* GetWindowSettings()
            {
                return m_WindowSettings;
            }

        private:
            Core::SceneSettingsPtr m_SceneSettings;
            Core::ViewportSettingsPtr m_ViewportSettings;
            Core::GridSettingsPtr m_GridSettings;
            VaultSettingsPtr m_VaultSettings;
            WindowSettingsPtr m_WindowSettings;

        public:
            static void EnumerateClass( Reflect::Compositor<Settings>& comp )
            {
                comp.AddField( &Settings::m_SceneSettings, "SceneSettings" );
                comp.AddField( &Settings::m_ViewportSettings, "ViewportSettings"  );
                comp.AddField( &Settings::m_GridSettings, "GridSettings" );
                comp.AddField( &Settings::m_VaultSettings, "VaultSettings" );
                comp.AddField( &Settings::m_WindowSettings, "WindowSettings" );
            }
        };
        typedef Helium::SmartPtr< Settings > SettingsPtr;

        /////////////////////////////////////////////////////////////////////////////
        // Choice of how file paths should be displayed in the UI.
        // 
        namespace FilePathOptions
        {
            enum FilePathOption
            {
                Basename,
                Filename,
                PartialPath,
                FullPath
            };

            static void FilePathOptionEnumerateEnum( Reflect::Enumeration* info )
            {
                info->AddElement(Basename, TXT( "Basename" ) );
                info->AddElement(Filename, TXT( "Filename" ) );
                info->AddElement(PartialPath, TXT( "PartialPath" ) );
                info->AddElement(FullPath, TXT( "FullPath" ) );
            }
        };
        typedef FilePathOptions::FilePathOption FilePathOption;

        // Get the UI label for a file path based upon the specified FilePathOption
        tstring PathToLabel( const Helium::Path& path, const FilePathOption filePathOption );
    }
}