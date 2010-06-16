///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Precompile.h"


#include "Application/UI/SortTreeCtrl.h"

#include "RegionsGenerated.h"

///////////////////////////////////////////////////////////////////////////
using namespace Luna;

RegionsPanelBase::RegionsPanelBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	m_Sizer = new wxBoxSizer( wxVERTICAL );
	
	m_RegionTree = new Nocturnal::SortTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_EDIT_LABELS|wxTR_HIDE_ROOT );
	m_Sizer->Add( m_RegionTree, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( m_Sizer );
	this->Layout();
	
	// Connect Events
	m_RegionTree->Connect( wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler( RegionsPanelBase::OnBeginLabelEdit ), NULL, this );
	m_RegionTree->Connect( wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler( RegionsPanelBase::OnEndLabelEdit ), NULL, this );
	m_RegionTree->Connect( wxEVT_COMMAND_TREE_ITEM_COLLAPSING, wxTreeEventHandler( RegionsPanelBase::OnTreeItemCollapsing ), NULL, this );
	m_RegionTree->Connect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( RegionsPanelBase::OnTreeItemMenu ), NULL, this );
	m_RegionTree->Connect( wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler( RegionsPanelBase::OnTreeKeyDown ), NULL, this );
	m_RegionTree->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( RegionsPanelBase::OnTreeSelChanged ), NULL, this );
}

RegionsPanelBase::~RegionsPanelBase()
{
	// Disconnect Events
	m_RegionTree->Disconnect( wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler( RegionsPanelBase::OnBeginLabelEdit ), NULL, this );
	m_RegionTree->Disconnect( wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler( RegionsPanelBase::OnEndLabelEdit ), NULL, this );
	m_RegionTree->Disconnect( wxEVT_COMMAND_TREE_ITEM_COLLAPSING, wxTreeEventHandler( RegionsPanelBase::OnTreeItemCollapsing ), NULL, this );
	m_RegionTree->Disconnect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( RegionsPanelBase::OnTreeItemMenu ), NULL, this );
	m_RegionTree->Disconnect( wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler( RegionsPanelBase::OnTreeKeyDown ), NULL, this );
	m_RegionTree->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( RegionsPanelBase::OnTreeSelChanged ), NULL, this );
}

ChooserDialog::ChooserDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* m_Sizer;
	m_Sizer = new wxBoxSizer( wxVERTICAL );
	
	wxArrayString m_ListBoxChoices;
	m_ListBox = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_ListBoxChoices, 0 );
	m_Sizer->Add( m_ListBox, 1, wxALL|wxEXPAND, 5 );
	
	m_ButtonSizer = new wxStdDialogButtonSizer();
	m_ButtonSizerOK = new wxButton( this, wxID_OK );
	m_ButtonSizer->AddButton( m_ButtonSizerOK );
	m_ButtonSizerCancel = new wxButton( this, wxID_CANCEL );
	m_ButtonSizer->AddButton( m_ButtonSizerCancel );
	m_ButtonSizer->Realize();
	m_Sizer->Add( m_ButtonSizer, 0, wxEXPAND, 5 );
	
	this->SetSizer( m_Sizer );
	this->Layout();
}

ChooserDialog::~ChooserDialog()
{
}

RegionCreateDialog::RegionCreateDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxVERTICAL );
	
	
	bSizer17->Add( 0, 30, 0, 0, 5 );
	
	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer18->Add( 10, 0, 0, 0, 5 );
	
	m_Label = new wxStaticText( this, wxID_ANY, wxT("Region Name"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Label->Wrap( -1 );
	bSizer18->Add( m_Label, 0, wxALL, 5 );
	
	m_TextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer18->Add( m_TextCtrl, 1, 0, 5 );
	
	
	bSizer18->Add( 15, 0, 0, 0, 5 );
	
	bSizer17->Add( bSizer18, 1, wxEXPAND, 5 );
	
	m_sdbSizer3 = new wxStdDialogButtonSizer();
	m_sdbSizer3OK = new wxButton( this, wxID_OK );
	m_sdbSizer3->AddButton( m_sdbSizer3OK );
	m_sdbSizer3Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer3->AddButton( m_sdbSizer3Cancel );
	m_sdbSizer3->Realize();
	bSizer17->Add( m_sdbSizer3, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer17 );
	this->Layout();
}

RegionCreateDialog::~RegionCreateDialog()
{
}
