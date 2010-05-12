#include "Precompile.h"
#include "Preferences.h"

#include "Common/CommandLine.h"
#include "File/Manager.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"


using namespace Luna;

// Definitions
REFLECT_DEFINE_ABSTRACT( Preferences )

void Preferences::EnumerateClass( Reflect::Compositor<Preferences>& comp )
{
}

const char* Preferences::s_ResetPreferences = "reset";
const char* Preferences::s_ResetPreferencesLong = "ResetPreferences";


namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // Helper function to convert a file ID to an appropriate string 
  // representation.
  // 
  std::string TuidToLabel( const tuid& fileID, const FilePathOption filePathOption )
  {
    std::string filePath;
    try
    {
      filePath = File::GlobalManager().GetPath( fileID );
    }
    catch ( const File::Exception& )
    {
      std::stringstream nameStream;
      nameStream << "<" << TUID::HexFormat << fileID << ">";
      return nameStream.str();
    }

    // Determine whether to show full path or not...
    if ( !filePath.empty() )
    {
      switch ( filePathOption )
      {
      case FilePathOptions::FileName:
        filePath = FileSystem::GetLeaf( filePath );
        FileSystem::StripExtension( filePath );
        break;

      case FilePathOptions::FileNamePlusExt:
        filePath = FileSystem::GetLeaf( filePath );
        break;

      case FilePathOptions::PartialPath:
        FileSystem::StripPrefix( Finder::ProjectAssets(), filePath );
        break;

      case FilePathOptions::FullPath:
        // Do nothing, it's already good to go
        break;
      }
    }

    return filePath;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void Preferences::InitializeType()
{
  Reflect::RegisterClass<Preferences>( "Preferences" );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void Preferences::CleanupType()
{
  Reflect::UnregisterClass<Preferences>();
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
Preferences::Preferences()
{
}

///////////////////////////////////////////////////////////////////////////////
// Save preferences to disk.  Derived classes just have to provide the path
// to the preferences file.
// 
void Preferences::SavePreferences()
{
  std::string error;
  if ( !SaveToFile( GetPreferencesPath(), error ) )
  {
    Console::Error( "%s\n", error.c_str() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Load preferences from disk.  Derived classes just have to provide the path
// to the preferences file.  This is via a virtual function, so you shouldn't
// call this function from a constructor.
// 
void Preferences::LoadPreferences()
{
  static bool promptReset = true;
  static bool resetPrefs = false;
  if ( Nocturnal::GetCmdLineFlag( Preferences::s_ResetPreferences ) )
  {
    if ( promptReset )
    {
      promptReset = false;

      if ( wxYES == wxMessageBox( "Are you sure that you want to reset all of your preferences?  This includes window positions and your 'most recently used' file list.", "Reset All Preferences?", wxCENTER | wxYES_NO ) )
      {
        resetPrefs = true;
      }
    }

    if ( resetPrefs )
    {
      return;
    }
  }

  std::string path = GetPreferencesPath();
  if ( !LoadFromFile( path ) )
  {
    Console::Debug( "Using default preferences; unable to load preferences from '%s'.\n", path.c_str() );
  }
}
