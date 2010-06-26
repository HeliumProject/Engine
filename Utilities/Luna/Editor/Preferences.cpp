#include "Precompile.h"
#include "Preferences.h"

using namespace Luna;

/////////////////////////////////////////////////////////////////////////////
// Helper function to convert a file ref to an appropriate string 
// representation.
// 
std::string Luna::PathToLabel( const Nocturnal::Path& path, const FilePathOption filePathOption )
{
    std::string filePath = path.Get();

    // Determine whether to show full path or not...
    if ( !filePath.empty() )
    {
        switch ( filePathOption )
        {
        case FilePathOptions::Basename:
            filePath = path.Basename();
            break;

        case FilePathOptions::Filename:
            filePath = path.Filename();
            break;

        case FilePathOptions::PartialPath:
            //FileSystem::StripPrefix( Finder::ProjectAssets(), filePath );
#pragma TODO( "Make this aware of the project root somehow, maybe the application/program options" )
            break;

        case FilePathOptions::FullPath:
            // Do nothing, it's already good to go
            break;
        }
    }

    return filePath;
}

// Definitions
REFLECT_DEFINE_ABSTRACT( Preferences )


void Preferences::EnumerateClass( Reflect::Compositor<Preferences>& comp )
{
}

const char* Preferences::s_ResetPreferences = "reset";
const char* Preferences::s_ResetPreferencesLong = "ResetPreferences";


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
        Log::Error( "%s\n", error.c_str() );
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

#pragma TODO ("Shouldn't be using the command line here, it should be an option IN luna to reset prefs")
    //if ( Nocturnal::GetCmdLineFlag( Preferences::s_ResetPreferences ) )
    //{
    //    if ( promptReset )
    //    {
    //        promptReset = false;

    //        if ( wxYES == wxMessageBox( "Are you sure that you want to reset all of your preferences?  This includes window positions and your 'most recently used' file list.", "Reset All Preferences?", wxCENTER | wxYES_NO ) )
    //        {
    //            resetPrefs = true;
    //        }
    //    }

    //    if ( resetPrefs )
    //    {
    //        return;
    //    }
    //}

    std::string path = GetPreferencesPath();
    if ( !LoadFromFile( path ) )
    {
        Log::Debug( "Using default preferences; unable to load preferences from '%s'.\n", path.c_str() );
    }
}
