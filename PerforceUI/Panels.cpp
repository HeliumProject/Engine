///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "Panels.h"

///////////////////////////////////////////////////////////////////////////
using namespace PerforceUI;

MainPanel::MainPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 550,400 ) );
	
	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizerTitle;
	bSizerTitle = new wxBoxSizer( wxVERTICAL );
	
	m_TitlePanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_TitlePanel->Hide();
	
	wxBoxSizer* bSizerTitleText;
	bSizerTitleText = new wxBoxSizer( wxVERTICAL );
	
	m_TitleStaticText = new wxStaticText( m_TitlePanel, wxID_ANY, wxT("<TITLE>"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TitleStaticText->Wrap( -1 );
	m_TitleStaticText->SetFont( wxFont( 10, 70, 90, 92, false, wxT("Arial") ) );
	
	bSizerTitleText->Add( m_TitleStaticText, 0, wxALL, 5 );
	
	m_PanelDescriptionStaticText = new wxStaticText( m_TitlePanel, wxID_ANY, wxT("Description goes here. Call wxStaticText::Wrap() to make this multiline."), wxDefaultPosition, wxDefaultSize, 0 );
	m_PanelDescriptionStaticText->Wrap( -1 );
	bSizerTitleText->Add( m_PanelDescriptionStaticText, 0, wxALL, 5 );
	
	m_TitlePanel->SetSizer( bSizerTitleText );
	m_TitlePanel->Layout();
	bSizerTitleText->Fit( m_TitlePanel );
	bSizerTitle->Add( m_TitlePanel, 1, wxEXPAND, 5 );
	
	bSizerMain->Add( bSizerTitle, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizerMainForm;
	bSizerMainForm = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizerClientDetails;
	bSizerClientDetails = new wxBoxSizer( wxHORIZONTAL );
	
	m_ClientDetailsPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizerClientDetailsPanel;
	bSizerClientDetailsPanel = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizerClientDetailTitles;
	bSizerClientDetailTitles = new wxBoxSizer( wxVERTICAL );
	
	m_ChangeStaticTextTitle = new wxStaticText( m_ClientDetailsPanel, wxID_ANY, wxT("Change:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ChangeStaticTextTitle->Wrap( -1 );
	bSizerClientDetailTitles->Add( m_ChangeStaticTextTitle, 0, wxALL, 5 );
	
	m_ClientStaticTextTitle = new wxStaticText( m_ClientDetailsPanel, wxID_ANY, wxT("Client:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ClientStaticTextTitle->Wrap( -1 );
	bSizerClientDetailTitles->Add( m_ClientStaticTextTitle, 0, wxALL, 5 );
	
	m_UserStaticTextTitle = new wxStaticText( m_ClientDetailsPanel, wxID_ANY, wxT("User:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_UserStaticTextTitle->Wrap( -1 );
	bSizerClientDetailTitles->Add( m_UserStaticTextTitle, 0, wxALL, 5 );
	
	
	bSizerClientDetailTitles->Add( 75, 0, 0, 0, 5 );
	
	bSizerClientDetailsPanel->Add( bSizerClientDetailTitles, 0, 0, 5 );
	
	wxBoxSizer* bSizerClientDetails1;
	bSizerClientDetails1 = new wxBoxSizer( wxVERTICAL );
	
	m_ChangeStaticText = new wxStaticText( m_ClientDetailsPanel, wxID_ANY, wxT("<CHANGELIST NUMBER>"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ChangeStaticText->Wrap( -1 );
	bSizerClientDetails1->Add( m_ChangeStaticText, 0, wxALL, 5 );
	
	m_ClientStaticText = new wxStaticText( m_ClientDetailsPanel, wxID_ANY, wxT("<CLIENT NAME>"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ClientStaticText->Wrap( -1 );
	bSizerClientDetails1->Add( m_ClientStaticText, 0, wxALL, 5 );
	
	m_UserStaticText = new wxStaticText( m_ClientDetailsPanel, wxID_ANY, wxT("<USER NAME>"), wxDefaultPosition, wxDefaultSize, 0 );
	m_UserStaticText->Wrap( -1 );
	bSizerClientDetails1->Add( m_UserStaticText, 0, wxALL, 5 );
	
	bSizerClientDetailsPanel->Add( bSizerClientDetails1, 0, 0, 5 );
	
	
	bSizerClientDetailsPanel->Add( 50, 0, 0, 0, 5 );
	
	wxBoxSizer* bSizerMoreClientDetailTitles;
	bSizerMoreClientDetailTitles = new wxBoxSizer( wxVERTICAL );
	
	
	bSizerMoreClientDetailTitles->Add( 75, 0, 1, 0, 5 );
	
	m_DateStaticTextTitle = new wxStaticText( m_ClientDetailsPanel, wxID_ANY, wxT("Date:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_DateStaticTextTitle->Wrap( -1 );
	bSizerMoreClientDetailTitles->Add( m_DateStaticTextTitle, 1, wxALL, 5 );
	
	m_StatusStaticTextTitle = new wxStaticText( m_ClientDetailsPanel, wxID_ANY, wxT("Status:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StatusStaticTextTitle->Wrap( -1 );
	bSizerMoreClientDetailTitles->Add( m_StatusStaticTextTitle, 1, wxALL, 5 );
	
	bSizerClientDetailsPanel->Add( bSizerMoreClientDetailTitles, 0, 0, 5 );
	
	wxBoxSizer* bSizerMoreClientDetails;
	bSizerMoreClientDetails = new wxBoxSizer( wxVERTICAL );
	
	
	bSizerMoreClientDetails->Add( 75, 0, 1, wxEXPAND, 5 );
	
	m_DateStaticText = new wxStaticText( m_ClientDetailsPanel, wxID_ANY, wxT("<DATETIME>"), wxDefaultPosition, wxDefaultSize, 0 );
	m_DateStaticText->Wrap( -1 );
	bSizerMoreClientDetails->Add( m_DateStaticText, 1, wxALL, 5 );
	
	m_StatusStaticText = new wxStaticText( m_ClientDetailsPanel, wxID_ANY, wxT("<STATUS>"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StatusStaticText->Wrap( -1 );
	bSizerMoreClientDetails->Add( m_StatusStaticText, 1, wxALL, 5 );
	
	bSizerClientDetailsPanel->Add( bSizerMoreClientDetails, 1, wxEXPAND, 5 );
	
	m_ClientDetailsPanel->SetSizer( bSizerClientDetailsPanel );
	m_ClientDetailsPanel->Layout();
	bSizerClientDetailsPanel->Fit( m_ClientDetailsPanel );
	bSizerClientDetails->Add( m_ClientDetailsPanel, 1, wxEXPAND, 5 );
	
	bSizerMainForm->Add( bSizerClientDetails, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizerDescription;
	bSizerDescription = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizerDescriptionText;
	bSizerDescriptionText = new wxBoxSizer( wxVERTICAL );
	
	m_DescriptionStaticText = new wxStaticText( this, wxID_ANY, wxT("*Description:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_DescriptionStaticText->Wrap( -1 );
	bSizerDescriptionText->Add( m_DescriptionStaticText, 0, wxALL, 5 );
	
	
	bSizerDescriptionText->Add( 75, 0, 0, 0, 5 );
	
	bSizerDescription->Add( bSizerDescriptionText, 0, 0, 5 );
	
	wxBoxSizer* bSizerDescriptionBox;
	bSizerDescriptionBox = new wxBoxSizer( wxVERTICAL );
	
	m_DescriptionTextCtrl = new wxTextCtrl( this, wxID_ANY, wxT("<CHANGELIST DESCRIPTION>"), wxDefaultPosition, wxDefaultSize, wxTE_BESTWRAP|wxTE_MULTILINE|wxTE_WORDWRAP );
	bSizerDescriptionBox->Add( m_DescriptionTextCtrl, 1, wxALL|wxEXPAND, 5 );
	
	bSizerDescription->Add( bSizerDescriptionBox, 1, wxEXPAND, 5 );
	
	bSizerMainForm->Add( bSizerDescription, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizerJobStatus;
	bSizerJobStatus = new wxBoxSizer( wxHORIZONTAL );
	
	m_JobStatusPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizerJobDescriptionPanel;
	bSizerJobDescriptionPanel = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizerJobDescriptionPanel->Add( 75, 0, 0, 0, 5 );
	
	wxBoxSizer* bSizerJobStatusBox;
	bSizerJobStatusBox = new wxBoxSizer( wxHORIZONTAL );
	
	m_JobStatusStaticText = new wxStaticText( m_JobStatusPanel, wxID_ANY, wxT("Job Status:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_JobStatusStaticText->Wrap( -1 );
	bSizerJobStatusBox->Add( m_JobStatusStaticText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString m_JobStatusChoiceChoices[] = { wxT("(default)"), wxT("opened"), wxT("suspended"), wxT("closed") };
	int m_JobStatusChoiceNChoices = sizeof( m_JobStatusChoiceChoices ) / sizeof( wxString );
	m_JobStatusChoice = new wxChoice( m_JobStatusPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_JobStatusChoiceNChoices, m_JobStatusChoiceChoices, 0 );
	m_JobStatusChoice->SetSelection( 0 );
	m_JobStatusChoice->SetMinSize( wxSize( 200,-1 ) );
	
	bSizerJobStatusBox->Add( m_JobStatusChoice, 0, 0, 5 );
	
	bSizerJobDescriptionPanel->Add( bSizerJobStatusBox, 1, wxEXPAND, 5 );
	
	m_JobStatusPanel->SetSizer( bSizerJobDescriptionPanel );
	m_JobStatusPanel->Layout();
	bSizerJobDescriptionPanel->Fit( m_JobStatusPanel );
	bSizerJobStatus->Add( m_JobStatusPanel, 1, wxEXPAND, 5 );
	
	bSizerMainForm->Add( bSizerJobStatus, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizerFiles;
	bSizerFiles = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizerFilesText;
	bSizerFilesText = new wxBoxSizer( wxVERTICAL );
	
	m_FilesStaticText = new wxStaticText( this, wxID_ANY, wxT("Files:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_FilesStaticText->Wrap( -1 );
	bSizerFilesText->Add( m_FilesStaticText, 0, wxALL, 5 );
	
	
	bSizerFilesText->Add( 75, 0, 0, 0, 5 );
	
	bSizerFiles->Add( bSizerFilesText, 0, 0, 5 );
	
	wxBoxSizer* bSizerFilesBox;
	bSizerFilesBox = new wxBoxSizer( wxVERTICAL );
	
	wxString m_FileCheckListChoices[] = { wxT("<FILE LIST>") };
	int m_FileCheckListNChoices = sizeof( m_FileCheckListChoices ) / sizeof( wxString );
	m_FileCheckList = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_FileCheckListNChoices, m_FileCheckListChoices, wxLB_HSCROLL|wxLB_MULTIPLE|wxLB_NEEDED_SB );
	bSizerFilesBox->Add( m_FileCheckList, 1, wxALL|wxEXPAND, 5 );
	
	m_FileListBox = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_HSCROLL|wxLB_NEEDED_SB );
	m_FileListBox->Append( wxT("<FILE LIST>") );
	m_FileListBox->Hide();
	
	bSizerFilesBox->Add( m_FileListBox, 1, wxALL|wxEXPAND, 5 );
	
	bSizerFiles->Add( bSizerFilesBox, 1, wxEXPAND, 5 );
	
	bSizerMainForm->Add( bSizerFiles, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizerSelectButtons;
	bSizerSelectButtons = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizerSelectButtons->Add( 75, 0, 0, 0, 5 );
	
	m_SelectAllButtonsPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer28;
	bSizer28 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizerSelectButtonsBox;
	bSizerSelectButtonsBox = new wxBoxSizer( wxHORIZONTAL );
	
	m_SelectAllButton = new wxButton( m_SelectAllButtonsPanel, SelectAll, wxT("Select All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerSelectButtonsBox->Add( m_SelectAllButton, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5 );
	
	
	bSizerSelectButtonsBox->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_UnselectButton = new wxButton( m_SelectAllButtonsPanel, UnselectAll, wxT("Unselect All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerSelectButtonsBox->Add( m_UnselectButton, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5 );
	
	bSizer28->Add( bSizerSelectButtonsBox, 1, wxEXPAND, 5 );
	
	m_SelectAllButtonsPanel->SetSizer( bSizer28 );
	m_SelectAllButtonsPanel->Layout();
	bSizer28->Fit( m_SelectAllButtonsPanel );
	bSizerSelectButtons->Add( m_SelectAllButtonsPanel, 1, wxEXPAND, 5 );
	
	bSizerMainForm->Add( bSizerSelectButtons, 0, wxEXPAND, 5 );
	
	bSizerMain->Add( bSizerMainForm, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizerButtons;
	bSizerButtons = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizerRequiredFields;
	bSizerRequiredFields = new wxBoxSizer( wxVERTICAL );
	
	
	bSizerRequiredFields->Add( 75, 0, 1, 0, 5 );
	
	m_RequiredFieldsStaticText = new wxStaticText( this, wxID_ANY, wxT("*Required"), wxDefaultPosition, wxDefaultSize, 0 );
	m_RequiredFieldsStaticText->Wrap( -1 );
	bSizerRequiredFields->Add( m_RequiredFieldsStaticText, 0, wxALIGN_BOTTOM|wxALL, 5 );
	
	bSizerButtons->Add( bSizerRequiredFields, 0, wxEXPAND, 5 );
	
	m_CommitButtonsPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizerButtonsPanelMain;
	bSizerButtonsPanelMain = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizerReopen;
	bSizerReopen = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizerReopenCheckbox;
	bSizerReopenCheckbox = new wxBoxSizer( wxHORIZONTAL );
	
	m_ReopenCheckBox = new wxCheckBox( m_CommitButtonsPanel, wxID_ANY, wxT("Reopen Files"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizerReopenCheckbox->Add( m_ReopenCheckBox, 0, wxALL, 5 );
	
	bSizerReopen->Add( bSizerReopenCheckbox, 6, wxEXPAND|wxLEFT|wxTOP, 2 );
	
	bSizerButtonsPanelMain->Add( bSizerReopen, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizerButtonsBox;
	bSizerButtonsBox = new wxBoxSizer( wxHORIZONTAL );
	
	m_SubmitButton = new wxButton( m_CommitButtonsPanel, wxID_OK, wxT("Submit"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerButtonsBox->Add( m_SubmitButton, 0, wxLEFT|wxRIGHT, 5 );
	
	m_UpdateButton = new wxButton( m_CommitButtonsPanel, wxID_SAVE, wxT("Update"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerButtonsBox->Add( m_UpdateButton, 0, wxLEFT|wxRIGHT, 5 );
	
	m_CancelButton = new wxButton( m_CommitButtonsPanel, wxID_CANCEL, wxT("Canel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerButtonsBox->Add( m_CancelButton, 0, wxLEFT|wxRIGHT, 5 );
	
	m_SpecNoteButton = new wxButton( m_CommitButtonsPanel, SpecNotes, wxT("Spec Notes"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerButtonsBox->Add( m_SpecNoteButton, 0, wxLEFT|wxRIGHT, 5 );
	
	m_HelpButton = new wxButton( m_CommitButtonsPanel, wxID_HELP, wxT("Help"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerButtonsBox->Add( m_HelpButton, 0, wxLEFT|wxRIGHT, 5 );
	
	bSizerButtonsPanelMain->Add( bSizerButtonsBox, 1, wxEXPAND, 5 );
	
	m_CommitButtonsPanel->SetSizer( bSizerButtonsPanelMain );
	m_CommitButtonsPanel->Layout();
	bSizerButtonsPanelMain->Fit( m_CommitButtonsPanel );
	bSizerButtons->Add( m_CommitButtonsPanel, 1, wxEXPAND, 5 );
	
	bSizerMain->Add( bSizerButtons, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizerMain );
	this->Layout();
}

MainPanel::~MainPanel()
{
}

WaitPanel::WaitPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 260,40 ) );
	this->SetMaxSize( wxSize( 260,40 ) );
	
	wxBoxSizer* bSizer30;
	bSizer30 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer30->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer34;
	bSizer34 = new wxBoxSizer( wxVERTICAL );
	
	
	bSizer34->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_Gauge = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	bSizer34->Add( m_Gauge, 4, wxALL|wxEXPAND, 5 );
	
	
	bSizer34->Add( 0, 0, 1, wxEXPAND, 5 );
	
	bSizer30->Add( bSizer34, 10, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer32;
	bSizer32 = new wxBoxSizer( wxVERTICAL );
	
	
	bSizer32->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_CancelButton = new wxButton( this, wxID_CANCEL, wxT("Work Offline"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer32->Add( m_CancelButton, 0, wxALL|wxEXPAND, 5 );
	
	
	bSizer32->Add( 0, 0, 1, wxEXPAND, 5 );
	
	bSizer30->Add( bSizer32, 10, wxALIGN_CENTER, 5 );
	
	
	bSizer30->Add( 0, 0, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer30 );
	this->Layout();
}

WaitPanel::~WaitPanel()
{
}
