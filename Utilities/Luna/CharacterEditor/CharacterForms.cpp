///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  7 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Precompile.h"

#ifdef WX_PRECOMP

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#else
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "CharacterForms.h"

///////////////////////////////////////////////////////////////////////////

MainPanel::MainPanel( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* m_Sizer;
	m_Sizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_SkeletonTree = new wxTreeCtrl( this, ID_SKELETON_TREE, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE);
	m_Sizer->Add( m_SkeletonTree, 1, wxALL|wxEXPAND|wxALIGN_RIGHT, 5 );
	
	this->SetSizer( m_Sizer );
	this->Layout();
	
	// Connect Events
	m_SkeletonTree->Connect( wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler( MainPanel::OnKeyDown ), NULL, this );
	m_SkeletonTree->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( MainPanel::OnTreeSelect ), NULL, this );
}

PropertiesPanel::PropertiesPanel( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) : wxPanel( parent, id, pos, size, style )
{
}

AttributesPanel::AttributesPanel( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* m_Sizer;
	m_Sizer = new wxBoxSizer( wxVERTICAL );
	
	m_AttributesTree = new wxTreeCtrl( this, ID_ATTRIBUTES_TREE, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_SINGLE);
	m_Sizer->Add( m_AttributesTree, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( m_Sizer );
	this->Layout();
	
	// Connect Events
	m_AttributesTree->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( AttributesPanel::OnSelect ), NULL, this );
}

JointListPanel::JointListPanel( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* m_Sizer;
	m_Sizer = new wxBoxSizer( wxVERTICAL );
	
	m_JointLister = new wxListCtrl( this, ID_DYNAMIC_JOINT_LISTER, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	m_Sizer->Add( m_JointLister, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( m_Sizer );
	this->Layout();
	
	// Connect Events
	m_JointLister->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( JointListPanel::OnSelect ), NULL, this );
}

LooseAttachWizardSingle::LooseAttachWizardSingle( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* m_MainSizer;
	m_MainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* m_TitleSizer;
	m_TitleSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Title = new wxStaticText( this, wxID_ANY, wxT("New Loose Attachment Chain: Single Ended"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Title->Wrap( -1 );
	m_Title->SetFont( wxFont( 10, 74, 90, 92, false, wxT("Arial") ) );
	
	m_TitleSizer->Add( m_Title, 0, 0, 5 );
	
	m_MainSizer->Add( m_TitleSizer, 0, wxALL|wxEXPAND, 5 );
	
	m_Line = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_MainSizer->Add( m_Line, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* m_NameSizer;
	m_NameSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_NameLabel = new wxStaticText( this, wxID_ANY, wxT("Name"), wxDefaultPosition, wxDefaultSize, 0 );
	m_NameLabel->Wrap( -1 );
	m_NameSizer->Add( m_NameLabel, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_NameText = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_NameSizer->Add( m_NameText, 1, wxALIGN_CENTER|wxLEFT|wxRIGHT, 5 );
	
	m_MainSizer->Add( m_NameSizer, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* m_StartSizer;
	m_StartSizer = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxT("Start Joint") ), wxHORIZONTAL );
	
	m_StartJointText = new wxTextCtrl( this, wxID_ANY, wxT("<no joint selected>"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_StartJointText->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	m_StartSizer->Add( m_StartJointText, 1, wxALIGN_CENTER|wxALL, 5 );
	
	m_ButtonPickStart = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 28,28 ), wxBU_AUTODRAW );
	m_ButtonPickStart->SetMinSize( wxSize( 28,28 ) );
	
	m_StartSizer->Add( m_ButtonPickStart, 0, wxALL, 5 );
	
	m_MainSizer->Add( m_StartSizer, 0, wxEXPAND|wxTOP, 5 );
	
	
	m_MainSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxBoxSizer* m_ButtonSizer;
	m_ButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	
	m_ButtonSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button5 = new wxButton( this, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ButtonSizer->Add( m_button5, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_button6 = new wxButton( this, wxID_ANY, wxT("Finish"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ButtonSizer->Add( m_button6, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_MainSizer->Add( m_ButtonSizer, 0, wxALIGN_RIGHT|wxEXPAND, 5 );
	
	this->SetSizer( m_MainSizer );
	this->Layout();
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( LooseAttachWizardSingle::OnClose ) );
	m_ButtonPickStart->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LooseAttachWizardSingle::OnButtonPickStart ), NULL, this );
	m_button5->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LooseAttachWizardSingle::OnButtonCancel ), NULL, this );
	m_button6->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LooseAttachWizardSingle::OnButtonFinish ), NULL, this );
}

LooseAttachWizardDouble::LooseAttachWizardDouble( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* m_MainSizer;
	m_MainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* m_TitleSizer;
	m_TitleSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Title = new wxStaticText( this, wxID_ANY, wxT("New Loose Attachment Chain: Double Ended"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Title->Wrap( -1 );
	m_Title->SetFont( wxFont( 10, 74, 90, 92, false, wxT("Arial") ) );
	
	m_TitleSizer->Add( m_Title, 0, 0, 5 );
	
	m_MainSizer->Add( m_TitleSizer, 0, wxALL|wxEXPAND, 5 );
	
	m_Line = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_MainSizer->Add( m_Line, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* m_NameSizer;
	m_NameSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_NameLabel = new wxStaticText( this, wxID_ANY, wxT("Name"), wxDefaultPosition, wxDefaultSize, 0 );
	m_NameLabel->Wrap( -1 );
	m_NameSizer->Add( m_NameLabel, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_NameText = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_NameSizer->Add( m_NameText, 1, wxALIGN_CENTER|wxLEFT|wxRIGHT, 5 );
	
	m_MainSizer->Add( m_NameSizer, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* m_StartSizer;
	m_StartSizer = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxT("Start Joint") ), wxHORIZONTAL );
	
	m_StartJointText = new wxTextCtrl( this, wxID_ANY, wxT("<no joint selected>"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_StartJointText->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	m_StartSizer->Add( m_StartJointText, 1, wxALIGN_CENTER|wxALL, 5 );
	
	m_ButtonPickStart = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 28,28 ), wxBU_AUTODRAW );
	m_ButtonPickStart->SetMinSize( wxSize( 28,28 ) );
	
	m_StartSizer->Add( m_ButtonPickStart, 0, wxALL, 5 );
	
	m_MainSizer->Add( m_StartSizer, 0, wxEXPAND|wxTOP, 5 );
	
	wxStaticBoxSizer* m_MiddleSizer;
	m_MiddleSizer = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxT("Middle Joint") ), wxHORIZONTAL );
	
	m_MiddleJointText = new wxTextCtrl( this, wxID_ANY, wxT("<no joint selected>"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_MiddleJointText->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	m_MiddleSizer->Add( m_MiddleJointText, 1, wxALIGN_CENTER|wxALL, 5 );
	
	m_ButtonPickMiddle = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 28,28 ), wxBU_AUTODRAW );
	m_ButtonPickMiddle->SetMinSize( wxSize( 28,28 ) );
	
	m_MiddleSizer->Add( m_ButtonPickMiddle, 0, wxALL, 5 );
	
	m_MainSizer->Add( m_MiddleSizer, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* m_EndSizer;
	m_EndSizer = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxT("End Joint") ), wxHORIZONTAL );
	
	m_EndJointText = new wxTextCtrl( this, wxID_ANY, wxT("<no joint selected>"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_EndJointText->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	m_EndSizer->Add( m_EndJointText, 1, wxALIGN_CENTER|wxALL, 5 );
	
	m_ButtonPickEnd = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 28,28 ), wxBU_AUTODRAW );
	m_ButtonPickEnd->SetMinSize( wxSize( 28,28 ) );
	
	m_EndSizer->Add( m_ButtonPickEnd, 0, wxALL, 5 );
	
	m_MainSizer->Add( m_EndSizer, 0, wxEXPAND, 5 );
	
	
	m_MainSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxBoxSizer* m_ButtonSizer;
	m_ButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	
	m_ButtonSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button5 = new wxButton( this, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ButtonSizer->Add( m_button5, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_button6 = new wxButton( this, wxID_ANY, wxT("Finish"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ButtonSizer->Add( m_button6, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_MainSizer->Add( m_ButtonSizer, 0, wxALIGN_RIGHT|wxEXPAND, 5 );
	
	this->SetSizer( m_MainSizer );
	this->Layout();
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( LooseAttachWizardDouble::OnClose ) );
	m_ButtonPickStart->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LooseAttachWizardDouble::OnButtonPickStart ), NULL, this );
	m_ButtonPickMiddle->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LooseAttachWizardDouble::OnButtonPickMiddle ), NULL, this );
	m_ButtonPickEnd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LooseAttachWizardDouble::OnButtonPickEnd ), NULL, this );
	m_button5->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LooseAttachWizardDouble::OnButtonCancel ), NULL, this );
	m_button6->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LooseAttachWizardDouble::OnButtonFinish ), NULL, this );
}

LooseAttachPanel::LooseAttachPanel( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* m_Sizer;
	m_Sizer = new wxBoxSizer( wxVERTICAL );
	
	m_LooseAttachTree = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_HIDE_ROOT);
	m_Sizer->Add( m_LooseAttachTree, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( m_Sizer );
	this->Layout();
	
	// Connect Events
	m_LooseAttachTree->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( LooseAttachPanel::OnTreeSelect ), NULL, this );
}
