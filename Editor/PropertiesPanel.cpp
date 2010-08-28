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

    m_IntersectionButton->SetHelpText( TXT( "Selecting this will only show the properties that are common to all the selected items." ) );
    m_UnionButton->SetHelpText( TXT( "Selecting this will show all the properties for all the selected items." ) );

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
