#include "Precompile.h"
#include "EditorSettings.h"

#include <wx/aui/aui.h>
#include <wx/display.h>

using namespace Helium;
using namespace Helium::Editor;

///////////////////////////////////////////////////////////////////////////////
// GeneralSettings
//
REFLECT_DEFINE_OBJECT( GeneralSettings );

GeneralSettings::GeneralSettings()
: m_ReopenLastProjectOnStartup( true )
, m_ShowFileExtensionsInProjectView( false )
{
}

void GeneralSettings::AcceptCompositeVisitor( Reflect::Composite& comp )
{
    comp.AddField( &GeneralSettings::m_MRUProjects, TXT( "m_MRUProjects" ), Reflect::FieldFlags::Hide );

    Reflect::Field* field = comp.AddField( &GeneralSettings::m_ReopenLastProjectOnStartup, TXT( "m_ReopenLastProjectOnStartup" ) );
    field->SetProperty( TXT( "UIName" ), TXT( "Reopen Last Project On Startup" ) );
    field->SetProperty( TXT( "HelpText" ), TXT( "If this is enabled, the editor will automatically load up the last project you were working on." ) );

    field = comp.AddField( &GeneralSettings::m_ShowFileExtensionsInProjectView, TXT( "m_ShowFileExtensionsInProjectView" ) );
    field->SetProperty( TXT( "UIName" ), TXT( "Show File Extensions In Project View" ) );
    field->SetProperty( TXT( "HelpText" ), TXT( "If this is enabled, the editor will display the file extensions for files referenced in the project." ) );
}

std::vector< tstring >& GeneralSettings::GetMRUProjects()
{
    return m_MRUProjects;
}

void GeneralSettings::SetMRUProjects( MRU< tstring >* mru )
{
    mru->ToVector( m_MRUProjects );
}

bool GeneralSettings::GetReopenLastProjectOnStartup() const
{
    return m_ReopenLastProjectOnStartup;
}

void GeneralSettings::SetReopenLastProjectOnStartup( bool value )
{
    m_ReopenLastProjectOnStartup = value;
}

bool GeneralSettings::GetShowFileExtensionsInProjectView() const
{
    return m_ShowFileExtensionsInProjectView;
}

void GeneralSettings::SetShowFileExtensionsInProjectView( bool value )
{
    m_ShowFileExtensionsInProjectView = value;
}
