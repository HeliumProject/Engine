///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "UIToolKit/AutoCompleteComboBox.h"
#include "UIToolKit/SortableListView.h"

#include "FileBrowserGenerated.h"

///////////////////////////////////////////////////////////////////////////
using namespace File;

FileBrowserGenerated::FileBrowserGenerated( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 600,500 ), wxDefaultSize );
	
	wxBoxSizer* bSizer29;
	bSizer29 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticTextMessage = new wxStaticText( this, ID_DEFAULT, wxT("Search by any or all of the criteria below."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextMessage->Wrap( -1 );
	m_staticTextMessage->SetFont( wxFont( 8, 70, 90, 92, false, wxT("MS Sans Serif") ) );
	
	bSizer23->Add( m_staticTextMessage, 1, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );
	
	bSizer29->Add( bSizer23, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxVERTICAL );
	
	m_panelBasic = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer85;
	bSizer85 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer361;
	bSizer361 = new wxBoxSizer( wxVERTICAL );
	
	m_staticTextSearchBasic = new wxStaticText( m_panelBasic, ID_DEFAULT, wxT("All or part of the file name (use '*' for wildcard):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextSearchBasic->Wrap( -1 );
	bSizer361->Add( m_staticTextSearchBasic, 0, wxLEFT|wxRIGHT|wxTOP, 5 );
	
	m_comboBoxSearch = new UIToolKit::AutoCompleteComboBox( m_panelBasic, ID_SearchQuery, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0|wxWANTS_CHARS ); 
	bSizer361->Add( m_comboBoxSearch, 0, wxALL|wxEXPAND, 5 );
	
	m_staticTextFileType = new wxStaticText( m_panelBasic, ID_DEFAULT, wxT("File type:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextFileType->Wrap( -1 );
	bSizer361->Add( m_staticTextFileType, 0, wxLEFT|wxRIGHT|wxTOP, 5 );
	
	wxString m_choiceFiletypeChoices[] = { wxT("All files (*.*)") };
	int m_choiceFiletypeNChoices = sizeof( m_choiceFiletypeChoices ) / sizeof( wxString );
	m_choiceFiletype = new wxChoice( m_panelBasic, ID_FileType, wxDefaultPosition, wxDefaultSize, m_choiceFiletypeNChoices, m_choiceFiletypeChoices, 0 );
	m_choiceFiletype->SetSelection( 0 );
	bSizer361->Add( m_choiceFiletype, 0, wxALL|wxEXPAND, 5 );
	
	m_staticTextLookIn = new wxStaticText( m_panelBasic, ID_DEFAULT, wxT("Look in:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextLookIn->Wrap( -1 );
	bSizer361->Add( m_staticTextLookIn, 0, wxLEFT|wxRIGHT|wxTOP, 5 );
	
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxHORIZONTAL );
	
	m_comboBoxLookIn = new UIToolKit::AutoCompleteComboBox( m_panelBasic, ID_LookInPath, wxT("X:/rcf/assets/devel"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0|wxWANTS_CHARS ); 
	bSizer13->Add( m_comboBoxLookIn, 1, wxALL|wxEXPAND, 5 );
	
	m_buttonBrowse = new wxButton( m_panelBasic, ID_BrowseButton, wxT("..."), wxDefaultPosition, wxSize( 24,-1 ), 0 );
	bSizer13->Add( m_buttonBrowse, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2 );
	
	bSizer361->Add( bSizer13, 0, wxEXPAND, 5 );
	
	bSizer85->Add( bSizer361, 1, wxEXPAND|wxRIGHT, 5 );
	
	m_staticline8 = new wxStaticLine( m_panelBasic, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	bSizer85->Add( m_staticline8, 0, wxALL|wxEXPAND|wxLEFT|wxRIGHT, 2 );
	
	wxBoxSizer* bSizer87;
	bSizer87 = new wxBoxSizer( wxVERTICAL );
	
	m_staticTextTUID = new wxStaticText( m_panelBasic, ID_DEFAULT, wxT("ID (TUID):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextTUID->Wrap( -1 );
	bSizer87->Add( m_staticTextTUID, 0, wxLEFT|wxRIGHT|wxTOP, 5 );
	
	m_comboBoxTUID = new UIToolKit::AutoCompleteComboBox( m_panelBasic, ID_AssetTUID, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0|wxWANTS_CHARS ); 
	m_comboBoxTUID->SetMinSize( wxSize( 200,-1 ) );
	
	bSizer87->Add( m_comboBoxTUID, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer491;
	bSizer491 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBoxSeachHistoryData = new wxCheckBox( m_panelBasic, wxID_ANY, wxT("Seach files that may have been renamed."), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer491->Add( m_checkBoxSeachHistoryData, 0, wxALL, 5 );
	
	bSizer87->Add( bSizer491, 1, wxEXPAND, 5 );
	
	bSizer85->Add( bSizer87, 0, wxEXPAND|wxLEFT, 5 );
	
	m_panelBasic->SetSizer( bSizer85 );
	m_panelBasic->Layout();
	bSizer85->Fit( m_panelBasic );
	bSizer21->Add( m_panelBasic, 0, wxEXPAND | wxALL, 5 );
	
	bSizer29->Add( bSizer21, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer401;
	bSizer401 = new wxBoxSizer( wxVERTICAL );
	
	m_staticline3 = new wxStaticLine( this, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer401->Add( m_staticline3, 0, wxEXPAND|wxLEFT|wxRIGHT, 5 );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer5->AddGrowableCol( 0 );
	fgSizer5->AddGrowableRow( 0 );
	fgSizer5->SetFlexibleDirection( wxHORIZONTAL );
	
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxVERTICAL );
	
	m_staticTextSearching = new wxStaticText( this, ID_DEFAULT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextSearching->Wrap( -1 );
	m_staticTextSearching->SetMinSize( wxSize( 65,-1 ) );
	
	bSizer16->Add( m_staticTextSearching, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );
	
	wxBoxSizer* bSizer311;
	bSizer311 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticTextLookingIn = new wxStaticText( this, ID_DEFAULT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextLookingIn->Wrap( -1 );
	bSizer311->Add( m_staticTextLookingIn, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	bSizer16->Add( bSizer311, 1, wxEXPAND, 5 );
	
	fgSizer5->Add( bSizer16, 0, wxEXPAND|wxLEFT, 5 );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonSearch = new wxButton( this, ID_SearchButton, wxT("Search"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_buttonSearch, 0, wxALIGN_BOTTOM|wxALL, 5 );
	
	m_buttonClear = new wxButton( this, ID_ClearButton, wxT("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_buttonClear, 0, wxALIGN_BOTTOM|wxALL, 5 );
	
	fgSizer5->Add( bSizer10, 0, wxALIGN_RIGHT|wxALIGN_TOP, 5 );
	
	bSizer401->Add( fgSizer5, 0, wxEXPAND, 5 );
	
	bSizer29->Add( bSizer401, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer22;
	bSizer22 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxVERTICAL );
	
	m_listCtrlResults = new UIToolKit::SortableListView( this, ID_ListCtrlResults, wxDefaultPosition, wxDefaultSize, wxLC_ALIGN_LEFT|wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_SORT_ASCENDING|wxLC_VRULES );
	m_listCtrlResults->SetMinSize( wxSize( 60,-1 ) );
	
	bSizer24->Add( m_listCtrlResults, 1, wxALL|wxEXPAND, 5 );
	
	bSizer22->Add( bSizer24, 2, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer221;
	bSizer221 = new wxBoxSizer( wxVERTICAL );
	
	bSizer221->SetMinSize( wxSize( 150,-1 ) ); 
	m_DetailsScrolledWindow = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL );
	m_DetailsScrolledWindow->SetScrollRate( 5, 5 );
	m_DetailsScrolledWindow->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNHIGHLIGHT ) );
	
	wxBoxSizer* bSizer36;
	bSizer36 = new wxBoxSizer( wxVERTICAL );
	
	m_bitmapThumbnail = new wxStaticBitmap( m_DetailsScrolledWindow, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 150,100 ), 0 );
	m_bitmapThumbnail->SetMaxSize( wxSize( 150,150 ) );
	
	bSizer36->Add( m_bitmapThumbnail, 0, wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 5 );
	
	
	bSizer36->Add( 0, 10, 0, wxEXPAND, 5 );
	
	m_textCtrlDetails = new wxTextCtrl( m_DetailsScrolledWindow, ID_DEFAULT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxTE_AUTO_URL|wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH|wxTE_WORDWRAP|wxNO_BORDER );
	bSizer36->Add( m_textCtrlDetails, 1, wxALL|wxEXPAND, 5 );
	
	m_DetailsScrolledWindow->SetSizer( bSizer36 );
	m_DetailsScrolledWindow->Layout();
	bSizer36->Fit( m_DetailsScrolledWindow );
	bSizer221->Add( m_DetailsScrolledWindow, 1, wxEXPAND | wxALL, 5 );
	
	bSizer22->Add( bSizer221, 1, wxEXPAND, 5 );
	
	bSizer29->Add( bSizer22, 1, wxEXPAND, 5 );
	
	m_panelDialogButtons = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer38;
	bSizer38 = new wxBoxSizer( wxVERTICAL );
	
	
	bSizer38->Add( 0, 5, 0, wxALL, 0 );
	
	m_staticlineButtonLine = new wxStaticLine( m_panelDialogButtons, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer38->Add( m_staticlineButtonLine, 0, wxEXPAND|wxLEFT|wxRIGHT, 5 );
	
	wxBoxSizer* bSizer49;
	bSizer49 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOperation = new wxButton( m_panelDialogButtons, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOperation->Enable( false );
	
	bSizer49->Add( m_buttonOperation, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( m_panelDialogButtons, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer49->Add( m_buttonCancel, 0, wxALIGN_RIGHT|wxALIGN_TOP|wxALL, 5 );
	
	bSizer38->Add( bSizer49, 0, wxALIGN_BOTTOM|wxALIGN_RIGHT, 5 );
	
	m_panelDialogButtons->SetSizer( bSizer38 );
	m_panelDialogButtons->Layout();
	bSizer38->Fit( m_panelDialogButtons );
	bSizer29->Add( m_panelDialogButtons, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer29 );
	this->Layout();
}

FileBrowserGenerated::~FileBrowserGenerated()
{
}
