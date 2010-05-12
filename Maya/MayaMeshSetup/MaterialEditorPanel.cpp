///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "UIToolKit/AutoCompleteComboBox.h"

#include "MaterialEditorPanel.h"

///////////////////////////////////////////////////////////////////////////
using namespace Maya;

MaterialEditorPanel::MaterialEditorPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 525,375 ) );
	
	wxBoxSizer* sizerMain;
	sizerMain = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* sizerTop;
	sizerTop = new wxBoxSizer( wxHORIZONTAL );
	
	m_Tabs = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_AllTab = new wxPanel( m_Tabs, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* sizerTopSettings;
	sizerTopSettings = new wxBoxSizer( wxVERTICAL );
	
	m_Heading1 = new wxStaticText( m_AllTab, wxID_ANY, wxT("Settings"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Heading1->Wrap( -1 );
	m_Heading1->SetFont( wxFont( 10, 70, 90, 92, false, wxT("Arial") ) );
	
	sizerTopSettings->Add( m_Heading1, 0, wxALL, 5 );
	
	wxBoxSizer* sizerCombos;
	sizerCombos = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* sizerCombo1;
	sizerCombo1 = new wxBoxSizer( wxVERTICAL );
	
	m_StaticVisual = new wxStaticText( m_AllTab, wxID_ANY, wxT("Visual"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticVisual->Wrap( -1 );
	sizerCombo1->Add( m_StaticVisual, 0, wxLEFT, 5 );
	
	m_ComboVisual = new UIToolKit::AutoCompleteComboBox( m_AllTab, wxID_ANY, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	sizerCombo1->Add( m_ComboVisual, 1, wxALL|wxEXPAND, 5 );
	
	sizerCombos->Add( sizerCombo1, 1, wxEXPAND, 5 );
	
	wxBoxSizer* sizerCombo2;
	sizerCombo2 = new wxBoxSizer( wxVERTICAL );
	
	m_StaticAcoustic = new wxStaticText( m_AllTab, wxID_ANY, wxT("Acoustic"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticAcoustic->Wrap( -1 );
	sizerCombo2->Add( m_StaticAcoustic, 0, wxLEFT, 5 );
	
	m_ComboAcoustic = new UIToolKit::AutoCompleteComboBox( m_AllTab, wxID_ANY, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	sizerCombo2->Add( m_ComboAcoustic, 1, wxALL|wxEXPAND, 5 );
	
	sizerCombos->Add( sizerCombo2, 1, wxEXPAND, 5 );
	
	wxBoxSizer* sizerCombo3;
	sizerCombo3 = new wxBoxSizer( wxVERTICAL );
	
	m_StaticPhysics = new wxStaticText( m_AllTab, wxID_ANY, wxT("Physics"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticPhysics->Wrap( -1 );
	sizerCombo3->Add( m_StaticPhysics, 0, wxLEFT, 5 );
	
	m_ComboPhysics = new UIToolKit::AutoCompleteComboBox( m_AllTab, wxID_ANY, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	sizerCombo3->Add( m_ComboPhysics, 1, wxALL|wxEXPAND, 5 );
	
	sizerCombos->Add( sizerCombo3, 1, wxEXPAND, 5 );
	
	sizerTopSettings->Add( sizerCombos, 0, wxEXPAND, 5 );
	
	bSizer19->Add( sizerTopSettings, 0, wxEXPAND, 5 );
	
	m_Line = new wxStaticLine( m_AllTab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer19->Add( m_Line, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* sizerMid;
	sizerMid = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* sizerMidHeading;
	sizerMidHeading = new wxBoxSizer( wxVERTICAL );
	
	m_Heading2 = new wxStaticText( m_AllTab, wxID_ANY, wxT("Display"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Heading2->Wrap( -1 );
	m_Heading2->SetFont( wxFont( 10, 70, 90, 92, false, wxT("Arial") ) );
	
	sizerMidHeading->Add( m_Heading2, 0, wxALL, 5 );
	
	sizerMid->Add( sizerMidHeading, 0, wxEXPAND, 5 );
	
	wxBoxSizer* sizerRadio;
	sizerRadio = new wxBoxSizer( wxHORIZONTAL );
	
	m_RadioOff = new wxRadioButton( m_AllTab, wxID_ANY, wxT("Off"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
	m_RadioOff->SetValue( true ); 
	m_RadioOff->SetToolTip( wxT("Turn off material display.") );
	
	sizerRadio->Add( m_RadioOff, 1, wxALL, 5 );
	
	m_RadioVisual = new wxRadioButton( m_AllTab, wxID_ANY, wxT("Visual"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
	m_RadioVisual->SetToolTip( wxT("Show objects with visual material settings.") );
	
	sizerRadio->Add( m_RadioVisual, 1, wxALL, 5 );
	
	m_RadioAcoustic = new wxRadioButton( m_AllTab, wxID_ANY, wxT("Acoustic"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
	m_RadioAcoustic->SetToolTip( wxT("Show objects with acoustic material settings.") );
	
	sizerRadio->Add( m_RadioAcoustic, 1, wxALL, 5 );
	
	m_RadioPhysics = new wxRadioButton( m_AllTab, wxID_ANY, wxT("Physics"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
	m_RadioPhysics->SetToolTip( wxT("Show objects with physical material settings.") );
	
	sizerRadio->Add( m_RadioPhysics, 1, wxALL, 5 );
	
	sizerMid->Add( sizerRadio, 0, wxEXPAND, 5 );
	
	wxString m_radioBox2Choices[] = { wxT("Off"), wxT("Visual"), wxT("Acoustic"), wxT("Physics") };
	int m_radioBox2NChoices = sizeof( m_radioBox2Choices ) / sizeof( wxString );
	m_radioBox2 = new wxRadioBox( m_AllTab, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_radioBox2NChoices, m_radioBox2Choices, 1, wxRA_SPECIFY_ROWS );
	m_radioBox2->SetSelection( 0 );
	m_radioBox2->Hide();
	
	sizerMid->Add( m_radioBox2, 0, wxEXPAND|wxLEFT|wxRIGHT, 5 );
	
	bSizer19->Add( sizerMid, 0, wxEXPAND, 5 );
	
	m_AllTab->SetSizer( bSizer19 );
	m_AllTab->Layout();
	bSizer19->Fit( m_AllTab );
	m_Tabs->AddPage( m_AllTab, wxT("All"), true );
	m_VisualTab = new wxPanel( m_Tabs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer191;
	bSizer191 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* sizerTop1;
	sizerTop1 = new wxBoxSizer( wxVERTICAL );
	
	m_Heading11 = new wxStaticText( m_VisualTab, wxID_ANY, wxT("Visual Setting"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Heading11->Wrap( -1 );
	m_Heading11->SetFont( wxFont( 10, 70, 90, 92, false, wxT("Arial") ) );
	
	sizerTop1->Add( m_Heading11, 0, wxALL, 5 );
	
	wxBoxSizer* sizerCombos1;
	sizerCombos1 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* sizerCombo11;
	sizerCombo11 = new wxBoxSizer( wxVERTICAL );
	
	m_StaticVisual1 = new wxStaticText( m_VisualTab, wxID_ANY, wxT("Visual"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticVisual1->Wrap( -1 );
	m_StaticVisual1->Hide();
	
	sizerCombo11->Add( m_StaticVisual1, 0, wxLEFT, 5 );
	
	m_VisualTabCombo = new UIToolKit::AutoCompleteComboBox( m_VisualTab, wxID_ANY, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	sizerCombo11->Add( m_VisualTabCombo, 1, wxALL, 5 );
	
	sizerCombos1->Add( sizerCombo11, 1, wxEXPAND, 5 );
	
	sizerTop1->Add( sizerCombos1, 0, wxEXPAND, 5 );
	
	bSizer191->Add( sizerTop1, 0, wxEXPAND, 5 );
	
	m_Line1 = new wxStaticLine( m_VisualTab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer191->Add( m_Line1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* sizerMid1;
	sizerMid1 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* sizerMidHeading1;
	sizerMidHeading1 = new wxBoxSizer( wxVERTICAL );
	
	m_Heading21 = new wxStaticText( m_VisualTab, wxID_ANY, wxT("Display"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Heading21->Wrap( -1 );
	m_Heading21->SetFont( wxFont( 10, 70, 90, 92, false, wxT("Arial") ) );
	
	sizerMidHeading1->Add( m_Heading21, 0, wxALL, 5 );
	
	sizerMid1->Add( sizerMidHeading1, 0, wxEXPAND, 5 );
	
	wxBoxSizer* sizerRadio1;
	sizerRadio1 = new wxBoxSizer( wxVERTICAL );
	
	m_RadioVisualTabOff = new wxRadioButton( m_VisualTab, wxID_ANY, wxT("Off"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
	m_RadioVisualTabOff->SetValue( true ); 
	m_RadioVisualTabOff->SetToolTip( wxT("Turn off material display.") );
	
	sizerRadio1->Add( m_RadioVisualTabOff, 1, wxALL, 5 );
	
	m_RadioVisualTabOn = new wxRadioButton( m_VisualTab, wxID_ANY, wxT("On"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
	m_RadioVisualTabOn->SetToolTip( wxT("Show objects with visual material settings.") );
	
	sizerRadio1->Add( m_RadioVisualTabOn, 1, wxALL, 5 );
	
	sizerMid1->Add( sizerRadio1, 0, wxEXPAND, 5 );
	
	wxString m_radioBox21Choices[] = { wxT("Off"), wxT("Visual"), wxT("Acoustic"), wxT("Physics") };
	int m_radioBox21NChoices = sizeof( m_radioBox21Choices ) / sizeof( wxString );
	m_radioBox21 = new wxRadioBox( m_VisualTab, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_radioBox21NChoices, m_radioBox21Choices, 1, wxRA_SPECIFY_ROWS );
	m_radioBox21->SetSelection( 0 );
	m_radioBox21->Hide();
	
	sizerMid1->Add( m_radioBox21, 0, wxEXPAND|wxLEFT|wxRIGHT, 5 );
	
	bSizer191->Add( sizerMid1, 0, wxEXPAND, 5 );
	
	m_VisualTab->SetSizer( bSizer191 );
	m_VisualTab->Layout();
	bSizer191->Fit( m_VisualTab );
	m_Tabs->AddPage( m_VisualTab, wxT("Visual"), false );
	m_AcousticTab = new wxPanel( m_Tabs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer1911;
	bSizer1911 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* sizerTop11;
	sizerTop11 = new wxBoxSizer( wxVERTICAL );
	
	m_Heading111 = new wxStaticText( m_AcousticTab, wxID_ANY, wxT("Acoustic Setting"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Heading111->Wrap( -1 );
	m_Heading111->SetFont( wxFont( 10, 70, 90, 92, false, wxT("Arial") ) );
	
	sizerTop11->Add( m_Heading111, 0, wxALL, 5 );
	
	wxBoxSizer* sizerCombos11;
	sizerCombos11 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* sizerCombo111;
	sizerCombo111 = new wxBoxSizer( wxVERTICAL );
	
	m_StaticVisual11 = new wxStaticText( m_AcousticTab, wxID_ANY, wxT("Visual"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticVisual11->Wrap( -1 );
	m_StaticVisual11->Hide();
	
	sizerCombo111->Add( m_StaticVisual11, 0, wxLEFT, 5 );
	
	m_AcousticTabCombo = new UIToolKit::AutoCompleteComboBox( m_AcousticTab, wxID_ANY, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	sizerCombo111->Add( m_AcousticTabCombo, 1, wxALL, 5 );
	
	sizerCombos11->Add( sizerCombo111, 1, wxEXPAND, 5 );
	
	sizerTop11->Add( sizerCombos11, 0, wxEXPAND, 5 );
	
	bSizer1911->Add( sizerTop11, 0, wxEXPAND, 5 );
	
	m_Line11 = new wxStaticLine( m_AcousticTab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1911->Add( m_Line11, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* sizerMid11;
	sizerMid11 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* sizerMidHeading11;
	sizerMidHeading11 = new wxBoxSizer( wxVERTICAL );
	
	m_Heading211 = new wxStaticText( m_AcousticTab, wxID_ANY, wxT("Display"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Heading211->Wrap( -1 );
	m_Heading211->SetFont( wxFont( 10, 70, 90, 92, false, wxT("Arial") ) );
	
	sizerMidHeading11->Add( m_Heading211, 0, wxALL, 5 );
	
	sizerMid11->Add( sizerMidHeading11, 0, wxEXPAND, 5 );
	
	wxBoxSizer* sizerRadio11;
	sizerRadio11 = new wxBoxSizer( wxVERTICAL );
	
	m_RadioAcousticTabOff = new wxRadioButton( m_AcousticTab, wxID_ANY, wxT("Off"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
	m_RadioAcousticTabOff->SetValue( true ); 
	m_RadioAcousticTabOff->SetToolTip( wxT("Turn off material display.") );
	
	sizerRadio11->Add( m_RadioAcousticTabOff, 1, wxALL, 5 );
	
	m_RadioAcousticTabOn = new wxRadioButton( m_AcousticTab, wxID_ANY, wxT("On"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
	m_RadioAcousticTabOn->SetToolTip( wxT("Show objects with visual material settings.") );
	
	sizerRadio11->Add( m_RadioAcousticTabOn, 1, wxALL, 5 );
	
	sizerMid11->Add( sizerRadio11, 0, wxEXPAND, 5 );
	
	wxString m_radioBox211Choices[] = { wxT("Off"), wxT("Visual"), wxT("Acoustic"), wxT("Physics") };
	int m_radioBox211NChoices = sizeof( m_radioBox211Choices ) / sizeof( wxString );
	m_radioBox211 = new wxRadioBox( m_AcousticTab, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_radioBox211NChoices, m_radioBox211Choices, 1, wxRA_SPECIFY_ROWS );
	m_radioBox211->SetSelection( 0 );
	m_radioBox211->Hide();
	
	sizerMid11->Add( m_radioBox211, 0, wxEXPAND|wxLEFT|wxRIGHT, 5 );
	
	bSizer1911->Add( sizerMid11, 0, wxEXPAND, 5 );
	
	m_AcousticTab->SetSizer( bSizer1911 );
	m_AcousticTab->Layout();
	bSizer1911->Fit( m_AcousticTab );
	m_Tabs->AddPage( m_AcousticTab, wxT("Acoustic"), false );
	m_PhysicsTab = new wxPanel( m_Tabs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer1912;
	bSizer1912 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* sizerTop12;
	sizerTop12 = new wxBoxSizer( wxVERTICAL );
	
	m_Heading112 = new wxStaticText( m_PhysicsTab, wxID_ANY, wxT("Physics Setting"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Heading112->Wrap( -1 );
	m_Heading112->SetFont( wxFont( 10, 70, 90, 92, false, wxT("Arial") ) );
	
	sizerTop12->Add( m_Heading112, 0, wxALL, 5 );
	
	wxBoxSizer* sizerCombos12;
	sizerCombos12 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* sizerCombo112;
	sizerCombo112 = new wxBoxSizer( wxVERTICAL );
	
	m_StaticVisual12 = new wxStaticText( m_PhysicsTab, wxID_ANY, wxT("Visual"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticVisual12->Wrap( -1 );
	m_StaticVisual12->Hide();
	
	sizerCombo112->Add( m_StaticVisual12, 0, wxLEFT, 5 );
	
	m_PhysicsTabCombo = new UIToolKit::AutoCompleteComboBox( m_PhysicsTab, wxID_ANY, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	sizerCombo112->Add( m_PhysicsTabCombo, 1, wxALL, 5 );
	
	sizerCombos12->Add( sizerCombo112, 1, wxEXPAND, 5 );
	
	sizerTop12->Add( sizerCombos12, 0, wxEXPAND, 5 );
	
	bSizer1912->Add( sizerTop12, 0, wxEXPAND, 5 );
	
	m_Line12 = new wxStaticLine( m_PhysicsTab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1912->Add( m_Line12, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* sizerMid12;
	sizerMid12 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* sizerMidHeading12;
	sizerMidHeading12 = new wxBoxSizer( wxVERTICAL );
	
	m_Heading212 = new wxStaticText( m_PhysicsTab, wxID_ANY, wxT("Display"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Heading212->Wrap( -1 );
	m_Heading212->SetFont( wxFont( 10, 70, 90, 92, false, wxT("Arial") ) );
	
	sizerMidHeading12->Add( m_Heading212, 0, wxALL, 5 );
	
	sizerMid12->Add( sizerMidHeading12, 0, wxEXPAND, 5 );
	
	wxBoxSizer* sizerRadio12;
	sizerRadio12 = new wxBoxSizer( wxVERTICAL );
	
	m_RadioPhysicsTabOff = new wxRadioButton( m_PhysicsTab, wxID_ANY, wxT("Off"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
	m_RadioPhysicsTabOff->SetValue( true ); 
	m_RadioPhysicsTabOff->SetToolTip( wxT("Turn off material display.") );
	
	sizerRadio12->Add( m_RadioPhysicsTabOff, 1, wxALL, 5 );
	
	m_RadioPhysicsTabOn = new wxRadioButton( m_PhysicsTab, wxID_ANY, wxT("On"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
	m_RadioPhysicsTabOn->SetToolTip( wxT("Show objects with visual material settings.") );
	
	sizerRadio12->Add( m_RadioPhysicsTabOn, 1, wxALL, 5 );
	
	sizerMid12->Add( sizerRadio12, 0, wxEXPAND, 5 );
	
	wxString m_radioBox212Choices[] = { wxT("Off"), wxT("Visual"), wxT("Acoustic"), wxT("Physics") };
	int m_radioBox212NChoices = sizeof( m_radioBox212Choices ) / sizeof( wxString );
	m_radioBox212 = new wxRadioBox( m_PhysicsTab, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_radioBox212NChoices, m_radioBox212Choices, 1, wxRA_SPECIFY_ROWS );
	m_radioBox212->SetSelection( 0 );
	m_radioBox212->Hide();
	
	sizerMid12->Add( m_radioBox212, 0, wxEXPAND|wxLEFT|wxRIGHT, 5 );
	
	bSizer1912->Add( sizerMid12, 0, wxEXPAND, 5 );
	
	m_PhysicsTab->SetSizer( bSizer1912 );
	m_PhysicsTab->Layout();
	bSizer1912->Fit( m_PhysicsTab );
	m_Tabs->AddPage( m_PhysicsTab, wxT("Physics"), false );
	
	sizerTop->Add( m_Tabs, 1, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* sizerButtons;
	sizerButtons = new wxBoxSizer( wxVERTICAL );
	
	m_ButtonApply = new wxButton( this, wxID_ANY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ButtonApply->SetToolTip( wxT("Apply the material settings to the currently selected objects.") );
	
	sizerButtons->Add( m_ButtonApply, 0, wxALL, 5 );
	
	m_ButtonSelect = new wxButton( this, wxID_ANY, wxT("Select"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ButtonSelect->SetToolTip( wxT("Select all objects that have the material settings specified above.") );
	
	sizerButtons->Add( m_ButtonSelect, 0, wxALL, 5 );
	
	m_ButtonClear = new wxButton( this, wxID_ANY, wxT("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ButtonClear->SetToolTip( wxT("Clear all material settings from the currently selected objects.") );
	
	sizerButtons->Add( m_ButtonClear, 0, wxALL, 5 );
	
	sizerTop->Add( sizerButtons, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	sizerMain->Add( sizerTop, 0, wxEXPAND, 5 );
	
	wxBoxSizer* sizerBottom;
	sizerBottom = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* sizerBottomInner;
	sizerBottomInner = new wxBoxSizer( wxVERTICAL );
	
	m_KeyScrollWindow = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxSTATIC_BORDER|wxVSCROLL );
	m_KeyScrollWindow->SetScrollRate( 5, 5 );
	wxFlexGridSizer* sizerKeyGrid;
	sizerKeyGrid = new wxFlexGridSizer( 0, 2, 0, 0 );
	sizerKeyGrid->AddGrowableCol( 0 );
	sizerKeyGrid->AddGrowableCol( 1 );
	sizerKeyGrid->SetFlexibleDirection( wxHORIZONTAL );
	sizerKeyGrid->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_NONE );
	
	wxBoxSizer* sizerRow;
	sizerRow = new wxBoxSizer( wxVERTICAL );
	
	m_TextDisplayOff = new wxStaticText( m_KeyScrollWindow, wxID_ANY, wxT("Display is turned off."), wxDefaultPosition, wxDefaultSize, 0 );
	m_TextDisplayOff->Wrap( -1 );
	sizerRow->Add( m_TextDisplayOff, 0, wxALL, 5 );
	
	sizerKeyGrid->Add( sizerRow, 1, wxEXPAND, 5 );
	
	m_KeyScrollWindow->SetSizer( sizerKeyGrid );
	m_KeyScrollWindow->Layout();
	sizerKeyGrid->Fit( m_KeyScrollWindow );
	sizerBottomInner->Add( m_KeyScrollWindow, 1, wxEXPAND, 5 );
	
	sizerBottom->Add( sizerBottomInner, 1, wxEXPAND, 5 );
	
	sizerMain->Add( sizerBottom, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( sizerMain );
	this->Layout();
}

MaterialEditorPanel::~MaterialEditorPanel()
{
}

TempMaterialEditorFrame::TempMaterialEditorFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Panel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer40;
	bSizer40 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* sizerRow;
	sizerRow = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* sizerPanel;
	sizerPanel = new wxBoxSizer( wxVERTICAL );
	
	colorBox = new wxPanel( m_Panel, wxID_ANY, wxDefaultPosition, wxSize( 20,20 ), wxSIMPLE_BORDER|wxTAB_TRAVERSAL );
	colorBox->SetBackgroundColour( wxColour( 0, 255, 0 ) );
	colorBox->SetMinSize( wxSize( 20,20 ) );
	colorBox->SetMaxSize( wxSize( 20,20 ) );
	
	sizerPanel->Add( colorBox, 1, wxLEFT|wxTOP, 5 );
	
	sizerRow->Add( sizerPanel, 0, wxEXPAND, 5 );
	
	rowName = new wxStaticText( m_Panel, wxID_ANY, wxT("Name"), wxDefaultPosition, wxDefaultSize, 0 );
	rowName->Wrap( -1 );
	sizerRow->Add( rowName, 1, wxALL|wxALIGN_BOTTOM, 5 );
	
	bSizer40->Add( sizerRow, 0, wxEXPAND, 5 );
	
	m_Panel->SetSizer( bSizer40 );
	m_Panel->Layout();
	bSizer40->Fit( m_Panel );
	mainSizer->Add( m_Panel, 1, wxEXPAND | wxALL, 0 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	m_StatusBar = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
}

TempMaterialEditorFrame::~TempMaterialEditorFrame()
{
}
