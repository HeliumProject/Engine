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
, m_ShowFileExtensionsInProjectView( false )
{
}

void GeneralSettings::AcceptCompositeVisitor( Reflect::Composite& comp )
{
    comp.AddField( &GeneralSettings::m_MRUProjects, "m_MRUProjects", Reflect::FieldFlags::Hide );

    Reflect::Field* field = comp.AddField( &GeneralSettings::m_LoadLastOpenedProjectOnStartup, "m_LoadLastOpenedProjectOnStartup" );
    field->SetProperty( TXT( "UIName"), TXT( "Load Last Opened Project On Startup" ) );
    field->SetProperty( TXT( "HelpText"), TXT( "If this is enabled, the editor will automatically load up the last project you were working on." ) );

    field = comp.AddField( &GeneralSettings::m_ShowFileExtensionsInProjectView, "m_ShowFileExtensionsInProjectView" );
    field->SetProperty( TXT( "UIName"), TXT( "Show File Extensions In Project View" ) );
    field->SetProperty( TXT( "HelpText"), TXT( "If this is enabled, the editor will display the file extensions for files referenced in the project." ) );
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

bool GeneralSettings::GetShowFileExtensionsInProjectView() const
{
    return m_ShowFileExtensionsInProjectView;
}

void GeneralSettings::SetShowFileExtensionsInProjectView( bool value )
{
    m_ShowFileExtensionsInProjectView = value;
}
