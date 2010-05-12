///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

stdafx.h

#include "CollisionModePanel.h"

///////////////////////////////////////////////////////////////////////////
using namespace Maya;

CollisionModePanel::CollisionModePanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 286,130 ) );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ScrollWindow = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL );
	m_ScrollWindow->SetScrollRate( 5, 5 );
	wxBoxSizer* scrollSizer;
	scrollSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* topSizer;
	topSizer = new wxBoxSizer( wxVERTICAL );
	
	m_TitleSettings = new wxStaticText( m_ScrollWindow, wxID_ANY, wxT("Settings"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TitleSettings->Wrap( -1 );
	m_TitleSettings->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	topSizer->Add( m_TitleSettings, 0, wxALL, 5 );
	
	wxBoxSizer* topInnerSizer;
	topInnerSizer = new wxBoxSizer( wxVERTICAL );
	
	wxGridSizer* checkboxSizer;
	checkboxSizer = new wxGridSizer( 2, 2, 0, 0 );
	
	m_CheckCameraIgnore = new wxCheckBox( m_ScrollWindow, wxID_ANY, wxT("Camera Ignore"), wxDefaultPosition, wxDefaultSize, 0 );
	
	m_CheckCameraIgnore->SetMinSize( wxSize( 100,-1 ) );
	
	checkboxSizer->Add( m_CheckCameraIgnore, 0, wxALL, 5 );
	
	m_CheckImpassable = new wxCheckBox( m_ScrollWindow, wxID_ANY, wxT("Impassable"), wxDefaultPosition, wxDefaultSize, 0 );
	
	m_CheckImpassable->SetMinSize( wxSize( 100,-1 ) );
	
	checkboxSizer->Add( m_CheckImpassable, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	m_CheckNonTraversable = new wxCheckBox( m_ScrollWindow, wxID_ANY, wxT("Non Traversable"), wxDefaultPosition, wxDefaultSize, 0 );
	
	m_CheckNonTraversable->SetMinSize( wxSize( 100,-1 ) );
	
	checkboxSizer->Add( m_CheckNonTraversable, 0, wxALL, 5 );
	
	m_CheckTransparent = new wxCheckBox( m_ScrollWindow, wxID_ANY, wxT("Transparent"), wxDefaultPosition, wxDefaultSize, 0 );
	
	m_CheckTransparent->SetMinSize( wxSize( 100,-1 ) );
	
	checkboxSizer->Add( m_CheckTransparent, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	topInnerSizer->Add( checkboxSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	
	buttonSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_ButtonApply = new wxButton( m_ScrollWindow, wxID_ANY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_ButtonApply, 0, wxALL, 5 );
	
	m_ButtonClear = new wxButton( m_ScrollWindow, wxID_ANY, wxT("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_ButtonClear, 0, wxALL, 5 );
	
	
	buttonSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	topInnerSizer->Add( buttonSizer, 0, wxEXPAND, 5 );
	
	topSizer->Add( topInnerSizer, 0, wxEXPAND, 5 );
	
	scrollSizer->Add( topSizer, 0, wxEXPAND, 5 );
	
	m_Separator = new wxStaticLine( m_ScrollWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	scrollSizer->Add( m_Separator, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bottomSizer;
	bottomSizer = new wxBoxSizer( wxVERTICAL );
	
	m_TitleDisplay = new wxStaticText( m_ScrollWindow, wxID_ANY, wxT("Display"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TitleDisplay->Wrap( -1 );
	m_TitleDisplay->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bottomSizer->Add( m_TitleDisplay, 0, wxALL, 5 );
	
	wxBoxSizer* bottomInnerSizer;
	bottomInnerSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* sizerOff;
	sizerOff = new wxBoxSizer( wxHORIZONTAL );
	
	m_ColorSwatchEmpty = new wxPanel( m_ScrollWindow, wxID_ANY, wxDefaultPosition, wxSize( 20,20 ), wxTAB_TRAVERSAL );
	m_ColorSwatchEmpty->SetMinSize( wxSize( 20,20 ) );
	m_ColorSwatchEmpty->SetMaxSize( wxSize( 20,20 ) );
	
	sizerOff->Add( m_ColorSwatchEmpty, 0, wxLEFT|wxTOP, 5 );
	
	m_RadioOff = new wxRadioButton( m_ScrollWindow, wxID_ANY, wxT("Off"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
	m_RadioOff->SetMinSize( wxSize( 100,-1 ) );
	
	sizerOff->Add( m_RadioOff, 0, wxLEFT|wxTOP|wxALIGN_CENTER_VERTICAL, 5 );
	
	bottomInnerSizer->Add( sizerOff, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* sizer1;
	sizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* sizerCamIg;
	sizerCamIg = new wxBoxSizer( wxHORIZONTAL );
	
	m_ColorSwatchCamIgnore = new wxPanel( m_ScrollWindow, wxID_ANY, wxDefaultPosition, wxSize( 20,20 ), wxSIMPLE_BORDER );
	m_ColorSwatchCamIgnore->SetBackgroundColour( wxColour( 255, 0, 0 ) );
	m_ColorSwatchCamIgnore->SetMinSize( wxSize( 20,20 ) );
	m_ColorSwatchCamIgnore->SetMaxSize( wxSize( 20,20 ) );
	
	sizerCamIg->Add( m_ColorSwatchCamIgnore, 0, wxLEFT|wxTOP, 5 );
	
	m_RadioCameraIgnore = new wxRadioButton( m_ScrollWindow, wxID_ANY, wxT("Camera Ignore"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
	m_RadioCameraIgnore->SetMinSize( wxSize( 100,-1 ) );
	
	sizerCamIg->Add( m_RadioCameraIgnore, 0, wxLEFT|wxTOP|wxALIGN_CENTER_VERTICAL, 5 );
	
	sizer1->Add( sizerCamIg, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	
	sizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxBoxSizer* sizerImpass;
	sizerImpass = new wxBoxSizer( wxHORIZONTAL );
	
	m_ColorSwatchImpassable = new wxPanel( m_ScrollWindow, wxID_ANY, wxDefaultPosition, wxSize( 20,20 ), wxSIMPLE_BORDER|wxTAB_TRAVERSAL );
	m_ColorSwatchImpassable->SetBackgroundColour( wxColour( 0, 255, 0 ) );
	m_ColorSwatchImpassable->SetMinSize( wxSize( 20,20 ) );
	m_ColorSwatchImpassable->SetMaxSize( wxSize( 20,20 ) );
	
	sizerImpass->Add( m_ColorSwatchImpassable, 0, wxLEFT|wxTOP, 5 );
	
	m_RadioImpassable = new wxRadioButton( m_ScrollWindow, wxID_ANY, wxT("Impassable"), wxDefaultPosition, wxSize( 100,-1 ), wxRB_SINGLE );
	sizerImpass->Add( m_RadioImpassable, 0, wxLEFT|wxTOP|wxALIGN_CENTER_VERTICAL, 5 );
	
	sizer1->Add( sizerImpass, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	bottomInnerSizer->Add( sizer1, 1, wxEXPAND, 5 );
	
	wxBoxSizer* sizer2;
	sizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* sizerNonTrav;
	sizerNonTrav = new wxBoxSizer( wxHORIZONTAL );
	
	m_ColorSwatchNonTrav = new wxPanel( m_ScrollWindow, wxID_ANY, wxDefaultPosition, wxSize( 20,2 ), wxSIMPLE_BORDER|wxTAB_TRAVERSAL );
	m_ColorSwatchNonTrav->SetBackgroundColour( wxColour( 255, 0, 255 ) );
	m_ColorSwatchNonTrav->SetMinSize( wxSize( 20,20 ) );
	m_ColorSwatchNonTrav->SetMaxSize( wxSize( 20,20 ) );
	
	sizerNonTrav->Add( m_ColorSwatchNonTrav, 0, wxLEFT|wxTOP, 5 );
	
	m_RadioNonTraversable = new wxRadioButton( m_ScrollWindow, wxID_ANY, wxT("Non Traversable"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
	m_RadioNonTraversable->SetMinSize( wxSize( 100,-1 ) );
	
	sizerNonTrav->Add( m_RadioNonTraversable, 0, wxLEFT|wxTOP|wxALIGN_CENTER_VERTICAL, 5 );
	
	sizer2->Add( sizerNonTrav, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	
	sizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxBoxSizer* sizerTrans;
	sizerTrans = new wxBoxSizer( wxHORIZONTAL );
	
	m_ColorSwatchTransparent = new wxPanel( m_ScrollWindow, wxID_ANY, wxDefaultPosition, wxSize( 20,20 ), wxSIMPLE_BORDER|wxTAB_TRAVERSAL );
	m_ColorSwatchTransparent->SetBackgroundColour( wxColour( 0, 0, 255 ) );
	m_ColorSwatchTransparent->SetMinSize( wxSize( 20,20 ) );
	m_ColorSwatchTransparent->SetMaxSize( wxSize( 20,20 ) );
	
	sizerTrans->Add( m_ColorSwatchTransparent, 0, wxLEFT|wxTOP, 5 );
	
	m_RadioTransparent = new wxRadioButton( m_ScrollWindow, wxID_ANY, wxT("Transparent"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
	m_RadioTransparent->SetMinSize( wxSize( 100,-1 ) );
	
	sizerTrans->Add( m_RadioTransparent, 0, wxLEFT|wxTOP|wxALIGN_CENTER_VERTICAL, 5 );
	
	sizer2->Add( sizerTrans, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	bottomInnerSizer->Add( sizer2, 1, wxEXPAND, 5 );
	
	bottomSizer->Add( bottomInnerSizer, 0, wxEXPAND, 5 );
	
	scrollSizer->Add( bottomSizer, 1, wxEXPAND, 5 );
	
	m_ScrollWindow->SetSizer( scrollSizer );
	m_ScrollWindow->Layout();
	scrollSizer->Fit( m_ScrollWindow );
	mainSizer->Add( m_ScrollWindow, 1, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

CollisionModePanel::~CollisionModePanel()
{
}
