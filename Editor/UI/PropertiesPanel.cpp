#include "Precompile.h"

#include "PropertiesPanel.h"

using namespace Helium;
using namespace Helium::Editor;

PropertiesPanel::PropertiesPanel( PropertiesManager* manager, wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style )
: PropertiesPanelGenerated( parent, id, pos, size, style )
, m_PropertiesManager( manager )
{
  m_PropertyCanvas = new Inspect::CanvasWindow ( m_PropertiesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN );
  m_PropertiesPanel->GetSizer()->Add( m_PropertyCanvas, 1, wxEXPAND | wxALL, 0 );
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
