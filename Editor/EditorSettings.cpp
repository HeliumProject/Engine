#include "Precompile.h"
#include "EditorSettings.h"

#include <wx/aui/aui.h>
#include <wx/display.h>

using namespace Helium;
using namespace Helium::Editor;

///////////////////////////////////////////////////////////////////////////////
// GeneralSettings
//
REFLECT_DEFINE_CLASS( GeneralSettings );

GeneralSettings::GeneralSettings()
{
}

std::vector< tstring >& GeneralSettings::GetMRUProjects()
{
    //std::vector< tstring > paths;
    //std::vector< tstring >::const_iterator itr = wxGetApp().GetSettings()->GetMRU()->GetPaths().begin();
    //std::vector< tstring >::const_iterator end = wxGetApp().GetSettings()->GetMRU()->GetPaths().end();
    //for ( ; itr != end; ++itr )
    //{
    //    Helium::Path path( *itr );
    //    if ( path.Exists() )
    //    {
    //        paths.push_back( *itr );
    //    }
    //}
    return m_MRUProjects;
}

void GeneralSettings::SetMRUProjects( MRU< tstring >* mru )
{
    mru->ToVector( m_MRUProjects );
}