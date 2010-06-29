///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Precompile.h"

#include "SearchGenerated.h"

///////////////////////////////////////////////////////////////////////////
using namespace Luna;

SearchPanel::SearchPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ScrollWindow = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_ScrollWindow->SetScrollRate( 5, 5 );
	wxBoxSizer* scrollSizer;
	scrollSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Tabs = new wxNotebook( m_ScrollWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, wxT("Search") );
	m_TextPanel = new wxPanel( m_Tabs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* searchOuterSizer;
	searchOuterSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* textSearchSizer;
	textSearchSizer = new wxBoxSizer( wxVERTICAL );
	
	m_StaticName = new wxStaticText( m_TextPanel, wxID_ANY, wxT("Text search (use * for wildcard)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticName->Wrap( -1 );
	textSearchSizer->Add( m_StaticName, 0, wxALL, 5 );
	
	wxBoxSizer* textSearchInnerSizer;
	textSearchInnerSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxString m_SearchOptionChoices[] = { wxT("Name"), wxT("ID"), wxT("Entity Class"), wxT("Runtime Class") };
	int m_SearchOptionNChoices = sizeof( m_SearchOptionChoices ) / sizeof( wxString );
	m_SearchOption = new wxChoice( m_TextPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_SearchOptionNChoices, m_SearchOptionChoices, 0 );
	m_SearchOption->SetSelection( 0 );
	textSearchInnerSizer->Add( m_SearchOption, 0, wxALL, 5 );
	
	m_SearchText = new wxTextCtrl( m_TextPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	textSearchInnerSizer->Add( m_SearchText, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	textSearchSizer->Add( textSearchInnerSizer, 0, wxEXPAND, 5 );
	
	searchOuterSizer->Add( textSearchSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* engTypeSizer;
	engTypeSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_StaticAssetType = new wxStaticText( m_TextPanel, wxID_ANY, wxT("Engine Type"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticAssetType->Wrap( -1 );
	engTypeSizer->Add( m_StaticAssetType, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	
	engTypeSizer->Add( 38, 0, 0, wxEXPAND, 5 );
	
	m_AssetType = new wxComboBox( m_TextPanel, wxID_ANY, wxT("Any"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_AssetType->Append( wxT("Any") );
	m_AssetType->Append( wxT("Moby") );
	m_AssetType->Append( wxT("Tie") );
	m_AssetType->Append( wxT("Shrub") );
	m_AssetType->Append( wxT("Ufrag") );
	engTypeSizer->Add( m_AssetType, 1, wxALL|wxEXPAND, 5 );
	
	searchOuterSizer->Add( engTypeSizer, 0, wxEXPAND, 5 );
	
	m_SearchAllZones = new wxCheckBox( m_TextPanel, wxID_ANY, wxT("Search all loaded zones."), wxDefaultPosition, wxDefaultSize, 0 );
	m_SearchAllZones->SetValue(true);
	
	m_SearchAllZones->SetToolTip( wxT("Uncheck to only search the currently selected zone.") );
	
	searchOuterSizer->Add( m_SearchAllZones, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_TextPanel->SetSizer( searchOuterSizer );
	m_TextPanel->Layout();
	searchOuterSizer->Fit( m_TextPanel );
	m_Tabs->AddPage( m_TextPanel, wxT("Search"), true );
	m_AttributesPanel = new wxPanel( m_Tabs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* attribsSizer;
	attribsSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* attribLabelSizer;
	attribLabelSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText8 = new wxStaticText( m_AttributesPanel, wxID_ANY, wxT("Select the attributes that each object should have."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	attribLabelSizer->Add( m_staticText8, 0, wxALL, 5 );
	
	attribsSizer->Add( attribLabelSizer, 0, wxEXPAND, 5 );
	
	wxString m_AttributesChoices[] = { wxT("Collision"), wxT("Post Effects Depth"), wxT("More"), wxT("More"), wxT("More"), wxT("More"), wxT("More"), wxT("More"), wxT("More"), wxT("More"), wxT("More"), wxT("More"), wxT("More"), wxT("More"), wxT("More"), wxT("More"), wxT("More"), wxT("More"), wxT("More"), wxT("More"), wxT("More") };
	int m_AttributesNChoices = sizeof( m_AttributesChoices ) / sizeof( wxString );
	m_Attributes = new wxCheckListBox( m_AttributesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_AttributesNChoices, m_AttributesChoices, wxLB_HSCROLL );
	m_Attributes->SetMinSize( wxSize( -1,100 ) );
	m_Attributes->SetMaxSize( wxSize( -1,100 ) );
	
	attribsSizer->Add( m_Attributes, 1, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 5 );
	
	m_AttributesPanel->SetSizer( attribsSizer );
	m_AttributesPanel->Layout();
	attribsSizer->Fit( m_AttributesPanel );
	m_Tabs->AddPage( m_AttributesPanel, wxT("Attributes"), false );
	m_BoundsPanel = new wxPanel( m_Tabs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* boundsSizer;
	boundsSizer = new wxBoxSizer( wxVERTICAL );
	
	m_StaticBounds = new wxStaticText( m_BoundsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticBounds->Wrap( -1 );
	boundsSizer->Add( m_StaticBounds, 0, wxALL, 5 );
	
	wxBoxSizer* boundsTopSizer;
	boundsTopSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxString m_BoundsOptionChoices[] = { wxT("AABB"), wxT("OBB") };
	int m_BoundsOptionNChoices = sizeof( m_BoundsOptionChoices ) / sizeof( wxString );
	m_BoundsOption = new wxChoice( m_BoundsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_BoundsOptionNChoices, m_BoundsOptionChoices, 0 );
	m_BoundsOption->SetSelection( 0 );
	boundsTopSizer->Add( m_BoundsOption, 1, wxLEFT, 5 );
	
	wxString m_BoundsLessOrGreaterChoices[] = { wxT("<"), wxT(">") };
	int m_BoundsLessOrGreaterNChoices = sizeof( m_BoundsLessOrGreaterChoices ) / sizeof( wxString );
	m_BoundsLessOrGreater = new wxChoice( m_BoundsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_BoundsLessOrGreaterNChoices, m_BoundsLessOrGreaterChoices, 0 );
	m_BoundsLessOrGreater->SetSelection( 0 );
	m_BoundsLessOrGreater->Hide();
	
	boundsTopSizer->Add( m_BoundsLessOrGreater, 0, wxLEFT, 5 );
	
	m_staticText9 = new wxStaticText( m_BoundsPanel, wxID_ANY, wxT(">"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	boundsTopSizer->Add( m_staticText9, 0, wxALL, 5 );
	
	m_BoundsGreaterThan = new wxTextCtrl( m_BoundsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	boundsTopSizer->Add( m_BoundsGreaterThan, 1, wxLEFT|wxRIGHT, 5 );
	
	boundsSizer->Add( boundsTopSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* boundsBottomSizer;
	boundsBottomSizer = new wxBoxSizer( wxHORIZONTAL );
	
	
	boundsBottomSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText10 = new wxStaticText( m_BoundsPanel, wxID_ANY, wxT("<"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	boundsBottomSizer->Add( m_staticText10, 0, wxALL, 5 );
	
	m_BoundsLessThan = new wxTextCtrl( m_BoundsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	boundsBottomSizer->Add( m_BoundsLessThan, 1, wxLEFT|wxRIGHT, 5 );
	
	boundsSizer->Add( boundsBottomSizer, 1, wxEXPAND, 5 );
	
	m_BoundsPanel->SetSizer( boundsSizer );
	m_BoundsPanel->Layout();
	boundsSizer->Fit( m_BoundsPanel );
	m_Tabs->AddPage( m_BoundsPanel, wxT("Bounds"), false );
	
	scrollSizer->Add( m_Tabs, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* statusSizer;
	statusSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_SearchButton = new wxButton( m_ScrollWindow, wxID_ANY, wxT("Search"), wxDefaultPosition, wxDefaultSize, 0 );
	m_SearchButton->SetDefault(); 
	statusSizer->Add( m_SearchButton, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5 );
	
	m_Status = new wxStaticText( m_ScrollWindow, wxID_ANY, wxT("Status is printed here."), wxDefaultPosition, wxDefaultSize, 0 );
	m_Status->Wrap( -1 );
	statusSizer->Add( m_Status, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	scrollSizer->Add( statusSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* resultsSizer;
	resultsSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Results = new wxListCtrl( m_ScrollWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	resultsSizer->Add( m_Results, 1, wxALL|wxEXPAND, 5 );
	
	scrollSizer->Add( resultsSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bottomSizer;
	bottomSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_SelectButton = new wxButton( m_ScrollWindow, wxID_ANY, wxT("Select"), wxDefaultPosition, wxDefaultSize, 0 );
	m_SelectButton->SetToolTip( wxT("Shift click to add the selected results to the current selection") );
	
	bottomSizer->Add( m_SelectButton, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5 );
	
	scrollSizer->Add( bottomSizer, 0, wxEXPAND, 5 );
	
	m_ScrollWindow->SetSizer( scrollSizer );
	m_ScrollWindow->Layout();
	scrollSizer->Fit( m_ScrollWindow );
	mainSizer->Add( m_ScrollWindow, 1, wxEXPAND | wxALL, 0 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

SearchPanel::~SearchPanel()
{
}
