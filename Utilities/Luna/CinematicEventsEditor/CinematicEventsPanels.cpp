///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 17 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Precompile.h"

#include "CinematicTimelineSlider.h"

#include "CinematicEventsPanels.h"

///////////////////////////////////////////////////////////////////////////

CinematicEventEditor::CinematicEventEditor( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	m_StatusBar = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	m_MenuBar = new wxMenuBar( 0 );
	m_FileMenu = new wxMenu();
	m_MenuBar->Append( m_FileMenu, wxT("File") );
	
	this->SetMenuBar( m_MenuBar );
	
	m_MainToolBar = this->CreateToolBar( wxTB_HORIZONTAL, wxID_ANY ); 
	m_MainToolBar->Realize();
	
}

CinematicEventEditor::~CinematicEventEditor()
{
}

CinematicPanel::CinematicPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 500,355 ) );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_panel3 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel3->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_ACTIVECAPTION ) );
	
	wxBoxSizer* bSizer37;
	bSizer37 = new wxBoxSizer( wxVERTICAL );
	
	m_panel2 = new wxPanel( m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel2->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer5;
	sbSizer5 = new wxStaticBoxSizer( new wxStaticBox( m_panel2, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ActorLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("Actors"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ActorLabel->Wrap( -1 );
	bSizer23->Add( m_ActorLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_ActorChoiceChoices;
	m_ActorChoice = new wxChoice( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_ActorChoiceChoices, 0 );
	m_ActorChoice->SetSelection( 0 );
	bSizer23->Add( m_ActorChoice, 1, wxALL, 5 );
	
	sbSizer5->Add( bSizer23, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxHORIZONTAL );
	
	m_JointLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("Joints"), wxDefaultPosition, wxDefaultSize, 0 );
	m_JointLabel->Wrap( -1 );
	bSizer24->Add( m_JointLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_JointChoiceChoices;
	m_JointChoice = new wxChoice( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_JointChoiceChoices, 0 );
	m_JointChoice->SetSelection( 0 );
	bSizer24->Add( m_JointChoice, 1, wxALL, 5 );
	
	sbSizer5->Add( bSizer24, 1, wxEXPAND, 5 );
	
	bSizer15->Add( sbSizer5, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer26;
	bSizer26 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( m_panel2, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	sbSizer4->SetMinSize( wxSize( -1,80 ) ); 
	wxBoxSizer* bSizer40;
	bSizer40 = new wxBoxSizer( wxHORIZONTAL );
	
	m_Untrigger = new wxCheckBox( m_panel2, wxID_ANY, wxT("Untrigger"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer40->Add( m_Untrigger, 1, wxALL, 5 );
	
	m_InfiniteDuration = new wxCheckBox( m_panel2, wxID_ANY, wxT("Infinite Duration"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer40->Add( m_InfiniteDuration, 1, wxALL, 5 );
	
	sbSizer4->Add( bSizer40, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer38;
	bSizer38 = new wxBoxSizer( wxVERTICAL );
	
	m_AddEventButton = new wxButton( m_panel2, wxID_ANY, wxT("Add Event At Frame"), wxDefaultPosition, wxDefaultSize, 0 );
	m_AddEventButton->SetToolTip( wxT("Add an event at the frame selected") );
	
	bSizer38->Add( m_AddEventButton, 1, wxALL|wxEXPAND, 5 );
	
	m_RebuildButton = new wxButton( m_panel2, wxID_ANY, wxT("Rebuild Cinematic"), wxDefaultPosition, wxDefaultSize, 0 );
	m_RebuildButton->SetToolTip( wxT("Rebuild the cinematic after you add new events") );
	
	bSizer38->Add( m_RebuildButton, 1, wxALL|wxEXPAND, 5 );
	
	sbSizer4->Add( bSizer38, 1, wxEXPAND, 5 );
	
	bSizer26->Add( sbSizer4, 1, 0, 5 );
	
	wxStaticBoxSizer* sbSizer7;
	sbSizer7 = new wxStaticBoxSizer( new wxStaticBox( m_panel2, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	sbSizer7->SetMinSize( wxSize( -1,80 ) ); 
	
	sbSizer7->Add( 0, 6, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer34;
	bSizer34 = new wxBoxSizer( wxHORIZONTAL );
	
	m_StopAllButton = new wxButton( m_panel2, wxID_ANY, wxT("Stop All Events"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StopAllButton->SetToolTip( wxT("Stops events from playing") );
	
	bSizer34->Add( m_StopAllButton, 1, wxALL, 5 );
	
	sbSizer7->Add( bSizer34, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxVERTICAL );
	
	m_DeleteSelection = new wxButton( m_panel2, wxID_ANY, wxT("Delete Selection"), wxDefaultPosition, wxDefaultSize, 0 );
	m_DeleteSelection->SetToolTip( wxT("Delete the selected events") );
	
	bSizer35->Add( m_DeleteSelection, 1, wxALL|wxEXPAND, 5 );
	
	sbSizer7->Add( bSizer35, 0, wxEXPAND, 5 );
	
	bSizer26->Add( sbSizer7, 1, wxEXPAND, 5 );
	
	bSizer15->Add( bSizer26, 0, wxEXPAND, 5 );
	
	bSizer14->Add( bSizer15, 2, wxALL|wxEXPAND, 0 );
	
	bSizer13->Add( bSizer14, 1, wxEXPAND, 0 );
	
	m_panel2->SetSizer( bSizer13 );
	m_panel2->Layout();
	bSizer13->Fit( m_panel2 );
	bSizer37->Add( m_panel2, 0, wxEXPAND|wxTOP, 2 );
	
	m_panel3->SetSizer( bSizer37 );
	m_panel3->Layout();
	bSizer37->Fit( m_panel3 );
	bSizer1->Add( m_panel3, 0, wxALL|wxEXPAND, 0 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
}

CinematicPanel::~CinematicPanel()
{
}

CinematicActivePanel::CinematicActivePanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_EventGrid = new wxGrid( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
	// Grid
	m_EventGrid->CreateGrid( 0, 2 );
	m_EventGrid->EnableEditing( true );
	m_EventGrid->EnableGridLines( true );
	m_EventGrid->EnableDragGridSize( false );
	m_EventGrid->SetMargins( 0, 0 );
	
	// Columns
	m_EventGrid->EnableDragColMove( false );
	m_EventGrid->EnableDragColSize( true );
	m_EventGrid->SetColLabelSize( 30 );
	m_EventGrid->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	m_EventGrid->EnableDragRowSize( true );
	m_EventGrid->SetRowLabelSize( 80 );
	m_EventGrid->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	m_EventGrid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	bSizer3->Add( m_EventGrid, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer3 );
	this->Layout();
}

CinematicActivePanel::~CinematicActivePanel()
{
}

CinematicTimelinePanel::CinematicTimelinePanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer20->Add( 0, 0, 1, wxEXPAND, 0 );
	
	wxBoxSizer* bSizer181;
	bSizer181 = new wxBoxSizer( wxHORIZONTAL );
	
	m_PauseButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer181->Add( m_PauseButton, 0, wxALL, 5 );
	
	m_PlayButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer181->Add( m_PlayButton, 0, wxALL, 5 );
	
	bSizer20->Add( bSizer181, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer22;
	bSizer22 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer261;
	bSizer261 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, wxT("Go To Frame:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	bSizer261->Add( m_staticText5, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_GotoFrameTextBox = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	bSizer261->Add( m_GotoFrameTextBox, 0, wxALIGN_CENTER|wxALIGN_RIGHT|wxALL, 5 );
	
	bSizer21->Add( bSizer261, 1, wxALIGN_RIGHT, 5 );
	
	wxBoxSizer* bSizer27;
	bSizer27 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, wxT("Move Selected Frames by:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	bSizer27->Add( m_staticText6, 0, wxALL, 5 );
	
	m_MoveSelectedFrames = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	bSizer27->Add( m_MoveSelectedFrames, 0, wxALL, 5 );
	
	bSizer21->Add( bSizer27, 1, wxALIGN_RIGHT, 5 );
	
	bSizer22->Add( bSizer21, 1, wxALIGN_RIGHT, 5 );
	
	bSizer20->Add( bSizer22, 1, wxEXPAND, 5 );
	
	bSizer5->Add( bSizer20, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxVERTICAL );
	
	m_TimeSlider = new Luna::CinematicTimelineSlider( this, wxID_ANY, 50, 0, 9999, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS|wxSL_HORIZONTAL|wxSL_LABELS );
	bSizer17->Add( m_TimeSlider, 0, wxALL|wxEXPAND, 5 );
	
	bSizer5->Add( bSizer17, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxHORIZONTAL );
	
	m_MarkersPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_MarkersPanel->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_GRAYTEXT ) );
	
	bSizer18->Add( m_MarkersPanel, 1, wxEXPAND | wxALL, 5 );
	
	bSizer5->Add( bSizer18, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer221;
	bSizer221 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* sbSizer5;
	sbSizer5 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Zoom") ), wxHORIZONTAL );
	
	wxBoxSizer* bSizer26;
	bSizer26 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxHORIZONTAL );
	
	m_StartFrame = new wxStaticText( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StartFrame->Wrap( -1 );
	bSizer23->Add( m_StartFrame, 0, wxALL, 5 );
	
	m_ZoomStart = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	bSizer23->Add( m_ZoomStart, 1, wxALL, 5 );
	
	bSizer26->Add( bSizer23, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxHORIZONTAL );
	
	bSizer26->Add( bSizer24, 1, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer25;
	bSizer25 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ZoomEnd = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	bSizer25->Add( m_ZoomEnd, 1, wxEXPAND, 5 );
	
	m_EndFrame = new wxStaticText( this, wxID_ANY, wxT("9999"), wxDefaultPosition, wxDefaultSize, 0 );
	m_EndFrame->Wrap( -1 );
	bSizer25->Add( m_EndFrame, 0, wxALIGN_CENTER|wxALIGN_RIGHT|wxALL, 5 );
	
	bSizer26->Add( bSizer25, 1, 0, 5 );
	
	sbSizer5->Add( bSizer26, 1, wxEXPAND, 5 );
	
	bSizer221->Add( sbSizer5, 1, wxEXPAND, 5 );
	
	bSizer5->Add( bSizer221, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer5 );
	this->Layout();
}

CinematicTimelinePanel::~CinematicTimelinePanel()
{
}
