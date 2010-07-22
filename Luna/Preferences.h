#pragma once

#include "Luna/API.h"

#include "Settings.h"
#include "Foundation/File/Path.h"

namespace Luna
{
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
  LUNA_EDITOR_API tstring PathToLabel( const Nocturnal::Path& path, const FilePathOption filePathOption );

  /////////////////////////////////////////////////////////////////////////////
  // Base class for preferenced within Luna.  Provides convenience functions
  // for saving and loading.
  // 
  class LUNA_EDITOR_API Preferences NOC_ABSTRACT : public Settings
  {
  public:
    static const tchar* s_ResetPreferences;
    static const tchar* s_ResetPreferencesLong;

  public:
    static void InitializeType();
    static void CleanupType();

    Preferences();

    virtual tstring GetPreferencesPath() const = 0;
    void SavePreferences();
    void LoadPreferences();

  public:
    REFLECT_DECLARE_ABSTRACT( Preferences, Settings );
    static void EnumerateClass( Reflect::Compositor<Preferences>& comp );
  };
  typedef Nocturnal::SmartPtr< Preferences > PreferencesPtr;
}