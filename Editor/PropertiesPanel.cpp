#include "Precompile.h"

#include "PropertiesPanel.h"

using namespace Helium;
using namespace Helium::Core;
using namespace Helium::Editor;

PropertiesPanel::PropertiesPanel( PropertiesManager* manager, wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style )
: PropertiesPanelGenerated( parent, id, pos, size, style )
, m_PropertiesManager( manager )
{
    SetHelpText( TXT( "This is the Properties Panel.  It will contain the properties for the items you have selected.  It has two modes:\n Common - Only display the properties common to all the selected items.\n All - Display all properties, regardless of if they are shared by the selected items." ) );

    m_CommonButton->SetHelpText( TXT( "Selecting this will only show the properties that are common to all the selected items." ) );
    m_AllButton->SetHelpText( TXT( "Selecting this will show all the properties for all the selected items." ) );

    m_ExpandAllButton->SetHelpText( TXT( "Clicking this button will expand all properties in the property hierarchy below." ) );
    m_CollapseAllButton->SetHelpText( TXT( "Clicking this button will collapse all properties in the property hierarchy below." ) );

    m_PropertyCanvas = new Inspect::TreeCanvasCtrl ( m_PropertiesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN );
    m_PropertiesPanel->GetSizer()->Add( m_PropertyCanvas, 1, wxEXPAND | wxALL, 0 );
    m_PropertyCanvas->SetHelpText( TXT( "This is the property canvas.  It displays properties for the selected objects." ) );
}

void PropertiesPanel::OnIntersection(wxCommandEvent& event)
{
    m_PropertiesManager->SetProperties( PropertySettings::Intersection );
}

void PropertiesPanel::OnUnion(wxCommandEvent& event)
{
    m_PropertiesManager->SetProperties( PropertySettings::Union );
}

void PropertiesPanel::OnExpandAll(wxCommandEvent& event)
{
    Inspect::Canvas* canvas = m_PropertyCanvas->GetCanvas();

    Inspect::V_Control::const_iterator itr = canvas->GetControls().begin();
    Inspect::V_Control::const_iterator end = canvas->GetControls().end();
    for ( ; itr != end; ++itr )
    {
        Inspect::Panel* panel = Reflect::ObjectCast<Inspect::Panel>( *itr );
        if (panel)
        {
            panel->SetExpandedRecursive( true, true );
        }
    }

    canvas->Layout();
}

void PropertiesPanel::OnCollapseAll(wxCommandEvent& event)
{
    Inspect::Canvas* canvas = m_PropertyCanvas->GetCanvas();

    Inspect::V_Control::const_iterator itr = canvas->GetControls().begin();
    Inspect::V_Control::const_iterator end = canvas->GetControls().end();
    for ( ; itr != end; ++itr )
    {
        Inspect::Panel* panel = Reflect::ObjectCast<Inspect::Panel>( *itr );
        if (panel)
        {
            panel->SetExpandedRecursive( false, true );
        }
    }

    canvas->Layout();
}
