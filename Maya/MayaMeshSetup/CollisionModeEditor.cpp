#include "stdafx.h"

#include "CollisionModeEditor.h"
#include "CollisionModeDisplayNode.h"
#include "CollisionModePanel.h"

#include <wx/tooltip.h>

using namespace Maya;


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
CollisionModeEditor::CollisionModeEditor( wxWindow* parent, wxPoint pos, wxSize size ) 
: wxDialog( parent, wxID_ANY, "Collision Mode Editor", pos, size, wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL, "Insomniac CollisionModeEditor" )
, m_Panel( NULL )
, m_StatusBar( NULL )
, m_CurrentLegend( NULL )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Panel = new CollisionModePanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
  
  wxSize minSize = m_Panel->GetMinSize() + wxSize( 30, 40 );
  SetMinSize( minSize );
  SetSize( minSize );
	SetSizeHints( minSize, wxDefaultSize );
	
  mainSizer->Add( m_Panel, 1, wxEXPAND | wxALL, 0 );

  int statusFieldWidths[1] = { -1 };
  m_StatusBar = new wxStatusBar( this, wxID_ANY, wxST_SIZEGRIP );
  mainSizer->Add( m_StatusBar, 0, wxEXPAND, 0 );
	
	SetSizer( mainSizer );
	Layout();

  // Init radio buttons
  m_Panel->m_RadioOff->SetValue( true );

  // Connect listeners
  m_Panel->m_ButtonApply->Connect( m_Panel->m_ButtonApply->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CollisionModeEditor::OnButtonClickApply ), NULL, this );
  m_Panel->m_ButtonClear->Connect( m_Panel->m_ButtonClear->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CollisionModeEditor::OnButtonClickClear ), NULL, this );

  m_Panel->m_RadioOff->Connect( m_Panel->m_RadioOff->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( CollisionModeEditor::OnRadioOff ), NULL, this );
  m_Panel->m_RadioCameraIgnore->Connect( m_Panel->m_RadioCameraIgnore->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( CollisionModeEditor::OnRadioCameraIgnore ), NULL, this );
  m_Panel->m_RadioImpassable->Connect( m_Panel->m_RadioImpassable->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( CollisionModeEditor::OnRadioImpassable ), NULL, this );
  m_Panel->m_RadioNonTraversable->Connect( m_Panel->m_RadioNonTraversable->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( CollisionModeEditor::OnRadioNonTraversable ), NULL, this );
  m_Panel->m_RadioTransparent->Connect( m_Panel->m_RadioTransparent->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( CollisionModeEditor::OnRadioTransparent ), NULL, this );

  Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( CollisionModeEditor::OnCloseEvent ) );

  // Connect additional listeners for displaying tooltips in the status bar
#pragma TODO( "Hookup tooltips to show up on the status bar" )
  //m_Panel->m_ButtonApply->Connect( m_Panel->m_ButtonApply->GetId(), wxEVT_MOTION, wxMouseEventHandler( CollisionModeEditor::OnMouseEnter ), NULL, this );
  //m_Panel->m_ButtonApply->Connect( m_Panel->m_ButtonApply->GetId(), wxEVT_LEAVE_WINDOW, wxMouseEventHandler( CollisionModeEditor::OnMouseLeave ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
CollisionModeEditor::~CollisionModeEditor()
{
  // Disconnect listeners
  m_Panel->m_ButtonApply->Disconnect( m_Panel->m_ButtonApply->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CollisionModeEditor::OnButtonClickApply ), NULL, this );
  m_Panel->m_ButtonClear->Disconnect( m_Panel->m_ButtonClear->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CollisionModeEditor::OnButtonClickClear ), NULL, this );

  m_Panel->m_RadioCameraIgnore->Disconnect( m_Panel->m_RadioCameraIgnore->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( CollisionModeEditor::OnRadioCameraIgnore ), NULL, this );
  m_Panel->m_RadioImpassable->Disconnect( m_Panel->m_RadioImpassable->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( CollisionModeEditor::OnRadioImpassable ), NULL, this );
  m_Panel->m_RadioNonTraversable->Disconnect( m_Panel->m_RadioNonTraversable->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( CollisionModeEditor::OnRadioNonTraversable ), NULL, this );
  m_Panel->m_RadioTransparent->Disconnect( m_Panel->m_RadioNonTraversable->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( CollisionModeEditor::OnRadioTransparent ), NULL, this );

  // Disconnect tool tip listeners
  //m_Panel->m_ButtonApply->Disconnect( m_Panel->m_ButtonApply->GetId(), wxEVT_MOTION, wxMouseEventHandler( CollisionModeEditor::OnMouseEnter ), NULL, this );
  //m_Panel->m_ButtonApply->Disconnect( m_Panel->m_ButtonApply->GetId(), wxEVT_LEAVE_WINDOW, wxMouseEventHandler( CollisionModeEditor::OnMouseLeave ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Sets the text displayed on the status bar of this window.
// 
void CollisionModeEditor::SetStatusText( const wxString& text )
{
  m_StatusBar->SetStatusText( text );
}

///////////////////////////////////////////////////////////////////////////////
// Fills out the specified structure with the current settings from this window.
// 
CollisionMode CollisionModeEditor::GetCollisionModeSettings()
{
  CollisionMode collisionMode = CollisionModes::Default;

	if ( m_Panel->m_CheckCameraIgnore->IsChecked() )
  {
    collisionMode |= CollisionModes::CameraIgnore;
  }

	if ( m_Panel->m_CheckImpassable->IsChecked() )
  {
    collisionMode |= CollisionModes::Impassable;
  }

	if ( m_Panel->m_CheckNonTraversable->IsChecked() )
  {
    collisionMode |= CollisionModes::NonTraversable;
  }

	if ( m_Panel->m_CheckTransparent->IsChecked() )
  {
    collisionMode |= CollisionModes::Transparent;
  }

  return collisionMode;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the value of all radio buttons to false, with the exception of 'except'
// which has its value set to true.  This is necessary because we have to 
// manually handle the exclusivity of the radio buttons when they are used
// within Maya to prevent a hang bug.  
// 
// From the wxWidgets documentation:
//    In some circumstances, radio buttons that are not consecutive siblings 
//    trigger a hang bug in Windows. If this happens, add the wxRB_SINGLE style 
//    to mark the button as not belonging to a group, and implement the 
//    mutually-exclusive group behaviour yourself.  
//    
// We could avoid this by using a wxRadioGroup, but then we lose control over
// how the buttons are laid out.
// 
void CollisionModeEditor::ClearRadioButtons( wxRadioButton* except )
{
  m_Panel->m_RadioOff->SetValue( m_Panel->m_RadioOff == except );
  m_Panel->m_RadioCameraIgnore->SetValue( m_Panel->m_RadioCameraIgnore == except );
  m_Panel->m_RadioImpassable->SetValue( m_Panel->m_RadioImpassable == except );
  m_Panel->m_RadioNonTraversable->SetValue( m_Panel->m_RadioNonTraversable == except );
  m_Panel->m_RadioTransparent->SetValue( m_Panel->m_RadioTransparent == except );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the apply button is clicked.  Takes the current settings
// in the combo boxes and applies them to the currently selected objects.
// 
void CollisionModeEditor::OnButtonClickApply( wxCommandEvent& event )
{
  CollisionMode collisionMode = GetCollisionModeSettings();
  CollisionModeDisplayNode::AssignCollisionModeToFaces( collisionMode );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the clear button is clicked.  Removes all collision settings
// from the selected objects.
// 
void CollisionModeEditor::OnButtonClickClear( wxCommandEvent& event )
{
  CollisionModeDisplayNode::ClearCollisionModeFromFaces();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the Off radio button is selected.  Clears the color legend.
// 
void CollisionModeEditor::OnRadioOff( wxCommandEvent& event )
{
  ClearRadioButtons( m_Panel->m_RadioOff );
  CollisionModeDisplayNode::UpdateDisplay( CollisionModeDisplayFlags::Off );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the CameraIgnore radio button is selected.  Updates the color 
// legend.
// 
void CollisionModeEditor::OnRadioCameraIgnore( wxCommandEvent& event )
{
  ClearRadioButtons( m_Panel->m_RadioCameraIgnore );
  CollisionModeDisplayNode::UpdateDisplay( CollisionModeDisplayFlags::CameraIgnore );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the Impassable radio button is selected.  Updates the color 
// legend.
// 
void CollisionModeEditor::OnRadioImpassable( wxCommandEvent& event )
{
  ClearRadioButtons( m_Panel->m_RadioImpassable );
  CollisionModeDisplayNode::UpdateDisplay( CollisionModeDisplayFlags::Impassable );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the NonTraversable radio button is selected.  Updates the color 
// legend.
// 
void CollisionModeEditor::OnRadioNonTraversable( wxCommandEvent& event )
{
  ClearRadioButtons( m_Panel->m_RadioNonTraversable );
  CollisionModeDisplayNode::UpdateDisplay( CollisionModeDisplayFlags::NonTraversable );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the Transparent radio button is selected.  Updates the color 
// legend.
// 
void CollisionModeEditor::OnRadioTransparent( wxCommandEvent& event )
{
  ClearRadioButtons( m_Panel->m_RadioTransparent );
  CollisionModeDisplayNode::UpdateDisplay( CollisionModeDisplayFlags::Transparent );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the mouse enters a sub window.  Updates the status text
// with the tool tip, since tool tips don't show up in Maya.
// 
void CollisionModeEditor::OnMouseEnter( wxMouseEvent& event )
{
  event.Skip();
  wxWindow* window = wxStaticCast( event.GetEventObject(), wxWindow );

  if ( window )
  {
    wxToolTip* tooltip = window->GetToolTip();
    if ( tooltip )
    {
      SetStatusText( tooltip->GetTip() );
    }
    else
    {
      SetStatusText( wxEmptyString );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the mouse leaves a sub window.  Clears the status text
// which was set when the tool tip entered the window.
// 
void CollisionModeEditor::OnMouseLeave( wxMouseEvent& event )
{
  event.Skip();
  SetStatusText( wxEmptyString );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the close button is pressed.  Simply hides this window.
// Destruction is handled by the owner of this object.
// 
void CollisionModeEditor::OnCloseEvent( wxCloseEvent& event )
{
  Hide();
  
  ClearRadioButtons( m_Panel->m_RadioOff );
  //CollisionModeDisplayNode::DeleteDisplayNode();
}
