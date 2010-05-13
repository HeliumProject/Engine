///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Precompile.h"

#include "UIToolKit/AutoCompleteComboBox.h"

#include "AssetEditorGenerated.h"

///////////////////////////////////////////////////////////////////////////
using namespace Luna;

ReplaceAnimSetsDlg::ReplaceAnimSetsDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* topSizer;
	topSizer = new wxBoxSizer( wxVERTICAL );
	
	m_StaticText = new wxStaticText( this, wxID_ANY, wxT("There are already Animation Sets associated with the following assets:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticText->Wrap( -1 );
	topSizer->Add( m_StaticText, 0, wxALL, 5 );
	
	m_ListBox = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_HSCROLL|wxLB_NEEDED_SB|wxLB_SINGLE|wxLB_SORT ); 
	topSizer->Add( m_ListBox, 1, wxALL|wxEXPAND, 5 );
	
	m_StaticPrompt = new wxStaticText( this, wxID_ANY, wxT("Are you sure that you want to create new Animation Sets for these assets?"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticPrompt->Wrap( -1 );
	topSizer->Add( m_StaticPrompt, 0, wxALL, 5 );
	
	mainSizer->Add( topSizer, 1, wxEXPAND, 5 );
	
	m_ButtonSizer = new wxStdDialogButtonSizer();
	m_ButtonSizerYes = new wxButton( this, wxID_YES );
	m_ButtonSizer->AddButton( m_ButtonSizerYes );
	m_ButtonSizerNo = new wxButton( this, wxID_NO );
	m_ButtonSizer->AddButton( m_ButtonSizerNo );
	m_ButtonSizer->Realize();
	mainSizer->Add( m_ButtonSizer, 0, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

ReplaceAnimSetsDlg::~ReplaceAnimSetsDlg()
{
}

MultiAnimPanelGenerated::MultiAnimPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, wxT("Main Entity"), wxDefaultPosition, wxSize( 64,-1 ), wxST_NO_AUTORESIZE );
	m_staticText5->Wrap( -1 );
	bSizer5->Add( m_staticText5, 0, wxALL, 5 );
	
	wxArrayString m_MainEntityChoiceChoices;
	m_MainEntityChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_MainEntityChoiceChoices, 0 );
	m_MainEntityChoice->SetSelection( 0 );
	bSizer5->Add( m_MainEntityChoice, 1, wxALL, 5 );
	
	m_staticText51 = new wxStaticText( this, wxID_ANY, wxT("Joint"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText51->Wrap( -1 );
	bSizer5->Add( m_staticText51, 0, wxALL, 5 );
	
	wxArrayString m_MainJointChoiceChoices;
	m_MainJointChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_MainJointChoiceChoices, 0 );
	m_MainJointChoice->SetSelection( 0 );
	m_MainJointChoice->SetMinSize( wxSize( 128,-1 ) );
	
	bSizer5->Add( m_MainJointChoice, 0, wxALL, 5 );
	
	bSizer4->Add( bSizer5, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText11 = new wxStaticText( this, wxID_ANY, wxT("Attach Entity"), wxDefaultPosition, wxSize( 64,-1 ), wxST_NO_AUTORESIZE );
	m_staticText11->Wrap( -1 );
	bSizer6->Add( m_staticText11, 0, wxALL, 5 );
	
	wxArrayString m_AttachEntityChoiceChoices;
	m_AttachEntityChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_AttachEntityChoiceChoices, 0 );
	m_AttachEntityChoice->SetSelection( 0 );
	bSizer6->Add( m_AttachEntityChoice, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText111 = new wxStaticText( this, wxID_ANY, wxT("Joint"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText111->Wrap( -1 );
	bSizer6->Add( m_staticText111, 0, wxALL, 5 );
	
	wxArrayString m_AttachJointChoiceChoices;
	m_AttachJointChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_AttachJointChoiceChoices, 0 );
	m_AttachJointChoice->SetSelection( 0 );
	m_AttachJointChoice->SetMinSize( wxSize( 128,-1 ) );
	
	bSizer6->Add( m_AttachJointChoice, 0, wxALL, 5 );
	
	bSizer4->Add( bSizer6, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Anim Clip"), wxDefaultPosition, wxSize( 64,-1 ), wxST_NO_AUTORESIZE );
	m_staticText1->Wrap( -1 );
	bSizer8->Add( m_staticText1, 0, wxALL, 5 );
	
	m_AnimClipCombo = new UIToolKit::AutoCompleteComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_SORT|wxTE_PROCESS_ENTER ); 
	bSizer8->Add( m_AnimClipCombo, 1, wxALL|wxEXPAND, 5 );
	
	bSizer4->Add( bSizer8, 0, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer4->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	bSizer13->Add( bSizer4, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_ButtonSnapBegin = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer2->Add( m_ButtonSnapBegin, 0, wxBOTTOM|wxTOP, 5 );
	
	m_ButtonPlayPause = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer2->Add( m_ButtonPlayPause, 0, wxBOTTOM|wxTOP, 5 );
	
	m_ButtonSnapEnd = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer2->Add( m_ButtonSnapEnd, 0, wxBOTTOM|wxTOP, 5 );
	
	
	bSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	bSizer13->Add( bSizer2, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_TimeSlider = new wxSlider( this, wxID_ANY, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS|wxSL_SELRANGE );
	bSizer3->Add( m_TimeSlider, 0, wxALL|wxEXPAND, 5 );
	
	bSizer13->Add( bSizer3, 1, wxEXPAND, 5 );
	
	bSizer1->Add( bSizer13, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxVERTICAL );
	
	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER );
	m_panel1->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_APPWORKSPACE ) );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxVERTICAL );
	
	m_ButtonEnableJointAttach = new wxBitmapButton( m_panel1, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), wxBU_AUTODRAW );
	m_ButtonEnableJointAttach->SetToolTip( wxT("Attachment Mode") );
	
	m_ButtonEnableJointAttach->SetToolTip( wxT("Attachment Mode") );
	
	bSizer17->Add( m_ButtonEnableJointAttach, 0, wxALL, 5 );
	
	m_ButtonForceLoop = new wxBitmapButton( m_panel1, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), wxBU_AUTODRAW );
	m_ButtonForceLoop->SetToolTip( wxT("Force Looping") );
	
	m_ButtonForceLoop->SetToolTip( wxT("Force Looping") );
	
	bSizer17->Add( m_ButtonForceLoop, 0, wxALL, 5 );
	
	m_ButtonFrameEntities = new wxBitmapButton( m_panel1, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), wxBU_AUTODRAW );
	m_ButtonFrameEntities->SetToolTip( wxT("Frame Entities") );
	
	m_ButtonFrameEntities->SetToolTip( wxT("Frame Entities") );
	
	bSizer17->Add( m_ButtonFrameEntities, 0, wxALL, 5 );
	
	m_panel1->SetSizer( bSizer17 );
	m_panel1->Layout();
	bSizer17->Fit( m_panel1 );
	bSizer16->Add( m_panel1, 1, wxEXPAND, 5 );
	
	bSizer14->Add( bSizer16, 1, wxEXPAND, 5 );
	
	bSizer1->Add( bSizer14, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	m_MainEntityChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MultiAnimPanelGenerated::OnMainEntityChoice ), NULL, this );
	m_MainJointChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MultiAnimPanelGenerated::OnMainJointChoice ), NULL, this );
	m_AttachEntityChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MultiAnimPanelGenerated::OnAttachEntityChoice ), NULL, this );
	m_AttachJointChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MultiAnimPanelGenerated::OnAttachJointChoice ), NULL, this );
	m_AnimClipCombo->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( MultiAnimPanelGenerated::OnAnimClipChoice ), NULL, this );
	m_AnimClipCombo->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( MultiAnimPanelGenerated::OnAnimClipTextChoice ), NULL, this );
	m_ButtonSnapBegin->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MultiAnimPanelGenerated::OnButtonSnapBegin ), NULL, this );
	m_ButtonPlayPause->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MultiAnimPanelGenerated::OnButtonPlayPause ), NULL, this );
	m_ButtonSnapEnd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MultiAnimPanelGenerated::OnButtonSnapEnd ), NULL, this );
	m_TimeSlider->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( MultiAnimPanelGenerated::OnScroll ), NULL, this );
	m_TimeSlider->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( MultiAnimPanelGenerated::OnScroll ), NULL, this );
	m_TimeSlider->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( MultiAnimPanelGenerated::OnScroll ), NULL, this );
	m_TimeSlider->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( MultiAnimPanelGenerated::OnScroll ), NULL, this );
	m_TimeSlider->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( MultiAnimPanelGenerated::OnScroll ), NULL, this );
	m_TimeSlider->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( MultiAnimPanelGenerated::OnScroll ), NULL, this );
	m_TimeSlider->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( MultiAnimPanelGenerated::OnScroll ), NULL, this );
	m_TimeSlider->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( MultiAnimPanelGenerated::OnScroll ), NULL, this );
	m_TimeSlider->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( MultiAnimPanelGenerated::OnScroll ), NULL, this );
	m_ButtonEnableJointAttach->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MultiAnimPanelGenerated::OnButtonEnableJointAttach ), NULL, this );
	m_ButtonForceLoop->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MultiAnimPanelGenerated::OnButtonForceLoop ), NULL, this );
	m_ButtonFrameEntities->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MultiAnimPanelGenerated::OnButtonFrameEntities ), NULL, this );
}

MultiAnimPanelGenerated::~MultiAnimPanelGenerated()
{
	// Disconnect Events
	m_MainEntityChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MultiAnimPanelGenerated::OnMainEntityChoice ), NULL, this );
	m_MainJointChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MultiAnimPanelGenerated::OnMainJointChoice ), NULL, this );
	m_AttachEntityChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MultiAnimPanelGenerated::OnAttachEntityChoice ), NULL, this );
	m_AttachJointChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MultiAnimPanelGenerated::OnAttachJointChoice ), NULL, this );
	m_AnimClipCombo->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( MultiAnimPanelGenerated::OnAnimClipChoice ), NULL, this );
	m_AnimClipCombo->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( MultiAnimPanelGenerated::OnAnimClipTextChoice ), NULL, this );
	m_ButtonSnapBegin->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MultiAnimPanelGenerated::OnButtonSnapBegin ), NULL, this );
	m_ButtonPlayPause->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MultiAnimPanelGenerated::OnButtonPlayPause ), NULL, this );
	m_ButtonSnapEnd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MultiAnimPanelGenerated::OnButtonSnapEnd ), NULL, this );
	m_TimeSlider->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( MultiAnimPanelGenerated::OnScroll ), NULL, this );
	m_TimeSlider->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( MultiAnimPanelGenerated::OnScroll ), NULL, this );
	m_TimeSlider->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( MultiAnimPanelGenerated::OnScroll ), NULL, this );
	m_TimeSlider->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( MultiAnimPanelGenerated::OnScroll ), NULL, this );
	m_TimeSlider->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( MultiAnimPanelGenerated::OnScroll ), NULL, this );
	m_TimeSlider->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( MultiAnimPanelGenerated::OnScroll ), NULL, this );
	m_TimeSlider->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( MultiAnimPanelGenerated::OnScroll ), NULL, this );
	m_TimeSlider->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( MultiAnimPanelGenerated::OnScroll ), NULL, this );
	m_TimeSlider->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( MultiAnimPanelGenerated::OnScroll ), NULL, this );
	m_ButtonEnableJointAttach->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MultiAnimPanelGenerated::OnButtonEnableJointAttach ), NULL, this );
	m_ButtonForceLoop->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MultiAnimPanelGenerated::OnButtonForceLoop ), NULL, this );
	m_ButtonFrameEntities->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MultiAnimPanelGenerated::OnButtonFrameEntities ), NULL, this );
}

AnimGroupPanel::AnimGroupPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 355,200 ) );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* descSizer;
	descSizer = new wxBoxSizer( wxVERTICAL );
	
	m_StaticDescription = new wxStaticText( this, wxID_ANY, wxT("Edit the Animation Group options below."), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticDescription->Wrap( -1 );
	descSizer->Add( m_StaticDescription, 0, wxALL, 5 );
	
	mainSizer->Add( descSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* categorySizer;
	categorySizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* innerCatSizer;
	innerCatSizer = new wxBoxSizer( wxHORIZONTAL );
	
	innerCatSizer->SetMinSize( wxSize( 160,-1 ) ); 
	m_StaticCategory = new wxStaticText( this, wxID_ANY, wxT("Category: Subcategory"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticCategory->Wrap( -1 );
	innerCatSizer->Add( m_StaticCategory, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_CategoryChoices;
	m_Category = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxSize( 150,-1 ), m_CategoryChoices, 0 );
	m_Category->SetSelection( 0 );
	m_Category->SetMinSize( wxSize( 150,-1 ) );
	m_Category->SetMaxSize( wxSize( 150,-1 ) );
	
	innerCatSizer->Add( m_Category, 1, wxALL|wxEXPAND, 5 );
	
	categorySizer->Add( innerCatSizer, 1, wxEXPAND, 5 );
	
	mainSizer->Add( categorySizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* propGridSizer;
	propGridSizer = new wxBoxSizer( wxVERTICAL );
	
	m_PropertyGrid = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE|wxPG_SPLITTER_AUTO_CENTER);
	propGridSizer->Add( m_PropertyGrid, 1, wxEXPAND | wxALL, 5 );
	
	mainSizer->Add( propGridSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

AnimGroupPanel::~AnimGroupPanel()
{
}

AttribChooserPanel::AttribChooserPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* treeSizer;
	treeSizer = new wxBoxSizer( wxVERTICAL );
	
	m_TreeCtrl = new wxTreeCtrl( this, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxTR_FULL_ROW_HIGHLIGHT|wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT|wxTR_LINES_AT_ROOT|wxTR_MULTIPLE|wxTR_NO_LINES|wxTR_ROW_LINES|wxTR_SINGLE );
	treeSizer->Add( m_TreeCtrl, 1, wxALL|wxEXPAND, 0 );
	
	mainSizer->Add( treeSizer, 1, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );
	
	wxBoxSizer* descriptionSizer;
	descriptionSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Description = new wxTextCtrl( this, ID_DEFAULT, wxT("Select an attribute (or attributes) to add from the list above."), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	m_Description->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	descriptionSizer->Add( m_Description, 0, wxEXPAND|wxLEFT|wxRIGHT, 5 );
	
	mainSizer->Add( descriptionSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bottomSizer;
	bottomSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* lineSizer;
	lineSizer = new wxBoxSizer( wxVERTICAL );
	
	m_StaticLine = new wxStaticLine( this, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	lineSizer->Add( m_StaticLine, 0, wxALL|wxEXPAND, 5 );
	
	bottomSizer->Add( lineSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	
	buttonSizer->Add( 0, 0, 1, wxALL, 5 );
	
	m_ButtonOk = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_ButtonOk, 0, wxRIGHT, 5 );
	
	m_ButtonCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_ButtonCancel, 0, wxALL, 0 );
	
	bottomSizer->Add( buttonSizer, 0, wxEXPAND, 5 );
	
	mainSizer->Add( bottomSizer, 0, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

AttribChooserPanel::~AttribChooserPanel()
{
}

AttributeCategoriesPanel::AttributeCategoriesPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* groupSizer;
	groupSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	wxBoxSizer* middleSizer;
	middleSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Tabs = new wxNotebook( this, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, 0 );
	
	middleSizer->Add( m_Tabs, 1, wxEXPAND | wxALL, 5 );
	
	groupSizer->Add( middleSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bottomSizer;
	bottomSizer = new wxBoxSizer( wxVERTICAL );
	
	m_MessagePanel = new wxPanel( this, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER|wxTAB_TRAVERSAL );
	m_MessagePanel->SetMinSize( wxSize( -1,60 ) );
	
	wxBoxSizer* descSizer;
	descSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* innerSizer;
	innerSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_Icon = new wxStaticBitmap( m_MessagePanel, ID_DEFAULT, wxNullBitmap, wxDefaultPosition, wxSize( 32,32 ), 0 );
	m_Icon->Hide();
	
	innerSizer->Add( m_Icon, 0, wxALL|wxALIGN_CENTER_VERTICAL, 1 );
	
	m_Message = new wxTextCtrl( m_MessagePanel, ID_DEFAULT, wxT("Select an attribute and click the add button. You can also double-click on an attribute to add it to the currently selected asset."), wxDefaultPosition, wxSize( -1,-1 ), wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH|wxNO_BORDER );
	m_Message->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	innerSizer->Add( m_Message, 1, wxALL|wxEXPAND, 5 );
	
	descSizer->Add( innerSizer, 1, wxEXPAND, 5 );
	
	m_MessagePanel->SetSizer( descSizer );
	m_MessagePanel->Layout();
	descSizer->Fit( m_MessagePanel );
	bottomSizer->Add( m_MessagePanel, 0, wxEXPAND | wxALL, 5 );
	
	groupSizer->Add( bottomSizer, 0, wxEXPAND, 5 );
	
	mainSizer->Add( groupSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

AttributeCategoriesPanel::~AttributeCategoriesPanel()
{
}

AttributeCategoryPanel::AttributeCategoryPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* topSizer;
	topSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_ShortDescription = new wxStaticText( this, ID_DEFAULT, wxT("Short description of category goes here."), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_ShortDescription->Wrap( -1 );
	m_ShortDescription->SetToolTip( wxT("Long description of category goes here.\nMultiple lines are supported.") );
	
	topSizer->Add( m_ShortDescription, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	mainSizer->Add( topSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bottomSizer;
	bottomSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ListCtrl = new wxListCtrl( this, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxLC_AUTOARRANGE|wxLC_HRULES|wxLC_LIST|wxLC_SINGLE_SEL );
	bottomSizer->Add( m_ListCtrl, 1, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( bottomSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

AttributeCategoryPanel::~AttributeCategoryPanel()
{
}

PromptNewExistingPanel::PromptNewExistingPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 300,210 ) );
	
	wxBoxSizer* panelSizer;
	panelSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* topSizer;
	topSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Description = new wxStaticText( this, ID_DEFAULT, wxT("Description goes here. Call Wrap() for multiline."), wxDefaultPosition, wxDefaultSize, 0 );
	m_Description->Wrap( -1 );
	topSizer->Add( m_Description, 0, wxALL, 5 );
	
	panelSizer->Add( topSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bottomSizer;
	bottomSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* radioSizer;
	radioSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* radioSizer2;
	radioSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_RadioBtnExisting = new wxRadioButton( this, ID_DEFAULT, wxT("USE_EXISTING"), wxDefaultPosition, wxDefaultSize, 0 );
	radioSizer2->Add( m_RadioBtnExisting, 0, wxALL, 5 );
	
	wxBoxSizer* pathSizer2;
	pathSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_FilePathExisting = new wxTextCtrl( this, ID_DEFAULT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_FilePathExisting->Enable( false );
	
	pathSizer2->Add( m_FilePathExisting, 1, wxALL, 5 );
	
	m_ButtonExistingFinder = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 24,24 ), wxBU_AUTODRAW );
	m_ButtonExistingFinder->Enable( false );
	
	m_ButtonExistingFinder->Enable( false );
	
	pathSizer2->Add( m_ButtonExistingFinder, 0, wxTOP, 3 );
	
	m_ButtonExisting = new wxButton( this, ID_DEFAULT, wxT("..."), wxDefaultPosition, wxSize( 24,24 ), 0 );
	m_ButtonExisting->Enable( false );
	
	pathSizer2->Add( m_ButtonExisting, 0, wxRIGHT|wxTOP, 3 );
	
	
	pathSizer2->Add( 2, 0, 0, wxEXPAND, 5 );
	
	radioSizer2->Add( pathSizer2, 1, wxEXPAND|wxLEFT, 15 );
	
	radioSizer->Add( radioSizer2, 1, wxEXPAND, 5 );
	
	wxBoxSizer* radioSizer1;
	radioSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_RadioBtnNew = new wxRadioButton( this, ID_DEFAULT, wxT("CREATE_NEW"), wxDefaultPosition, wxDefaultSize, 0 );
	radioSizer1->Add( m_RadioBtnNew, 0, wxALL, 5 );
	
	wxBoxSizer* pathSizer1;
	pathSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	m_FilePathNew = new wxTextCtrl( this, ID_DEFAULT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_FilePathNew->Enable( false );
	
	pathSizer1->Add( m_FilePathNew, 1, wxALL, 5 );
	
	radioSizer1->Add( pathSizer1, 1, wxEXPAND|wxLEFT, 15 );
	
	radioSizer->Add( radioSizer1, 1, wxBOTTOM|wxEXPAND, 5 );
	
	bottomSizer->Add( radioSizer, 0, wxEXPAND, 5 );
	
	panelSizer->Add( bottomSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( panelSizer );
	this->Layout();
}

PromptNewExistingPanel::~PromptNewExistingPanel()
{
}

SwapShadersPanel::SwapShadersPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* topSizer;
	topSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* topLeftSizer;
	topLeftSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* comboSizer;
	comboSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_StaticTextChoice = new wxStaticText( this, wxID_ANY, wxT("Default Shader Group:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticTextChoice->Wrap( -1 );
	m_StaticTextChoice->Hide();
	
	comboSizer->Add( m_StaticTextChoice, 0, wxALL, 5 );
	
	wxString m_DefaultGroupChoices[] = { wxT("None") };
	int m_DefaultGroupNChoices = sizeof( m_DefaultGroupChoices ) / sizeof( wxString );
	m_DefaultGroup = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_DefaultGroupNChoices, m_DefaultGroupChoices, 0 );
	m_DefaultGroup->SetSelection( 0 );
	m_DefaultGroup->Hide();
	
	comboSizer->Add( m_DefaultGroup, 2, wxALL, 5 );
	
	m_StaticTextInfo = new wxStaticText( this, wxID_ANY, wxT("Use the buttons below to create and remove Shader Groups from this asset."), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticTextInfo->Wrap( 350 );
	comboSizer->Add( m_StaticTextInfo, 0, wxALL, 5 );
	
	topLeftSizer->Add( comboSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* topButtonSizer;
	topButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_ButtonNew = new wxButton( this, wxID_ANY, wxT("New"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	topButtonSizer->Add( m_ButtonNew, 0, wxALL, 5 );
	
	m_ButtonRemove = new wxButton( this, wxID_ANY, wxT("Remove"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	topButtonSizer->Add( m_ButtonRemove, 0, wxALL, 5 );
	
	topLeftSizer->Add( topButtonSizer, 0, wxEXPAND, 5 );
	
	topSizer->Add( topLeftSizer, 4, wxEXPAND, 5 );
	
	
	topSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxBoxSizer* thumbGroupSizer;
	thumbGroupSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Thumbnail = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 64,64 ), wxSTATIC_BORDER );
	thumbGroupSizer->Add( m_Thumbnail, 0, wxRIGHT, 5 );
	
	topSizer->Add( thumbGroupSizer, 0, wxEXPAND, 5 );
	
	mainSizer->Add( topSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* gridSizer;
	gridSizer = new wxBoxSizer( wxVERTICAL );
	
	mainSizer->Add( gridSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bottomSizer;
	bottomSizer = new wxBoxSizer( wxVERTICAL );
	
	
	bottomSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_ButtonSizer = new wxStdDialogButtonSizer();
	m_ButtonSizerOK = new wxButton( this, wxID_OK );
	m_ButtonSizer->AddButton( m_ButtonSizerOK );
	m_ButtonSizerCancel = new wxButton( this, wxID_CANCEL );
	m_ButtonSizer->AddButton( m_ButtonSizerCancel );
	m_ButtonSizer->Realize();
	bottomSizer->Add( m_ButtonSizer, 0, wxEXPAND, 5 );
	
	mainSizer->Add( bottomSizer, 0, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

SwapShadersPanel::~SwapShadersPanel()
{
}
