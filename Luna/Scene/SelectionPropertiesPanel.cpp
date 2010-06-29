#include "Precompile.h"
#include "SelectionPropertiesPanel.h"

#include "Core/Enumerator.h"
#include "Core/PropertiesManager.h"

using namespace Luna;

LSelectionPropertiesPanel::LSelectionPropertiesPanel( PropertiesManager* propertiesManager, wxWindow* parent, int id, wxPoint pos, wxSize size, int style )
: wxPanel( parent, id, pos, size, style )
, m_PropertyManager ( propertiesManager )
{
  wxBoxSizer* mainSizer;
  mainSizer = new wxBoxSizer( wxVERTICAL );

  wxStaticBoxSizer* groupBox;
  groupBox = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxEmptyString ), wxHORIZONTAL );

  m_Intersection = new wxRadioButton( this, wxID_ANY, wxT("Common"), wxDefaultPosition, wxDefaultSize, 0);
  m_Intersection->SetValue(true);
  m_Intersection->Connect( m_Intersection->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( LSelectionPropertiesPanel::OnIntersection ), NULL, this );
  groupBox->Add( m_Intersection, 0, wxALL, 5 );

  m_Union = new wxRadioButton( this, wxID_ANY, wxT("All"), wxDefaultPosition, wxDefaultSize, 0);
  m_Union->SetValue(false);
  m_Union->Connect( m_Union->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( LSelectionPropertiesPanel::OnUnion ), NULL, this );
  groupBox->Add( m_Union, 0, wxALL, 5 );

  groupBox->Add( 0, 0, 1, wxEXPAND, 5 );

  m_ExpandAll = new wxButton( this, wxID_ANY, wxT("Expand All"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
  m_ExpandAll->Connect( m_ExpandAll->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LSelectionPropertiesPanel::OnExpandAll ), NULL, this );
  groupBox->Add( m_ExpandAll, 0, wxALL, 1 );

  m_CollapseAll = new wxButton( this, wxID_ANY, wxT("Collapse All"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
  m_CollapseAll->Connect( m_CollapseAll->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LSelectionPropertiesPanel::OnCollapseAll ), NULL, this );
  groupBox->Add( m_CollapseAll, 0, wxALL, 1 );

  mainSizer->Add( groupBox, 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 5 );

  wxBoxSizer* bottomSizer;
  bottomSizer = new wxBoxSizer( wxHORIZONTAL );

  m_PropertyCanvas = new Inspect::CanvasWindow (this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN);	
  bottomSizer->Add( m_PropertyCanvas, 1, wxEXPAND | wxALL, 0 );

  mainSizer->Add( bottomSizer, 1, wxEXPAND, 5 );

  this->SetSizer( mainSizer );
  this->Layout();
}

void LSelectionPropertiesPanel::OnIntersection(wxCommandEvent& event)
{
  m_PropertyManager->SetProperties( PropertySettings::Intersection );
}

void LSelectionPropertiesPanel::OnUnion(wxCommandEvent& event)
{
  m_PropertyManager->SetProperties( PropertySettings::Union );
}

void LSelectionPropertiesPanel::OnExpandAll(wxCommandEvent& event)
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

void LSelectionPropertiesPanel::OnCollapseAll(wxCommandEvent& event)
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
