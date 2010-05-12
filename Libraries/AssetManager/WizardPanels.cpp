///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  7 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef WX_PRECOMP

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#else
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "WizardPanels.h"

///////////////////////////////////////////////////////////////////////////
using namespace AssetManager;

LocationPanel::LocationPanel( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 550,400 ) );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* titleSizer;
	titleSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Title = new wxStaticText( this, ID_DEFAULT, wxT("<WIZARD TITLE>"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Title->Wrap( -1 );
	m_Title->SetFont( wxFont( 10, 74, 90, 92, false, wxT("Arial") ) );
	
	titleSizer->Add( m_Title, 0, wxALL, 5 );
	
	m_Description = new wxStaticText( this, ID_DEFAULT, wxT("<PAGE DESCRIPTION> Call wxStaticText::Wrap( m_Panel->GetMinWidth() - 10 ) to make this multiline."), wxDefaultPosition, wxDefaultSize, 0 );
	m_Description->Wrap( -1 );
	titleSizer->Add( m_Description, 0, wxALL, 5 );
	
	mainSizer->Add( titleSizer, 0, wxEXPAND, 5 );
	
	
	mainSizer->Add( 0, 0, 0, wxALL, 5 );
	
	wxBoxSizer* nameSizer;
	nameSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_StaticName = new wxStaticText( this, wxID_ANY, wxT("Name of asset:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticName->Wrap( -1 );
	nameSizer->Add( m_StaticName, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	m_Name = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	nameSizer->Add( m_Name, 1, wxALL, 5 );
	
	m_checkBoxCreateSubfolder = new wxCheckBox( this, wxID_ANY, wxT("Create asset subfolder"), wxDefaultPosition, wxDefaultSize, 0 );
	
	m_checkBoxCreateSubfolder->SetToolTip( wxT("Automatically create a subfolder using the asset name.") );
	
	nameSizer->Add( m_checkBoxCreateSubfolder, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
	
	
	nameSizer->Add( 24, 24, 0, wxEXPAND|wxLEFT, 5 );
	
	mainSizer->Add( nameSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* captionSizer;
	captionSizer = new wxBoxSizer( wxVERTICAL );
	
	m_StaticDirectory = new wxStaticText( this, ID_DEFAULT, wxT("Directory:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticDirectory->Wrap( -1 );
	captionSizer->Add( m_StaticDirectory, 0, wxLEFT|wxTOP, 5 );
	
	mainSizer->Add( captionSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* directorySizer;
	directorySizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_Directory = new wxTextCtrl( this, ID_DEFAULT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	directorySizer->Add( m_Directory, 1, wxALL, 5 );
	
	m_ButtonDirectoryPicker = new wxButton( this, ID_DEFAULT, wxT("..."), wxDefaultPosition, wxSize( 24,24 ), 0 );
	directorySizer->Add( m_ButtonDirectoryPicker, 0, wxALL, 3 );
	
	mainSizer->Add( directorySizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* newSizer;
	newSizer = new wxBoxSizer( wxVERTICAL );
	
	m_StaticNewPath = new wxStaticText( this, wxID_ANY, wxT("Path to new asset:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticNewPath->Wrap( -1 );
	m_StaticNewPath->Hide();
	
	newSizer->Add( m_StaticNewPath, 1, wxLEFT|wxTOP, 5 );
	
	wxBoxSizer* newPathSizer;
	newPathSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_NewAssetPath = new wxTextCtrl( this, wxID_ANY, wxT("FULL_PATH_DISPLAYED_HERE"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxNO_BORDER );
	m_NewAssetPath->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	newPathSizer->Add( m_NewAssetPath, 1, wxLEFT, 5 );
	
	newSizer->Add( newPathSizer, 0, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( newSizer, 0, wxEXPAND, 5 );
	
	m_ScrollWindow = new wxScrolledWindow( this, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_ScrollWindow->SetScrollRate( 5, 5 );
	wxBoxSizer* scrollSizer;
	scrollSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ScrollWindow->SetSizer( scrollSizer );
	m_ScrollWindow->Layout();
	scrollSizer->Fit( m_ScrollWindow );
	mainSizer->Add( m_ScrollWindow, 1, wxEXPAND | wxALL, 5 );
	
	m_DuplicatePathPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* origSizer;
	origSizer = new wxBoxSizer( wxVERTICAL );
	
	m_StaticOrigPath = new wxStaticText( m_DuplicatePathPanel, wxID_ANY, wxT("Asset being duplicated:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticOrigPath->Wrap( -1 );
	origSizer->Add( m_StaticOrigPath, 0, wxLEFT|wxTOP, 5 );
	
	wxBoxSizer* origPathSizer;
	origPathSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_OriginalAssetPath = new wxTextCtrl( m_DuplicatePathPanel, wxID_ANY, wxT("FULL_PATH_TO_PREVIOUS_LOCATION_DISPLAYED_HERE"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxNO_BORDER );
	m_OriginalAssetPath->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	origPathSizer->Add( m_OriginalAssetPath, 1, wxLEFT, 5 );
	
	origSizer->Add( origPathSizer, 0, wxALL|wxEXPAND, 5 );
	
	m_DuplicatePathPanel->SetSizer( origSizer );
	m_DuplicatePathPanel->Layout();
	origSizer->Fit( m_DuplicatePathPanel );
	mainSizer->Add( m_DuplicatePathPanel, 0, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

ShaderTypeSelectionPanel::ShaderTypeSelectionPanel( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 550,400 ) );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* titleSizer;
	titleSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Title = new wxStaticText( this, ID_DEFAULT, wxT("<WIZARD TITLE>"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Title->Wrap( -1 );
	m_Title->SetFont( wxFont( 10, 74, 90, 92, false, wxT("Arial") ) );
	
	titleSizer->Add( m_Title, 0, wxALL, 5 );
	
	m_Description = new wxStaticText( this, wxID_ANY, wxT("<PAGE DESCRIPTION> Call wxStaticText::Wrap( m_Panel->GetMinWidth() - 10 ) to make this multiline."), wxDefaultPosition, wxDefaultSize, 0 );
	m_Description->Wrap( -1 );
	titleSizer->Add( m_Description, 0, wxALL, 5 );
	
	mainSizer->Add( titleSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* scrollWindowSizer;
	scrollWindowSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ScrollWindow = new wxScrolledWindow( this, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_ScrollWindow->SetScrollRate( 5, 5 );
	wxBoxSizer* scrollSizer;
	scrollSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ScrollWindow->SetSizer( scrollSizer );
	m_ScrollWindow->Layout();
	scrollSizer->Fit( m_ScrollWindow );
	scrollWindowSizer->Add( m_ScrollWindow, 1, wxEXPAND | wxALL, 5 );
	
	mainSizer->Add( scrollWindowSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

AttribsPanel::AttribsPanel( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 550,400 ) );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* titleSizer;
	titleSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Title = new wxStaticText( this, ID_DEFAULT, wxT("<PAGE TITLE>"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Title->Wrap( -1 );
	m_Title->SetFont( wxFont( 10, 74, 90, 92, false, wxT("Arial") ) );
	
	titleSizer->Add( m_Title, 0, wxALL, 5 );
	
	m_Description = new wxStaticText( this, ID_DEFAULT, wxT("<PAGE DESCRIPTION> Call wxStaticText::Wrap( m_Panel->GetMinWidth() - 10 ) to make this multiline."), wxDefaultPosition, wxDefaultSize, 0 );
	m_Description->Wrap( -1 );
	titleSizer->Add( m_Description, 0, wxALL, 5 );
	
	mainSizer->Add( titleSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bottomSizer;
	bottomSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* horizontalSizer;
	horizontalSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* leftSizer;
	leftSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ScrollWindowAttributes = new wxScrolledWindow( this, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxSIMPLE_BORDER|wxVSCROLL );
	m_ScrollWindowAttributes->SetScrollRate( 5, 5 );
	m_ScrollWindowAttributes->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
	
	wxBoxSizer* attributeSizer;
	attributeSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ScrollWindowAttributes->SetSizer( attributeSizer );
	m_ScrollWindowAttributes->Layout();
	attributeSizer->Fit( m_ScrollWindowAttributes );
	leftSizer->Add( m_ScrollWindowAttributes, 1, wxEXPAND | wxALL, 5 );
	
	horizontalSizer->Add( leftSizer, 1, wxEXPAND|wxTOP, 5 );
	
	wxStaticBoxSizer* rightSizer;
	rightSizer = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxT("Description") ), wxVERTICAL );
	
	m_StaticAttribDescHeading = new wxStaticText( this, ID_DEFAULT, wxT("ATTRIB_NAME (required/optional)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticAttribDescHeading->Wrap( -1 );
	rightSizer->Add( m_StaticAttribDescHeading, 0, wxALL, 5 );
	
	m_StaticAttribDescBody = new wxStaticText( this, ID_DEFAULT, wxT("Attribute description - call Wrap()"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticAttribDescBody->Wrap( -1 );
	rightSizer->Add( m_StaticAttribDescBody, 0, wxALL, 5 );
	
	horizontalSizer->Add( rightSizer, 1, wxALL|wxEXPAND, 5 );
	
	bottomSizer->Add( horizontalSizer, 1, wxEXPAND, 5 );
	
	mainSizer->Add( bottomSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

TextureMapPanel::TextureMapPanel( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer58;
	bSizer58 = new wxBoxSizer( wxVERTICAL );
	
	m_TextureMapPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* textureMapSizer;
	textureMapSizer = new wxBoxSizer( wxVERTICAL );
	
	m_FieldLabel = new wxStaticText( m_TextureMapPanel, wxID_ANY, wxT("Texture Map File"), wxDefaultPosition, wxDefaultSize, 0 );
	m_FieldLabel->Wrap( -1 );
	textureMapSizer->Add( m_FieldLabel, 0, wxLEFT|wxRIGHT|wxTOP, 5 );
	
	wxBoxSizer* pathSizer;
	pathSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_FilePathField = new wxTextCtrl( m_TextureMapPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_FilePathField->SetMaxLength( 260 ); 
	pathSizer->Add( m_FilePathField, 1, wxALL, 5 );
	
	m_ExplorerButton = new wxButton( m_TextureMapPanel, wxID_ANY, wxT("..."), wxDefaultPosition, wxSize( 24,24 ), 0 );
	pathSizer->Add( m_ExplorerButton, 0, wxALL, 3 );
	
	m_FileBrowserButton = new wxBitmapButton( m_TextureMapPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 24,24 ), wxBU_AUTODRAW );
	pathSizer->Add( m_FileBrowserButton, 0, wxBOTTOM|wxRIGHT|wxTOP, 3 );
	
	textureMapSizer->Add( pathSizer, 1, wxEXPAND|wxLEFT, 15 );
	
	m_TextureMapPanel->SetSizer( textureMapSizer );
	m_TextureMapPanel->Layout();
	textureMapSizer->Fit( m_TextureMapPanel );
	bSizer58->Add( m_TextureMapPanel, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer58 );
	this->Layout();
	
	// Connect Events
	m_FilePathField->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( TextureMapPanel::OnFieldTextEnter ), NULL, this );
	m_ExplorerButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TextureMapPanel::OnFileBrowserButtonClick ), NULL, this );
	m_FileBrowserButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TextureMapPanel::OnExplorerButtonClick ), NULL, this );
}

ShaderTexturesPanel::ShaderTexturesPanel( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 550,400 ) );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* titleSizer;
	titleSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Title = new wxStaticText( this, ID_DEFAULT, wxT("<PAGE TITLE>"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Title->Wrap( -1 );
	m_Title->SetFont( wxFont( 10, 74, 90, 92, false, wxT("Arial") ) );
	
	titleSizer->Add( m_Title, 0, wxALL, 5 );
	
	m_Description = new wxStaticText( this, ID_DEFAULT, wxT("<PAGE DESCRIPTION> Call wxStaticText::Wrap( m_Panel->GetMinWidth() - 10 ) to make this multiline."), wxDefaultPosition, wxDefaultSize, 0 );
	m_Description->Wrap( -1 );
	titleSizer->Add( m_Description, 0, wxALL, 5 );
	
	mainSizer->Add( titleSizer, 0, wxBOTTOM|wxEXPAND, 5 );
	
	wxBoxSizer* bottomSizer;
	bottomSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ScrollWindow = new wxScrolledWindow( this, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_ScrollWindow->SetScrollRate( 5, 5 );
	wxBoxSizer* scrollSizer;
	scrollSizer = new wxBoxSizer( wxVERTICAL );
	
	m_TextureMapPanel = new wxPanel( m_ScrollWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* textureMapSizer;
	textureMapSizer = new wxBoxSizer( wxVERTICAL );
	
	m_FieldLabel = new wxStaticText( m_TextureMapPanel, wxID_ANY, wxT("Texture Map File"), wxDefaultPosition, wxDefaultSize, 0 );
	m_FieldLabel->Wrap( -1 );
	textureMapSizer->Add( m_FieldLabel, 0, wxLEFT|wxRIGHT|wxTOP, 5 );
	
	wxBoxSizer* pathSizer;
	pathSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_FilePathField = new wxTextCtrl( m_TextureMapPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_FilePathField->SetMaxLength( 260 ); 
	pathSizer->Add( m_FilePathField, 1, wxALL, 5 );
	
	m_ExplorerButton = new wxButton( m_TextureMapPanel, wxID_ANY, wxT("..."), wxDefaultPosition, wxSize( 24,24 ), 0 );
	pathSizer->Add( m_ExplorerButton, 0, wxALL, 3 );
	
	m_FileBrowserButton = new wxBitmapButton( m_TextureMapPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 24,24 ), wxBU_AUTODRAW );
	pathSizer->Add( m_FileBrowserButton, 0, wxBOTTOM|wxRIGHT|wxTOP, 3 );
	
	textureMapSizer->Add( pathSizer, 1, wxEXPAND|wxLEFT, 15 );
	
	m_TextureMapPanel->SetSizer( textureMapSizer );
	m_TextureMapPanel->Layout();
	textureMapSizer->Fit( m_TextureMapPanel );
	scrollSizer->Add( m_TextureMapPanel, 0, wxEXPAND | wxALL, 5 );
	
	m_ScrollWindow->SetSizer( scrollSizer );
	m_ScrollWindow->Layout();
	scrollSizer->Fit( m_ScrollWindow );
	bottomSizer->Add( m_ScrollWindow, 1, wxBOTTOM|wxEXPAND|wxTOP, 5 );
	
	mainSizer->Add( bottomSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	// Connect Events
	m_FilePathField->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ShaderTexturesPanel::OnFieldTextEnter ), NULL, this );
	m_ExplorerButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ShaderTexturesPanel::OnFileBrowserButtonClick ), NULL, this );
	m_FileBrowserButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ShaderTexturesPanel::OnExplorerButtonClick ), NULL, this );
}

FileBackedAttrPanel::FileBackedAttrPanel( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 550,400 ) );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* titleSizer;
	titleSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Title = new wxStaticText( this, ID_DEFAULT, wxT("<PAGE TITLE>"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Title->Wrap( -1 );
	m_Title->SetFont( wxFont( 10, 74, 90, 92, false, wxT("Arial") ) );
	
	titleSizer->Add( m_Title, 0, wxALL, 5 );
	
	m_Description = new wxStaticText( this, ID_DEFAULT, wxT("<PAGE DESCRIPTION> Call wxStaticText::Wrap( m_Panel->GetMinWidth() - 10 ) to make this multiline."), wxDefaultPosition, wxDefaultSize, 0 );
	m_Description->Wrap( -1 );
	titleSizer->Add( m_Description, 0, wxALL, 5 );
	
	mainSizer->Add( titleSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bottomSizer;
	bottomSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* radioSizer;
	radioSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* radioSizer1;
	radioSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_RadioBtnNew = new wxRadioButton( this, ID_DEFAULT, wxT("CREATE_NEW"), wxDefaultPosition, wxDefaultSize, 0);
	radioSizer1->Add( m_RadioBtnNew, 0, wxALL, 5 );
	
	wxBoxSizer* pathSizer1;
	pathSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	m_FilePathNew = new wxTextCtrl( this, ID_DEFAULT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_FilePathNew->Enable( false );
	
	pathSizer1->Add( m_FilePathNew, 1, wxALL, 5 );
	
	
	pathSizer1->Add( 26, 0, 0, wxALL, 2 );
	
	radioSizer1->Add( pathSizer1, 1, wxEXPAND|wxLEFT, 15 );
	
	radioSizer->Add( radioSizer1, 1, wxBOTTOM|wxEXPAND, 5 );
	
	wxBoxSizer* radioSizer2;
	radioSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_RadioBtnExisting = new wxRadioButton( this, ID_DEFAULT, wxT("USE_EXISTING"), wxDefaultPosition, wxDefaultSize, 0);
	radioSizer2->Add( m_RadioBtnExisting, 0, wxALL, 5 );
	
	wxBoxSizer* pathSizer2;
	pathSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_FilePathExisting = new wxTextCtrl( this, ID_DEFAULT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_FilePathExisting->Enable( false );
	
	pathSizer2->Add( m_FilePathExisting, 1, wxALL, 5 );
	
	m_ButtonExisting = new wxButton( this, ID_DEFAULT, wxT("..."), wxDefaultPosition, wxSize( 24,24 ), 0 );
	m_ButtonExisting->Enable( false );
	
	pathSizer2->Add( m_ButtonExisting, 0, wxALL, 3 );
	
	m_ButtonFindExisting = new wxBitmapButton( this, ID_DEFAULT, wxNullBitmap, wxDefaultPosition, wxSize( 24,24 ), wxBU_AUTODRAW );
	m_ButtonFindExisting->Enable( false );
	
	pathSizer2->Add( m_ButtonFindExisting, 0, wxBOTTOM|wxRIGHT|wxTOP, 3 );
	
	radioSizer2->Add( pathSizer2, 1, wxEXPAND|wxLEFT, 15 );
	
	radioSizer->Add( radioSizer2, 1, wxEXPAND, 5 );
	
	bottomSizer->Add( radioSizer, 0, wxEXPAND, 5 );
	
	mainSizer->Add( bottomSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

CinematicAttrLevelZonePanel::CinematicAttrLevelZonePanel( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 550,400 ) );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* titleSizer;
	titleSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Title = new wxStaticText( this, ID_DEFAULT, wxT("<PAGE TITLE>"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Title->Wrap( -1 );
	m_Title->SetFont( wxFont( 10, 74, 90, 92, false, wxT("Arial") ) );
	
	titleSizer->Add( m_Title, 0, wxALL, 5 );
	
	m_Description = new wxStaticText( this, ID_DEFAULT, wxT("<PAGE DESCRIPTION> Call wxStaticText::Wrap( m_Panel->GetMinWidth() - 10 ) to make this multiline."), wxDefaultPosition, wxDefaultSize, 0 );
	m_Description->Wrap( -1 );
	titleSizer->Add( m_Description, 0, wxALL, 5 );
	
	mainSizer->Add( titleSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* pathSizer2;
	pathSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_FilePathLevel = new wxTextCtrl( this, ID_DEFAULT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	pathSizer2->Add( m_FilePathLevel, 1, wxALL, 5 );
	
	m_ButtonBrowse = new wxButton( this, ID_DEFAULT, wxT("..."), wxDefaultPosition, wxSize( 24,24 ), 0 );
	pathSizer2->Add( m_ButtonBrowse, 0, wxALL, 2 );
	
	m_ButtonFind = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 24,24 ), wxBU_AUTODRAW );
	pathSizer2->Add( m_ButtonFind, 0, wxALL, 2 );
	
	mainSizer->Add( pathSizer2, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer36;
	bSizer36 = new wxBoxSizer( wxVERTICAL );
	
	m_CheckBoxAllZones = new wxCheckBox( this, wxID_ANY, wxT("All Zones"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE );
	m_CheckBoxAllZones->SetValue(true);
	
	bSizer36->Add( m_CheckBoxAllZones, 0, wxALL, 5 );
	
	mainSizer->Add( bSizer36, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer38;
	bSizer38 = new wxBoxSizer( wxVERTICAL );
	
	m_ListBoxZones = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_HSCROLL|wxLB_MULTIPLE|wxLB_NEEDED_SB|wxLB_SORT ); 
	bSizer38->Add( m_ListBoxZones, 1, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( bSizer38, 1, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

GridPanel::GridPanel( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 550,400 ) );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* titleSizer;
	titleSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Title = new wxStaticText( this, ID_DEFAULT, wxT("Duplicate Asset"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Title->Wrap( -1 );
	m_Title->SetFont( wxFont( 10, 74, 90, 92, false, wxT("Arial") ) );
	
	titleSizer->Add( m_Title, 0, wxALL, 5 );
	
	m_Description = new wxStaticText( this, ID_DEFAULT, wxT("Below are the files associated with this asset. You can choose whether or not any of these files need to be duplicated as well."), wxDefaultPosition, wxDefaultSize, 0 );
	m_Description->Wrap( -1 );
	titleSizer->Add( m_Description, 0, wxALL, 5 );
	
	mainSizer->Add( titleSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* propGridSizer;
	propGridSizer = new wxBoxSizer( wxVERTICAL );
	
	m_PropertyGrid = new wxPropertyGridManager(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPGMAN_DEFAULT_STYLE|wxPG_TOOLTIPS|wxTAB_TRAVERSAL|wxHSCROLL);
	m_PropertyGrid->SetExtraStyle( wxPG_EX_MODE_BUTTONS ); 
	propGridSizer->Add( m_PropertyGrid, 1, wxEXPAND | wxALL, 5 );
	
	mainSizer->Add( propGridSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizerButtons;
	bSizerButtons = new wxBoxSizer( wxHORIZONTAL );
	
	m_SelectButtonsPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizerSelectSizer;
	bSizerSelectSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_SelectAllButton = new wxButton( m_SelectButtonsPanel, wxID_ANY, wxT("Select All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerSelectSizer->Add( m_SelectAllButton, 0, 0, 5 );
	
	
	bSizerSelectSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_UnselectAllButton = new wxButton( m_SelectButtonsPanel, wxID_ANY, wxT("Unselect All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerSelectSizer->Add( m_UnselectAllButton, 0, 0, 5 );
	
	m_SelectButtonsPanel->SetSizer( bSizerSelectSizer );
	m_SelectButtonsPanel->Layout();
	bSizerSelectSizer->Fit( m_SelectButtonsPanel );
	bSizerButtons->Add( m_SelectButtonsPanel, 1, wxEXPAND|wxLEFT|wxRIGHT, 5 );
	
	mainSizer->Add( bSizerButtons, 0, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

SummaryPanel::SummaryPanel( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 550,400 ) );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* titleSizer;
	titleSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Title = new wxStaticText( this, ID_DEFAULT, wxT("Summary"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Title->Wrap( -1 );
	m_Title->SetFont( wxFont( 10, 74, 90, 92, false, wxT("Arial") ) );
	
	titleSizer->Add( m_Title, 0, wxALL, 5 );
	
	m_Description = new wxStaticText( this, wxID_ANY, wxT("Your new asset will be created with the following settings:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Description->Wrap( -1 );
	titleSizer->Add( m_Description, 0, wxALL, 5 );
	
	mainSizer->Add( titleSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* textBoxSizer;
	textBoxSizer = new wxBoxSizer( wxVERTICAL );
	
	m_SummaryTextBox = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH );
	textBoxSizer->Add( m_SummaryTextBox, 1, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( textBoxSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}
