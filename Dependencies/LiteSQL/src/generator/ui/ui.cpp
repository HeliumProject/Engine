///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO LITESQL_L("NOT") EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ui.h"

// Using the construction of a static object to ensure that the help provider is set
class wxFBContextSensitiveHelpSetter
{
public:
wxFBContextSensitiveHelpSetter()
{
wxHelpProvider::Set( new wxHelpControllerHelpProvider );
}
};

static wxFBContextSensitiveHelpSetter s_wxFBSetTheHelpProvider;
///////////////////////////////////////////////////////////////////////////
using namespace ui;

ObjectPanel::ObjectPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 250,80 ) );
	
	wxFlexGridSizer* gSizer1;
	gSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	gSizer1->AddGrowableCol( 1 );
	gSizer1->SetFlexibleDirection( wxBOTH );
	gSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	lblName = new wxStaticText( this, wxID_ANY, _(LITESQL_L("Name")), wxDefaultPosition, wxDefaultSize, 0 );
	lblName->Wrap( -1 );
	gSizer1->Add( lblName, 1, wxALL|wxEXPAND, 5 );
	
	m_textCtrlName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_textCtrlName, 2, wxALL|wxFIXED_MINSIZE|wxEXPAND, 5 );
	
	lblInherits = new wxStaticText( this, wxID_ANY, _(LITESQL_L("Inherit From")), wxDefaultPosition, wxDefaultSize, 0 );
	lblInherits->Wrap( -1 );
	gSizer1->Add( lblInherits, 0, wxALL|wxEXPAND, 5 );
	
	wxArrayString m_choiceInheritsFromChoices;
	m_choiceInheritsFrom = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceInheritsFromChoices, 0 );
	m_choiceInheritsFrom->SetSelection( 0 );
	m_choiceInheritsFrom->SetMinSize( wxSize( 150,-1 ) );
	
	gSizer1->Add( m_choiceInheritsFrom, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( gSizer1 );
	this->Layout();
}

ObjectPanel::~ObjectPanel()
{
}

FieldPanel::FieldPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 300,120 ) );
	
	wxFlexGridSizer* gSizer1;
	gSizer1 = new wxFlexGridSizer( 7, 2, 0, 0 );
	gSizer1->AddGrowableCol( 1 );
	gSizer1->SetFlexibleDirection( wxBOTH );
	gSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	lblName = new wxStaticText( this, wxID_ANY, _(LITESQL_L("Name")), wxDefaultPosition, wxDefaultSize, 0 );
	lblName->Wrap( -1 );
	gSizer1->Add( lblName, 1, wxALL, 5 );
	
	m_textCtrlName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_textCtrlName, 1, wxALL|wxEXPAND, 5 );
	
	lblFieldtype = new wxStaticText( this, wxID_ANY, _(LITESQL_L("Fieldtype")), wxDefaultPosition, wxDefaultSize, 0 );
	lblFieldtype->Wrap( -1 );
	gSizer1->Add( lblFieldtype, 1, wxALL, 5 );
	
	wxArrayString m_choiceFieldtypeChoices;
	m_choiceFieldtype = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceFieldtypeChoices, 0 );
	m_choiceFieldtype->SetSelection( 0 );
	m_choiceFieldtype->SetMinSize( wxSize( 150,-1 ) );
	
	gSizer1->Add( m_choiceFieldtype, 1, wxALL|wxEXPAND, 5 );
	
	lblDefault = new wxStaticText( this, wxID_ANY, _(LITESQL_L("Default Value:")), wxDefaultPosition, wxDefaultSize, 0 );
	lblDefault->Wrap( -1 );
	gSizer1->Add( lblDefault, 1, wxALL, 5 );
	
	m_textCtrlDefaultValue = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_textCtrlDefaultValue, 1, wxALL|wxEXPAND, 5 );
	
	lblIndexed = new wxStaticText( this, wxID_ANY, _(LITESQL_L("Indexed")), wxDefaultPosition, wxDefaultSize, 0 );
	lblIndexed->Wrap( -1 );
	gSizer1->Add( lblIndexed, 1, wxALL, 5 );
	
	m_checkBoxIndexed = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer1->Add( m_checkBoxIndexed, 0, wxALL, 5 );
	
	lblUnique = new wxStaticText( this, wxID_ANY, _(LITESQL_L("Unique")), wxDefaultPosition, wxDefaultSize, 0 );
	lblUnique->Wrap( -1 );
	gSizer1->Add( lblUnique, 1, wxALL, 5 );
	
	m_checkBoxUnique = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer1->Add( m_checkBoxUnique, 0, wxALL, 5 );
	
	lblValues = new wxStaticText( this, wxID_ANY, _(LITESQL_L("Values")), wxDefaultPosition, wxDefaultSize, 0 );
	lblValues->Wrap( -1 );
	gSizer1->Add( lblValues, 0, wxALL, 5 );
	
	m_listValues = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_EDIT_LABELS|wxLC_REPORT );
	gSizer1->Add( m_listValues, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText20 = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText20->Wrap( -1 );
	gSizer1->Add( m_staticText20, 1, wxALL, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_btnAdd = new wxButton( this, wxID_ANY, _(LITESQL_L("+")), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer2->Add( m_btnAdd, 0, wxALL, 5 );
	
	m_btnRemove = new wxButton( this, wxID_ANY, _(LITESQL_L("-")), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer2->Add( m_btnRemove, 0, wxALL, 5 );
	
	gSizer1->Add( bSizer2, 1, wxEXPAND, 5 );
	
	this->SetSizer( gSizer1 );
	this->Layout();
	
	// Connect Events
	m_btnAdd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FieldPanel::OnAddValue ), NULL, this );
	m_btnRemove->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FieldPanel::OnRemoveValue ), NULL, this );
}

FieldPanel::~FieldPanel()
{
	// Disconnect Events
	m_btnAdd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FieldPanel::OnAddValue ), NULL, this );
	m_btnRemove->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FieldPanel::OnRemoveValue ), NULL, this );
}

MethodPanel::MethodPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 300,120 ) );
	
	wxFlexGridSizer* gSizer1;
	gSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	gSizer1->AddGrowableCol( 1 );
	gSizer1->SetFlexibleDirection( wxBOTH );
	gSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	lblName = new wxStaticText( this, wxID_ANY, _(LITESQL_L("Name")), wxDefaultPosition, wxDefaultSize, 0 );
	lblName->Wrap( -1 );
	gSizer1->Add( lblName, 0, wxALL, 5 );
	
	m_textCtrlName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_textCtrlName, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( gSizer1 );
	this->Layout();
}

MethodPanel::~MethodPanel()
{
}

RelationPanel::RelationPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 300,120 ) );
	
	wxFlexGridSizer* gSizer1;
	gSizer1 = new wxFlexGridSizer( 3, 2, 0, 0 );
	gSizer1->AddGrowableCol( 1 );
	gSizer1->SetFlexibleDirection( wxBOTH );
	gSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	lblName = new wxStaticText( this, wxID_ANY, _(LITESQL_L("Name")), wxDefaultPosition, wxDefaultSize, 0 );
	lblName->Wrap( -1 );
	gSizer1->Add( lblName, 0, wxALL|wxEXPAND, 5 );
	
	m_textCtrlName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_textCtrlName, 0, wxALL|wxEXPAND, 5 );
	
	lblObject1 = new wxStaticText( this, wxID_ANY, _(LITESQL_L("Object 1")), wxDefaultPosition, wxDefaultSize, 0 );
	lblObject1->Wrap( -1 );
	gSizer1->Add( lblObject1, 1, wxALL, 5 );
	
	wxArrayString m_choiceObject1Choices;
	m_choiceObject1 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceObject1Choices, 0 );
	m_choiceObject1->SetSelection( 0 );
	m_choiceObject1->SetMinSize( wxSize( 150,-1 ) );
	
	gSizer1->Add( m_choiceObject1, 0, wxALL|wxEXPAND, 5 );
	
	lblObject2 = new wxStaticText( this, wxID_ANY, _(LITESQL_L("Object 2")), wxDefaultPosition, wxDefaultSize, 0 );
	lblObject2->Wrap( -1 );
	gSizer1->Add( lblObject2, 1, wxALL, 5 );
	
	wxArrayString m_choiceObject2Choices;
	m_choiceObject2 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceObject2Choices, 0 );
	m_choiceObject2->SetSelection( 0 );
	m_choiceObject2->SetMinSize( wxSize( 150,-1 ) );
	
	gSizer1->Add( m_choiceObject2, 0, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( gSizer1 );
	this->Layout();
}

RelationPanel::~RelationPanel()
{
}

DatabasePanel::DatabasePanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 300,120 ) );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 3, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );
	
	m_staticName = new wxStaticText( this, wxID_ANY, _(LITESQL_L("Name")), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticName->Wrap( -1 );
	fgSizer1->Add( m_staticName, 0, wxALL, 5 );
	
	m_textName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textName, 1, wxALL|wxEXPAND, 5 );
	
	m_staticNamespace = new wxStaticText( this, wxID_ANY, _(LITESQL_L("Namespace")), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticNamespace->Wrap( -1 );
	fgSizer1->Add( m_staticNamespace, 0, wxALL, 5 );
	
	m_textNamespace = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textNamespace, 1, wxALL|wxEXPAND, 5 );
	
	m_staticInclude = new wxStaticText( this, wxID_ANY, _(LITESQL_L("additional Include")), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticInclude->Wrap( -1 );
	fgSizer1->Add( m_staticInclude, 0, wxALL, 5 );
	
	m_textInclude = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textInclude, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( fgSizer1 );
	this->Layout();
}

DatabasePanel::~DatabasePanel()
{
}

AboutDialog::AboutDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, _(LITESQL_L("Visual-Litesql")), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_staticText4->Wrap( -1 );
	bSizer4->Add( m_staticText4, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, _(LITESQL_L("Version")), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_staticText5->Wrap( -1 );
	bSizer4->Add( m_staticText5, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_btnClose = new wxButton( this, wxID_OK, _(LITESQL_L("Close")), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_btnClose, 0, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer4 );
	this->Layout();
}

AboutDialog::~AboutDialog()
{
}

GeneratePanel::GeneratePanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 400,210 ) );
	
	wxFlexGridSizer* panelSizer;
	panelSizer = new wxFlexGridSizer( 4, 2, 0, 0 );
	panelSizer->AddGrowableCol( 1 );
	panelSizer->SetFlexibleDirection( wxBOTH );
	panelSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );
	
	m_staticOutputDir = new wxStaticText( this, wxID_ANY, _(LITESQL_L("Output Directory")), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticOutputDir->Wrap( -1 );
	panelSizer->Add( m_staticOutputDir, 0, wxALL, 5 );
	
	m_dirPickerOutputDir = new wxDirPickerCtrl( this, wxID_ANY, wxEmptyString, _(LITESQL_L("Select a folder")), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE|wxDIRP_DIR_MUST_EXIST );
	panelSizer->Add( m_dirPickerOutputDir, 1, wxALL|wxEXPAND, 5 );
	
	m_staticGenerators = new wxStaticText( this, wxID_ANY, _(LITESQL_L("Select generators")), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticGenerators->Wrap( -1 );
	panelSizer->Add( m_staticGenerators, 0, wxALL, 5 );
	
	wxArrayString m_checkListGeneratorsChoices;
	m_checkListGenerators = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListGeneratorsChoices, wxLB_MULTIPLE );
	m_checkListGenerators->SetMinSize( wxSize( -1,120 ) );
	
	panelSizer->Add( m_checkListGenerators, 10, wxALL|wxEXPAND, 5 );
	
	m_buttonRun = new wxButton( this, wxID_ANY, _(LITESQL_L("Run")), wxDefaultPosition, wxDefaultSize, 0 );
	panelSizer->Add( m_buttonRun, 0, wxALL, 5 );
	
	m_gaugeRunProgress = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxSize( -1,20 ), wxGA_HORIZONTAL );
	m_gaugeRunProgress->SetMinSize( wxSize( 100,20 ) );
	m_gaugeRunProgress->SetMaxSize( wxSize( -1,20 ) );
	
	panelSizer->Add( m_gaugeRunProgress, 1, wxALL|wxFIXED_MINSIZE|wxEXPAND, 5 );
	
	this->SetSizer( panelSizer );
	this->Layout();
	
	// Connect Events
	m_buttonRun->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GeneratePanel::OnRunClick ), NULL, this );
}

GeneratePanel::~GeneratePanel()
{
	// Disconnect Events
	m_buttonRun->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GeneratePanel::OnRunClick ), NULL, this );
}

ModelTreePanel::ModelTreePanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bMainSizer;
	bMainSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_mainSplitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_mainSplitter->Connect( wxEVT_IDLE, wxIdleEventHandler( ModelTreePanel::m_mainSplitterOnIdle ), NULL, this );
	m_treePanel = new wxPanel( m_mainSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* btreePanelSizer;
	btreePanelSizer = new wxBoxSizer( wxVERTICAL );
	
	m_modelTreeCtrl = new wxTreeCtrl( m_treePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_HIDE_ROOT );
	btreePanelSizer->Add( m_modelTreeCtrl, 5, wxALL|wxEXPAND, 5 );
	
	m_treePanel->SetSizer( btreePanelSizer );
	m_treePanel->Layout();
	btreePanelSizer->Fit( m_treePanel );
	m_detailPanel = new wxPanel( m_mainSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bDetailPanelSizer;
	bDetailPanelSizer = new wxBoxSizer( wxVERTICAL );
	
  m_detailNotebook = new wxNotebook( m_detailPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
  bDetailPanelSizer->Add( m_detailNotebook, 1, wxEXPAND | wxALL, 5 );
	
	m_detailPanel->SetSizer( bDetailPanelSizer );
	m_detailPanel->Layout();
	bDetailPanelSizer->Fit( m_detailPanel );
	m_mainSplitter->SplitVertically( m_treePanel, m_detailPanel, 0 );
	bMainSizer->Add( m_mainSplitter, 1, wxEXPAND, 5 );
	
	this->SetSizer( bMainSizer );
	this->Layout();
	
	// Connect Events
	m_modelTreeCtrl->Connect( wxEVT_COMMAND_TREE_DELETE_ITEM, wxTreeEventHandler( ModelTreePanel::OnTreeDeleteItem ), NULL, this );
	m_modelTreeCtrl->Connect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( ModelTreePanel::OnTreeItemActivated ), NULL, this );
	m_modelTreeCtrl->Connect( wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP, wxTreeEventHandler( ModelTreePanel::OnTreeItemGetTooltip ), NULL, this );
	m_modelTreeCtrl->Connect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( ModelTreePanel::OnTreeItemMenu ), NULL, this );
	m_modelTreeCtrl->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( ModelTreePanel::OnTreeSelChanged ), NULL, this );
	m_modelTreeCtrl->Connect( wxEVT_COMMAND_TREE_SEL_CHANGING, wxTreeEventHandler( ModelTreePanel::OnTreeSelChanging ), NULL, this );
}

ModelTreePanel::~ModelTreePanel()
{
	// Disconnect Events
	m_modelTreeCtrl->Disconnect( wxEVT_COMMAND_TREE_DELETE_ITEM, wxTreeEventHandler( ModelTreePanel::OnTreeDeleteItem ), NULL, this );
	m_modelTreeCtrl->Disconnect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( ModelTreePanel::OnTreeItemActivated ), NULL, this );
	m_modelTreeCtrl->Disconnect( wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP, wxTreeEventHandler( ModelTreePanel::OnTreeItemGetTooltip ), NULL, this );
	m_modelTreeCtrl->Disconnect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( ModelTreePanel::OnTreeItemMenu ), NULL, this );
	m_modelTreeCtrl->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( ModelTreePanel::OnTreeSelChanged ), NULL, this );
	m_modelTreeCtrl->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGING, wxTreeEventHandler( ModelTreePanel::OnTreeSelChanging ), NULL, this );
}