#pragma once

#include "Editor/API.h"

#include "Settings.h"
#include "Scene/ScenePreferences.h"
#include "Scene/ViewportPreferences.h"
#include "Scene/GridPreferences.h"
#include "Vault/VaultPreferences.h"

namespace Editor
{
  /////////////////////////////////////////////////////////////////////////////
  // Base class for preferenced within Editor.  Provides convenience functions
  // for saving and loading.
  // 
  class LUNA_EDITOR_API Preferences : public Reflect::ConcreteInheritor< Preferences, Settings >
  {
  public:
    Preferences();

    ScenePreferences* GetScenePreferences()
    {
        return m_ScenePreferences;
    }

    ViewportPreferences* GetViewportPreferences()
    {
        return m_ViewportPreferences;
    }

    GridPreferences* GetGridPreferences()
    {
        return m_GridPreferences;
    }

    VaultPreferences* GetVaultPreferences()
    {
        return m_VaultPreferences;
    }

  private:
    ScenePreferencesPtr m_ScenePreferences;
    ViewportPreferencesPtr m_ViewportPreferences;
    GridPreferencesPtr m_GridPreferences;
    VaultPreferencesPtr m_VaultPreferences;

  public:
    static void EnumerateClass( Reflect::Compositor<Preferences>& comp )
    {
        comp.AddField( &Preferences::m_ScenePreferences, "ScenePreferences" );
        comp.AddField( &Preferences::m_ViewportPreferences, "ViewportPreferences"  );
        comp.AddField( &Preferences::m_GridPreferences, "GridPreferences" );
        comp.AddField( &Preferences::m_VaultPreferences, "VaultPreferences" );
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

    static void FilePathOptionEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(Basename, TXT( "Basename" ) );
      info->AddElement(Filename, TXT( "Filename" ) );
      info->AddElement(PartialPath, TXT( "PartialPath" ) );
      info->AddElement(FullPath, TXT( "FullPath" ) );
    }
  };
  typedef FilePathOptions::FilePathOption FilePathOption;

  // Get the UI label for a file path based upon the specified FilePathOption
  LUNA_EDITOR_API tstring PathToLabel( const Helium::Path& path, const FilePathOption filePathOption );
}