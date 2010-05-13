///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
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

#include "EditorChooserInnerPanel.h"

///////////////////////////////////////////////////////////////////////////

EditorChooserInnerPanel::EditorChooserInnerPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 210,130 ) );
	
	wxBoxSizer* outerPanelSizer;
	outerPanelSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ScrollWindow = new wxScrolledWindow( this, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxTAB_TRAVERSAL|wxVSCROLL );
	m_ScrollWindow->SetScrollRate( 5, 5 );
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxGridSizer* gridSizer;
	gridSizer = new wxGridSizer( 2, 2, 0, 0 );
	
	wxBoxSizer* runGameSizer;
	runGameSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ButtonRunGame = new wxBitmapButton( m_ScrollWindow, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 70,70 ), wxBU_AUTODRAW );
	m_ButtonRunGame->SetToolTip( wxT("Run Game\nUses the build server to launch the game.") );
	
	m_ButtonRunGame->SetToolTip( wxT("Run Game\nUses the build server to launch the game.") );
	
	runGameSizer->Add( m_ButtonRunGame, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 2 );
	
	m_TextRunGame = new wxStaticText( m_ScrollWindow, wxID_ANY, wxT("Run Game"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TextRunGame->Wrap( -1 );
	runGameSizer->Add( m_TextRunGame, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 0 );
	
	gridSizer->Add( runGameSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* assetEdSizer;
	assetEdSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ButtonAssetEditor = new wxBitmapButton( m_ScrollWindow, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 70,70 ), wxBU_AUTODRAW );
	m_ButtonAssetEditor->SetToolTip( wxT("Asset Editor\nAllows you to create assets and edit their attributes.") );
	
	m_ButtonAssetEditor->SetToolTip( wxT("Asset Editor\nAllows you to create assets and edit their attributes.") );
	
	assetEdSizer->Add( m_ButtonAssetEditor, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 2 );
	
	m_TextAssetEditor = new wxStaticText( m_ScrollWindow, wxID_ANY, wxT("Asset Editor"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TextAssetEditor->Wrap( -1 );
	assetEdSizer->Add( m_TextAssetEditor, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 0 );
	
	gridSizer->Add( assetEdSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* sceneEdSizer;
	sceneEdSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ButtonSceneEditor = new wxBitmapButton( m_ScrollWindow, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 70,70 ), wxBU_AUTODRAW );
	m_ButtonSceneEditor->SetToolTip( wxT("Scene Editor\nAllows you to build game levels from geometry that has been exported from Maya.") );
	
	m_ButtonSceneEditor->SetToolTip( wxT("Scene Editor\nAllows you to build game levels from geometry that has been exported from Maya.") );
	
	sceneEdSizer->Add( m_ButtonSceneEditor, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 2 );
	
	m_TextSceneEditor = new wxStaticText( m_ScrollWindow, wxID_ANY, wxT("Scene Editor"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TextSceneEditor->Wrap( -1 );
	sceneEdSizer->Add( m_TextSceneEditor, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 0 );
	
	gridSizer->Add( sceneEdSizer, 1, wxEXPAND, 5 );
	
	mainSizer->Add( gridSizer, 1, wxEXPAND, 5 );
	
	m_ScrollWindow->SetSizer( mainSizer );
	m_ScrollWindow->Layout();
	mainSizer->Fit( m_ScrollWindow );
	outerPanelSizer->Add( m_ScrollWindow, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( outerPanelSizer );
	this->Layout();
}

EditorChooserInnerPanel::~EditorChooserInnerPanel()
{
}
