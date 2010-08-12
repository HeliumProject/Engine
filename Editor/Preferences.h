#pragma once

#include "Core/API.h"

#include "Core/Scene/Settings.h"
#include "Core/Scene/ScenePreferences.h"
#include "Core/Scene/GridPreferences.h"
#include "Core/Scene/ViewportPreferences.h"
#include "Editor/Vault/VaultPreferences.h"
#include "Editor/WindowSettings.h"

namespace Helium
{
    namespace Editor
    {
        /////////////////////////////////////////////////////////////////////////////
        // Base class for preferenced within Editor.  Provides convenience functions
        // for saving and loading.
        // 
        class Preferences : public Reflect::ConcreteInheritor< Preferences, Core::Settings >
        {
        public:
            Preferences();

            Core::ScenePreferences* GetScenePreferences()
            {
                return m_ScenePreferences;
            }

            Core::ViewportPreferences* GetViewportPreferences()
            {
                return m_ViewportPreferences;
            }

            Core::GridPreferences* GetGridPreferences()
            {
                return m_GridPreferences;
            }

            VaultPreferences* GetVaultPreferences()
            {
                return m_VaultPreferences;
            }

            WindowSettings* GetWindowSettings()
            {
                return m_WindowSettings;
            }

        private:
            Core::ScenePreferencesPtr m_ScenePreferences;
            Core::ViewportPreferencesPtr m_ViewportPreferences;
            Core::GridPreferencesPtr m_GridPreferences;
            VaultPreferencesPtr m_VaultPreferences;
            WindowSettingsPtr m_WindowSettings;

        public:
            static void EnumerateClass( Reflect::Compositor<Preferences>& comp )
            {
                comp.AddField( &Preferences::m_ScenePreferences, "ScenePreferences" );
                comp.AddField( &Preferences::m_ViewportPreferences, "ViewportPreferences"  );
                comp.AddField( &Preferences::m_GridPreferences, "GridPreferences" );
                comp.AddField( &Preferences::m_VaultPreferences, "VaultPreferences" );
                comp.AddField( &Preferences::m_WindowSettings, "WindowSettings" );
            }
        };
        typedef Helium::SmartPtr< Preferences > PreferencesPtr;

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