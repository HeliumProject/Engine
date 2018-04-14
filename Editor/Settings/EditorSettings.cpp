#include "EditorPch.h"
#include "EditorSettings.h"

#include <wx/aui/aui.h>
#include <wx/display.h>

HELIUM_DEFINE_ENUM( Helium::Editor::IconSize );
HELIUM_DEFINE_CLASS( Helium::Editor::EditorSettings );

using namespace Helium;
using namespace Helium::Editor;

EditorSettings::EditorSettings()
: m_ReopenLastProjectOnStartup( true )
, m_ShowFileExtensionsInProjectView( false )
, m_EnableAssetTracker( true )
, m_ShowTextOnButtons( false )
, m_ShowIconsOnButtons( true )
, m_IconSizeOnButtons( IconSize::Medium )
{
}

void EditorSettings::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField( &EditorSettings::m_MRUProjects, "m_MRUProjects", Reflect::FieldFlags::Hide );

    Reflect::Field* field = NULL;

    field = comp.AddField( &EditorSettings::m_ReopenLastProjectOnStartup, "m_ReopenLastProjectOnStartup" );
    field->SetProperty( "UIName", "Reopen Last Project On Startup" );
    field->SetProperty( "HelpText", "If this is enabled, the editor will automatically load up the last project you were working on." );

    field = comp.AddField( &EditorSettings::m_ShowFileExtensionsInProjectView, "m_ShowFileExtensionsInProjectView" );
    field->SetProperty( "UIName", "Show File Extensions In Project View" );
    field->SetProperty( "HelpText", "If this is enabled, the editor will display the file extensions for files referenced in the project." );

    field = comp.AddField( &EditorSettings::m_EnableAssetTracker, "m_EnableAssetTracker" );
    field->SetProperty( "UIName", "Enable Asset Tracker" );
    field->SetProperty( "HelpText", "If this is enabled, the editor will find and index assets that can be used in the currently loaded project.  This allows for fast searches through your asset library." );

    field = comp.AddField( &EditorSettings::m_ShowTextOnButtons, "m_ShowTextOnButtons" );
    field->SetProperty( "UIName", "Display Text on Buttons" );
    field->SetProperty( "HelpText", "If this is enabled, the editor will display text on buttons in the UI." );

    field = comp.AddField( &EditorSettings::m_ShowIconsOnButtons, "m_ShowIconsOnButtons" );
    field->SetProperty( "UIName", "Display Icons on Buttons" );
    field->SetProperty( "HelpText", "If this is enabled, the editor will display icons on buttons in the UI." );

    field = comp.AddField( &EditorSettings::m_IconSizeOnButtons, "m_IconSizeOnButtons" );
    field->SetProperty( "UIName", "Icon Size on Buttons" );
    field->SetProperty( "HelpText", "Select the size of the icon to display on buttons." );
    
}

std::vector< std::string >& EditorSettings::GetMRUProjects()
{
    return m_MRUProjects;
}

void EditorSettings::SetMRUProjects( MRU< std::string >* mru )
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
