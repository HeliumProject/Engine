///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Precompile.h"

#include "Application/UI//MenuButton.h"
#include "Application/UI/AutoCompleteComboBox.h"
#include "Application/UI/Button.h"
#include "Application/UI/MenuButton.h"
#include "Application/UI/SortTreeCtrl.h"
#include "DirectoryCtrl.h"

#include "BrowserGenerated.h"

///////////////////////////////////////////////////////////////////////////
using namespace Luna;

BrowserFrameGenerated::BrowserFrameGenerated( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : Frame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( BrowserFrameGenerated::OnClose ) );
}

BrowserFrameGenerated::~BrowserFrameGenerated()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( BrowserFrameGenerated::OnClose ) );
}

NavigationPanelGenerated::NavigationPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxHORIZONTAL );
	
	m_BackButton = new Nocturnal::MenuButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 24,24 ), wxBU_AUTODRAW );
	m_BackButton->Enable( false );
	
	m_BackButton->Enable( false );
	
	bSizer20->Add( m_BackButton, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5 );
	
	m_ForwardButton = new Nocturnal::MenuButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 24,24 ), wxBU_AUTODRAW );
	m_ForwardButton->Enable( false );
	
	m_ForwardButton->Enable( false );
	
	bSizer20->Add( m_ForwardButton, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5 );
	
	m_UpFolderButton = new Nocturnal::Button( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 24,24 ), wxBU_AUTODRAW );
	m_UpFolderButton->Enable( false );
	
	m_UpFolderButton->Enable( false );
	
	bSizer20->Add( m_UpFolderButton, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5 );
	
	m_NavBarComboBox = new Nocturnal::AutoCompleteComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN|wxTE_PROCESS_ENTER|wxWANTS_CHARS ); 
	bSizer20->Add( m_NavBarComboBox, 1, wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	m_GoButton = new Nocturnal::Button( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 24,24 ), wxBU_AUTODRAW );
	m_GoButton->Enable( false );
	
	m_GoButton->Enable( false );
	
	bSizer20->Add( m_GoButton, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5 );
	
	wxStaticLine* m_staticline13;
	m_staticline13 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	bSizer20->Add( m_staticline13, 0, wxEXPAND | wxALL, 5 );
	
	m_OptionsButton = new Nocturnal::MenuButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 24,24 ), wxBU_AUTODRAW );
	bSizer20->Add( m_OptionsButton, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT|wxTOP, 5 );
	
	this->SetSizer( bSizer20 );
	this->Layout();
	
	// Connect Events
	m_BackButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NavigationPanelGenerated::OnBackButtonClick ), NULL, this );
	m_ForwardButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NavigationPanelGenerated::OnForwardButtonClick ), NULL, this );
	m_UpFolderButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NavigationPanelGenerated::OnUpFolderButtonClick ), NULL, this );
	m_NavBarComboBox->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( NavigationPanelGenerated::OnNavBarComboBox ), NULL, this );
	m_NavBarComboBox->Connect( wxEVT_MOUSEWHEEL, wxMouseEventHandler( NavigationPanelGenerated::OnNavBarComboBoxMouseWheel ), NULL, this );
	m_NavBarComboBox->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( NavigationPanelGenerated::OnNavBarText ), NULL, this );
	m_NavBarComboBox->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( NavigationPanelGenerated::OnNavBarTextEnter ), NULL, this );
	m_GoButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NavigationPanelGenerated::OnGoButtonClick ), NULL, this );
	m_OptionsButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NavigationPanelGenerated::OnOptionsButtonClick ), NULL, this );
}

NavigationPanelGenerated::~NavigationPanelGenerated()
{
	// Disconnect Events
	m_BackButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NavigationPanelGenerated::OnBackButtonClick ), NULL, this );
	m_ForwardButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NavigationPanelGenerated::OnForwardButtonClick ), NULL, this );
	m_UpFolderButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NavigationPanelGenerated::OnUpFolderButtonClick ), NULL, this );
	m_NavBarComboBox->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( NavigationPanelGenerated::OnNavBarComboBox ), NULL, this );
	m_NavBarComboBox->Disconnect( wxEVT_MOUSEWHEEL, wxMouseEventHandler( NavigationPanelGenerated::OnNavBarComboBoxMouseWheel ), NULL, this );
	m_NavBarComboBox->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( NavigationPanelGenerated::OnNavBarText ), NULL, this );
	m_NavBarComboBox->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( NavigationPanelGenerated::OnNavBarTextEnter ), NULL, this );
	m_GoButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NavigationPanelGenerated::OnGoButtonClick ), NULL, this );
	m_OptionsButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NavigationPanelGenerated::OnOptionsButtonClick ), NULL, this );
}

FoldersPanelGenerated::FoldersPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxVERTICAL );
	
	m_FoldersTreeCtrl = new Luna::DirectoryCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDIRCTRL_DIR_ONLY|wxDIRCTRL_EDIT_LABELS|wxNO_BORDER, wxEmptyString, 0 );
	
	m_FoldersTreeCtrl->ShowHidden( false );
	bSizer12->Add( m_FoldersTreeCtrl, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer12 );
	this->Layout();
	
	// Connect Events
	m_FoldersTreeCtrl->Connect( wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler( FoldersPanelGenerated::OnTreeBeginDrag ), NULL, this );
	m_FoldersTreeCtrl->Connect( wxEVT_COMMAND_TREE_DELETE_ITEM, wxTreeEventHandler( FoldersPanelGenerated::OnTreeItemDelete ), NULL, this );
	m_FoldersTreeCtrl->Connect( wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler( FoldersPanelGenerated::OnTreeEndDrag ), NULL, this );
	m_FoldersTreeCtrl->Connect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( FoldersPanelGenerated::OnTreeItemMenu ), NULL, this );
	m_FoldersTreeCtrl->Connect( wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler( FoldersPanelGenerated::OnTreeItemRightClick ), NULL, this );
}

FoldersPanelGenerated::~FoldersPanelGenerated()
{
	// Disconnect Events
	m_FoldersTreeCtrl->Disconnect( wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler( FoldersPanelGenerated::OnTreeBeginDrag ), NULL, this );
	m_FoldersTreeCtrl->Disconnect( wxEVT_COMMAND_TREE_DELETE_ITEM, wxTreeEventHandler( FoldersPanelGenerated::OnTreeItemDelete ), NULL, this );
	m_FoldersTreeCtrl->Disconnect( wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler( FoldersPanelGenerated::OnTreeEndDrag ), NULL, this );
	m_FoldersTreeCtrl->Disconnect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( FoldersPanelGenerated::OnTreeItemMenu ), NULL, this );
	m_FoldersTreeCtrl->Disconnect( wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler( FoldersPanelGenerated::OnTreeItemRightClick ), NULL, this );
}

ResultsPanelGenerated::ResultsPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
}

ResultsPanelGenerated::~ResultsPanelGenerated()
{
}

SearchPanelGenerated::SearchPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );
	
	m_ScrolledWindow = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxHSCROLL|wxTAB_TRAVERSAL|wxVSCROLL );
	m_ScrolledWindow->SetScrollRate( 20, 20 );
	wxBoxSizer* bSizer58;
	bSizer58 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText44 = new wxStaticText( m_ScrolledWindow, wxID_ANY, wxT("Advanced Search"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText44->Wrap( -1 );
	m_staticText44->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer58->Add( m_staticText44, 0, wxLEFT|wxRIGHT|wxTOP, 5 );
	
	wxBoxSizer* bSizer67;
	bSizer67 = new wxBoxSizer( wxHORIZONTAL );
	
	m_SearchVaultRadio = new wxRadioButton( m_ScrolledWindow, wxID_ANY, wxT("Search Vault"), wxDefaultPosition, wxDefaultSize, 0 );
	m_SearchVaultRadio->SetValue( true ); 
	bSizer67->Add( m_SearchVaultRadio, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5 );
	
	m_SearchCollectionRadio = new wxRadioButton( m_ScrolledWindow, wxID_ANY, wxT("Search within Collection:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer67->Add( m_SearchCollectionRadio, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5 );
	
	wxString m_CollectionChoiceChoices[] = { wxT("<Select Collection>") };
	int m_CollectionChoiceNChoices = sizeof( m_CollectionChoiceChoices ) / sizeof( wxString );
	m_CollectionChoice = new wxChoice( m_ScrolledWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_CollectionChoiceNChoices, m_CollectionChoiceChoices, 0 );
	m_CollectionChoice->SetSelection( 0 );
	m_CollectionChoice->Enable( false );
	m_CollectionChoice->SetMinSize( wxSize( 150,-1 ) );
	
	bSizer67->Add( m_CollectionChoice, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_CollectionHelpBitmap = new wxStaticBitmap( m_ScrolledWindow, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	m_CollectionHelpBitmap->SetToolTip( wxT("You can search the entire Vault, or\nchoose to search within a given collection.") );
	
	bSizer67->Add( m_CollectionHelpBitmap, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2 );
	
	bSizer58->Add( bSizer67, 0, wxEXPAND|wxLEFT, 5 );
	
	
	bSizer58->Add( 0, 10, 0, 0, 5 );
	
	wxStaticText* m_staticText4;
	m_staticText4 = new wxStaticText( m_ScrolledWindow, wxID_ANY, wxT("File Path (All or part of the asset file path; use '*' for wildcard):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	m_staticText4->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer58->Add( m_staticText4, 0, wxLEFT|wxRIGHT|wxTOP, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticText* m_staticText5;
	m_staticText5 = new wxStaticText( m_ScrolledWindow, wxID_ANY, wxT("All of these words:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer1->Add( m_staticText5, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5 );
	
	wxBoxSizer* bSizer62;
	bSizer62 = new wxBoxSizer( wxHORIZONTAL );
	
	m_WordsTextCtrl = new wxTextCtrl( m_ScrolledWindow, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_WordsTextCtrl->SetToolTip( wxT("This will search for file paths that\ncontain all of these words, in any order.") );
	
	bSizer62->Add( m_WordsTextCtrl, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_WordsHelpBitmap = new wxStaticBitmap( m_ScrolledWindow, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	m_WordsHelpBitmap->SetToolTip( wxT("This will search for file paths that\ncontain all of these words, in any order.") );
	
	bSizer62->Add( m_WordsHelpBitmap, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2 );
	
	fgSizer1->Add( bSizer62, 0, wxEXPAND, 5 );
	
	wxStaticText* m_staticText51;
	m_staticText51 = new wxStaticText( m_ScrolledWindow, wxID_ANY, wxT("This exact phrase:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText51->Wrap( -1 );
	fgSizer1->Add( m_staticText51, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5 );
	
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxHORIZONTAL );
	
	m_PhraseTextCtrl = new wxTextCtrl( m_ScrolledWindow, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_PhraseTextCtrl->SetToolTip( wxT("You can do this in standard search by\n\"surrounding your phrase with quotes\"") );
	
	bSizer14->Add( m_PhraseTextCtrl, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_PhraseHelpBitmap = new wxStaticBitmap( m_ScrolledWindow, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	m_PhraseHelpBitmap->SetToolTip( wxT("You can do this in standard search by\n\"surrounding your phrase with quotes\"") );
	
	bSizer14->Add( m_PhraseHelpBitmap, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2 );
	
	fgSizer1->Add( bSizer14, 0, wxEXPAND, 5 );
	
	wxStaticText* m_staticText20;
	m_staticText20 = new wxStaticText( m_ScrolledWindow, wxID_ANY, wxT("File Type:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText20->Wrap( -1 );
	fgSizer1->Add( m_staticText20, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5 );
	
	wxBoxSizer* bSizer141;
	bSizer141 = new wxBoxSizer( wxHORIZONTAL );
	
	wxString m_FileTypeChoiceChoices[] = { wxT("All files (*.*)") };
	int m_FileTypeChoiceNChoices = sizeof( m_FileTypeChoiceChoices ) / sizeof( wxString );
	m_FileTypeChoice = new wxChoice( m_ScrolledWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_FileTypeChoiceNChoices, m_FileTypeChoiceChoices, 0 );
	m_FileTypeChoice->SetSelection( 0 );
	bSizer141->Add( m_FileTypeChoice, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	fgSizer1->Add( bSizer141, 0, wxEXPAND, 5 );
	
	wxStaticText* m_staticText15;
	m_staticText15 = new wxStaticText( m_ScrolledWindow, wxID_ANY, wxT("Search within folder:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	fgSizer1->Add( m_staticText15, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5 );
	
	wxBoxSizer* bSizer26;
	bSizer26 = new wxBoxSizer( wxHORIZONTAL );
	
	wxString m_FolderChoiceChoices[] = { wxT("X:\\resistance\\assets\\devel\\") };
	int m_FolderChoiceNChoices = sizeof( m_FolderChoiceChoices ) / sizeof( wxString );
	m_FolderChoice = new wxChoice( m_ScrolledWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_FolderChoiceNChoices, m_FolderChoiceChoices, 0 );
	m_FolderChoice->SetSelection( 0 );
	bSizer26->Add( m_FolderChoice, 1, wxALL, 5 );
	
	m_BrowserFoldersButton = new wxButton( m_ScrolledWindow, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer26->Add( m_BrowserFoldersButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	fgSizer1->Add( bSizer26, 0, wxEXPAND, 5 );
	
	bSizer58->Add( fgSizer1, 0, wxEXPAND|wxLEFT, 5 );
	
	
	bSizer58->Add( 0, 8, 0, 0, 5 );
	
	wxStaticText* m_staticText41;
	m_staticText41 = new wxStaticText( m_ScrolledWindow, wxID_ANY, wxT("Special Components:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText41->Wrap( -1 );
	m_staticText41->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer58->Add( m_staticText41, 0, wxLEFT|wxRIGHT|wxTOP, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticText* m_staticText19;
	m_staticText19 = new wxStaticText( m_ScrolledWindow, wxID_ANY, wxT("Create by:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	fgSizer2->Add( m_staticText19, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5 );
	
	wxBoxSizer* bSizer28;
	bSizer28 = new wxBoxSizer( wxHORIZONTAL );
	
	m_CreatedByComboBox = new wxComboBox( m_ScrolledWindow, wxID_ANY, wxT("<Select User>"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_CreatedByComboBox->Append( wxT("<Select User>") );
	m_CreatedByComboBox->SetMinSize( wxSize( 150,-1 ) );
	
	bSizer28->Add( m_CreatedByComboBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	fgSizer2->Add( bSizer28, 0, wxEXPAND, 5 );
	
	wxStaticText* m_staticText191;
	m_staticText191 = new wxStaticText( m_ScrolledWindow, wxID_ANY, wxT("Asset Type:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText191->Wrap( -1 );
	fgSizer2->Add( m_staticText191, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5 );
	
	wxBoxSizer* bSizer285;
	bSizer285 = new wxBoxSizer( wxHORIZONTAL );
	
	wxString m_AssetTypeChoiceChoices[] = { wxT("<Select Type>") };
	int m_AssetTypeChoiceNChoices = sizeof( m_AssetTypeChoiceChoices ) / sizeof( wxString );
	m_AssetTypeChoice = new wxChoice( m_ScrolledWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_AssetTypeChoiceNChoices, m_AssetTypeChoiceChoices, 0 );
	m_AssetTypeChoice->SetSelection( 0 );
	m_AssetTypeChoice->SetMinSize( wxSize( 150,-1 ) );
	
	bSizer285->Add( m_AssetTypeChoice, 0, wxALL, 5 );
	
	fgSizer2->Add( bSizer285, 0, wxEXPAND, 5 );
	
	wxStaticText* m_staticText26;
	m_staticText26 = new wxStaticText( m_ScrolledWindow, wxID_ANY, wxT("File ID:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText26->Wrap( -1 );
	fgSizer2->Add( m_staticText26, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5 );
	
	wxBoxSizer* bSizer282;
	bSizer282 = new wxBoxSizer( wxHORIZONTAL );
	
	m_FileIDTextCtrl = new wxTextCtrl( m_ScrolledWindow, wxID_ANY, wxT("<Hex or Decimal file ID>"), wxDefaultPosition, wxDefaultSize, 0 );
	m_FileIDTextCtrl->SetMaxLength( 30 ); 
	m_FileIDTextCtrl->SetToolTip( wxT("Search by the file's ID (or TUID) using\nthe Hex or Decimal value.") );
	m_FileIDTextCtrl->SetMinSize( wxSize( 200,-1 ) );
	
	bSizer282->Add( m_FileIDTextCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_FileIDHelpBitmap = new wxStaticBitmap( m_ScrolledWindow, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	m_FileIDHelpBitmap->SetToolTip( wxT("Search by the file's ID (or TUID) using\nthe Hex or Decimal value.") );
	
	bSizer282->Add( m_FileIDHelpBitmap, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2 );
	
	fgSizer2->Add( bSizer282, 0, wxEXPAND, 5 );
	
	m_staticText151 = new wxStaticText( m_ScrolledWindow, wxID_ANY, wxT("Has Component:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText151->Wrap( -1 );
	fgSizer2->Add( m_staticText151, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5 );
	
	wxBoxSizer* bSizer261;
	bSizer261 = new wxBoxSizer( wxHORIZONTAL );
	
	wxString m_ComponentNameChoiceChoices[] = { wxT("<Select Component Name>") };
	int m_ComponentNameChoiceNChoices = sizeof( m_ComponentNameChoiceChoices ) / sizeof( wxString );
	m_ComponentNameChoice = new wxChoice( m_ScrolledWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_ComponentNameChoiceNChoices, m_ComponentNameChoiceChoices, 0 );
	m_ComponentNameChoice->SetSelection( 0 );
	m_ComponentNameChoice->SetToolTip( wxT("Here you can search for the value of\na given component.") );
	m_ComponentNameChoice->SetMinSize( wxSize( 150,-1 ) );
	
	bSizer261->Add( m_ComponentNameChoice, 0, wxALL, 5 );
	
	m_ComponentValueTextCtrl = new wxTextCtrl( m_ScrolledWindow, wxID_ANY, wxT("<Component Value>"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ComponentValueTextCtrl->SetToolTip( wxT("Here you can search for the value of\na given component.") );
	m_ComponentValueTextCtrl->SetMinSize( wxSize( 150,-1 ) );
	
	bSizer261->Add( m_ComponentValueTextCtrl, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_ComponentHelpBitmap = new wxStaticBitmap( m_ScrolledWindow, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	m_ComponentHelpBitmap->SetToolTip( wxT("Here you can search for the value of\na given component.") );
	
	bSizer261->Add( m_ComponentHelpBitmap, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2 );
	
	fgSizer2->Add( bSizer261, 1, wxEXPAND, 5 );
	
	wxStaticText* m_staticText52;
	m_staticText52 = new wxStaticText( m_ScrolledWindow, wxID_ANY, wxT("Placed in Level:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText52->Wrap( -1 );
	fgSizer2->Add( m_staticText52, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5 );
	
	wxBoxSizer* bSizer142;
	bSizer142 = new wxBoxSizer( wxHORIZONTAL );
	
	m_LevelTextCtrl = new wxTextCtrl( m_ScrolledWindow, wxID_ANY, wxT("<Level Name>"), wxDefaultPosition, wxDefaultSize, 0 );
	m_LevelTextCtrl->SetToolTip( wxT("Add a level name here to search for\nassets that are placed in a given level.") );
	m_LevelTextCtrl->SetMinSize( wxSize( 200,-1 ) );
	
	bSizer142->Add( m_LevelTextCtrl, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_LevelHelpBitmap = new wxStaticBitmap( m_ScrolledWindow, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	m_LevelHelpBitmap->SetToolTip( wxT("Add a level name here to search for\nassets that are placed in a given level.") );
	
	bSizer142->Add( m_LevelHelpBitmap, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2 );
	
	fgSizer2->Add( bSizer142, 0, wxEXPAND, 5 );
	
	wxStaticText* m_staticText511;
	m_staticText511 = new wxStaticText( m_ScrolledWindow, wxID_ANY, wxT("Uses the Shader:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText511->Wrap( -1 );
	fgSizer2->Add( m_staticText511, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5 );
	
	wxBoxSizer* bSizer1411;
	bSizer1411 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ShaderTextCtrl = new wxTextCtrl( m_ScrolledWindow, wxID_ANY, wxT("<Shader Name>"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ShaderTextCtrl->SetToolTip( wxT("Add a shader name here to only find\nassets that use the given shader.") );
	m_ShaderTextCtrl->SetMinSize( wxSize( 200,-1 ) );
	
	bSizer1411->Add( m_ShaderTextCtrl, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_ShaderHelpBitmap = new wxStaticBitmap( m_ScrolledWindow, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	m_ShaderHelpBitmap->SetToolTip( wxT("Add a shader name here to only find\nassets that use the given shader.") );
	
	bSizer1411->Add( m_ShaderHelpBitmap, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2 );
	
	fgSizer2->Add( bSizer1411, 0, wxEXPAND, 5 );
	
	bSizer58->Add( fgSizer2, 0, wxEXPAND|wxLEFT, 5 );
	
	
	bSizer58->Add( 0, 10, 0, 0, 5 );
	
	m_staticline3 = new wxStaticLine( m_ScrolledWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer58->Add( m_staticline3, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer64;
	bSizer64 = new wxBoxSizer( wxHORIZONTAL );
	
	m_GoButton = new wxButton( m_ScrolledWindow, wxID_ANY, wxT("Search"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer64->Add( m_GoButton, 0, wxBOTTOM|wxLEFT, 5 );
	
	m_CancelButton = new wxButton( m_ScrolledWindow, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer64->Add( m_CancelButton, 0, wxBOTTOM|wxLEFT, 5 );
	
	
	bSizer64->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_ClearButton = new wxButton( m_ScrolledWindow, wxID_ANY, wxT("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ClearButton->Enable( false );
	m_ClearButton->Hide();
	
	bSizer64->Add( m_ClearButton, 0, wxBOTTOM|wxRIGHT, 5 );
	
	bSizer58->Add( bSizer64, 0, wxEXPAND, 5 );
	
	m_ScrolledWindow->SetSizer( bSizer58 );
	m_ScrolledWindow->Layout();
	bSizer58->Fit( m_ScrolledWindow );
	bSizer13->Add( m_ScrolledWindow, 1, wxEXPAND, 2 );
	
	this->SetSizer( bSizer13 );
	this->Layout();
	
	// Connect Events
	this->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SearchPanelGenerated::OnMouseLeftDoubleClick ) );
	this->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( SearchPanelGenerated::OnMouseLeftDown ) );
	this->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( SearchPanelGenerated::OnMouseRightDown ) );
	m_WordsTextCtrl->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( SearchPanelGenerated::OnFieldTextEnter ), NULL, this );
	m_PhraseTextCtrl->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( SearchPanelGenerated::OnFieldTextEnter ), NULL, this );
	m_BrowserFoldersButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SearchPanelGenerated::OnFolderBrowseButtonClick ), NULL, this );
	m_CreatedByComboBox->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SearchPanelGenerated::OnFieldText ), NULL, this );
	m_CreatedByComboBox->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( SearchPanelGenerated::OnFieldTextEnter ), NULL, this );
	m_FileIDTextCtrl->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SearchPanelGenerated::OnFieldText ), NULL, this );
	m_FileIDTextCtrl->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( SearchPanelGenerated::OnFieldTextEnter ), NULL, this );
	m_LevelTextCtrl->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SearchPanelGenerated::OnFieldText ), NULL, this );
	m_LevelTextCtrl->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( SearchPanelGenerated::OnFieldTextEnter ), NULL, this );
	m_ShaderTextCtrl->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SearchPanelGenerated::OnFieldText ), NULL, this );
	m_ShaderTextCtrl->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( SearchPanelGenerated::OnFieldTextEnter ), NULL, this );
	m_GoButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SearchPanelGenerated::OnSearchButtonClick ), NULL, this );
	m_CancelButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SearchPanelGenerated::OnCancelButtonClick ), NULL, this );
}

SearchPanelGenerated::~SearchPanelGenerated()
{
	// Disconnect Events
	this->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SearchPanelGenerated::OnMouseLeftDoubleClick ) );
	this->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( SearchPanelGenerated::OnMouseLeftDown ) );
	this->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( SearchPanelGenerated::OnMouseRightDown ) );
	m_WordsTextCtrl->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( SearchPanelGenerated::OnFieldTextEnter ), NULL, this );
	m_PhraseTextCtrl->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( SearchPanelGenerated::OnFieldTextEnter ), NULL, this );
	m_BrowserFoldersButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SearchPanelGenerated::OnFolderBrowseButtonClick ), NULL, this );
	m_CreatedByComboBox->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SearchPanelGenerated::OnFieldText ), NULL, this );
	m_CreatedByComboBox->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( SearchPanelGenerated::OnFieldTextEnter ), NULL, this );
	m_FileIDTextCtrl->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SearchPanelGenerated::OnFieldText ), NULL, this );
	m_FileIDTextCtrl->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( SearchPanelGenerated::OnFieldTextEnter ), NULL, this );
	m_LevelTextCtrl->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SearchPanelGenerated::OnFieldText ), NULL, this );
	m_LevelTextCtrl->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( SearchPanelGenerated::OnFieldTextEnter ), NULL, this );
	m_ShaderTextCtrl->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SearchPanelGenerated::OnFieldText ), NULL, this );
	m_ShaderTextCtrl->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( SearchPanelGenerated::OnFieldTextEnter ), NULL, this );
	m_GoButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SearchPanelGenerated::OnSearchButtonClick ), NULL, this );
	m_CancelButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SearchPanelGenerated::OnCancelButtonClick ), NULL, this );
}

CollectionsPanelGenerated::CollectionsPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );
	
	m_MyCollectionsToolBar = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL ); 
	m_MyCollectionsTitle = new wxStaticText( m_MyCollectionsToolBar, wxID_ANY, wxT("My Collections:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_MyCollectionsTitle->Wrap( -1 );
	m_MyCollectionsTitle->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	m_MyCollectionsToolBar->AddControl( m_MyCollectionsTitle );
	m_MyCollectionsToolBar->Realize();
	
	bSizer14->Add( m_MyCollectionsToolBar, 0, wxEXPAND|wxLEFT, 5 );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxHORIZONTAL );
	
	m_NewDependencyCollectionButton = new Nocturnal::Button( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_NewDependencyCollectionButton->Hide();
	
	m_NewDependencyCollectionButton->Hide();
	
	bSizer17->Add( m_NewDependencyCollectionButton, 0, wxALL, 5 );
	
	m_NewCollectionButton = new Nocturnal::Button( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_NewCollectionButton->Hide();
	
	m_NewCollectionButton->Hide();
	
	bSizer17->Add( m_NewCollectionButton, 0, wxALL, 5 );
	
	bSizer14->Add( bSizer17, 0, wxEXPAND, 5 );
	
	wxBoxSizer* m_MyCollectionsCtrlSizer;
	m_MyCollectionsCtrlSizer = new wxBoxSizer( wxVERTICAL );
	
	m_MyCollectionsTreeCtrl = new Nocturnal::SortTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxTR_EDIT_LABELS|wxTR_HIDE_ROOT|wxTR_NO_BUTTONS|wxTR_SINGLE|wxNO_BORDER );
	m_MyCollectionsTreeCtrl->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	m_MyCollectionsCtrlSizer->Add( m_MyCollectionsTreeCtrl, 1, wxEXPAND|wxLEFT, 5 );
	
	bSizer14->Add( m_MyCollectionsCtrlSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer171;
	bSizer171 = new wxBoxSizer( wxVERTICAL );
	
	m_TempCollectionsTitle = new wxStaticText( this, wxID_ANY, wxT("Dynamic Collections:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TempCollectionsTitle->Wrap( -1 );
	m_TempCollectionsTitle->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer171->Add( m_TempCollectionsTitle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	bSizer14->Add( bSizer171, 0, wxEXPAND, 5 );
	
	wxBoxSizer* m_TempCollectionsCtrlSizer;
	m_TempCollectionsCtrlSizer = new wxBoxSizer( wxVERTICAL );
	
	m_TempCollectionsTreeCtrl = new Nocturnal::SortTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_HIDE_ROOT|wxTR_NO_BUTTONS|wxTR_SINGLE|wxNO_BORDER );
	m_TempCollectionsTreeCtrl->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	m_TempCollectionsTreeCtrl->Enable( false );
	
	m_TempCollectionsCtrlSizer->Add( m_TempCollectionsTreeCtrl, 1, wxEXPAND|wxLEFT, 5 );
	
	bSizer14->Add( m_TempCollectionsCtrlSizer, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer14 );
	this->Layout();
	
	// Connect Events
	this->Connect( wxEVT_SIZE, wxSizeEventHandler( CollectionsPanelGenerated::OnSizeCollectionsPanel ) );
	m_MyCollectionsTitle->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( CollectionsPanelGenerated::OnMyCollectionsTitleMenu ), NULL, this );
	m_NewDependencyCollectionButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CollectionsPanelGenerated::OnNewCollection ), NULL, this );
	m_NewCollectionButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CollectionsPanelGenerated::OnNewCollection ), NULL, this );
	m_MyCollectionsTreeCtrl->Connect( wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler( CollectionsPanelGenerated::OnMyCollectionsBeginLabelEdit ), NULL, this );
	m_MyCollectionsTreeCtrl->Connect( wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler( CollectionsPanelGenerated::OnMyCollectionsEndLabelEdit ), NULL, this );
	m_MyCollectionsTreeCtrl->Connect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( CollectionsPanelGenerated::OnMyCollectionsDoubleClick ), NULL, this );
	m_MyCollectionsTreeCtrl->Connect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( CollectionsPanelGenerated::OnMyCollectionsMenu ), NULL, this );
	m_TempCollectionsTreeCtrl->Connect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( CollectionsPanelGenerated::OnTempCollectionsDoubleClick ), NULL, this );
	m_TempCollectionsTreeCtrl->Connect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( CollectionsPanelGenerated::OnTempCollectionsMenu ), NULL, this );
	m_TempCollectionsTreeCtrl->Connect( wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler( CollectionsPanelGenerated::OnTempCollectionsMenu ), NULL, this );
}

CollectionsPanelGenerated::~CollectionsPanelGenerated()
{
	// Disconnect Events
	this->Disconnect( wxEVT_SIZE, wxSizeEventHandler( CollectionsPanelGenerated::OnSizeCollectionsPanel ) );
	m_MyCollectionsTitle->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( CollectionsPanelGenerated::OnMyCollectionsTitleMenu ), NULL, this );
	m_NewDependencyCollectionButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CollectionsPanelGenerated::OnNewCollection ), NULL, this );
	m_NewCollectionButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CollectionsPanelGenerated::OnNewCollection ), NULL, this );
	m_MyCollectionsTreeCtrl->Disconnect( wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler( CollectionsPanelGenerated::OnMyCollectionsBeginLabelEdit ), NULL, this );
	m_MyCollectionsTreeCtrl->Disconnect( wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler( CollectionsPanelGenerated::OnMyCollectionsEndLabelEdit ), NULL, this );
	m_MyCollectionsTreeCtrl->Disconnect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( CollectionsPanelGenerated::OnMyCollectionsDoubleClick ), NULL, this );
	m_MyCollectionsTreeCtrl->Disconnect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( CollectionsPanelGenerated::OnMyCollectionsMenu ), NULL, this );
	m_TempCollectionsTreeCtrl->Disconnect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( CollectionsPanelGenerated::OnTempCollectionsDoubleClick ), NULL, this );
	m_TempCollectionsTreeCtrl->Disconnect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( CollectionsPanelGenerated::OnTempCollectionsMenu ), NULL, this );
	m_TempCollectionsTreeCtrl->Disconnect( wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler( CollectionsPanelGenerated::OnTempCollectionsMenu ), NULL, this );
}

PreviewPanelGenerated::PreviewPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* sizer;
	sizer = new wxBoxSizer( wxVERTICAL );
	
	m_PreviewWindow = new BrowserPreviewWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxFULL_REPAINT_ON_RESIZE, wxT( "Luna::PreviewWindow" ) );
	m_PreviewWindow->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_APPWORKSPACE ) );
	
	sizer->Add( m_PreviewWindow, 1, wxEXPAND, 5 );
	
	m_BottomPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_BottomPanel->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	wxBoxSizer* labelSizer;
	labelSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_Label = new wxTextCtrl( m_BottomPanel, wxID_ANY, wxT("<Label>"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE|wxTE_READONLY|wxNO_BORDER );
	m_Label->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	labelSizer->Add( m_Label, 1, wxALL, 2 );
	
	m_BottomPanel->SetSizer( labelSizer );
	m_BottomPanel->Layout();
	labelSizer->Fit( m_BottomPanel );
	sizer->Add( m_BottomPanel, 0, wxEXPAND, 5 );
	
	this->SetSizer( sizer );
	this->Layout();
}

PreviewPanelGenerated::~PreviewPanelGenerated()
{
}

DetailsFrameGenerated::DetailsFrameGenerated( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 300,350 ), wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ScrollWindow = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_ScrollWindow->SetScrollRate( 5, 5 );
	wxBoxSizer* scrollSizer;
	scrollSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* sizer1;
	sizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	m_LabelName = new wxStaticText( m_ScrollWindow, wxID_ANY, wxT("Name"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_LabelName->Wrap( -1 );
	sizer1->Add( m_LabelName, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Name = new wxTextCtrl( m_ScrollWindow, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_Name->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	sizer1->Add( m_Name, 1, wxALL|wxALIGN_CENTER_VERTICAL, 2 );
	
	scrollSizer->Add( sizer1, 0, wxEXPAND, 5 );
	
	wxBoxSizer* sizer2;
	sizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_LabelFileType = new wxStaticText( m_ScrollWindow, wxID_ANY, wxT("File Type"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_LabelFileType->Wrap( -1 );
	sizer2->Add( m_LabelFileType, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_FileType = new wxTextCtrl( m_ScrollWindow, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_FileType->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	sizer2->Add( m_FileType, 1, wxALL|wxALIGN_CENTER_VERTICAL, 2 );
	
	scrollSizer->Add( sizer2, 0, wxEXPAND, 5 );
	
	wxBoxSizer* sizer3;
	sizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_LabelFileID = new wxStaticText( m_ScrollWindow, wxID_ANY, wxT("File ID"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_LabelFileID->Wrap( -1 );
	sizer3->Add( m_LabelFileID, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_FileID = new wxTextCtrl( m_ScrollWindow, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_FileID->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	sizer3->Add( m_FileID, 1, wxALL|wxALIGN_CENTER_VERTICAL, 2 );
	
	scrollSizer->Add( sizer3, 0, wxEXPAND, 5 );
	
	wxBoxSizer* sizer4;
	sizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_LabelFolder = new wxStaticText( m_ScrollWindow, wxID_ANY, wxT("Folder"), wxDefaultPosition, wxDefaultSize, 0 );
	m_LabelFolder->Wrap( -1 );
	sizer4->Add( m_LabelFolder, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP, 5 );
	
	m_Folder = new wxTextCtrl( m_ScrollWindow, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_Folder->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	sizer4->Add( m_Folder, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 2 );
	
	scrollSizer->Add( sizer4, 0, wxBOTTOM|wxEXPAND, 5 );
	
	wxBoxSizer* sizer5;
	sizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_RevisionPanel = new wxPanel( m_ScrollWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* revisionSizer;
	revisionSizer = new wxBoxSizer( wxVERTICAL );
	
	m_LabelPerforce = new wxStaticText( m_RevisionPanel, wxID_ANY, wxT("Perforce"), wxDefaultPosition, wxDefaultSize, 0 );
	m_LabelPerforce->Wrap( -1 );
	m_LabelPerforce->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	revisionSizer->Add( m_LabelPerforce, 0, wxALL, 5 );
	
	wxBoxSizer* statusSizer;
	statusSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_LabelRevisionStatus = new wxStaticText( m_RevisionPanel, wxID_ANY, wxT("Status"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_LabelRevisionStatus->Wrap( -1 );
	statusSizer->Add( m_LabelRevisionStatus, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_StatusPanel = new wxPanel( m_RevisionPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* statusInnerSizer;
	statusInnerSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_RevisionStatusIcon = new wxStaticBitmap( m_StatusPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 16,16 ), 0 );
	statusInnerSizer->Add( m_RevisionStatusIcon, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 1 );
	
	m_RevisionStatus = new wxStaticText( m_StatusPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_RevisionStatus->Wrap( -1 );
	statusInnerSizer->Add( m_RevisionStatus, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_StatusPanel->SetSizer( statusInnerSizer );
	m_StatusPanel->Layout();
	statusInnerSizer->Fit( m_StatusPanel );
	statusSizer->Add( m_StatusPanel, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 2 );
	
	revisionSizer->Add( statusSizer, 0, wxEXPAND, 5 );
	
	m_LastCheckInPanel = new wxPanel( m_RevisionPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* lastCheckinSizer;
	lastCheckinSizer = new wxBoxSizer( wxVERTICAL );
	
	m_LabelLastCheckIn = new wxStaticText( m_LastCheckInPanel, wxID_ANY, wxT("Last Check In"), wxDefaultPosition, wxDefaultSize, 0 );
	m_LabelLastCheckIn->Wrap( -1 );
	lastCheckinSizer->Add( m_LabelLastCheckIn, 0, wxLEFT|wxTOP, 5 );
	
	wxBoxSizer* innerCheckinSizer;
	innerCheckinSizer = new wxBoxSizer( wxVERTICAL );
	
	m_LastCheckIn = new wxTextCtrl( m_LastCheckInPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,75 ), wxTE_DONTWRAP|wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH );
	m_LastCheckIn->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	innerCheckinSizer->Add( m_LastCheckIn, 0, wxALL|wxEXPAND, 2 );
	
	lastCheckinSizer->Add( innerCheckinSizer, 1, wxEXPAND, 5 );
	
	m_LastCheckInPanel->SetSizer( lastCheckinSizer );
	m_LastCheckInPanel->Layout();
	lastCheckinSizer->Fit( m_LastCheckInPanel );
	revisionSizer->Add( m_LastCheckInPanel, 1, wxEXPAND, 5 );
	
	m_FirstCheckInPanel = new wxPanel( m_RevisionPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* firstCheckInSizer;
	firstCheckInSizer = new wxBoxSizer( wxVERTICAL );
	
	m_LabelFirstCheckIn = new wxStaticText( m_FirstCheckInPanel, wxID_ANY, wxT("First Check In"), wxDefaultPosition, wxDefaultSize, 0 );
	m_LabelFirstCheckIn->Wrap( -1 );
	firstCheckInSizer->Add( m_LabelFirstCheckIn, 0, wxLEFT|wxTOP, 5 );
	
	wxBoxSizer* innerFirstCheckInSizer;
	innerFirstCheckInSizer = new wxBoxSizer( wxVERTICAL );
	
	m_FirstCheckIn = new wxTextCtrl( m_FirstCheckInPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,75 ), wxTE_DONTWRAP|wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH );
	m_FirstCheckIn->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	innerFirstCheckInSizer->Add( m_FirstCheckIn, 0, wxALL|wxEXPAND, 2 );
	
	firstCheckInSizer->Add( innerFirstCheckInSizer, 1, wxEXPAND, 5 );
	
	m_FirstCheckInPanel->SetSizer( firstCheckInSizer );
	m_FirstCheckInPanel->Layout();
	firstCheckInSizer->Fit( m_FirstCheckInPanel );
	revisionSizer->Add( m_FirstCheckInPanel, 1, wxEXPAND, 5 );
	
	m_RevisionPanel->SetSizer( revisionSizer );
	m_RevisionPanel->Layout();
	revisionSizer->Fit( m_RevisionPanel );
	sizer5->Add( m_RevisionPanel, 0, wxEXPAND, 5 );
	
	scrollSizer->Add( sizer5, 0, wxEXPAND, 5 );
	
	m_ScrollWindow->SetSizer( scrollSizer );
	m_ScrollWindow->Layout();
	scrollSizer->Fit( m_ScrollWindow );
	mainSizer->Add( m_ScrollWindow, 1, wxEXPAND | wxALL, 0 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

DetailsFrameGenerated::~DetailsFrameGenerated()
{
}
