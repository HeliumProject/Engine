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
	
	wxBoxSizer* getAssetsSizer;
	getAssetsSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ButtonGetAssets = new wxBitmapButton( m_ScrollWindow, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 70,70 ), wxBU_AUTODRAW );
	m_ButtonGetAssets->SetToolTip( wxT("Get Assets\nFetches the latest version of assets from Perforce.") );
	
	m_ButtonGetAssets->SetToolTip( wxT("Get Assets\nFetches the latest version of assets from Perforce.") );
	
	getAssetsSizer->Add( m_ButtonGetAssets, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 2 );
	
	m_TextGetAssets = new wxStaticText( m_ScrollWindow, wxID_ANY, wxT("Get Assets"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TextGetAssets->Wrap( -1 );
	getAssetsSizer->Add( m_TextGetAssets, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 0 );
	
	gridSizer->Add( getAssetsSizer, 1, wxEXPAND, 5 );
	
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
	
	wxBoxSizer* liveLinkSizer;
	liveLinkSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ButtonLive = new wxBitmapButton( m_ScrollWindow, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 70,70 ), wxBU_AUTODRAW );
	m_ButtonLive->SetToolTip( wxT("Live-Link\nOpens the Live-Link status window for connecting to the devkit.") );
	
	m_ButtonLive->SetToolTip( wxT("Live-Link\nOpens the Live-Link status window for connecting to the devkit.") );
	
	liveLinkSizer->Add( m_ButtonLive, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 2 );
	
	m_TextRunViewer = new wxStaticText( m_ScrollWindow, wxID_ANY, wxT("Live Link"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TextRunViewer->Wrap( -1 );
	liveLinkSizer->Add( m_TextRunViewer, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 0 );
	
	gridSizer->Add( liveLinkSizer, 1, wxEXPAND, 5 );
	
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
	
	wxBoxSizer* effectEdSizer;
	effectEdSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ButtonAnimationEventsEditor = new wxBitmapButton( m_ScrollWindow, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 70,70 ), wxBU_AUTODRAW );
	m_ButtonAnimationEventsEditor->SetToolTip( wxT("Animation Events Editor\nAllows you to edit events triggered by an entity's animations.") );
	
	m_ButtonAnimationEventsEditor->SetToolTip( wxT("Animation Events Editor\nAllows you to edit events triggered by an entity's animations.") );
	
	effectEdSizer->Add( m_ButtonAnimationEventsEditor, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 2 );
	
	m_TextEffectEditor = new wxStaticText( m_ScrollWindow, wxID_ANY, wxT("Animation Events"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TextEffectEditor->Wrap( -1 );
	effectEdSizer->Add( m_TextEffectEditor, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 0 );
	
	gridSizer->Add( effectEdSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* cinematicSizer;
	cinematicSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ButtonCinematicEventsEditor = new wxBitmapButton( m_ScrollWindow, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 70,70 ), wxBU_AUTODRAW );
	m_ButtonCinematicEventsEditor->SetToolTip( wxT("Cinematic Events Editor\nAllows you to edit events triggered in a cinematic.") );
	
	m_ButtonCinematicEventsEditor->SetToolTip( wxT("Cinematic Events Editor\nAllows you to edit events triggered in a cinematic.") );
	
	cinematicSizer->Add( m_ButtonCinematicEventsEditor, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 2 );
	
	m_TextCinematicEditor = new wxStaticText( m_ScrollWindow, wxID_ANY, wxT("Cinematic Events"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TextCinematicEditor->Wrap( -1 );
	cinematicSizer->Add( m_TextCinematicEditor, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 0 );
	
	gridSizer->Add( cinematicSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* charSizer;
	charSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ButtonCharacterEditor = new wxBitmapButton( m_ScrollWindow, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 70,70 ), wxBU_AUTODRAW );
	m_ButtonCharacterEditor->SetToolTip( wxT("Character Editor\nAllows you to edit joint attributes on your character.") );
	
	m_ButtonCharacterEditor->SetToolTip( wxT("Character Editor\nAllows you to edit joint attributes on your character.") );
	
	charSizer->Add( m_ButtonCharacterEditor, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 2 );
	
	m_TextCharacterEditor = new wxStaticText( m_ScrollWindow, wxID_ANY, wxT("Character Editor"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TextCharacterEditor->Wrap( -1 );
	charSizer->Add( m_TextCharacterEditor, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 0 );
	
	gridSizer->Add( charSizer, 1, wxEXPAND, 5 );
	
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
