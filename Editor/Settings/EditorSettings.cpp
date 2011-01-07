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
: m_LoadLastOpenedProjectOnStartup( true )
{
}

void GeneralSettings::AcceptCompositeVisitor( Reflect::Composite& comp )
{
    comp.AddField( &GeneralSettings::m_MRUProjects, TXT( "m_MRUProjects" ), Reflect::FieldFlags::Hide );

    Reflect::Field* field = comp.AddField( &GeneralSettings::m_LoadLastOpenedProjectOnStartup, TXT( "m_LoadLastOpenedProjectOnStartup" ) );
    field->SetProperty( TXT( "UIName"), TXT( "Load Last Opened Project On Startup" ) );
    field->SetProperty( TXT( "HelpText"), TXT( "If this is enabled, the editor will automatically load up the last project you were working on." ) );
}

std::vector< tstring >& GeneralSettings::GetMRUProjects()
{
    return m_MRUProjects;
}

void GeneralSettings::SetMRUProjects( MRU< tstring >* mru )
{
    mru->ToVector( m_MRUProjects );
}

bool GeneralSettings::GetLoadLastOpenedProjectOnStartup() const
{
    return m_LoadLastOpenedProjectOnStartup;
}

void GeneralSettings::SetLoadLastOpenedProjectOnStartup( bool value )
{
    m_LoadLastOpenedProjectOnStartup = value;
}
