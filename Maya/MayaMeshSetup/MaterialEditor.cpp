#include "stdafx.h"
#include "MaterialEditor.h"
#include "MaterialEditorCmd.h"
#include "MaterialDisplayNode.h"
#include "MaterialEditorPanel.h"
#include "MeshSetupDisplayNode.h"

#include "Content/Material.h"
#include "Symbol/SymbolBuilder.h"
#include "UIToolKit/AutoCompleteComboBox.h"
#include <wx/tooltip.h>

#include <algorithm>
using namespace Maya;


///////////////////////////////////////////////////////////////////////////////
// Helper function to remove the specified prefix from the value and return a 
// new string.
// 
static inline std::string StripPrefix( Content::MaterialComponent materialComponent, const std::string& value, bool format = false )
{
  std::string result( value );

  if ( value.find( Content::MaterialComponentPrefixes[materialComponent] ) == 0 )
  {
    result = value.substr( strlen( Content::MaterialComponentPrefixes[materialComponent] ) );
  }

  if ( format && result.length() > 1 )
  {
    std::transform( ++result.begin(), result.end(), ++result.begin(), tolower );
  }

  return result;
}


static inline void AddPrefix( Content::MaterialComponent materialComponent, std::string& value, bool format = false )
{
  Content::Material::AddPrefix( materialComponent, value );

  if ( format && value.length() > 1 )
  {
    std::transform( value.begin(), value.end(), value.begin(), toupper );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
MaterialEditor::MaterialEditor( wxWindow* parent, wxPoint pos, wxSize size ) 
: wxDialog( parent, wxID_ANY, "Material Editor", pos, size, wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL, "Insomniac MaterialEditor" )
, m_Panel( NULL )
, m_StatusBar( NULL )
, m_CurrentLegend( NULL )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Panel = new MaterialEditorPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
  
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

  // Fetch enums
  m_VisualEnum = Symbol::SymbolBuilder::GetInstance()->FindEnum( Content::MaterialComponentSymbols[Content::MaterialComponents::Substance] );
  m_AcousticEnum = Symbol::SymbolBuilder::GetInstance()->FindEnum( Content::MaterialComponentSymbols[Content::MaterialComponents::Acoustic] );
  m_PhysicsEnum = Symbol::SymbolBuilder::GetInstance()->FindEnum( Content::MaterialComponentSymbols[Content::MaterialComponents::Physics] );

  // Populate combo boxes
  RefreshComboBox( m_Panel->m_ComboVisual, m_VisualEnum, (int) Content::MaterialComponents::Substance );
  RefreshComboBox( m_Panel->m_VisualTabCombo, m_VisualEnum, (int) Content::MaterialComponents::Substance );

  RefreshComboBox( m_Panel->m_ComboAcoustic, m_AcousticEnum, (int) Content::MaterialComponents::Acoustic );
  RefreshComboBox( m_Panel->m_AcousticTabCombo, m_AcousticEnum, (int) Content::MaterialComponents::Acoustic );

  RefreshComboBox( m_Panel->m_ComboPhysics, m_PhysicsEnum, (int) Content::MaterialComponents::Physics );
  RefreshComboBox( m_Panel->m_PhysicsTabCombo, m_PhysicsEnum, (int) Content::MaterialComponents::Physics );

  // Init radio buttons
  m_Panel->m_RadioOff->SetValue( true );
  RefreshColorLegend( NULL, (int) Content::MaterialComponents::Unknown );

  m_Panel->m_Tabs->SetSelection( 0 );

  // Connect listeners
  m_Panel->m_ButtonApply->Connect( m_Panel->m_ButtonApply->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MaterialEditor::OnButtonClickApply ), NULL, this );
  m_Panel->m_ButtonClear->Connect( m_Panel->m_ButtonClear->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MaterialEditor::OnButtonClickClear ), NULL, this );
  m_Panel->m_ButtonSelect->Connect( m_Panel->m_ButtonSelect->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MaterialEditor::OnButtonClickSelect ), NULL, this );
  
  m_Panel->m_RadioOff->Connect( m_Panel->m_RadioOff->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MaterialEditor::OnRadioOff ), NULL, this );
  m_Panel->m_RadioVisualTabOff->Connect( m_Panel->m_RadioVisualTabOff->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MaterialEditor::OnRadioOff ), NULL, this );
  m_Panel->m_RadioAcousticTabOff->Connect( m_Panel->m_RadioAcousticTabOff->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MaterialEditor::OnRadioOff ), NULL, this );
  m_Panel->m_RadioPhysicsTabOff->Connect( m_Panel->m_RadioPhysicsTabOff->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MaterialEditor::OnRadioOff ), NULL, this );
  
  m_Panel->m_RadioVisual->Connect( m_Panel->m_RadioVisual->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MaterialEditor::OnRadioVisual ), NULL, this );
  m_Panel->m_RadioVisualTabOn->Connect( m_Panel->m_RadioVisualTabOn->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MaterialEditor::OnRadioVisual ), NULL, this );
  
  m_Panel->m_RadioAcoustic->Connect( m_Panel->m_RadioAcoustic->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MaterialEditor::OnRadioAcoustic ), NULL, this );
  m_Panel->m_RadioAcousticTabOn->Connect( m_Panel->m_RadioAcousticTabOn->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MaterialEditor::OnRadioAcoustic ), NULL, this );
  
  m_Panel->m_RadioPhysics->Connect( m_Panel->m_RadioPhysics->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MaterialEditor::OnRadioPhysics ), NULL, this );
  m_Panel->m_RadioPhysicsTabOn->Connect( m_Panel->m_RadioPhysicsTabOn->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MaterialEditor::OnRadioPhysics ), NULL, this );
  
  m_Panel->m_Tabs->Connect( m_Panel->m_Tabs->GetId(), wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( MaterialEditor::OnNotebookPageChanged ), NULL, this );

  Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MaterialEditor::OnCloseEvent ) );

  // Connect additional listeners for displaying tooltips in the status bar
#pragma TODO( "Hookup tooltips to show up on the status bar" )
  //m_Panel->m_ButtonApply->Connect( m_Panel->m_ButtonApply->GetId(), wxEVT_MOTION, wxMouseEventHandler( MaterialEditor::OnMouseEnter ), NULL, this );
  //m_Panel->m_ButtonApply->Connect( m_Panel->m_ButtonApply->GetId(), wxEVT_LEAVE_WINDOW, wxMouseEventHandler( MaterialEditor::OnMouseLeave ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
MaterialEditor::~MaterialEditor()
{
  // Disconnect listeners
  m_Panel->m_ButtonApply->Disconnect( m_Panel->m_ButtonApply->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MaterialEditor::OnButtonClickApply ), NULL, this );
  m_Panel->m_ButtonClear->Disconnect( m_Panel->m_ButtonClear->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MaterialEditor::OnButtonClickClear ), NULL, this );
  m_Panel->m_ButtonSelect->Disconnect( m_Panel->m_ButtonSelect->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MaterialEditor::OnButtonClickSelect ), NULL, this );
  
  m_Panel->m_RadioOff->Disconnect( m_Panel->m_RadioOff->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MaterialEditor::OnRadioOff ), NULL, this );
  m_Panel->m_RadioVisualTabOff->Disconnect( m_Panel->m_RadioVisualTabOff->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MaterialEditor::OnRadioOff ), NULL, this );
  m_Panel->m_RadioAcousticTabOff->Disconnect( m_Panel->m_RadioAcousticTabOff->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MaterialEditor::OnRadioOff ), NULL, this );
  m_Panel->m_RadioPhysicsTabOff->Disconnect( m_Panel->m_RadioPhysicsTabOff->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MaterialEditor::OnRadioOff ), NULL, this );
  
  m_Panel->m_RadioVisual->Disconnect( m_Panel->m_RadioVisual->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MaterialEditor::OnRadioVisual ), NULL, this );
  m_Panel->m_RadioVisualTabOn->Disconnect( m_Panel->m_RadioVisualTabOn->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MaterialEditor::OnRadioVisual ), NULL, this );
  
  m_Panel->m_RadioAcoustic->Disconnect( m_Panel->m_RadioAcoustic->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MaterialEditor::OnRadioAcoustic ), NULL, this );
  m_Panel->m_RadioAcousticTabOn->Disconnect( m_Panel->m_RadioAcousticTabOn->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MaterialEditor::OnRadioAcoustic ), NULL, this );
  
  m_Panel->m_RadioPhysics->Disconnect( m_Panel->m_RadioPhysics->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MaterialEditor::OnRadioPhysics ), NULL, this );
  m_Panel->m_RadioPhysicsTabOn->Disconnect( m_Panel->m_RadioPhysicsTabOn->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MaterialEditor::OnRadioPhysics ), NULL, this );

  m_Panel->m_Tabs->Disconnect( m_Panel->m_Tabs->GetId(), wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( MaterialEditor::OnNotebookPageChanged ), NULL, this );

  // Disconnect tool tip listeners
  //m_Panel->m_ButtonApply->Disconnect( m_Panel->m_ButtonApply->GetId(), wxEVT_MOTION, wxMouseEventHandler( MaterialEditor::OnMouseEnter ), NULL, this );
  //m_Panel->m_ButtonApply->Disconnect( m_Panel->m_ButtonApply->GetId(), wxEVT_LEAVE_WINDOW, wxMouseEventHandler( MaterialEditor::OnMouseLeave ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Sets the text displayed on the status bar of this window.
// 
void MaterialEditor::SetStatusText( const wxString& text )
{
  m_StatusBar->SetStatusText( text );
}

///////////////////////////////////////////////////////////////////////////////
// Fills out the specified structure with the current settings from this window.
// 
void MaterialEditor::GetMaterialSettings( const Content::MaterialPtr& material )
{
  material->m_Substance = m_Panel->m_ComboVisual->GetStringSelection().c_str();
  material->m_Acoustic = m_Panel->m_ComboAcoustic->GetStringSelection().c_str();
  material->m_Physics = m_Panel->m_ComboPhysics->GetStringSelection().c_str();

  AddPrefix(  Content::MaterialComponents::Substance, material->m_Substance, true );
  AddPrefix(  Content::MaterialComponents::Acoustic, material->m_Acoustic, true );
  AddPrefix(  Content::MaterialComponents::Physics, material->m_Physics, true );
}

///////////////////////////////////////////////////////////////////////////////
// Populates the specified combo box with elements from the specified enum (or
// just clears the combo box if no enum is specified).  If any items are in
// the combo box, the first one is selected.
// 
void MaterialEditor::RefreshComboBox( UIToolKit::AutoCompleteComboBox* comboBox, Symbol::Enum* symEnum, int materialComponent )
{
  comboBox->Clear();
  if ( symEnum )
  {
    wxArrayString wxNames;
    V_string names;
    symEnum->GetElemNames( names );
    for each ( const std::string& name in names )
    {
      wxNames.push_back( StripPrefix( (Content::MaterialComponent) materialComponent, name, true ).c_str() );
    }
    comboBox->Append( wxNames );
    if ( !wxNames.empty() )
    {
      comboBox->Select( 0 );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Recreates the color legend at the bottom of the window, using data from the
// specified enum.  If no enum is specified, the legend just shows a "display
// off" message.
// 
void MaterialEditor::RefreshColorLegend( Symbol::Enum* symEnum, int materialComponent )
{
  // If the legend needs to be refreshed...
  if ( symEnum != m_CurrentLegend )
  {
    m_CurrentLegend = symEnum;
    
    m_Panel->m_KeyScrollWindow->Freeze();
    
    // Remove all items from the legend
    wxSizer* sizerKeyGrid = m_Panel->m_KeyScrollWindow->GetSizer();
    sizerKeyGrid->Clear( true );

    // Repopulate the legend
    if ( symEnum )
    {
      V_string names;
      symEnum->GetElemNames( names );
      for each ( const std::string& name in names )
      {
        std::string fullName( name );
        AddPrefix(  (Content::MaterialComponent) materialComponent, fullName, true );

 	      wxBoxSizer* sizerRow = new wxBoxSizer( wxHORIZONTAL );
	      wxBoxSizer* sizerPanel = new wxBoxSizer( wxVERTICAL );

        wxPanel* colorBox = new wxPanel( m_Panel->m_KeyScrollWindow, wxID_ANY, wxDefaultPosition, wxSize( 20,20 ), wxSIMPLE_BORDER|wxTAB_TRAVERSAL );
        Math::Color3 color = MaterialDisplayNode::GetColorFromMaterialComponent( StripPrefix( (Content::MaterialComponent) materialComponent, name ) );
	      colorBox->SetBackgroundColour( wxColour( color.r, color.g, color.b ) );
	      colorBox->SetMinSize( wxSize( 20,20 ) );
	      colorBox->SetMaxSize( wxSize( 20,20 ) );

        sizerPanel->Add( colorBox, 1, wxLEFT|wxTOP, 5 );
	      sizerRow->Add( sizerPanel, 0, wxALIGN_CENTER_VERTICAL, 2 );

        wxStaticText* rowName = new wxStaticText( m_Panel->m_KeyScrollWindow, wxID_ANY, StripPrefix( (Content::MaterialComponent) materialComponent, name, true ).c_str(), wxDefaultPosition, wxDefaultSize, 0 );
	      rowName->Wrap( -1 );
	      sizerRow->Add( rowName, 1, wxALL|wxALIGN_BOTTOM, 5 );
    	  
	      sizerKeyGrid->Add( sizerRow, 1, wxEXPAND, 5 );
      }
    }
    else
    {
      wxBoxSizer* sizerRow = new wxBoxSizer( wxHORIZONTAL );
      wxStaticText* rowName = new wxStaticText( m_Panel->m_KeyScrollWindow, wxID_ANY, "Display is turned off.", wxDefaultPosition, wxDefaultSize, 0 );
      rowName->Wrap( -1 );
      sizerRow->Add( rowName, 1, wxALL|wxALIGN_BOTTOM, 5 );
      sizerKeyGrid->Add( sizerRow, 1, wxEXPAND, 5 );
    }

    // Force the sizer to resize since we changed its contents.
    sizerKeyGrid->FitInside( m_Panel->m_KeyScrollWindow );

    m_Panel->m_KeyScrollWindow->Thaw();
  }
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
void MaterialEditor::ClearRadioButtons( wxRadioButton* except )
{
  m_Panel->m_RadioOff->SetValue( m_Panel->m_RadioOff == except );
  m_Panel->m_RadioVisualTabOff->SetValue( m_Panel->m_RadioOff == except );
  m_Panel->m_RadioAcousticTabOff->SetValue( m_Panel->m_RadioOff == except );
  m_Panel->m_RadioPhysicsTabOff->SetValue( m_Panel->m_RadioOff == except );

  m_Panel->m_RadioVisual->SetValue( m_Panel->m_RadioVisual == except );
  m_Panel->m_RadioVisualTabOn->SetValue( m_Panel->m_RadioVisual == except );

  m_Panel->m_RadioAcoustic->SetValue( m_Panel->m_RadioAcoustic == except );
  m_Panel->m_RadioAcousticTabOn->SetValue( m_Panel->m_RadioAcoustic == except );

  m_Panel->m_RadioPhysics->SetValue( m_Panel->m_RadioPhysics == except );
  m_Panel->m_RadioPhysicsTabOn->SetValue( m_Panel->m_RadioPhysics == except );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the apply button is clicked.  Takes the current settings
// in the combo boxes and applies them to the currently selected objects.
// 
void MaterialEditor::OnButtonClickApply( wxCommandEvent& event )
{
  wxBusyCursor bc;
  
  switch( m_Panel->m_Tabs->GetSelection() )
  {
  default:
  case 0:
    // All/Default
    {
      Content::MaterialPtr material = new Content::Material();
      GetMaterialSettings( material );
      MaterialDisplayNode::AssignMaterialToSelection( material );
      break;
    }

  case 1:
    // Visual
    {
      std::string substance = m_Panel->m_VisualTabCombo->GetStringSelection().c_str();
      AddPrefix(  Content::MaterialComponents::Substance, substance, true );
      MaterialDisplayNode::AssignMaterialToSelection( Content::MaterialComponents::Substance, substance );
      break;
    }

  case 2:
    // Acoustic
    {
      std::string acoustic = m_Panel->m_AcousticTabCombo->GetStringSelection().c_str();
      AddPrefix(  Content::MaterialComponents::Acoustic, acoustic, true );
      MaterialDisplayNode::AssignMaterialToSelection( Content::MaterialComponents::Acoustic, acoustic );
      break;
    }

  case 3:
    // Physics
    {
      std::string physics = m_Panel->m_PhysicsTabCombo->GetStringSelection().c_str();
      AddPrefix(  Content::MaterialComponents::Physics, physics, true );
      MaterialDisplayNode::AssignMaterialToSelection( Content::MaterialComponents::Physics, physics );
      break;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the clear button is clicked.  Removes all material settings
// from the selected objects.
// 
void MaterialEditor::OnButtonClickClear( wxCommandEvent& event )
{
  wxBusyCursor bc;
  
  switch( m_Panel->m_Tabs->GetSelection() )
  {
  default:
  case 0:
    // All/Default
    {
      MaterialDisplayNode::ClearMaterialsFromObjects();
      break;
    }

  case 1:
    // Visual
    {
      MaterialDisplayNode::ClearMaterialsFromObjects( Content::MaterialComponents::Substance );
      break;
    }

  case 2:
    // Acoustic
    {
      MaterialDisplayNode::ClearMaterialsFromObjects( Content::MaterialComponents::Acoustic );
      break;
    }

  case 3:
    // Physics
    {
      MaterialDisplayNode::ClearMaterialsFromObjects( Content::MaterialComponents::Physics );
      break;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the select button is clicked.  Selects all objects which
// have the settings currently specified in the combo boxes.
// 
void MaterialEditor::OnButtonClickSelect( wxCommandEvent& event )
{
  wxBusyCursor bc;
  
  switch( m_Panel->m_Tabs->GetSelection() )
  {
  default:
  case 0:
    // All/Default
    {
      Content::MaterialPtr material = new Content::Material();
      GetMaterialSettings( material );
      MaterialDisplayNode::SelectObjectsWithMaterial( material );
      break;
    }

  case 1:
    // Visual
    {
      std::string substance = m_Panel->m_VisualTabCombo->GetStringSelection().c_str();
      AddPrefix(  Content::MaterialComponents::Substance, substance, true );
      MaterialDisplayNode::SelectObjectsWithMaterial( Content::MaterialComponents::Substance, substance );
      break;
    }

  case 2:
    // Acoustic
    {
      std::string acoustic = m_Panel->m_AcousticTabCombo->GetStringSelection().c_str();
      AddPrefix(  Content::MaterialComponents::Acoustic, acoustic, true );
      MaterialDisplayNode::SelectObjectsWithMaterial( Content::MaterialComponents::Acoustic, acoustic );
      break;
    }

  case 3:
    // Physics
    {
      std::string physics = m_Panel->m_PhysicsTabCombo->GetStringSelection().c_str();
      AddPrefix(  Content::MaterialComponents::Physics, physics, true );
      MaterialDisplayNode::SelectObjectsWithMaterial( Content::MaterialComponents::Physics, physics );
      break;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the Off radio button is selected.  Clears the color legend.
// 
void MaterialEditor::OnRadioOff( wxCommandEvent& event )
{
  wxBusyCursor bc;
  
  ClearRadioButtons( m_Panel->m_RadioOff );
  RefreshColorLegend( NULL, (int) Content::MaterialComponents::Unknown );
  MaterialDisplayNode::UpdateDisplay( MaterialDisplayFlags::Off );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the Visual radio button is selected.  Updates the color 
// legend.
// 
void MaterialEditor::OnRadioVisual( wxCommandEvent& event )
{
  wxBusyCursor bc;
  
  ClearRadioButtons( m_Panel->m_RadioVisual );
  RefreshColorLegend( m_VisualEnum, (int) Content::MaterialComponents::Substance );
  MaterialDisplayNode::UpdateDisplay( MaterialDisplayFlags::Substance );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the Acoustic radio button is selected.  Updates the color 
// legend.
// 
void MaterialEditor::OnRadioAcoustic( wxCommandEvent& event )
{
  wxBusyCursor bc;
  
  ClearRadioButtons( m_Panel->m_RadioAcoustic );
  RefreshColorLegend( m_AcousticEnum, (int) Content::MaterialComponents::Acoustic );
  MaterialDisplayNode::UpdateDisplay( MaterialDisplayFlags::Acoustic );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the Physics radio button is selected.  Updates the color 
// legend.
// 
void MaterialEditor::OnRadioPhysics( wxCommandEvent& event )
{
  wxBusyCursor bc;
  
  ClearRadioButtons( m_Panel->m_RadioPhysics );
  RefreshColorLegend( m_PhysicsEnum, (int) Content::MaterialComponents::Physics );
  MaterialDisplayNode::UpdateDisplay( MaterialDisplayFlags::Physics );
}


///////////////////////////////////////////////////////////////////////////////
// Callback for when the current notebook page is changed.
// 
void MaterialEditor::OnNotebookPageChanged( wxNotebookEvent& event )
{
  wxBusyCursor bc;
  
  switch( event.GetSelection() )
  {
  default:
  case 0:
    // All/Default
    {
      break;
    }

  case 1:
    // Visual
    {
      // update the display settings
      if ( !m_Panel->m_RadioVisual->GetValue() && !m_Panel->m_RadioOff->GetValue() )
      {
        ClearRadioButtons( m_Panel->m_RadioVisual );
        RefreshColorLegend( m_VisualEnum, (int) Content::MaterialComponents::Substance );
        MaterialDisplayNode::UpdateDisplay( MaterialDisplayFlags::Substance );
      }
      break;
    }

  case 2:
    // Acoustic
    {
      // update the display settings
      if ( !m_Panel->m_RadioAcoustic->GetValue() && !m_Panel->m_RadioOff->GetValue() )
      {
        ClearRadioButtons( m_Panel->m_RadioAcoustic );
        RefreshColorLegend( m_AcousticEnum, (int) Content::MaterialComponents::Acoustic );
        MaterialDisplayNode::UpdateDisplay( MaterialDisplayFlags::Acoustic );
      }
      break;
    }

  case 3:
    // Physics
    {
      // update the display settings
      if ( !m_Panel->m_RadioPhysics->GetValue() && !m_Panel->m_RadioOff->GetValue() )
      {
        ClearRadioButtons( m_Panel->m_RadioPhysics );
        RefreshColorLegend( m_PhysicsEnum, (int) Content::MaterialComponents::Physics );
        MaterialDisplayNode::UpdateDisplay( MaterialDisplayFlags::Physics );
      }
      break;
    }
  }

  event.Skip();
}


///////////////////////////////////////////////////////////////////////////////
// Callback for when the mouse enters a sub window.  Updates the status text
// with the tool tip, since tool tips don't show up in Maya.
// 
void MaterialEditor::OnMouseEnter( wxMouseEvent& event )
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
void MaterialEditor::OnMouseLeave( wxMouseEvent& event )
{
  event.Skip();
  SetStatusText( wxEmptyString );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the close button is pressed.  Simply hides this window.
// Destruction is handled by the owner of this object.
// 
void MaterialEditor::OnCloseEvent( wxCloseEvent& event )
{
  Hide();
  
  ClearRadioButtons( m_Panel->m_RadioOff );
  RefreshColorLegend( NULL, (int) Content::MaterialComponents::Unknown );
  //MaterialDisplayNode::DeleteMaterialDisplayNode();
}
