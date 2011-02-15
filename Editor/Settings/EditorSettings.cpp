#include "Precompile.h"
#include "EditorSettings.h"

#include <wx/aui/aui.h>
#include <wx/display.h>

using namespace Helium;
using namespace Helium::Editor;

REFLECT_DEFINE_ENUMERATION( IconSize );
REFLECT_DEFINE_OBJECT( EditorSettings );

EditorSettings::EditorSettings()
: m_ReopenLastProjectOnStartup( true )
, m_ShowFileExtensionsInProjectView( false )
, m_EnableAssetTracker( true )
, m_ShowTextOnButtons( false )
, m_ShowIconsOnButtons( true )
, m_IconSizeOnButtons( IconSize::Medium )
{
}

void EditorSettings::AcceptCompositeVisitor( Reflect::Composite& comp )
{
    comp.AddField( &EditorSettings::m_MRUProjects, TXT( "m_MRUProjects" ), Reflect::FieldFlags::Hide );

    Reflect::Field* field = comp.AddField( &EditorSettings::m_ReopenLastProjectOnStartup, TXT( "m_ReopenLastProjectOnStartup" ) );
    field->SetProperty( TXT( "UIName" ), TXT( "Reopen Last Project On Startup" ) );
    field->SetProperty( TXT( "HelpText" ), TXT( "If this is enabled, the editor will automatically load up the last project you were working on." ) );

    field = comp.AddField( &EditorSettings::m_ShowFileExtensionsInProjectView, TXT( "m_ShowFileExtensionsInProjectView" ) );
    field->SetProperty( TXT( "UIName" ), TXT( "Show File Extensions In Project View" ) );
    field->SetProperty( TXT( "HelpText" ), TXT( "If this is enabled, the editor will display the file extensions for files referenced in the project." ) );

    field = comp.AddField( &EditorSettings::m_EnableAssetTracker, TXT( "m_EnableAssetTracker" ) );
    field->SetProperty( TXT( "UIName" ), TXT( "Enable Asset Tracker" ) );
    field->SetProperty( TXT( "HelpText" ), TXT( "If this is enabled, the editor will find and index assets that can be used in the currently loaded project.  This allows for fast searches through your asset library." ) );

    field = comp.AddField( &EditorSettings::m_ShowTextOnButtons, TXT( "m_ShowTextOnButtons" ) );
    field->SetProperty( TXT( "UIName" ), TXT( "Display Text on Buttons" ) );
    field->SetProperty( TXT( "HelpText" ), TXT( "If this is enabled, the editor will display text on buttons in the UI." ) );

    field = comp.AddField( &EditorSettings::m_ShowIconsOnButtons, TXT( "m_ShowIconsOnButtons" ) );
    field->SetProperty( TXT( "UIName" ), TXT( "Display Icons on Buttons" ) );
    field->SetProperty( TXT( "HelpText" ), TXT( "If this is enabled, the editor will display icons on buttons in the UI." ) );

    field = comp.AddEnumerationField( &EditorSettings::m_IconSizeOnButtons, TXT( "m_IconSizeOnButtons" ) );
    field->SetProperty( TXT( "UIName" ), TXT( "Icon Size on Buttons" ) );
    field->SetProperty( TXT( "HelpText" ), TXT( "Select the size of the icon to display on buttons." ) );
    
}

std::vector< tstring >& EditorSettings::GetMRUProjects()
{
    return m_MRUProjects;
}

void EditorSettings::SetMRUProjects( MRU< tstring >* mru )
{
    mru->ToVector( m_MRUProjects );
}

bool EditorSettings::GetReopenLastProjectOnStartup() const
{
    return m_ReopenLastProjectOnStartup;
}

void EditorSettings::SetReopenLastProjectOnStartup( bool value )
{
    m_ReopenLastProjectOnStartup = value;
}

bool EditorSettings::GetShowFileExtensionsInProjectView() const
{
    return m_ShowFileExtensionsInProjectView;
}

void EditorSettings::SetShowFileExtensionsInProjectView( bool value )
{
    m_ShowFileExtensionsInProjectView = value;
}

bool EditorSettings::GetEnableAssetTracker() const
{
    return m_EnableAssetTracker;
}

void EditorSettings::SetEnableAssetTracker( bool value )
{
    m_EnableAssetTracker = value;
}
