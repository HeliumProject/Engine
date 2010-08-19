#include "Precompile.h"
#include "Settings.h"

using namespace Helium;
using namespace Helium::Editor;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
Settings::Settings()
: m_SceneSettings( new Core::SceneSettings )
, m_ViewportSettings( new Core::ViewportSettings )
, m_GridSettings( new Core::GridSettings )
, m_VaultSettings( new VaultSettings )
, m_WindowSettings( new WindowSettings() )
{
}

/////////////////////////////////////////////////////////////////////////////
// Helper function to convert a file ref to an appropriate string 
// representation.
// 
tstring Editor::PathToLabel( const Helium::Path& path, const FilePathOption filePathOption )
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