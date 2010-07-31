///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  4 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifdef WX_PRECOMP

#include "stdafx.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#else
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "ReflectMapEntryDialog.h"

///////////////////////////////////////////////////////////////////////////
using namespace Helium::Inspect;

ReflectMapEntryDialog::ReflectMapEntryDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Panel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* panelSizer;
	panelSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* keySizer;
	keySizer = new wxBoxSizer( wxVERTICAL );
	
	m_StaticKey = new wxStaticText( m_Panel, wxID_ANY, wxT("Key"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticKey->Wrap( -1 );
	keySizer->Add( m_StaticKey, 0, wxLEFT|wxRIGHT|wxTOP, 5 );
	
	m_Key = new wxTextCtrl( m_Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	keySizer->Add( m_Key, 0, wxALL, 2 );
	
	panelSizer->Add( keySizer, 1, wxBOTTOM|wxEXPAND|wxRIGHT, 3 );
	
	wxBoxSizer* valueSizer;
	valueSizer = new wxBoxSizer( wxVERTICAL );
	
	m_StaticValue = new wxStaticText( m_Panel, wxID_ANY, wxT("Value"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticValue->Wrap( -1 );
	valueSizer->Add( m_StaticValue, 0, wxFIXED_MINSIZE|wxLEFT|wxTOP, 5 );
	
	m_Value = new wxTextCtrl( m_Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	valueSizer->Add( m_Value, 0, wxALL, 2 );
	
	panelSizer->Add( valueSizer, 1, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 3 );
	
	m_Panel->SetSizer( panelSizer );
	m_Panel->Layout();
	panelSizer->Fit( m_Panel );
	mainSizer->Add( m_Panel, 1, wxEXPAND | wxALL, 5 );
	
	m_ButtonSizer = new wxStdDialogButtonSizer();
	m_ButtonSizerOK = new wxButton( this, wxID_OK );
	m_ButtonSizer->AddButton( m_ButtonSizerOK );
	m_ButtonSizerCancel = new wxButton( this, wxID_CANCEL );
	m_ButtonSizer->AddButton( m_ButtonSizerCancel );
	m_ButtonSizer->Realize();
	mainSizer->Add( m_ButtonSizer, 0, wxALL|wxEXPAND, 2 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
}

ReflectMapEntryDialog::~ReflectMapEntryDialog()
{
}
