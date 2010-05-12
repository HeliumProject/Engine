///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Precompile.h"

#include "AnimationTimelineSlider.h"

#include "AnimationEventsPanels.h"

///////////////////////////////////////////////////////////////////////////

AnimationEventEditor::AnimationEventEditor( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
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

AnimationPanel::AnimationPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 500,355 ) );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxEmptyString ), wxVERTICAL );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_UpdateClassLabel = new wxStaticText( this, wxID_ANY, wxT("Update Class"), wxDefaultPosition, wxDefaultSize, 0 );
	m_UpdateClassLabel->Wrap( -1 );
	bSizer2->Add( m_UpdateClassLabel, 0, wxALIGN_RIGHT|wxEXPAND|wxTOP, 5 );
	
	m_ClipLabel = new wxStaticText( this, wxID_ANY, wxT("Clip"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ClipLabel->Wrap( -1 );
	bSizer2->Add( m_ClipLabel, 0, wxALIGN_RIGHT|wxBOTTOM|wxTOP, 15 );
	
	bSizer18->Add( bSizer2, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	wxString m_UpdateClassChoiceChoices[] = { wxT("blah") };
	int m_UpdateClassChoiceNChoices = sizeof( m_UpdateClassChoiceChoices ) / sizeof( wxString );
	m_UpdateClassChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_UpdateClassChoiceNChoices, m_UpdateClassChoiceChoices, 0 );
	bSizer8->Add( m_UpdateClassChoice, 0, wxBOTTOM|wxEXPAND, 5 );
	
	wxString m_ClipChoiceChoices[] = { wxT("blah") };
	int m_ClipChoiceNChoices = sizeof( m_ClipChoiceChoices ) / sizeof( wxString );
	m_ClipChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_ClipChoiceNChoices, m_ClipChoiceChoices, 0 );
	m_ClipChoice->SetMinSize( wxSize( 200,-1 ) );
	
	bSizer8->Add( m_ClipChoice, 1, wxEXPAND|wxTOP, 5 );
	
	m_OverrideEventsCheckbox = new wxCheckBox( this, wxID_ANY, wxT("Override Clip Events"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer8->Add( m_OverrideEventsCheckbox, 0, wxBOTTOM|wxRIGHT|wxTOP, 5 );
	
	bSizer18->Add( bSizer8, 1, wxALL, 5 );
	
	bSizer11->Add( bSizer18, 1, wxEXPAND, 5 );
	
	sbSizer2->Add( bSizer11, 0, wxALL|wxEXPAND, 5 );
	
	bSizer1->Add( sbSizer2, 0, wxEXPAND, 5 );
	
	
	bSizer1->Add( 0, 5, 0, wxEXPAND, 5 );
	
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
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( m_panel2, -1, wxEmptyString ), wxVERTICAL );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxHORIZONTAL );
	
	m_EventTypeLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("Event Type"), wxDefaultPosition, wxDefaultSize, 0 );
	m_EventTypeLabel->Wrap( -1 );
	bSizer17->Add( m_EventTypeLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_EventTypeChoiceChoices[] = { wxT("blah") };
	int m_EventTypeChoiceNChoices = sizeof( m_EventTypeChoiceChoices ) / sizeof( wxString );
	m_EventTypeChoice = new wxChoice( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_EventTypeChoiceNChoices, m_EventTypeChoiceChoices, 0 );
	m_EventTypeChoice->SetMinSize( wxSize( 100,-1 ) );
	
	bSizer17->Add( m_EventTypeChoice, 1, wxALL, 5 );
	
	m_RefreshEvents = new wxButton( m_panel2, wxID_ANY, wxT("Refresh Event List"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer17->Add( m_RefreshEvents, 0, wxALL, 5 );
	
	sbSizer1->Add( bSizer17, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer28;
	bSizer28 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ShowOnlyMatchingEvents = new wxCheckBox( m_panel2, wxID_ANY, wxT("Show Only Applied Events"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer28->Add( m_ShowOnlyMatchingEvents, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer1->Add( bSizer28, 0, wxEXPAND, 5 );
	
	bSizer15->Add( sbSizer1, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( m_panel2, -1, wxEmptyString ), wxHORIZONTAL );
	
	m_ValueLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("Value"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ValueLabel->Wrap( -1 );
	sbSizer3->Add( m_ValueLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_ValueCombo = new wxComboBox( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_ValueCombo->Append( wxT("blah") );
	m_ValueCombo->SetMinSize( wxSize( 100,-1 ) );
	
	sbSizer3->Add( m_ValueCombo, 1, wxALL, 5 );
	
	bSizer15->Add( sbSizer3, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer26;
	bSizer26 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( m_panel2, -1, wxEmptyString ), wxVERTICAL );
	
	sbSizer4->SetMinSize( wxSize( -1,80 ) ); 
	wxBoxSizer* bSizer40;
	bSizer40 = new wxBoxSizer( wxHORIZONTAL );
	
	m_Untrigger = new wxCheckBox( m_panel2, wxID_ANY, wxT("Untrigger"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer40->Add( m_Untrigger, 1, wxALL, 5 );
	
	m_InfiniteDuration = new wxCheckBox( m_panel2, wxID_ANY, wxT("Infinite Duration"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer40->Add( m_InfiniteDuration, 1, wxALL, 5 );
	
	sbSizer4->Add( bSizer40, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer39;
	bSizer39 = new wxBoxSizer( wxHORIZONTAL );
	
	m_DominantOnly = new wxCheckBox( m_panel2, wxID_ANY, wxT("Dominant Only"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer39->Add( m_DominantOnly, 1, wxALL, 5 );
	
	m_GameplayEvent = new wxCheckBox( m_panel2, wxID_ANY, wxT("Gameplay Event"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer39->Add( m_GameplayEvent, 1, wxALL, 5 );
	
	sbSizer4->Add( bSizer39, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer38;
	bSizer38 = new wxBoxSizer( wxHORIZONTAL );
	
	m_AddEventButton = new wxButton( m_panel2, wxID_ANY, wxT("Add Event At Frame"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer38->Add( m_AddEventButton, 1, wxALL, 5 );
	
	m_AuditionButton = new wxButton( m_panel2, wxID_ANY, wxT("Audition"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer38->Add( m_AuditionButton, 1, wxALL, 5 );
	
	sbSizer4->Add( bSizer38, 1, wxEXPAND, 5 );
	
	bSizer26->Add( sbSizer4, 1, 0, 5 );
	
	wxStaticBoxSizer* sbSizer7;
	sbSizer7 = new wxStaticBoxSizer( new wxStaticBox( m_panel2, -1, wxEmptyString ), wxVERTICAL );
	
	sbSizer7->SetMinSize( wxSize( -1,80 ) ); 
	
	sbSizer7->Add( 0, 6, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer34;
	bSizer34 = new wxBoxSizer( wxHORIZONTAL );
	
	m_StopAllButton = new wxButton( m_panel2, wxID_ANY, wxT("Stop All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer34->Add( m_StopAllButton, 1, wxALL, 5 );
	
	sbSizer7->Add( bSizer34, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxVERTICAL );
	
	m_DeleteSelection = new wxButton( m_panel2, wxID_ANY, wxT("Delete Selection"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer35->Add( m_DeleteSelection, 1, wxALL|wxEXPAND, 5 );
	
	sbSizer7->Add( bSizer35, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer36;
	bSizer36 = new wxBoxSizer( wxVERTICAL );
	
	m_RebuildMoby = new wxButton( m_panel2, wxID_ANY, wxT("Rebuild Moby"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer36->Add( m_RebuildMoby, 1, wxALL|wxEXPAND, 5 );

        sbSizer7->Add( bSizer36, 0, wxEXPAND, 5 );
	
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

AnimationActivePanel::AnimationActivePanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
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

AnimationTimelinePanel::AnimationTimelinePanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
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
	bSizer21 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, wxT("Go To Frame"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	bSizer21->Add( m_staticText5, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_GotoFrameTextBox = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	bSizer21->Add( m_GotoFrameTextBox, 0, wxALIGN_CENTER|wxALIGN_RIGHT|wxALL, 5 );
	
	bSizer22->Add( bSizer21, 1, wxALIGN_RIGHT, 5 );
	
	bSizer20->Add( bSizer22, 1, wxEXPAND, 5 );
	
	bSizer5->Add( bSizer20, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxVERTICAL );
	
	m_TimeSlider = new Luna::AnimationTimelineSlider( this, wxID_ANY, 50, 0, 50, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS|wxSL_HORIZONTAL|wxSL_LABELS );
	bSizer17->Add( m_TimeSlider, 0, wxALL|wxEXPAND, 5 );
	
	bSizer5->Add( bSizer17, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxHORIZONTAL );
	
	m_MarkersPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_MarkersPanel->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_GRAYTEXT ) );
	
	bSizer18->Add( m_MarkersPanel, 1, wxEXPAND | wxALL, 5 );
	
	bSizer5->Add( bSizer18, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer5 );
	this->Layout();
}
