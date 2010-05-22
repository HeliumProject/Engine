#include "stdafx.h"
#include "FilePickerEditor.h"

#include "UIToolKit/IconMagnify.xpm"

namespace File
{

  WX_PG_IMPLEMENT_EDITOR_CLASS( FilePickerEditor, FilePickerEditor, wxPGTextCtrlEditor );


  /////////////////////////////////////////////////////////////////////////////
  // Constructor
  // 
  FilePickerEditor::FilePickerEditor()
    : m_Button1ID( wxID_ANY )
    , m_Button2ID( wxID_ANY )
  {
  }

  /////////////////////////////////////////////////////////////////////////////
  // Destructor
  // 
  FilePickerEditor::~FilePickerEditor()
  {
  }

  /////////////////////////////////////////////////////////////////////////////
  // Returns the one and only instance of this class.
  // 
  FilePickerEditor* FilePickerEditor::GetInstance()
  {
    wxPGRegisterEditorClass( FilePickerEditor );
    return static_cast< FilePickerEditor* >( wxPG_EDITOR( FilePickerEditor ) );
  }

  /////////////////////////////////////////////////////////////////////////////
  // Creates the controls associated with this editor (a text control and two
  // buttons).
  // 
  wxPGWindowList FilePickerEditor::CreateControls( wxPropertyGrid* propgrid, wxPGProperty* property, const wxPoint& pos, const wxSize& size ) const
  {
    const int margin = 1;
    const wxSize buttonSize( 18, size.y );
    const wxSize panelSize( ( 2 * buttonSize.x ) + ( 2 * margin ), size.y );
    const wxSize textCtrlSize( size.x - panelSize.x, size.y );
    const wxPoint panelPos( pos.x + textCtrlSize.x + margin, pos.y );

    // Panel that the buttons will be part of
    wxPanel* panel = new wxPanel();
    panel->Hide();
    panel->Create( propgrid->GetPanel(), wxID_ANY, panelPos, panelSize );
    panel->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

    wxBoxSizer* sizer = new wxBoxSizer( wxHORIZONTAL );

    // First button
    wxButton* button1 = new wxButton();
    button1->Create( panel, wxID_ANY, "...", wxDefaultPosition, buttonSize );
    m_Button1ID = button1->GetId();

    // Second button
    wxBitmapButton* button2 = new wxBitmapButton();
    button2->Create( panel, wxID_ANY, wxBitmap( ( const char** )g_IconMagnify ), wxDefaultPosition, buttonSize );
    m_Button2ID = button2->GetId();

    sizer->Add( button1, 0, wxRIGHT, 1 );
    sizer->Add( button2, 0, 0, 0 );
    panel->SetSizer( sizer );
    panel->Layout();

    // Connect button click events
    propgrid->Connect( button1->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxPropertyGrid::OnCustomEditorEvent ), NULL, propgrid );
    propgrid->Connect( button2->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxPropertyGrid::OnCustomEditorEvent ), NULL, propgrid );

    // Text control
    wxPGWindowList windowList = wxPGTextCtrlEditor::CreateControls( propgrid, property, pos, textCtrlSize );
    wxTextCtrl* textCtrl = (wxTextCtrl*) windowList.m_primary;

    // THIS IS HOW YOU DO A READ-ONLY TEXT FIELD
    // Should incorporate this into the wxPropertyGrid source code.
    //wxString text;
    //if ( !( property->GetFlags() & wxPG_PROP_UNSPECIFIED ) )
    //{
    //  text = property->GetValueAsString( 0 );
    //}
    //wxWindow* textCtrl = propgrid->GenerateEditorTextCtrl( pos, textCtrlSize, text, (wxWindow*)NULL, wxTE_READONLY, property->GetMaxLength() );

    windowList.SetSecondary( panel );
    return windowList;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Returns the ID of the first button that was created by this editor (the
  // "normal" file open dialog button).
  // 
  int FilePickerEditor::GetButton1ID() const
  {
    return m_Button1ID;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Returns the ID of the second button that was created by this editor (the
  // button that should bring up the Asset Finder dialog).
  // 
  int FilePickerEditor::GetButton2ID() const
  {
    return m_Button2ID;
  }
}
