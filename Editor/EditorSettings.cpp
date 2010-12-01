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
    return m_MRUProjects;
}

void GeneralSettings::SetMRUProjects( MRU< tstring >* mru )
{
    mru->ToVector( m_MRUProjects );
}