///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Workspace.h"

#include "PropToolGenerated.h"

///////////////////////////////////////////////////////////////////////////

PropToolFrameBase::PropToolFrameBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 800,760 ), wxDefaultSize );
	
	wxBoxSizer* m_MainSizer;
	m_MainSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_MainSplitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_MainSplitter->Connect( wxEVT_IDLE, wxIdleEventHandler( PropToolFrameBase::m_MainSplitterOnIdle ), NULL, this );
	m_NotebookPanel = new wxPanel( m_MainSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* m_NotebookSizer;
	m_NotebookSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Notebook = new wxNotebook( m_NotebookPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_ShaderPanel = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* shaderPanelSizer;
	shaderPanelSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ShaderListBox = new wxListBox( m_ShaderPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	shaderPanelSizer->Add( m_ShaderListBox, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText7 = new wxStaticText( m_ShaderPanel, wxID_ANY, wxT("Alpha Mode"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	m_staticText7->SetMinSize( wxSize( 75,-1 ) );
	
	bSizer15->Add( m_staticText7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString m_AlphaModeChoices[] = { wxT("Opaque"), wxT("Additive"), wxT("Blended"), wxT("Cutout") };
	int m_AlphaModeNChoices = sizeof( m_AlphaModeChoices ) / sizeof( wxString );
	m_AlphaMode = new wxChoice( m_ShaderPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_AlphaModeNChoices, m_AlphaModeChoices, 0 );
	m_AlphaMode->SetSelection( 0 );
	bSizer15->Add( m_AlphaMode, 1, wxALL, 5 );
	
	shaderPanelSizer->Add( bSizer15, 0, wxEXPAND, 5 );
	
	wxBoxSizer* colorMapSizer;
	colorMapSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* colorMapDetailSizer;
	colorMapDetailSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_ColorMapLabel = new wxStaticText( m_ShaderPanel, wxID_ANY, wxT("Color Map"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ColorMapLabel->Wrap( -1 );
	m_ColorMapLabel->SetMinSize( wxSize( 75,-1 ) );
	
	colorMapDetailSizer->Add( m_ColorMapLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_ColorMapFilePicker = new wxFilePickerCtrl( m_ShaderPanel, wxID_ANY, wxEmptyString, wxT("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE|wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_USE_TEXTCTRL );
	colorMapDetailSizer->Add( m_ColorMapFilePicker, 1, wxALL, 5 );
	
	colorMapSizer->Add( colorMapDetailSizer, 0, wxEXPAND, 5 );
	
	m_ColorMapPreviewPanel = new Nocturnal::ImageViewPanel( m_ShaderPanel );
	m_ColorMapPreviewPanel->SetMinSize( wxSize( 128,128 ) );
	m_ColorMapPreviewPanel->SetMaxSize( wxSize( 128,128 ) );
	
	colorMapSizer->Add( m_ColorMapPreviewPanel, 0, wxALIGN_CENTER|wxALL, 5 );
	
	shaderPanelSizer->Add( colorMapSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* normalMapSizer;
	normalMapSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* normalMapDetailSizer;
	normalMapDetailSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_NormalMapLabel = new wxStaticText( m_ShaderPanel, wxID_ANY, wxT("Normal Map"), wxDefaultPosition, wxDefaultSize, 0 );
	m_NormalMapLabel->Wrap( -1 );
	m_NormalMapLabel->SetMinSize( wxSize( 75,-1 ) );
	
	normalMapDetailSizer->Add( m_NormalMapLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_NormalMapFilePicker = new wxFilePickerCtrl( m_ShaderPanel, wxID_ANY, wxEmptyString, wxT("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE|wxFLP_FILE_MUST_EXIST|wxFLP_OPEN );
	normalMapDetailSizer->Add( m_NormalMapFilePicker, 1, wxALL, 5 );
	
	normalMapSizer->Add( normalMapDetailSizer, 0, wxEXPAND, 5 );
	
	m_NormalMapPreviewPanel = new Nocturnal::ImageViewPanel( m_ShaderPanel );
	m_NormalMapPreviewPanel->SetMinSize( wxSize( 128,128 ) );
	m_NormalMapPreviewPanel->SetMaxSize( wxSize( 128,128 ) );
	
	normalMapSizer->Add( m_NormalMapPreviewPanel, 0, wxALIGN_CENTER|wxALL, 5 );
	
	shaderPanelSizer->Add( normalMapSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* gpiMapSizer;
	gpiMapSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* gpilMapDetailSizer;
	gpilMapDetailSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_GPIMapLabel = new wxStaticText( m_ShaderPanel, wxID_ANY, wxT("GPI Map"), wxDefaultPosition, wxDefaultSize, 0 );
	m_GPIMapLabel->Wrap( -1 );
	m_GPIMapLabel->SetMinSize( wxSize( 75,-1 ) );
	
	gpilMapDetailSizer->Add( m_GPIMapLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_GPIMapFilePicker = new wxFilePickerCtrl( m_ShaderPanel, wxID_ANY, wxEmptyString, wxT("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE|wxFLP_FILE_MUST_EXIST|wxFLP_OPEN );
	gpilMapDetailSizer->Add( m_GPIMapFilePicker, 1, wxALL, 5 );
	
	gpiMapSizer->Add( gpilMapDetailSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	m_GlossMapPreviewPanel = new Nocturnal::ImageViewPanel( m_ShaderPanel );
	m_GlossMapPreviewPanel->SetMinSize( wxSize( 128,128 ) );
	m_GlossMapPreviewPanel->SetMaxSize( wxSize( 128,128 ) );
	
	bSizer11->Add( m_GlossMapPreviewPanel, 1, wxALIGN_CENTER|wxALL, 5 );
	
	m_GlossMapPreviewLabel = new wxStaticText( m_ShaderPanel, wxID_ANY, wxT("Gloss"), wxDefaultPosition, wxDefaultSize, 0 );
	m_GlossMapPreviewLabel->Wrap( -1 );
	bSizer11->Add( m_GlossMapPreviewLabel, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	bSizer12->Add( bSizer11, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer121;
	bSizer121 = new wxBoxSizer( wxVERTICAL );
	
	m_ParallaxMapPreviewPanel = new Nocturnal::ImageViewPanel( m_ShaderPanel );
	m_ParallaxMapPreviewPanel->SetMinSize( wxSize( 128,128 ) );
	m_ParallaxMapPreviewPanel->SetMaxSize( wxSize( 128,128 ) );
	
	bSizer121->Add( m_ParallaxMapPreviewPanel, 1, wxALIGN_CENTER|wxALL, 5 );
	
	m_ParallaxMapPreviewLabel = new wxStaticText( m_ShaderPanel, wxID_ANY, wxT("Parallax"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ParallaxMapPreviewLabel->Wrap( -1 );
	bSizer121->Add( m_ParallaxMapPreviewLabel, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	bSizer12->Add( bSizer121, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );
	
	m_IncanMapPreviewPanel = new Nocturnal::ImageViewPanel( m_ShaderPanel );
	m_IncanMapPreviewPanel->SetMinSize( wxSize( 128,128 ) );
	m_IncanMapPreviewPanel->SetMaxSize( wxSize( 128,128 ) );
	
	bSizer13->Add( m_IncanMapPreviewPanel, 1, wxALIGN_CENTER|wxALL, 5 );
	
	m_IncanMapPreviewLabel = new wxStaticText( m_ShaderPanel, wxID_ANY, wxT("Incandescent"), wxDefaultPosition, wxDefaultSize, 0 );
	m_IncanMapPreviewLabel->Wrap( -1 );
	bSizer13->Add( m_IncanMapPreviewLabel, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	bSizer12->Add( bSizer13, 1, wxEXPAND, 5 );
	
	gpiMapSizer->Add( bSizer12, 1, wxEXPAND, 5 );
	
	shaderPanelSizer->Add( gpiMapSizer, 0, wxEXPAND, 5 );
	
	m_ShaderPanel->SetSizer( shaderPanelSizer );
	m_ShaderPanel->Layout();
	shaderPanelSizer->Fit( m_ShaderPanel );
	m_Notebook->AddPage( m_ShaderPanel, wxT("Shader"), true );
	
	m_NotebookSizer->Add( m_Notebook, 1, wxEXPAND | wxALL, 5 );
	
	m_NotebookPanel->SetSizer( m_NotebookSizer );
	m_NotebookPanel->Layout();
	m_NotebookSizer->Fit( m_NotebookPanel );
	m_Workspace = new Workspace( m_MainSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_Workspace->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_APPWORKSPACE ) );
	
	m_MainSplitter->SplitVertically( m_NotebookPanel, m_Workspace, 470 );
	m_MainSizer->Add( m_MainSplitter, 1, wxEXPAND, 5 );
	
	this->SetSizer( m_MainSizer );
	this->Layout();
	m_StatusBar = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	m_MainMenubar = new wxMenuBar( 0 );
	m_FileMenu = new wxMenu();
	wxMenuItem* m_OpenItem;
	m_OpenItem = new wxMenuItem( m_FileMenu, wxID_ANY, wxString( wxT("Open...") ) , wxEmptyString, wxITEM_NORMAL );
	m_FileMenu->Append( m_OpenItem );
	
	m_FileMenu->AppendSeparator();
	
	wxMenuItem* m_ExitItem;
	m_ExitItem = new wxMenuItem( m_FileMenu, wxID_ANY, wxString( wxT("Exit") ) , wxEmptyString, wxITEM_NORMAL );
	m_FileMenu->Append( m_ExitItem );
	
	m_MainMenubar->Append( m_FileMenu, wxT("File") );
	
	m_HelpMenu = new wxMenu();
	wxMenuItem* m_AboutItem;
	m_AboutItem = new wxMenuItem( m_HelpMenu, wxID_ANY, wxString( wxT("About...") ) , wxEmptyString, wxITEM_NORMAL );
	m_HelpMenu->Append( m_AboutItem );
	
	m_MainMenubar->Append( m_HelpMenu, wxT("Help") );
	
	this->SetMenuBar( m_MainMenubar );
	
	
	// Connect Events
	m_ShaderListBox->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( PropToolFrameBase::OnShaderSelected ), NULL, this );
	m_AlphaMode->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( PropToolFrameBase::OnAlphaModeChanged ), NULL, this );
	m_ColorMapFilePicker->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( PropToolFrameBase::OnColorMapFileChanged ), NULL, this );
	m_NormalMapFilePicker->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( PropToolFrameBase::OnNormalMapFileChanged ), NULL, this );
	m_GPIMapFilePicker->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( PropToolFrameBase::OnGPIMapFileChanged ), NULL, this );
	this->Connect( m_OpenItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PropToolFrameBase::OnOpen ) );
	this->Connect( m_ExitItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PropToolFrameBase::OnExit ) );
	this->Connect( m_AboutItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PropToolFrameBase::OnAbout ) );
}

PropToolFrameBase::~PropToolFrameBase()
{
	// Disconnect Events
	m_ShaderListBox->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( PropToolFrameBase::OnShaderSelected ), NULL, this );
	m_AlphaMode->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( PropToolFrameBase::OnAlphaModeChanged ), NULL, this );
	m_ColorMapFilePicker->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( PropToolFrameBase::OnColorMapFileChanged ), NULL, this );
	m_NormalMapFilePicker->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( PropToolFrameBase::OnNormalMapFileChanged ), NULL, this );
	m_GPIMapFilePicker->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( PropToolFrameBase::OnGPIMapFileChanged ), NULL, this );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PropToolFrameBase::OnOpen ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PropToolFrameBase::OnExit ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PropToolFrameBase::OnAbout ) );
}
