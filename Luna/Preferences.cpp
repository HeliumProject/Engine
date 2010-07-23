#include "Precompile.h"
#include "Preferences.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
Preferences::Preferences()
: m_ScenePreferences( new ScenePreferences )
, m_ViewportPreferences( new ViewportPreferences )
, m_GridPreferences( new GridPreferences )
, m_VaultPreferences( new VaultPreferences )
{
}

/////////////////////////////////////////////////////////////////////////////
// Helper function to convert a file ref to an appropriate string 
// representation.
// 
tstring Luna::PathToLabel( const Nocturnal::Path& path, const FilePathOption filePathOption )
{
    tstring filePath = path.Get();

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