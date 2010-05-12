///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Precompile.h"

#include "UIToolKit/SortableListView.h"

#include "EditorGenerated.h"

///////////////////////////////////////////////////////////////////////////
using namespace Luna;

SubmitChangesPanel::SubmitChangesPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 340,250 ) );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_StaticText = new wxStaticText( this, ID_DEFAULT, wxT("This is your DEFAULT changelist (you may have other changelists):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticText->Wrap( -1 );
	bSizer5->Add( m_StaticText, 0, wxALL, 5 );
	
	mainSizer->Add( bSizer5, 0, wxEXPAND, 5 );
	
	wxBoxSizer* topSizer;
	topSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ListBoxFiles = new wxListBox( this, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_HSCROLL|wxLB_NEEDED_SB|wxLB_SINGLE|wxLB_SORT ); 
	topSizer->Add( m_ListBoxFiles, 1, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( topSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* captionSizer;
	captionSizer = new wxBoxSizer( wxVERTICAL );
	
	m_StaticQuesion = new wxStaticText( this, ID_DEFAULT, wxT("Would you like to commit the above changes to revision control?"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticQuesion->Wrap( -1 );
	captionSizer->Add( m_StaticQuesion, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	mainSizer->Add( captionSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer4->Add( 0, 0, 1, wxALL, 5 );
	
	m_ButtonYes = new wxButton( this, wxID_YES, wxT("Yes"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_ButtonYes, 0, wxALL, 5 );
	
	m_ButtonNo = new wxButton( this, wxID_NO, wxT("No - Launch P4V"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_ButtonNo, 0, wxALL, 5 );
	
	m_ButtonCancel = new wxButton( this, wxID_CANCEL, wxT("Canel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_ButtonCancel, 0, wxALL, 5 );
	
	
	bSizer4->Add( 0, 0, 1, wxALL, 5 );
	
	mainSizer->Add( bSizer4, 0, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

SubmitChangesPanel::~SubmitChangesPanel()
{
}

RunGamePanel::RunGamePanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 370,300 ) );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* buildSizer;
	buildSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Choose Build") ), wxVERTICAL );
	
	wxString m_BuildChoiceChoices[] = { wxT("08_01_08_1117 (Current Good Build)"), wxT("08_01_08_1020"), wxT("08_01_08_1019"), wxT("alpha_sunday_testing") };
	int m_BuildChoiceNChoices = sizeof( m_BuildChoiceChoices ) / sizeof( wxString );
	m_BuildChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_BuildChoiceNChoices, m_BuildChoiceChoices, 0 );
	m_BuildChoice->SetSelection( 0 );
	buildSizer->Add( m_BuildChoice, 0, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( buildSizer, 0, wxEXPAND, 5 );
	
	
	mainSizer->Add( 0, 5, 0, 0, 5 );
	
	wxStaticBoxSizer* levelSizer;
	levelSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Choose Levels") ), wxVERTICAL );
	
	m_LevelHelpText = new wxStaticText( this, wxID_ANY, wxT("Levels with a red X do not have valid server data for this particular build."), wxDefaultPosition, wxDefaultSize, 0 );
	m_LevelHelpText->Wrap( -1 );
	m_LevelHelpText->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 90, false, wxEmptyString ) );
	
	levelSizer->Add( m_LevelHelpText, 0, wxALL, 5 );
	
	m_Levels = new UIToolKit::SortableListView( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_ALIGN_LEFT|wxLC_AUTOARRANGE|wxLC_LIST );
	levelSizer->Add( m_Levels, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* levelButtonSizer;
	levelButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_SelectNone = new wxButton( this, wxID_ANY, wxT("Select None"), wxDefaultPosition, wxDefaultSize, 0 );
	levelButtonSizer->Add( m_SelectNone, 0, wxALL, 5 );
	
	
	levelButtonSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_SelectAll = new wxButton( this, wxID_ANY, wxT("Select All"), wxDefaultPosition, wxDefaultSize, 0 );
	levelButtonSizer->Add( m_SelectAll, 0, wxALL, 5 );
	
	levelSizer->Add( levelButtonSizer, 0, wxEXPAND, 5 );
	
	mainSizer->Add( levelSizer, 1, wxEXPAND, 5 );
	
	
	mainSizer->Add( 0, 5, 0, 0, 5 );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Command Line (optional)") ), wxVERTICAL );
	
	m_Options = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer3->Add( m_Options, 0, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( sbSizer3, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bottomSizer;
	bottomSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_BuildserverLink = new wxHyperlinkCtrl( this, wxID_ANY, wxT("Buildserver Web Site"), wxT("http://buildserver"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	buttonSizer->Add( m_BuildserverLink, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	buttonSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_Rerun = new wxButton( this, wxID_ANY, wxT("Rerun"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_Rerun, 0, wxALL, 5 );
	
	m_RunGame = new wxButton( this, wxID_ANY, wxT("Run Game"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_RunGame, 0, wxALL, 5 );
	
	bottomSizer->Add( buttonSizer, 1, wxEXPAND|wxLEFT|wxRIGHT, 5 );
	
	mainSizer->Add( bottomSizer, 0, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

RunGamePanel::~RunGamePanel()
{
}

ClassChooserPanel::ClassChooserPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* outerSizer;
	outerSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* topSizer;
	topSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_DirectionsText = new wxTextCtrl( this, ID_DEFAULT, wxT("Choose a Runtime Class. \nThe dropdown shows classes alphabetically.\nThe tree shows classes in their class hierarchy. "), wxDefaultPosition, wxDefaultSize, wxTE_LEFT|wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH|wxTE_WORDWRAP|wxNO_BORDER );
	m_DirectionsText->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	m_DirectionsText->SetMinSize( wxSize( -1,50 ) );
	
	topSizer->Add( m_DirectionsText, 1, wxALL|wxEXPAND, 5 );
	
	outerSizer->Add( topSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* choiceSizer;
	choiceSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	m_Label = new wxStaticText( this, wxID_ANY, wxT("Runtime Class"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Label->Wrap( -1 );
	bSizer8->Add( m_Label, 0, wxALIGN_CENTER|wxALL, 5 );
	
	wxArrayString m_DropdownChoices;
	m_Dropdown = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_DropdownChoices, 0 );
	m_Dropdown->SetSelection( 0 );
	bSizer8->Add( m_Dropdown, 1, wxALL|wxEXPAND, 5 );
	
	choiceSizer->Add( bSizer8, 1, wxEXPAND, 5 );
	
	outerSizer->Add( choiceSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* treeSizer;
	treeSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Tree = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_HIDE_ROOT );
	treeSizer->Add( m_Tree, 1, wxALL|wxEXPAND, 5 );
	
	outerSizer->Add( treeSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* lineSizer;
	lineSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_StaticLine = new wxStaticLine( this, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	lineSizer->Add( m_StaticLine, 1, wxALL, 5 );
	
	outerSizer->Add( lineSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	
	buttonSizer->Add( 0, 0, 1, wxALL, 5 );
	
	m_Buttons = new wxStdDialogButtonSizer();
	m_ButtonsOK = new wxButton( this, wxID_OK );
	m_Buttons->AddButton( m_ButtonsOK );
	m_ButtonsCancel = new wxButton( this, wxID_CANCEL );
	m_Buttons->AddButton( m_ButtonsCancel );
	m_Buttons->Realize();
	buttonSizer->Add( m_Buttons, 0, wxALL, 5 );
	
	outerSizer->Add( buttonSizer, 0, wxALIGN_RIGHT|wxEXPAND, 5 );
	
	this->SetSizer( outerSizer );
	this->Layout();
}

ClassChooserPanel::~ClassChooserPanel()
{
}
