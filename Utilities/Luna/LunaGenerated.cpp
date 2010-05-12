///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Precompile.h"

#include "UIToolKit/Button.h"
#include "UIToolKit/MenuButton.h"

#include "LunaGenerated.h"

///////////////////////////////////////////////////////////////////////////
using namespace Luna;

DrawerFrameBase::DrawerFrameBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 100,100 ), wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_EmbeddedWindowSizer = new wxBoxSizer( wxVERTICAL );
	
	mainSizer->Add( m_EmbeddedWindowSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

DrawerFrameBase::~DrawerFrameBase()
{
}

ToolsPanelGenerated::ToolsPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 170,325 ) );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer31;
	bSizer31 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ButtonCreateEntity = new UIToolKit::Button( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), wxBU_AUTODRAW );
	bSizer10->Add( m_ButtonCreateEntity, 0, wxALL, 0 );
	
	m_ButtonCreateVolume = new UIToolKit::Button( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), wxBU_AUTODRAW );
	bSizer10->Add( m_ButtonCreateVolume, 0, wxALL, 0 );
	
	m_ButtonCreateClue = new UIToolKit::Button( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), wxBU_AUTODRAW );
	bSizer10->Add( m_ButtonCreateClue, 0, wxALL, 0 );
	
	m_ButtonCurve = new UIToolKit::MenuButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), wxBU_AUTODRAW );
	bSizer10->Add( m_ButtonCurve, 0, wxALL, 0 );
	
	bSizer31->Add( bSizer10, 0, wxEXPAND, 5 );
	
	mainSizer->Add( bSizer31, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer221;
	bSizer221 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Class:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer21->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("agorian_beast"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	m_staticText2->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer21->Add( m_staticText2, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_bpButton445 = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 16,16 ), 0 );
	bSizer21->Add( m_bpButton445, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0 );
	
	bSizer221->Add( bSizer21, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer29;
	bSizer29 = new wxBoxSizer( wxVERTICAL );
	
	m_CurrentAssetThumbnail = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	m_CurrentAssetThumbnail->SetMinSize( wxSize( 64,64 ) );
	
	bSizer29->Add( m_CurrentAssetThumbnail, 1, wxALL|wxEXPAND, 5 );
	
	bSizer221->Add( bSizer29, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer22;
	bSizer22 = new wxBoxSizer( wxVERTICAL );
	
	m_button1 = new wxButton( this, wxID_ANY, wxT("Select New Class"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer22->Add( m_button1, 0, wxALL|wxEXPAND, 1 );
	
	bSizer221->Add( bSizer22, 0, wxEXPAND, 0 );
	
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBox1 = new wxCheckBox( this, wxID_ANY, wxT("Show Pointer"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBox1->SetValue(true);
	
	bSizer23->Add( m_checkBox1, 0, wxALL, 5 );
	
	bSizer221->Add( bSizer23, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer231;
	bSizer231 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBox11 = new wxCheckBox( this, wxID_ANY, wxT("Show Bounds"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBox11->SetValue(true);
	
	bSizer231->Add( m_checkBox11, 0, wxALL, 5 );
	
	bSizer221->Add( bSizer231, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer232;
	bSizer232 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBox12 = new wxCheckBox( this, wxID_ANY, wxT("Show Geometry"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer232->Add( m_checkBox12, 0, wxALL, 5 );
	
	bSizer221->Add( bSizer232, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer30;
	bSizer30 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button2 = new wxButton( this, wxID_ANY, wxT("Snap"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer30->Add( m_button2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 1 );
	
	m_button3 = new wxButton( this, wxID_ANY, wxT("Randomize"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer30->Add( m_button3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 1 );
	
	bSizer221->Add( bSizer30, 0, wxEXPAND, 5 );
	
	mainSizer->Add( bSizer221, 1, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

ToolsPanelGenerated::~ToolsPanelGenerated()
{
}

GameRowGenerated::GameRowGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 200,-1 ) );
	
	wxBoxSizer* outerSizer;
	outerSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_InnerSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_Icon1 = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 16,16 ), 0 );
	m_InnerSizer->Add( m_Icon1, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	m_Icon2 = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 16,16 ), 0 );
	m_InnerSizer->Add( m_Icon2, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_CheckBox = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	m_InnerSizer->Add( m_CheckBox, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5 );
	
	m_Label = new wxStaticText( this, wxID_ANY, wxT("NAME"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Label->Wrap( -1 );
	m_InnerSizer->Add( m_Label, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2 );
	
	outerSizer->Add( m_InnerSizer, 1, wxALL|wxEXPAND, 0 );
	
	this->SetSizer( outerSizer );
	this->Layout();
}

GameRowGenerated::~GameRowGenerated()
{
}

GamePanelGenerated::GamePanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 345,100 ) );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ToolBar = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxTB_HORZ_TEXT ); 
	m_ToolBar->AddTool( wxID_ANY, wxT("New"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_ToolBar->AddTool( wxID_ANY, wxT("Existing"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_ToolBar->AddSeparator();
	m_ToolBar->AddTool( wxID_ANY, wxT("Edit"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_ToolBar->AddTool( wxID_ANY, wxT("Save"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_ToolBar->AddSeparator();
	m_ToolBar->AddTool( wxID_ANY, wxT("Delete"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_ToolBar->Realize();
	
	mainSizer->Add( m_ToolBar, 0, wxEXPAND, 5 );
	
	m_ScrollWindow = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_ScrollWindow->SetScrollRate( 5, 5 );
	wxBoxSizer* innerSizer;
	innerSizer = new wxBoxSizer( wxVERTICAL );
	
	m_panel2 = new wxPanel( m_ScrollWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL );
	m_panel2->SetBackgroundColour( wxColour( 255, 255, 21 ) );
	
	wxBoxSizer* sizer;
	sizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_Icon = new wxStaticBitmap( m_panel2, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	sizer->Add( m_Icon, 0, wxALL|wxALIGN_CENTER_VERTICAL, 1 );
	
	m_CheckBox = new wxCheckBox( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	sizer->Add( m_CheckBox, 0, wxALL|wxALIGN_CENTER_VERTICAL, 1 );
	
	m_Label = new wxStaticText( m_panel2, wxID_ANY, wxT("FooBar"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Label->Wrap( -1 );
	m_Label->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
	m_Label->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
	
	sizer->Add( m_Label, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	
	sizer->Add( 0, 0, 1, wxEXPAND|wxALIGN_CENTER_VERTICAL, 0 );
	
	m_bitmap10 = new wxStaticBitmap( m_panel2, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	sizer->Add( m_bitmap10, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0 );
	
	m_panel2->SetSizer( sizer );
	m_panel2->Layout();
	sizer->Fit( m_panel2 );
	innerSizer->Add( m_panel2, 0, wxEXPAND | wxALL, 1 );
	
	m_panel21 = new wxPanel( m_ScrollWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* sizer1;
	sizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	m_Icon1 = new wxStaticBitmap( m_panel21, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	sizer1->Add( m_Icon1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 1 );
	
	m_CheckBox1 = new wxCheckBox( m_panel21, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	sizer1->Add( m_CheckBox1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 1 );
	
	m_Label1 = new wxStaticText( m_panel21, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_Label1->Wrap( -1 );
	sizer1->Add( m_Label1, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5 );
	
	m_panel21->SetSizer( sizer1 );
	m_panel21->Layout();
	sizer1->Fit( m_panel21 );
	innerSizer->Add( m_panel21, 0, wxEXPAND | wxALL, 1 );
	
	m_panel22 = new wxPanel( m_ScrollWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel22->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_ACTIVEBORDER ) );
	
	wxBoxSizer* sizer2;
	sizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_Icon2 = new wxStaticBitmap( m_panel22, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	sizer2->Add( m_Icon2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 1 );
	
	m_CheckBox2 = new wxCheckBox( m_panel22, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	sizer2->Add( m_CheckBox2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 1 );
	
	m_Label2 = new wxStaticText( m_panel22, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_Label2->Wrap( -1 );
	sizer2->Add( m_Label2, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5 );
	
	m_panel22->SetSizer( sizer2 );
	m_panel22->Layout();
	sizer2->Fit( m_panel22 );
	innerSizer->Add( m_panel22, 0, wxEXPAND | wxALL, 1 );
	
	m_ScrollWindow->SetSizer( innerSizer );
	m_ScrollWindow->Layout();
	innerSizer->Fit( m_ScrollWindow );
	mainSizer->Add( m_ScrollWindow, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

GamePanelGenerated::~GamePanelGenerated()
{
}
