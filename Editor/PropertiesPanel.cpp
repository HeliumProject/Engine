#include "EditorPch.h"

#include "PropertiesPanel.h"

using namespace Helium;
using namespace Helium::Editor;

PropertiesPanel::PropertiesPanel( wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style )
: PropertiesPanelGenerated( parent, id, pos, size, style )
, m_CommandQueue( this )
, m_PropertiesGenerator( &m_PropertiesCanvas )
, m_PropertiesManager( &m_PropertiesGenerator, &m_CommandQueue )
{
	m_PropertiesGenerator.IncrRefCount();
    m_PropertiesCanvas.SetTreeWndCtrl( m_TreeWndCtrl );

    SetHelpText( TXT( "This is the Properties Panel.  It will contain the properties for the items you have selected.  It has two modes:\n Common - Only display the properties common to all the selected items.\n All - Display all properties, regardless of if they are shared by the selected items." ) );

    m_IntersectionButton->SetHelpText( TXT( "Selecting this will only show the properties that are common to all the selected items." ) );
    m_UnionButton->SetHelpText( TXT( "Selecting this will show all the properties for all the selected items." ) );
}

void PropertiesPanel::OnIntersection(wxCommandEvent& event)
{
    m_PropertiesManager.SetProperties( PropertiesStyles::Intersection );
}

void PropertiesPanel::OnUnion(wxCommandEvent& event)
{
    m_PropertiesManager.SetProperties( PropertiesStyles::Union );
}
