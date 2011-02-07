///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Precompile.h"


#include "Editor/Controls/DynamicBitmap.h"
#include "Editor/Controls/EditorButton.h"
#include "Editor/Controls/MenuButton.h"

#include "EditorGenerated.h"

///////////////////////////////////////////////////////////////////////////
using namespace Helium::Editor;

MainFrameGenerated::MainFrameGenerated( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : Helium::Editor::Frame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	m_MainMenuBar = new wxMenuBar( 0 );
	m_MenuFile = new wxMenu();
	m_MenuFileNew = new wxMenu();
	wxMenuItem* m_ItemFileNewScene;
	m_ItemFileNewScene = new wxMenuItem( m_MenuFileNew, ID_NewScene, wxString( _("Scene...") ) , _("Creates a new scene."), wxITEM_NORMAL );
	m_MenuFileNew->Append( m_ItemFileNewScene );
	
	wxMenuItem* m_ItemFileNewEntity;
	m_ItemFileNewEntity = new wxMenuItem( m_MenuFileNew, ID_NewEntity, wxString( _("Entity...") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuFileNew->Append( m_ItemFileNewEntity );
	
	wxMenuItem* m_ItemFileNewProject;
	m_ItemFileNewProject = new wxMenuItem( m_MenuFileNew, ID_NewProject, wxString( _("Project...") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuFileNew->Append( m_ItemFileNewProject );
	
	m_MenuFile->Append( -1, _("New"), m_MenuFileNew );
	
	wxMenuItem* m_ItemOpen;
	m_ItemOpen = new wxMenuItem( m_MenuFile, ID_Open, wxString( _("Open...") ) + wxT('\t') + wxT("Ctrl-O"), wxEmptyString, wxITEM_NORMAL );
	m_MenuFile->Append( m_ItemOpen );
	
	m_MenuFileOpenRecent = new wxMenu();
	m_MenuFile->Append( -1, _("Open Recent"), m_MenuFileOpenRecent );
	
	wxMenuItem* m_ItemClose;
	m_ItemClose = new wxMenuItem( m_MenuFile, ID_Close, wxString( _("Close") ) + wxT('\t') + wxT("Ctrl-W"), wxEmptyString, wxITEM_NORMAL );
	m_MenuFile->Append( m_ItemClose );
	
	wxMenuItem* m_separator1;
	m_separator1 = m_MenuFile->AppendSeparator();
	
	wxMenuItem* m_ItemSaveAll;
	m_ItemSaveAll = new wxMenuItem( m_MenuFile, ID_SaveAll, wxString( _("Save All") ) + wxT('\t') + wxT("Ctrl-S"), wxEmptyString, wxITEM_NORMAL );
	m_MenuFile->Append( m_ItemSaveAll );
	
	wxMenuItem* m_separator2;
	m_separator2 = m_MenuFile->AppendSeparator();
	
	wxMenuItem* m_ItemImport;
	m_ItemImport = new wxMenuItem( m_MenuFile, ID_Import, wxString( _("Import...") ) + wxT('\t') + wxT("Ctrl-I"), wxEmptyString, wxITEM_NORMAL );
	m_MenuFile->Append( m_ItemImport );
	
	wxMenuItem* m_ItemImportFromClipboard;
	m_ItemImportFromClipboard = new wxMenuItem( m_MenuFile, ID_ImportFromClipboard, wxString( _("Import from Clipboard...") ) + wxT('\t') + wxT("Shift-Ctrl-I"), wxEmptyString, wxITEM_NORMAL );
	m_MenuFile->Append( m_ItemImportFromClipboard );
	
	wxMenuItem* m_ItemExport;
	m_ItemExport = new wxMenuItem( m_MenuFile, ID_Export, wxString( _("Export...") ) + wxT('\t') + wxT("Ctrl-E"), wxEmptyString, wxITEM_NORMAL );
	m_MenuFile->Append( m_ItemExport );
	
	wxMenuItem* m_ItemExportToClipboard;
	m_ItemExportToClipboard = new wxMenuItem( m_MenuFile, ID_ExportToClipboard, wxString( _("Export to Clipboard...") ) + wxT('\t') + wxT("Shift-Ctrl-E"), wxEmptyString, wxITEM_NORMAL );
	m_MenuFile->Append( m_ItemExportToClipboard );
	
	wxMenuItem* m_separator4;
	m_separator4 = m_MenuFile->AppendSeparator();
	
	wxMenuItem* m_ItemExit;
	m_ItemExit = new wxMenuItem( m_MenuFile, ID_Exit, wxString( _("Exit") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuFile->Append( m_ItemExit );
	
	m_MainMenuBar->Append( m_MenuFile, _("File") ); 
	
	m_MenuEdit = new wxMenu();
	wxMenuItem* m_ItemUndo;
	m_ItemUndo = new wxMenuItem( m_MenuEdit, wxID_UNDO, wxString( _("Undo") ) + wxT('\t') + wxT("Ctrl+Z"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemUndo );
	
	wxMenuItem* m_ItemRedo;
	m_ItemRedo = new wxMenuItem( m_MenuEdit, wxID_REDO, wxString( _("Redo") ) + wxT('\t') + wxT("Ctrl+Shift+Z"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemRedo );
	
	wxMenuItem* m_separator5;
	m_separator5 = m_MenuEdit->AppendSeparator();
	
	wxMenuItem* m_ItemCut;
	m_ItemCut = new wxMenuItem( m_MenuEdit, wxID_CUT, wxString( _("Cut") ) + wxT('\t') + wxT("Ctrl+X"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemCut );
	
	wxMenuItem* m_ItemCopy;
	m_ItemCopy = new wxMenuItem( m_MenuEdit, wxID_COPY, wxString( _("Copy") ) + wxT('\t') + wxT("Ctrl+C"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemCopy );
	
	wxMenuItem* m_ItemPaste;
	m_ItemPaste = new wxMenuItem( m_MenuEdit, wxID_PASTE, wxString( _("Paste") ) + wxT('\t') + wxT("Ctrl+V"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemPaste );
	
	wxMenuItem* m_ItemDelete;
	m_ItemDelete = new wxMenuItem( m_MenuEdit, wxID_DELETE, wxString( _("Delete") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemDelete );
	
	wxMenuItem* m_separator6;
	m_separator6 = m_MenuEdit->AppendSeparator();
	
	wxMenuItem* m_ItemSelectAll;
	m_ItemSelectAll = new wxMenuItem( m_MenuEdit, ID_SelectAll, wxString( _("Select All") ) + wxT('\t') + wxT("Ctrl+A"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemSelectAll );
	
	wxMenuItem* m_ItemInvertSelection;
	m_ItemInvertSelection = new wxMenuItem( m_MenuEdit, ID_InvertSelection, wxString( _("Invert Selection") ) + wxT('\t') + wxT("Ctrl+I"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemInvertSelection );
	
	wxMenuItem* m_separator8;
	m_separator8 = m_MenuEdit->AppendSeparator();
	
	wxMenuItem* m_ItemParent;
	m_ItemParent = new wxMenuItem( m_MenuEdit, ID_Parent, wxString( _("Parent") ) + wxT('\t') + wxT("Ctrl+P"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemParent );
	
	wxMenuItem* m_ItemUnparent;
	m_ItemUnparent = new wxMenuItem( m_MenuEdit, ID_Unparent, wxString( _("Unparent") ) + wxT('\t') + wxT("Ctrl+Shift+P"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemUnparent );
	
	wxMenuItem* m_ItemGroup;
	m_ItemGroup = new wxMenuItem( m_MenuEdit, ID_Group, wxString( _("Group") ) + wxT('\t') + wxT("Ctrl+G"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemGroup );
	
	wxMenuItem* m_ItemUngroup;
	m_ItemUngroup = new wxMenuItem( m_MenuEdit, ID_Ungroup, wxString( _("Ungroup") ) + wxT('\t') + wxT("Ctrl+Shift+G"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemUngroup );
	
	wxMenuItem* m_ItemCenter;
	m_ItemCenter = new wxMenuItem( m_MenuEdit, ID_Center, wxString( _("Center") ) + wxT('\t') + wxT("Ctrl+Shift+C"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemCenter );
	
	wxMenuItem* m_separator9;
	m_separator9 = m_MenuEdit->AppendSeparator();
	
	wxMenuItem* m_ItemDuplicate;
	m_ItemDuplicate = new wxMenuItem( m_MenuEdit, ID_Duplicate, wxString( _("Duplicate") ) + wxT('\t') + wxT("Ctrl+D"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemDuplicate );
	
	wxMenuItem* m_ItemSmartDuplicate;
	m_ItemSmartDuplicate = new wxMenuItem( m_MenuEdit, ID_SmartDuplicate, wxString( _("Smart Duplicate") ) + wxT('\t') + wxT("Ctrl+Shift+D"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemSmartDuplicate );
	
	wxMenuItem* m_separator10;
	m_separator10 = m_MenuEdit->AppendSeparator();
	
	wxMenuItem* m_ItemCopyTransform;
	m_ItemCopyTransform = new wxMenuItem( m_MenuEdit, ID_CopyTransform, wxString( _("Copy Transform") ) + wxT('\t') + wxT("Alt+T"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemCopyTransform );
	
	wxMenuItem* m_ItemPasteTransform;
	m_ItemPasteTransform = new wxMenuItem( m_MenuEdit, ID_PasteTransform, wxString( _("Paste Transform") ) + wxT('\t') + wxT("Alt+Shift+T"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemPasteTransform );
	
	wxMenuItem* m_separator11;
	m_separator11 = m_MenuEdit->AppendSeparator();
	
	wxMenuItem* m_ItemSnapToCamera;
	m_ItemSnapToCamera = new wxMenuItem( m_MenuEdit, ID_SnapToCamera, wxString( _("Snap To Camera") ) + wxT('\t') + wxT("Alt+C"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemSnapToCamera );
	
	wxMenuItem* m_ItemSnapCameraTo;
	m_ItemSnapCameraTo = new wxMenuItem( m_MenuEdit, ID_SnapCameraTo, wxString( _("Snap Camera To") ) + wxT('\t') + wxT("Alt+Shift+C"), wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemSnapCameraTo );
	
	wxMenuItem* m_separator12;
	m_separator12 = m_MenuEdit->AppendSeparator();
	
	wxMenuItem* m_ItemWalkUp;
	m_ItemWalkUp = new wxMenuItem( m_MenuEdit, ID_WalkUp, wxString( _("Walk Up") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemWalkUp );
	
	wxMenuItem* m_ItemWalkDown;
	m_ItemWalkDown = new wxMenuItem( m_MenuEdit, ID_WalkDown, wxString( _("Walk Down") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemWalkDown );
	
	wxMenuItem* m_ItemWalkForward;
	m_ItemWalkForward = new wxMenuItem( m_MenuEdit, ID_WalkForward, wxString( _("Walk Forward") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemWalkForward );
	
	wxMenuItem* m_ItemWalkBackward;
	m_ItemWalkBackward = new wxMenuItem( m_MenuEdit, ID_WalkBackward, wxString( _("Walk Backward") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuEdit->Append( m_ItemWalkBackward );
	
	m_MainMenuBar->Append( m_MenuEdit, _("Edit") ); 
	
	m_MenuPanels = new wxMenu();
	m_MainMenuBar->Append( m_MenuPanels, _("Panels") ); 
	
	m_MenuSettings = new wxMenu();
	wxMenuItem* m_ItemSettings;
	m_ItemSettings = new wxMenuItem( m_MenuSettings, ID_Settings, wxString( _("Settings...") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuSettings->Append( m_ItemSettings );
	
	m_MainMenuBar->Append( m_MenuSettings, _("Settings") ); 
	
	m_MenuHelp = new wxMenu();
	wxMenuItem* m_ItemAbout;
	m_ItemAbout = new wxMenuItem( m_MenuHelp, ID_About, wxString( _("About...") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuHelp->Append( m_ItemAbout );
	
	m_MainMenuBar->Append( m_MenuHelp, _("Help") ); 
	
	this->SetMenuBar( m_MainMenuBar );
	
	m_MainStatusBar = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	
	// Connect Events
	this->Connect( m_ItemFileNewScene->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnNewScene ) );
	this->Connect( m_ItemFileNewEntity->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnNewEntity ) );
	this->Connect( m_ItemFileNewProject->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnNewProject ) );
	this->Connect( m_ItemOpen->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnOpen ) );
	this->Connect( m_ItemClose->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnClose ) );
	this->Connect( m_ItemSaveAll->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnSaveAll ) );
	this->Connect( m_ItemImport->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnImport ) );
	this->Connect( m_ItemImportFromClipboard->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnImport ) );
	this->Connect( m_ItemExport->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnExport ) );
	this->Connect( m_ItemExportToClipboard->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnExport ) );
	this->Connect( m_ItemExit->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnExit ) );
	this->Connect( m_ItemUndo->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnUndo ) );
	this->Connect( m_ItemRedo->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnRedo ) );
	this->Connect( m_ItemCut->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnCut ) );
	this->Connect( m_ItemCopy->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnCopy ) );
	this->Connect( m_ItemPaste->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnPaste ) );
	this->Connect( m_ItemDelete->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnDelete ) );
	this->Connect( m_ItemSelectAll->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnSelectAll ) );
	this->Connect( m_ItemInvertSelection->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnInvertSelection ) );
	this->Connect( m_ItemParent->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnParent ) );
	this->Connect( m_ItemUnparent->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnUnparent ) );
	this->Connect( m_ItemGroup->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnGroup ) );
	this->Connect( m_ItemUngroup->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnUngroup ) );
	this->Connect( m_ItemCenter->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnCenter ) );
	this->Connect( m_ItemDuplicate->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnDuplicate ) );
	this->Connect( m_ItemSmartDuplicate->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnSmartDuplicate ) );
	this->Connect( m_ItemCopyTransform->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnCopyTransform ) );
	this->Connect( m_ItemPasteTransform->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnPasteTransform ) );
	this->Connect( m_ItemSnapToCamera->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnSnapToCamera ) );
	this->Connect( m_ItemSnapCameraTo->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnSnapCameraTo ) );
	this->Connect( m_ItemWalkUp->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnPickWalk ) );
	this->Connect( m_ItemWalkForward->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnPickWalk ) );
	this->Connect( m_ItemSettings->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnSettings ) );
	this->Connect( m_ItemAbout->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnAbout ) );
}

MainFrameGenerated::~MainFrameGenerated()
{
	// Disconnect Events
	this->Disconnect( ID_NewScene, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnNewScene ) );
	this->Disconnect( ID_NewEntity, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnNewEntity ) );
	this->Disconnect( ID_NewProject, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnNewProject ) );
	this->Disconnect( ID_Open, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnOpen ) );
	this->Disconnect( ID_Close, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnClose ) );
	this->Disconnect( ID_SaveAll, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnSaveAll ) );
	this->Disconnect( ID_Import, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnImport ) );
	this->Disconnect( ID_ImportFromClipboard, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnImport ) );
	this->Disconnect( ID_Export, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnExport ) );
	this->Disconnect( ID_ExportToClipboard, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnExport ) );
	this->Disconnect( ID_Exit, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnExit ) );
	this->Disconnect( wxID_UNDO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnUndo ) );
	this->Disconnect( wxID_REDO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnRedo ) );
	this->Disconnect( wxID_CUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnCut ) );
	this->Disconnect( wxID_COPY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnCopy ) );
	this->Disconnect( wxID_PASTE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnPaste ) );
	this->Disconnect( wxID_DELETE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnDelete ) );
	this->Disconnect( ID_SelectAll, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnSelectAll ) );
	this->Disconnect( ID_InvertSelection, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnInvertSelection ) );
	this->Disconnect( ID_Parent, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnParent ) );
	this->Disconnect( ID_Unparent, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnUnparent ) );
	this->Disconnect( ID_Group, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnGroup ) );
	this->Disconnect( ID_Ungroup, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnUngroup ) );
	this->Disconnect( ID_Center, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnCenter ) );
	this->Disconnect( ID_Duplicate, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnDuplicate ) );
	this->Disconnect( ID_SmartDuplicate, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnSmartDuplicate ) );
	this->Disconnect( ID_CopyTransform, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnCopyTransform ) );
	this->Disconnect( ID_PasteTransform, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnPasteTransform ) );
	this->Disconnect( ID_SnapToCamera, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnSnapToCamera ) );
	this->Disconnect( ID_SnapCameraTo, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnSnapCameraTo ) );
	this->Disconnect( ID_WalkUp, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnPickWalk ) );
	this->Disconnect( ID_WalkForward, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnPickWalk ) );
	this->Disconnect( ID_Settings, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnSettings ) );
	this->Disconnect( ID_About, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnAbout ) );
	
}

DetailsPanelGenerated::DetailsPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 300,350 ) );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ScrollWindow = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_ScrollWindow->SetScrollRate( 5, 5 );
	wxBoxSizer* scrollSizer;
	scrollSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* sizer1;
	sizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	m_LabelName = new wxStaticText( m_ScrollWindow, wxID_ANY, _("Name"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_LabelName->Wrap( -1 );
	sizer1->Add( m_LabelName, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Name = new wxTextCtrl( m_ScrollWindow, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_Name->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	sizer1->Add( m_Name, 1, wxALL|wxALIGN_CENTER_VERTICAL, 2 );
	
	scrollSizer->Add( sizer1, 0, wxEXPAND, 5 );
	
	wxBoxSizer* sizer2;
	sizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_LabelFileType = new wxStaticText( m_ScrollWindow, wxID_ANY, _("File Type"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_LabelFileType->Wrap( -1 );
	sizer2->Add( m_LabelFileType, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_FileType = new wxTextCtrl( m_ScrollWindow, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_FileType->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	sizer2->Add( m_FileType, 1, wxALL|wxALIGN_CENTER_VERTICAL, 2 );
	
	scrollSizer->Add( sizer2, 0, wxEXPAND, 5 );
	
	wxBoxSizer* sizer3;
	sizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_LabelFileID = new wxStaticText( m_ScrollWindow, wxID_ANY, _("File ID"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_LabelFileID->Wrap( -1 );
	sizer3->Add( m_LabelFileID, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_FileID = new wxTextCtrl( m_ScrollWindow, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_FileID->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	sizer3->Add( m_FileID, 1, wxALL|wxALIGN_CENTER_VERTICAL, 2 );
	
	scrollSizer->Add( sizer3, 0, wxEXPAND, 5 );
	
	wxBoxSizer* sizer4;
	sizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_LabelFolder = new wxStaticText( m_ScrollWindow, wxID_ANY, _("Folder"), wxDefaultPosition, wxDefaultSize, 0 );
	m_LabelFolder->Wrap( -1 );
	sizer4->Add( m_LabelFolder, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP, 5 );
	
	m_Folder = new wxTextCtrl( m_ScrollWindow, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_Folder->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	sizer4->Add( m_Folder, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 2 );
	
	scrollSizer->Add( sizer4, 0, wxBOTTOM|wxEXPAND, 5 );
	
	wxBoxSizer* sizer5;
	sizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_RevisionPanel = new wxPanel( m_ScrollWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* revisionSizer;
	revisionSizer = new wxBoxSizer( wxVERTICAL );
	
	m_LabelPerforce = new wxStaticText( m_RevisionPanel, wxID_ANY, _("Perforce"), wxDefaultPosition, wxDefaultSize, 0 );
	m_LabelPerforce->Wrap( -1 );
	m_LabelPerforce->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	revisionSizer->Add( m_LabelPerforce, 0, wxALL, 5 );
	
	wxBoxSizer* statusSizer;
	statusSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_LabelRevisionStatus = new wxStaticText( m_RevisionPanel, wxID_ANY, _("Status"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_LabelRevisionStatus->Wrap( -1 );
	statusSizer->Add( m_LabelRevisionStatus, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_StatusPanel = new wxPanel( m_RevisionPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* statusInnerSizer;
	statusInnerSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_RevisionStatusIcon = new wxStaticBitmap( m_StatusPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 16,16 ), 0 );
	statusInnerSizer->Add( m_RevisionStatusIcon, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 1 );
	
	m_RevisionStatus = new wxStaticText( m_StatusPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_RevisionStatus->Wrap( -1 );
	statusInnerSizer->Add( m_RevisionStatus, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_StatusPanel->SetSizer( statusInnerSizer );
	m_StatusPanel->Layout();
	statusInnerSizer->Fit( m_StatusPanel );
	statusSizer->Add( m_StatusPanel, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 2 );
	
	revisionSizer->Add( statusSizer, 0, wxEXPAND, 5 );
	
	m_LastCheckInPanel = new wxPanel( m_RevisionPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* lastCheckinSizer;
	lastCheckinSizer = new wxBoxSizer( wxVERTICAL );
	
	m_LabelLastCheckIn = new wxStaticText( m_LastCheckInPanel, wxID_ANY, _("Last Check In"), wxDefaultPosition, wxDefaultSize, 0 );
	m_LabelLastCheckIn->Wrap( -1 );
	lastCheckinSizer->Add( m_LabelLastCheckIn, 0, wxLEFT|wxTOP, 5 );
	
	wxBoxSizer* innerCheckinSizer;
	innerCheckinSizer = new wxBoxSizer( wxVERTICAL );
	
	m_LastCheckIn = new wxTextCtrl( m_LastCheckInPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,75 ), wxTE_DONTWRAP|wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH );
	m_LastCheckIn->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	innerCheckinSizer->Add( m_LastCheckIn, 0, wxALL|wxEXPAND, 2 );
	
	lastCheckinSizer->Add( innerCheckinSizer, 1, wxEXPAND, 5 );
	
	m_LastCheckInPanel->SetSizer( lastCheckinSizer );
	m_LastCheckInPanel->Layout();
	lastCheckinSizer->Fit( m_LastCheckInPanel );
	revisionSizer->Add( m_LastCheckInPanel, 1, wxEXPAND, 5 );
	
	m_FirstCheckInPanel = new wxPanel( m_RevisionPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* firstCheckInSizer;
	firstCheckInSizer = new wxBoxSizer( wxVERTICAL );
	
	m_LabelFirstCheckIn = new wxStaticText( m_FirstCheckInPanel, wxID_ANY, _("First Check In"), wxDefaultPosition, wxDefaultSize, 0 );
	m_LabelFirstCheckIn->Wrap( -1 );
	firstCheckInSizer->Add( m_LabelFirstCheckIn, 0, wxLEFT|wxTOP, 5 );
	
	wxBoxSizer* innerFirstCheckInSizer;
	innerFirstCheckInSizer = new wxBoxSizer( wxVERTICAL );
	
	m_FirstCheckIn = new wxTextCtrl( m_FirstCheckInPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,75 ), wxTE_DONTWRAP|wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH );
	m_FirstCheckIn->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	innerFirstCheckInSizer->Add( m_FirstCheckIn, 0, wxALL|wxEXPAND, 2 );
	
	firstCheckInSizer->Add( innerFirstCheckInSizer, 1, wxEXPAND, 5 );
	
	m_FirstCheckInPanel->SetSizer( firstCheckInSizer );
	m_FirstCheckInPanel->Layout();
	firstCheckInSizer->Fit( m_FirstCheckInPanel );
	revisionSizer->Add( m_FirstCheckInPanel, 1, wxEXPAND, 5 );
	
	m_RevisionPanel->SetSizer( revisionSizer );
	m_RevisionPanel->Layout();
	revisionSizer->Fit( m_RevisionPanel );
	sizer5->Add( m_RevisionPanel, 0, wxEXPAND, 5 );
	
	scrollSizer->Add( sizer5, 0, wxEXPAND, 5 );
	
	m_ScrollWindow->SetSizer( scrollSizer );
	m_ScrollWindow->Layout();
	scrollSizer->Fit( m_ScrollWindow );
	mainSizer->Add( m_ScrollWindow, 1, wxEXPAND | wxALL, 0 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

DetailsPanelGenerated::~DetailsPanelGenerated()
{
}

DirectoryPanelGenerated::DirectoryPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxVERTICAL );
	
	m_DirectoryNotebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_HierarchyPanel = new wxPanel( m_DirectoryNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer31;
	bSizer31 = new wxBoxSizer( wxVERTICAL );
	
	m_HierarchyPanel->SetSizer( bSizer31 );
	m_HierarchyPanel->Layout();
	bSizer31->Fit( m_HierarchyPanel );
	m_DirectoryNotebook->AddPage( m_HierarchyPanel, _("Hierarchy"), false );
	m_EntitiesPanel = new wxPanel( m_DirectoryNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer32;
	bSizer32 = new wxBoxSizer( wxVERTICAL );
	
	m_EntitiesPanel->SetSizer( bSizer32 );
	m_EntitiesPanel->Layout();
	bSizer32->Fit( m_EntitiesPanel );
	m_DirectoryNotebook->AddPage( m_EntitiesPanel, _("Entities"), false );
	m_TypesPanel = new wxPanel( m_DirectoryNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer33;
	bSizer33 = new wxBoxSizer( wxVERTICAL );
	
	m_TypesPanel->SetSizer( bSizer33 );
	m_TypesPanel->Layout();
	bSizer33->Fit( m_TypesPanel );
	m_DirectoryNotebook->AddPage( m_TypesPanel, _("Types"), true );
	
	bSizer21->Add( m_DirectoryNotebook, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer21 );
	this->Layout();
}

DirectoryPanelGenerated::~DirectoryPanelGenerated()
{
}

HelpPanelGenerated::HelpPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 300,200 ) );
	
	wxBoxSizer* bSizer32;
	bSizer32 = new wxBoxSizer( wxVERTICAL );
	
	m_HelpRichText = new wxRichTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0|wxHSCROLL|wxNO_BORDER|wxVSCROLL );
	bSizer32->Add( m_HelpRichText, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer32 );
	this->Layout();
}

HelpPanelGenerated::~HelpPanelGenerated()
{
}

LayersPanelGenerated::LayersPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxVERTICAL );
	
	m_LayerManagementPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer36;
	bSizer36 = new wxBoxSizer( wxHORIZONTAL );
	
	m_NewLayerFromSelectionButton = new Helium::Editor::EditorButton( m_LayerManagementPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer43;
	bSizer43 = new wxBoxSizer( wxHORIZONTAL );
	
	m_NewLayerFromSelectionText = new wxStaticText( m_NewLayerFromSelectionButton, wxID_ANY, _("New Layer (from selection)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_NewLayerFromSelectionText->Wrap( -1 );
	bSizer43->Add( m_NewLayerFromSelectionText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_NewLayerFromSelectionBitmap = new Helium::Editor::DynamicBitmap( m_NewLayerFromSelectionButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer43->Add( m_NewLayerFromSelectionBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_NewLayerFromSelectionButton->SetSizer( bSizer43 );
	m_NewLayerFromSelectionButton->Layout();
	bSizer43->Fit( m_NewLayerFromSelectionButton );
	bSizer36->Add( m_NewLayerFromSelectionButton, 0, wxEXPAND | wxALL, 0 );
	
	m_NewLayerButton = new Helium::Editor::EditorButton( m_LayerManagementPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer431;
	bSizer431 = new wxBoxSizer( wxHORIZONTAL );
	
	m_NewLayerText = new wxStaticText( m_NewLayerButton, wxID_ANY, _("New Layer"), wxDefaultPosition, wxDefaultSize, 0 );
	m_NewLayerText->Wrap( -1 );
	bSizer431->Add( m_NewLayerText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_NewLayerBitmap = new Helium::Editor::DynamicBitmap( m_NewLayerButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer431->Add( m_NewLayerBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_NewLayerButton->SetSizer( bSizer431 );
	m_NewLayerButton->Layout();
	bSizer431->Fit( m_NewLayerButton );
	bSizer36->Add( m_NewLayerButton, 0, wxEXPAND | wxALL, 0 );
	
	m_DeleteLayersButton = new Helium::Editor::EditorButton( m_LayerManagementPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4311;
	bSizer4311 = new wxBoxSizer( wxHORIZONTAL );
	
	m_DeleteLayersText = new wxStaticText( m_DeleteLayersButton, wxID_ANY, _("Delete  Layer(s)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_DeleteLayersText->Wrap( -1 );
	bSizer4311->Add( m_DeleteLayersText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_DeleteLayersBitmap = new Helium::Editor::DynamicBitmap( m_DeleteLayersButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4311->Add( m_DeleteLayersBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_DeleteLayersButton->SetSizer( bSizer4311 );
	m_DeleteLayersButton->Layout();
	bSizer4311->Fit( m_DeleteLayersButton );
	bSizer36->Add( m_DeleteLayersButton, 0, wxEXPAND | wxALL, 0 );
	
	m_staticline4 = new wxStaticLine( m_LayerManagementPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	bSizer36->Add( m_staticline4, 0, wxEXPAND | wxALL, 5 );
	
	m_AddToLayerButton = new Helium::Editor::EditorButton( m_LayerManagementPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer43111;
	bSizer43111 = new wxBoxSizer( wxHORIZONTAL );
	
	m_AddToLayerText = new wxStaticText( m_AddToLayerButton, wxID_ANY, _("Add to Layer"), wxDefaultPosition, wxDefaultSize, 0 );
	m_AddToLayerText->Wrap( -1 );
	bSizer43111->Add( m_AddToLayerText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_AddToLayerBitmap = new Helium::Editor::DynamicBitmap( m_AddToLayerButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer43111->Add( m_AddToLayerBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_AddToLayerButton->SetSizer( bSizer43111 );
	m_AddToLayerButton->Layout();
	bSizer43111->Fit( m_AddToLayerButton );
	bSizer36->Add( m_AddToLayerButton, 0, wxEXPAND | wxALL, 0 );
	
	m_RemoveFromLayerButton = new Helium::Editor::EditorButton( m_LayerManagementPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer431111;
	bSizer431111 = new wxBoxSizer( wxHORIZONTAL );
	
	m_RemoveFromLayerText = new wxStaticText( m_RemoveFromLayerButton, wxID_ANY, _("Remove from Layer"), wxDefaultPosition, wxDefaultSize, 0 );
	m_RemoveFromLayerText->Wrap( -1 );
	bSizer431111->Add( m_RemoveFromLayerText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_RemoveFromLayerBitmap = new Helium::Editor::DynamicBitmap( m_RemoveFromLayerButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer431111->Add( m_RemoveFromLayerBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_RemoveFromLayerButton->SetSizer( bSizer431111 );
	m_RemoveFromLayerButton->Layout();
	bSizer431111->Fit( m_RemoveFromLayerButton );
	bSizer36->Add( m_RemoveFromLayerButton, 0, wxEXPAND | wxALL, 0 );
	
	m_staticline5 = new wxStaticLine( m_LayerManagementPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	bSizer36->Add( m_staticline5, 0, wxEXPAND | wxALL, 5 );
	
	m_SelectMembersButton = new Helium::Editor::EditorButton( m_LayerManagementPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4311111;
	bSizer4311111 = new wxBoxSizer( wxHORIZONTAL );
	
	m_SelectMembersText = new wxStaticText( m_SelectMembersButton, wxID_ANY, _("Select Layer Members"), wxDefaultPosition, wxDefaultSize, 0 );
	m_SelectMembersText->Wrap( -1 );
	bSizer4311111->Add( m_SelectMembersText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_SelectMembersBitmap = new Helium::Editor::DynamicBitmap( m_SelectMembersButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4311111->Add( m_SelectMembersBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_SelectMembersButton->SetSizer( bSizer4311111 );
	m_SelectMembersButton->Layout();
	bSizer4311111->Fit( m_SelectMembersButton );
	bSizer36->Add( m_SelectMembersButton, 0, wxEXPAND | wxALL, 0 );
	
	m_LayerManagementPanel->SetSizer( bSizer36 );
	m_LayerManagementPanel->Layout();
	bSizer36->Fit( m_LayerManagementPanel );
	bSizer19->Add( m_LayerManagementPanel, 0, wxEXPAND | wxALL, 0 );
	
	this->SetSizer( bSizer19 );
	this->Layout();
}

LayersPanelGenerated::~LayersPanelGenerated()
{
}

ProjectPanelGenerated::ProjectPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxVERTICAL );
	
	m_ProjectManagementPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, wxT("m_ProjectManagementPanel") );
	wxBoxSizer* bSizer36;
	bSizer36 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer39;
	bSizer39 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ProjectNameStaticText = new wxStaticText( m_ProjectManagementPanel, wxID_ANY, _("PROJECT NAME"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ProjectNameStaticText->Wrap( -1 );
	m_ProjectNameStaticText->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer39->Add( m_ProjectNameStaticText, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_OptionsButton = new Helium::Editor::MenuButton( m_ProjectManagementPanel, wxID_ANY, _("Options"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer39->Add( m_OptionsButton, 0, wxALL, 5 );
	
	bSizer36->Add( bSizer39, 0, wxEXPAND, 5 );
	
	m_DataViewCtrl = new wxDataViewCtrl ( m_ProjectManagementPanel, wxID_ANY );
	bSizer36->Add( m_DataViewCtrl, 1, wxEXPAND, 5 );
	
	m_ProjectManagementPanel->SetSizer( bSizer36 );
	m_ProjectManagementPanel->Layout();
	bSizer36->Fit( m_ProjectManagementPanel );
	bSizer24->Add( m_ProjectManagementPanel, 1, wxALL|wxEXPAND, 5 );
	
	m_OpenProjectPanel = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL, wxT("m_OpenProjectPanel") );
	m_OpenProjectPanel->SetScrollRate( 5, 5 );
	wxBoxSizer* bSizer48;
	bSizer48 = new wxBoxSizer( wxVERTICAL );
	
	m_RecentProjectsPanel = new wxPanel( m_OpenProjectPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, wxT("m_RecentProjectsPanel") );
	wxBoxSizer* bSizer45;
	bSizer45 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer43;
	bSizer43 = new wxBoxSizer( wxHORIZONTAL );
	
	m_RecentProjectsBitmap = new wxStaticBitmap( m_RecentProjectsPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	m_RecentProjectsBitmap->Hide();
	
	bSizer43->Add( m_RecentProjectsBitmap, 0, wxALL, 5 );
	
	m_RecentProjectsStaticText = new wxStaticText( m_RecentProjectsPanel, wxID_ANY, _("Recent Projects:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_RecentProjectsStaticText->Wrap( -1 );
	m_RecentProjectsStaticText->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer43->Add( m_RecentProjectsStaticText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	bSizer45->Add( bSizer43, 0, 0, 5 );
	
	m_staticline16 = new wxStaticLine( m_RecentProjectsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_staticline16->Hide();
	
	bSizer45->Add( m_staticline16, 0, wxEXPAND | wxALL, 12 );
	
	m_RecentProjectsSizer = new wxBoxSizer( wxVERTICAL );
	
	bSizer45->Add( m_RecentProjectsSizer, 1, wxEXPAND, 5 );
	
	m_staticline13 = new wxStaticLine( m_RecentProjectsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer45->Add( m_staticline13, 0, wxALL|wxEXPAND, 12 );
	
	m_RecentProjectsPanel->SetSizer( bSizer45 );
	m_RecentProjectsPanel->Layout();
	bSizer45->Fit( m_RecentProjectsPanel );
	bSizer48->Add( m_RecentProjectsPanel, 0, wxEXPAND, 5 );
	
	m_OpenOrCreateProjectPanel = new wxPanel( m_OpenProjectPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, wxT("m_OpenOrCreateProjectPanel") );
	wxBoxSizer* bSizer361;
	bSizer361 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer40;
	bSizer40 = new wxBoxSizer( wxVERTICAL );
	
	m_OpenProjectButton = new Helium::Editor::EditorButton( m_OpenOrCreateProjectPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer43111116;
	bSizer43111116 = new wxBoxSizer( wxHORIZONTAL );
	
	m_OpenProjectBitmap = new Helium::Editor::DynamicBitmap( m_OpenProjectButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer43111116->Add( m_OpenProjectBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_OpenProjectText = new wxStaticText( m_OpenProjectButton, wxID_ANY, _("Open Project..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_OpenProjectText->Wrap( -1 );
	bSizer43111116->Add( m_OpenProjectText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_OpenProjectButton->SetSizer( bSizer43111116 );
	m_OpenProjectButton->Layout();
	bSizer43111116->Fit( m_OpenProjectButton );
	bSizer40->Add( m_OpenProjectButton, 0, wxEXPAND | wxALL, 0 );
	
	m_CreateNewProjectButton = new Helium::Editor::EditorButton( m_OpenOrCreateProjectPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer431111161;
	bSizer431111161 = new wxBoxSizer( wxHORIZONTAL );
	
	m_CreateNewProjectBitmap = new Helium::Editor::DynamicBitmap( m_CreateNewProjectButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer431111161->Add( m_CreateNewProjectBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_CreateNewProjectText = new wxStaticText( m_CreateNewProjectButton, wxID_ANY, _("Create New Project..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_CreateNewProjectText->Wrap( -1 );
	bSizer431111161->Add( m_CreateNewProjectText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_CreateNewProjectButton->SetSizer( bSizer431111161 );
	m_CreateNewProjectButton->Layout();
	bSizer431111161->Fit( m_CreateNewProjectButton );
	bSizer40->Add( m_CreateNewProjectButton, 0, wxEXPAND | wxALL, 0 );
	
	bSizer361->Add( bSizer40, 1, wxEXPAND, 5 );
	
	m_OpenOrCreateProjectPanel->SetSizer( bSizer361 );
	m_OpenOrCreateProjectPanel->Layout();
	bSizer361->Fit( m_OpenOrCreateProjectPanel );
	bSizer48->Add( m_OpenOrCreateProjectPanel, 1, wxEXPAND, 5 );
	
	m_OpenProjectPanel->SetSizer( bSizer48 );
	m_OpenProjectPanel->Layout();
	bSizer48->Fit( m_OpenProjectPanel );
	bSizer24->Add( m_OpenProjectPanel, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer24 );
	this->Layout();
}

ProjectPanelGenerated::~ProjectPanelGenerated()
{
}

PropertiesPanelGenerated::PropertiesPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer32;
	bSizer32 = new wxBoxSizer( wxVERTICAL );
	
	m_ControlsPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer33;
	bSizer33 = new wxBoxSizer( wxHORIZONTAL );
	
	m_IntersectionButton = new wxRadioButton( m_ControlsPanel, wxID_ANY, _("Intersection"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer33->Add( m_IntersectionButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2 );
	
	m_UnionButton = new wxRadioButton( m_ControlsPanel, wxID_ANY, _("Union"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer33->Add( m_UnionButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2 );
	
	m_ControlsPanel->SetSizer( bSizer33 );
	m_ControlsPanel->Layout();
	bSizer33->Fit( m_ControlsPanel );
	bSizer32->Add( m_ControlsPanel, 0, wxEXPAND | wxALL, 2 );
	
	m_TreeWndCtrl = new Helium::TreeWndCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxScrolledWindowStyle | wxALWAYS_SHOW_SB | wxCLIP_CHILDREN | wxNO_BORDER, wxPanelNameStr,  wxTR_HIDE_ROOT );
	bSizer32->Add( m_TreeWndCtrl, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer32 );
	this->Layout();
	
	// Connect Events
	m_IntersectionButton->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( PropertiesPanelGenerated::OnIntersection ), NULL, this );
	m_UnionButton->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( PropertiesPanelGenerated::OnUnion ), NULL, this );
}

PropertiesPanelGenerated::~PropertiesPanelGenerated()
{
	// Disconnect Events
	m_IntersectionButton->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( PropertiesPanelGenerated::OnIntersection ), NULL, this );
	m_UnionButton->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( PropertiesPanelGenerated::OnUnion ), NULL, this );
	
}

ToolbarPanelGenerated::ToolbarPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( -1,88 ) );
	this->SetMaxSize( wxSize( -1,88 ) );
	
	wxBoxSizer* bSizer26;
	bSizer26 = new wxBoxSizer( wxVERTICAL );
	
	m_MainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer27;
	bSizer27 = new wxBoxSizer( wxHORIZONTAL );
	
	m_SelectButton = new Helium::Editor::EditorButton( m_MainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer43;
	bSizer43 = new wxBoxSizer( wxHORIZONTAL );
	
	m_SelectText = new wxStaticText( m_SelectButton, wxID_ANY, _("Select"), wxDefaultPosition, wxDefaultSize, 0 );
	m_SelectText->Wrap( -1 );
	bSizer43->Add( m_SelectText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_SelectBitmap = new Helium::Editor::DynamicBitmap( m_SelectButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer43->Add( m_SelectBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_SelectButton->SetSizer( bSizer43 );
	m_SelectButton->Layout();
	bSizer43->Fit( m_SelectButton );
	bSizer27->Add( m_SelectButton, 0, wxEXPAND | wxALL, 0 );
	
	m_TranslateButton = new Helium::Editor::EditorButton( m_MainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer44;
	bSizer44 = new wxBoxSizer( wxHORIZONTAL );
	
	m_TranslateText = new wxStaticText( m_TranslateButton, wxID_ANY, _("Translate"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TranslateText->Wrap( -1 );
	bSizer44->Add( m_TranslateText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_TranslateBitmap = new Helium::Editor::DynamicBitmap( m_TranslateButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer44->Add( m_TranslateBitmap, 0, wxALL, 5 );
	
	m_TranslateButton->SetSizer( bSizer44 );
	m_TranslateButton->Layout();
	bSizer44->Fit( m_TranslateButton );
	bSizer27->Add( m_TranslateButton, 0, wxEXPAND | wxALL, 0 );
	
	m_RotateButton = new Helium::Editor::EditorButton( m_MainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer45;
	bSizer45 = new wxBoxSizer( wxHORIZONTAL );
	
	m_RotateText = new wxStaticText( m_RotateButton, wxID_ANY, _("Rotate"), wxDefaultPosition, wxDefaultSize, 0 );
	m_RotateText->Wrap( -1 );
	bSizer45->Add( m_RotateText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_RotateBitmap = new Helium::Editor::DynamicBitmap( m_RotateButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer45->Add( m_RotateBitmap, 0, wxALL, 5 );
	
	m_RotateButton->SetSizer( bSizer45 );
	m_RotateButton->Layout();
	bSizer45->Fit( m_RotateButton );
	bSizer27->Add( m_RotateButton, 0, wxEXPAND | wxALL, 0 );
	
	m_ScaleButton = new Helium::Editor::EditorButton( m_MainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer451;
	bSizer451 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ScaleText = new wxStaticText( m_ScaleButton, wxID_ANY, _("Scale"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ScaleText->Wrap( -1 );
	bSizer451->Add( m_ScaleText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_ScaleBitmap = new Helium::Editor::DynamicBitmap( m_ScaleButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer451->Add( m_ScaleBitmap, 0, wxALL, 5 );
	
	m_ScaleButton->SetSizer( bSizer451 );
	m_ScaleButton->Layout();
	bSizer451->Fit( m_ScaleButton );
	bSizer27->Add( m_ScaleButton, 0, wxEXPAND | wxALL, 0 );
	
	wxStaticLine* m_staticline16;
	m_staticline16 = new wxStaticLine( m_MainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	bSizer27->Add( m_staticline16, 0, wxEXPAND | wxALL, 2 );
	
	m_DuplicateToolButton = new Helium::Editor::EditorButton( m_MainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer452;
	bSizer452 = new wxBoxSizer( wxHORIZONTAL );
	
	m_DuplicateToolText = new wxStaticText( m_DuplicateToolButton, wxID_ANY, _("Duplicate"), wxDefaultPosition, wxDefaultSize, 0 );
	m_DuplicateToolText->Wrap( -1 );
	bSizer452->Add( m_DuplicateToolText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_DuplicateToolBitmap = new Helium::Editor::DynamicBitmap( m_DuplicateToolButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer452->Add( m_DuplicateToolBitmap, 0, wxALL, 5 );
	
	m_DuplicateToolButton->SetSizer( bSizer452 );
	m_DuplicateToolButton->Layout();
	bSizer452->Fit( m_DuplicateToolButton );
	bSizer27->Add( m_DuplicateToolButton, 0, wxEXPAND | wxALL, 0 );
	
	m_LocatorToolButton = new Helium::Editor::EditorButton( m_MainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4521;
	bSizer4521 = new wxBoxSizer( wxHORIZONTAL );
	
	m_LocatorToolText = new wxStaticText( m_LocatorToolButton, wxID_ANY, _("Duplicate"), wxDefaultPosition, wxDefaultSize, 0 );
	m_LocatorToolText->Wrap( -1 );
	bSizer4521->Add( m_LocatorToolText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_LocatorToolBitmap = new Helium::Editor::DynamicBitmap( m_LocatorToolButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4521->Add( m_LocatorToolBitmap, 0, wxALL, 5 );
	
	m_LocatorToolButton->SetSizer( bSizer4521 );
	m_LocatorToolButton->Layout();
	bSizer4521->Fit( m_LocatorToolButton );
	bSizer27->Add( m_LocatorToolButton, 0, wxEXPAND | wxALL, 0 );
	
	m_VolumeToolButton = new Helium::Editor::EditorButton( m_MainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer45211;
	bSizer45211 = new wxBoxSizer( wxHORIZONTAL );
	
	m_VolumeToolText = new wxStaticText( m_VolumeToolButton, wxID_ANY, _("Volume"), wxDefaultPosition, wxDefaultSize, 0 );
	m_VolumeToolText->Wrap( -1 );
	bSizer45211->Add( m_VolumeToolText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_VolumeToolBitmap = new Helium::Editor::DynamicBitmap( m_VolumeToolButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer45211->Add( m_VolumeToolBitmap, 0, wxALL, 5 );
	
	m_VolumeToolButton->SetSizer( bSizer45211 );
	m_VolumeToolButton->Layout();
	bSizer45211->Fit( m_VolumeToolButton );
	bSizer27->Add( m_VolumeToolButton, 0, wxEXPAND | wxALL, 0 );
	
	m_EntityToolButton = new Helium::Editor::EditorButton( m_MainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer452111;
	bSizer452111 = new wxBoxSizer( wxHORIZONTAL );
	
	m_EntityToolText = new wxStaticText( m_EntityToolButton, wxID_ANY, _("Entity"), wxDefaultPosition, wxDefaultSize, 0 );
	m_EntityToolText->Wrap( -1 );
	bSizer452111->Add( m_EntityToolText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_EntityToolBitmap = new Helium::Editor::DynamicBitmap( m_EntityToolButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer452111->Add( m_EntityToolBitmap, 0, wxALL, 5 );
	
	m_EntityToolButton->SetSizer( bSizer452111 );
	m_EntityToolButton->Layout();
	bSizer452111->Fit( m_EntityToolButton );
	bSizer27->Add( m_EntityToolButton, 0, wxEXPAND | wxALL, 0 );
	
	m_CurveToolButton = new Helium::Editor::EditorButton( m_MainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4521111;
	bSizer4521111 = new wxBoxSizer( wxHORIZONTAL );
	
	m_CurveToolText = new wxStaticText( m_CurveToolButton, wxID_ANY, _("Curve"), wxDefaultPosition, wxDefaultSize, 0 );
	m_CurveToolText->Wrap( -1 );
	bSizer4521111->Add( m_CurveToolText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_CurveToolBitmap = new Helium::Editor::DynamicBitmap( m_CurveToolButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4521111->Add( m_CurveToolBitmap, 0, wxALL, 5 );
	
	m_CurveToolButton->SetSizer( bSizer4521111 );
	m_CurveToolButton->Layout();
	bSizer4521111->Fit( m_CurveToolButton );
	bSizer27->Add( m_CurveToolButton, 0, wxEXPAND | wxALL, 0 );
	
	m_CurveEditToolButton = new Helium::Editor::EditorButton( m_MainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer45211111;
	bSizer45211111 = new wxBoxSizer( wxHORIZONTAL );
	
	m_CurveEditToolText = new wxStaticText( m_CurveEditToolButton, wxID_ANY, _("Curve Edit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_CurveEditToolText->Wrap( -1 );
	bSizer45211111->Add( m_CurveEditToolText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_CurveEditToolBitmap = new Helium::Editor::DynamicBitmap( m_CurveEditToolButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer45211111->Add( m_CurveEditToolBitmap, 0, wxALL, 5 );
	
	m_CurveEditToolButton->SetSizer( bSizer45211111 );
	m_CurveEditToolButton->Layout();
	bSizer45211111->Fit( m_CurveEditToolButton );
	bSizer27->Add( m_CurveEditToolButton, 0, wxEXPAND | wxALL, 0 );
	
	wxStaticLine* m_staticline15;
	m_staticline15 = new wxStaticLine( m_MainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	bSizer27->Add( m_staticline15, 0, wxALL|wxEXPAND, 2 );
	
	wxStaticText* m_staticText10;
	m_staticText10 = new wxStaticText( m_MainPanel, wxID_ANY, _("Game:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	bSizer27->Add( m_staticText10, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3 );
	
	m_PlayButton = new Helium::Editor::EditorButton( m_MainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer45211112;
	bSizer45211112 = new wxBoxSizer( wxHORIZONTAL );
	
	m_PlayText = new wxStaticText( m_PlayButton, wxID_ANY, _("Play"), wxDefaultPosition, wxDefaultSize, 0 );
	m_PlayText->Wrap( -1 );
	bSizer45211112->Add( m_PlayText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_PlayBitmap = new Helium::Editor::DynamicBitmap( m_PlayButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer45211112->Add( m_PlayBitmap, 0, wxALL, 5 );
	
	m_PlayButton->SetSizer( bSizer45211112 );
	m_PlayButton->Layout();
	bSizer45211112->Fit( m_PlayButton );
	bSizer27->Add( m_PlayButton, 0, wxEXPAND | wxALL, 0 );
	
	m_PauseButton = new Helium::Editor::EditorButton( m_MainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer452111121;
	bSizer452111121 = new wxBoxSizer( wxHORIZONTAL );
	
	m_PauseText = new wxStaticText( m_PauseButton, wxID_ANY, _("Pause"), wxDefaultPosition, wxDefaultSize, 0 );
	m_PauseText->Wrap( -1 );
	bSizer452111121->Add( m_PauseText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_PauseBitmap = new Helium::Editor::DynamicBitmap( m_PauseButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer452111121->Add( m_PauseBitmap, 0, wxALL, 5 );
	
	m_PauseButton->SetSizer( bSizer452111121 );
	m_PauseButton->Layout();
	bSizer452111121->Fit( m_PauseButton );
	bSizer27->Add( m_PauseButton, 0, wxEXPAND | wxALL, 0 );
	
	m_StopButton = new Helium::Editor::EditorButton( m_MainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4521111211;
	bSizer4521111211 = new wxBoxSizer( wxHORIZONTAL );
	
	m_StopText = new wxStaticText( m_StopButton, wxID_ANY, _("Stop"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StopText->Wrap( -1 );
	bSizer4521111211->Add( m_StopText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_StopBitmap = new Helium::Editor::DynamicBitmap( m_StopButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4521111211->Add( m_StopBitmap, 0, wxALL, 5 );
	
	m_StopButton->SetSizer( bSizer4521111211 );
	m_StopButton->Layout();
	bSizer4521111211->Fit( m_StopButton );
	bSizer27->Add( m_StopButton, 0, wxEXPAND | wxALL, 0 );
	
	
	bSizer27->Add( 20, 0, 1, wxEXPAND, 5 );
	
	wxStaticText* m_staticText12;
	m_staticText12 = new wxStaticText( m_MainPanel, wxID_ANY, _("Vault:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	bSizer27->Add( m_staticText12, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3 );
	
	m_VaultSearchBox = new wxSearchCtrl( m_MainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 400,-1 ), wxTE_PROCESS_ENTER );
	#ifndef __WXMAC__
	m_VaultSearchBox->ShowSearchButton( true );
	#endif
	m_VaultSearchBox->ShowCancelButton( false );
	m_VaultSearchBox->SetToolTip( _("Vault Search Box") );
	m_VaultSearchBox->SetMinSize( wxSize( 300,-1 ) );
	
	bSizer27->Add( m_VaultSearchBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2 );
	
	
	bSizer27->Add( 5, 0, 0, wxEXPAND, 5 );
	
	m_MainPanel->SetSizer( bSizer27 );
	m_MainPanel->Layout();
	bSizer27->Fit( m_MainPanel );
	bSizer26->Add( m_MainPanel, 0, wxEXPAND | wxALL, 0 );
	
	m_ToolsPropertiesPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	bSizer26->Add( m_ToolsPropertiesPanel, 1, wxEXPAND | wxALL, 0 );
	
	this->SetSizer( bSizer26 );
	this->Layout();
	
	// Connect Events
	m_VaultSearchBox->Connect( wxEVT_COMMAND_SEARCHCTRL_SEARCH_BTN, wxCommandEventHandler( ToolbarPanelGenerated::OnSearchGoButtonClick ), NULL, this );
	m_VaultSearchBox->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ToolbarPanelGenerated::OnSearchTextEnter ), NULL, this );
}

ToolbarPanelGenerated::~ToolbarPanelGenerated()
{
	// Disconnect Events
	m_VaultSearchBox->Disconnect( wxEVT_COMMAND_SEARCHCTRL_SEARCH_BTN, wxCommandEventHandler( ToolbarPanelGenerated::OnSearchGoButtonClick ), NULL, this );
	m_VaultSearchBox->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ToolbarPanelGenerated::OnSearchTextEnter ), NULL, this );
	
}

VaultPanelGenerated::VaultPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer33;
	bSizer33 = new wxBoxSizer( wxVERTICAL );
	
	m_NavigationPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer34;
	bSizer34 = new wxBoxSizer( wxHORIZONTAL );
	
	m_SearchCtrl = new wxSearchCtrl( m_NavigationPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	#ifndef __WXMAC__
	m_SearchCtrl->ShowSearchButton( true );
	#endif
	m_SearchCtrl->ShowCancelButton( true );
	bSizer34->Add( m_SearchCtrl, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_OptionsButton = new Helium::Editor::MenuButton( m_NavigationPanel, wxID_ANY, _("Options"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer34->Add( m_OptionsButton, 0, wxALL, 5 );
	
	m_NavigationPanel->SetSizer( bSizer34 );
	m_NavigationPanel->Layout();
	bSizer34->Fit( m_NavigationPanel );
	bSizer33->Add( m_NavigationPanel, 0, wxEXPAND, 0 );
	
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ResultsPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bResultsSizer;
	bResultsSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ResultsPanel->SetSizer( bResultsSizer );
	m_ResultsPanel->Layout();
	bResultsSizer->Fit( m_ResultsPanel );
	bSizer24->Add( m_ResultsPanel, 1, wxEXPAND, 0 );
	
	bSizer33->Add( bSizer24, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer33 );
	this->Layout();
	
	// Connect Events
	m_SearchCtrl->Connect( wxEVT_COMMAND_SEARCHCTRL_CANCEL_BTN, wxCommandEventHandler( VaultPanelGenerated::OnSearchCancelButtonClick ), NULL, this );
	m_SearchCtrl->Connect( wxEVT_COMMAND_SEARCHCTRL_SEARCH_BTN, wxCommandEventHandler( VaultPanelGenerated::OnSearchGoButtonClick ), NULL, this );
	m_SearchCtrl->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( VaultPanelGenerated::OnSearchTextEnter ), NULL, this );
}

VaultPanelGenerated::~VaultPanelGenerated()
{
	// Disconnect Events
	m_SearchCtrl->Disconnect( wxEVT_COMMAND_SEARCHCTRL_CANCEL_BTN, wxCommandEventHandler( VaultPanelGenerated::OnSearchCancelButtonClick ), NULL, this );
	m_SearchCtrl->Disconnect( wxEVT_COMMAND_SEARCHCTRL_SEARCH_BTN, wxCommandEventHandler( VaultPanelGenerated::OnSearchGoButtonClick ), NULL, this );
	m_SearchCtrl->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( VaultPanelGenerated::OnSearchTextEnter ), NULL, this );
	
}

ViewPanelGenerated::ViewPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxVERTICAL );
	
	m_ToolPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxHORIZONTAL );
	
	m_FrameOriginButton = new Helium::Editor::EditorButton( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer431111;
	bSizer431111 = new wxBoxSizer( wxHORIZONTAL );
	
	m_FrameOriginText = new wxStaticText( m_FrameOriginButton, wxID_ANY, _("Frame Origin"), wxDefaultPosition, wxDefaultSize, 0 );
	m_FrameOriginText->Wrap( -1 );
	bSizer431111->Add( m_FrameOriginText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_FrameOriginBitmap = new Helium::Editor::DynamicBitmap( m_FrameOriginButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer431111->Add( m_FrameOriginBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_FrameOriginButton->SetSizer( bSizer431111 );
	m_FrameOriginButton->Layout();
	bSizer431111->Fit( m_FrameOriginButton );
	bSizer35->Add( m_FrameOriginButton, 0, wxEXPAND | wxALL, 0 );
	
	m_FrameSelectionButton = new Helium::Editor::EditorButton( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4311111;
	bSizer4311111 = new wxBoxSizer( wxHORIZONTAL );
	
	m_FrameSelectionText = new wxStaticText( m_FrameSelectionButton, wxID_ANY, _("Frame Selection"), wxDefaultPosition, wxDefaultSize, 0 );
	m_FrameSelectionText->Wrap( -1 );
	bSizer4311111->Add( m_FrameSelectionText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_FrameSelectionBitmap = new Helium::Editor::DynamicBitmap( m_FrameSelectionButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4311111->Add( m_FrameSelectionBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_FrameSelectionButton->SetSizer( bSizer4311111 );
	m_FrameSelectionButton->Layout();
	bSizer4311111->Fit( m_FrameSelectionButton );
	bSizer35->Add( m_FrameSelectionButton, 0, wxEXPAND | wxALL, 0 );
	
	wxStaticLine* m_staticline3;
	m_staticline3 = new wxStaticLine( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	bSizer35->Add( m_staticline3, 0, wxALL|wxEXPAND, 2 );
	
	m_HighlightModeToggleButton = new Helium::Editor::EditorButton( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4311112;
	bSizer4311112 = new wxBoxSizer( wxHORIZONTAL );
	
	m_HighlightModeToggleText = new wxStaticText( m_HighlightModeToggleButton, wxID_ANY, _("Highlight Mode"), wxDefaultPosition, wxDefaultSize, 0 );
	m_HighlightModeToggleText->Wrap( -1 );
	bSizer4311112->Add( m_HighlightModeToggleText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_HighlightModeToggleBitmap = new Helium::Editor::DynamicBitmap( m_HighlightModeToggleButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4311112->Add( m_HighlightModeToggleBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_HighlightModeToggleButton->SetSizer( bSizer4311112 );
	m_HighlightModeToggleButton->Layout();
	bSizer4311112->Fit( m_HighlightModeToggleButton );
	bSizer35->Add( m_HighlightModeToggleButton, 0, wxEXPAND | wxALL, 0 );
	
	wxStaticLine* m_staticline4;
	m_staticline4 = new wxStaticLine( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	bSizer35->Add( m_staticline4, 0, wxALL|wxEXPAND, 2 );
	
	m_PreviousViewButton = new Helium::Editor::EditorButton( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4311113;
	bSizer4311113 = new wxBoxSizer( wxHORIZONTAL );
	
	m_PreviousViewText = new wxStaticText( m_PreviousViewButton, wxID_ANY, _("Previous View"), wxDefaultPosition, wxDefaultSize, 0 );
	m_PreviousViewText->Wrap( -1 );
	bSizer4311113->Add( m_PreviousViewText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_PreviousViewBitmap = new Helium::Editor::DynamicBitmap( m_PreviousViewButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4311113->Add( m_PreviousViewBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_PreviousViewButton->SetSizer( bSizer4311113 );
	m_PreviousViewButton->Layout();
	bSizer4311113->Fit( m_PreviousViewButton );
	bSizer35->Add( m_PreviousViewButton, 0, wxEXPAND | wxALL, 0 );
	
	m_NextViewButton = new Helium::Editor::EditorButton( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4311114;
	bSizer4311114 = new wxBoxSizer( wxHORIZONTAL );
	
	m_NextViewText = new wxStaticText( m_NextViewButton, wxID_ANY, _("Next View"), wxDefaultPosition, wxDefaultSize, 0 );
	m_NextViewText->Wrap( -1 );
	bSizer4311114->Add( m_NextViewText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_NextViewBitmap = new Helium::Editor::DynamicBitmap( m_NextViewButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4311114->Add( m_NextViewBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_NextViewButton->SetSizer( bSizer4311114 );
	m_NextViewButton->Layout();
	bSizer4311114->Fit( m_NextViewButton );
	bSizer35->Add( m_NextViewButton, 0, wxEXPAND | wxALL, 0 );
	
	wxStaticLine* m_staticline5;
	m_staticline5 = new wxStaticLine( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	bSizer35->Add( m_staticline5, 0, wxALL|wxEXPAND, 2 );
	
	m_OrbitViewToggleButton = new Helium::Editor::EditorButton( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4311115;
	bSizer4311115 = new wxBoxSizer( wxHORIZONTAL );
	
	m_OrbitViewToggleText = new wxStaticText( m_OrbitViewToggleButton, wxID_ANY, _("Orbit View"), wxDefaultPosition, wxDefaultSize, 0 );
	m_OrbitViewToggleText->Wrap( -1 );
	bSizer4311115->Add( m_OrbitViewToggleText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_OrbitViewToggleBitmap = new Helium::Editor::DynamicBitmap( m_OrbitViewToggleButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4311115->Add( m_OrbitViewToggleBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_OrbitViewToggleButton->SetSizer( bSizer4311115 );
	m_OrbitViewToggleButton->Layout();
	bSizer4311115->Fit( m_OrbitViewToggleButton );
	bSizer35->Add( m_OrbitViewToggleButton, 0, wxEXPAND | wxALL, 0 );
	
	m_FrontViewToggleButton = new Helium::Editor::EditorButton( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4311116;
	bSizer4311116 = new wxBoxSizer( wxHORIZONTAL );
	
	m_FrontViewToggleText = new wxStaticText( m_FrontViewToggleButton, wxID_ANY, _("Front View"), wxDefaultPosition, wxDefaultSize, 0 );
	m_FrontViewToggleText->Wrap( -1 );
	bSizer4311116->Add( m_FrontViewToggleText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_FrontViewToggleBitmap = new Helium::Editor::DynamicBitmap( m_FrontViewToggleButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4311116->Add( m_FrontViewToggleBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_FrontViewToggleButton->SetSizer( bSizer4311116 );
	m_FrontViewToggleButton->Layout();
	bSizer4311116->Fit( m_FrontViewToggleButton );
	bSizer35->Add( m_FrontViewToggleButton, 0, wxEXPAND | wxALL, 0 );
	
	m_SideViewToggleButton = new Helium::Editor::EditorButton( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4311117;
	bSizer4311117 = new wxBoxSizer( wxHORIZONTAL );
	
	m_SideViewToggleText = new wxStaticText( m_SideViewToggleButton, wxID_ANY, _("Side View"), wxDefaultPosition, wxDefaultSize, 0 );
	m_SideViewToggleText->Wrap( -1 );
	bSizer4311117->Add( m_SideViewToggleText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_SideViewToggleBitmap = new Helium::Editor::DynamicBitmap( m_SideViewToggleButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4311117->Add( m_SideViewToggleBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_SideViewToggleButton->SetSizer( bSizer4311117 );
	m_SideViewToggleButton->Layout();
	bSizer4311117->Fit( m_SideViewToggleButton );
	bSizer35->Add( m_SideViewToggleButton, 0, wxEXPAND | wxALL, 0 );
	
	m_TopViewToggleButton = new Helium::Editor::EditorButton( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4311118;
	bSizer4311118 = new wxBoxSizer( wxHORIZONTAL );
	
	m_TopViewToggleText = new wxStaticText( m_TopViewToggleButton, wxID_ANY, _("Top View"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TopViewToggleText->Wrap( -1 );
	bSizer4311118->Add( m_TopViewToggleText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_TopViewToggleBitmap = new Helium::Editor::DynamicBitmap( m_TopViewToggleButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4311118->Add( m_TopViewToggleBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_TopViewToggleButton->SetSizer( bSizer4311118 );
	m_TopViewToggleButton->Layout();
	bSizer4311118->Fit( m_TopViewToggleButton );
	bSizer35->Add( m_TopViewToggleButton, 0, wxEXPAND | wxALL, 0 );
	
	wxStaticLine* m_staticline51;
	m_staticline51 = new wxStaticLine( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	bSizer35->Add( m_staticline51, 0, wxEXPAND | wxALL, 2 );
	
	m_ShowAxesToggleButton = new Helium::Editor::EditorButton( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4311119;
	bSizer4311119 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ShowAxesToggleText = new wxStaticText( m_ShowAxesToggleButton, wxID_ANY, _("Show Axes"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ShowAxesToggleText->Wrap( -1 );
	bSizer4311119->Add( m_ShowAxesToggleText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_ShowAxesToggleBitmap = new Helium::Editor::DynamicBitmap( m_ShowAxesToggleButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4311119->Add( m_ShowAxesToggleBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_ShowAxesToggleButton->SetSizer( bSizer4311119 );
	m_ShowAxesToggleButton->Layout();
	bSizer4311119->Fit( m_ShowAxesToggleButton );
	bSizer35->Add( m_ShowAxesToggleButton, 0, wxEXPAND | wxALL, 0 );
	
	m_ShowGridToggleButton = new Helium::Editor::EditorButton( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer43111110;
	bSizer43111110 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ShowGridToggleText = new wxStaticText( m_ShowGridToggleButton, wxID_ANY, _("Show Grid"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ShowGridToggleText->Wrap( -1 );
	bSizer43111110->Add( m_ShowGridToggleText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_ShowGridToggleBitmap = new Helium::Editor::DynamicBitmap( m_ShowGridToggleButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer43111110->Add( m_ShowGridToggleBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_ShowGridToggleButton->SetSizer( bSizer43111110 );
	m_ShowGridToggleButton->Layout();
	bSizer43111110->Fit( m_ShowGridToggleButton );
	bSizer35->Add( m_ShowGridToggleButton, 0, wxEXPAND | wxALL, 0 );
	
	m_ShowBoundsToggleButton = new Helium::Editor::EditorButton( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer43111111;
	bSizer43111111 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ShowBoundsToggleText = new wxStaticText( m_ShowBoundsToggleButton, wxID_ANY, _("Show Bounds"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ShowBoundsToggleText->Wrap( -1 );
	bSizer43111111->Add( m_ShowBoundsToggleText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_ShowBoundsToggleBitmap = new Helium::Editor::DynamicBitmap( m_ShowBoundsToggleButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer43111111->Add( m_ShowBoundsToggleBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_ShowBoundsToggleButton->SetSizer( bSizer43111111 );
	m_ShowBoundsToggleButton->Layout();
	bSizer43111111->Fit( m_ShowBoundsToggleButton );
	bSizer35->Add( m_ShowBoundsToggleButton, 0, wxEXPAND | wxALL, 0 );
	
	m_ShowStatisticsToggleButton = new Helium::Editor::EditorButton( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer43111112;
	bSizer43111112 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ShowStatisticsToggleText = new wxStaticText( m_ShowStatisticsToggleButton, wxID_ANY, _("Show Statistics"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ShowStatisticsToggleText->Wrap( -1 );
	bSizer43111112->Add( m_ShowStatisticsToggleText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_ShowStatisticsToggleBitmap = new Helium::Editor::DynamicBitmap( m_ShowStatisticsToggleButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer43111112->Add( m_ShowStatisticsToggleBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_ShowStatisticsToggleButton->SetSizer( bSizer43111112 );
	m_ShowStatisticsToggleButton->Layout();
	bSizer43111112->Fit( m_ShowStatisticsToggleButton );
	bSizer35->Add( m_ShowStatisticsToggleButton, 0, wxEXPAND | wxALL, 0 );
	
	wxStaticLine* m_staticline52;
	m_staticline52 = new wxStaticLine( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	bSizer35->Add( m_staticline52, 0, wxEXPAND | wxALL, 2 );
	
	m_FrustumCullingToggleButton = new Helium::Editor::EditorButton( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer43111113;
	bSizer43111113 = new wxBoxSizer( wxHORIZONTAL );
	
	m_FrustumCullingToggleText = new wxStaticText( m_FrustumCullingToggleButton, wxID_ANY, _("Frustum Culling"), wxDefaultPosition, wxDefaultSize, 0 );
	m_FrustumCullingToggleText->Wrap( -1 );
	bSizer43111113->Add( m_FrustumCullingToggleText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_FrustumCullingToggleBitmap = new Helium::Editor::DynamicBitmap( m_FrustumCullingToggleButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer43111113->Add( m_FrustumCullingToggleBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_FrustumCullingToggleButton->SetSizer( bSizer43111113 );
	m_FrustumCullingToggleButton->Layout();
	bSizer43111113->Fit( m_FrustumCullingToggleButton );
	bSizer35->Add( m_FrustumCullingToggleButton, 0, wxEXPAND | wxALL, 0 );
	
	m_BackfaceCullingToggleButton = new Helium::Editor::EditorButton( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer43111114;
	bSizer43111114 = new wxBoxSizer( wxHORIZONTAL );
	
	m_BackfaceCullingToggleText = new wxStaticText( m_BackfaceCullingToggleButton, wxID_ANY, _("Backface Culling"), wxDefaultPosition, wxDefaultSize, 0 );
	m_BackfaceCullingToggleText->Wrap( -1 );
	bSizer43111114->Add( m_BackfaceCullingToggleText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_BackfaceCullingToggleBitmap = new Helium::Editor::DynamicBitmap( m_BackfaceCullingToggleButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer43111114->Add( m_BackfaceCullingToggleBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_BackfaceCullingToggleButton->SetSizer( bSizer43111114 );
	m_BackfaceCullingToggleButton->Layout();
	bSizer43111114->Fit( m_BackfaceCullingToggleButton );
	bSizer35->Add( m_BackfaceCullingToggleButton, 0, wxEXPAND | wxALL, 0 );
	
	wxStaticLine* m_staticline53;
	m_staticline53 = new wxStaticLine( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	bSizer35->Add( m_staticline53, 0, wxEXPAND | wxALL, 2 );
	
	m_WireframeShadingToggleButton = new Helium::Editor::EditorButton( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer43111115;
	bSizer43111115 = new wxBoxSizer( wxHORIZONTAL );
	
	m_WireframeShadingToggleText = new wxStaticText( m_WireframeShadingToggleButton, wxID_ANY, _("Wireframe Shading"), wxDefaultPosition, wxDefaultSize, 0 );
	m_WireframeShadingToggleText->Wrap( -1 );
	bSizer43111115->Add( m_WireframeShadingToggleText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_WireframeShadingToggleBitmap = new Helium::Editor::DynamicBitmap( m_WireframeShadingToggleButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer43111115->Add( m_WireframeShadingToggleBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_WireframeShadingToggleButton->SetSizer( bSizer43111115 );
	m_WireframeShadingToggleButton->Layout();
	bSizer43111115->Fit( m_WireframeShadingToggleButton );
	bSizer35->Add( m_WireframeShadingToggleButton, 0, wxEXPAND | wxALL, 0 );
	
	m_MaterialShadingToggleButton = new Helium::Editor::EditorButton( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer43111116;
	bSizer43111116 = new wxBoxSizer( wxHORIZONTAL );
	
	m_MaterialShadingToggleText = new wxStaticText( m_MaterialShadingToggleButton, wxID_ANY, _("Material Shading"), wxDefaultPosition, wxDefaultSize, 0 );
	m_MaterialShadingToggleText->Wrap( -1 );
	bSizer43111116->Add( m_MaterialShadingToggleText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_MaterialShadingToggleBitmap = new Helium::Editor::DynamicBitmap( m_MaterialShadingToggleButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer43111116->Add( m_MaterialShadingToggleBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_MaterialShadingToggleButton->SetSizer( bSizer43111116 );
	m_MaterialShadingToggleButton->Layout();
	bSizer43111116->Fit( m_MaterialShadingToggleButton );
	bSizer35->Add( m_MaterialShadingToggleButton, 0, wxEXPAND | wxALL, 0 );
	
	m_TextureShadingToggleButton = new Helium::Editor::EditorButton( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer431111161;
	bSizer431111161 = new wxBoxSizer( wxHORIZONTAL );
	
	m_TextureShadingToggleText = new wxStaticText( m_TextureShadingToggleButton, wxID_ANY, _("Texture Shading"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TextureShadingToggleText->Wrap( -1 );
	bSizer431111161->Add( m_TextureShadingToggleText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_TextureShadingToggleBitmap = new Helium::Editor::DynamicBitmap( m_TextureShadingToggleButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer431111161->Add( m_TextureShadingToggleBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_TextureShadingToggleButton->SetSizer( bSizer431111161 );
	m_TextureShadingToggleButton->Layout();
	bSizer431111161->Fit( m_TextureShadingToggleButton );
	bSizer35->Add( m_TextureShadingToggleButton, 0, wxEXPAND | wxALL, 0 );
	
	wxStaticLine* m_staticline54;
	m_staticline54 = new wxStaticLine( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	bSizer35->Add( m_staticline54, 0, wxEXPAND | wxALL, 2 );
	
	m_ColorModeSceneToggleButton = new Helium::Editor::EditorButton( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer43111117;
	bSizer43111117 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ColorModeSceneToggleText = new wxStaticText( m_ColorModeSceneToggleButton, wxID_ANY, _("Scene Coloring"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ColorModeSceneToggleText->Wrap( -1 );
	bSizer43111117->Add( m_ColorModeSceneToggleText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_ColorModeSceneToggleBitmap = new Helium::Editor::DynamicBitmap( m_ColorModeSceneToggleButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer43111117->Add( m_ColorModeSceneToggleBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_ColorModeSceneToggleButton->SetSizer( bSizer43111117 );
	m_ColorModeSceneToggleButton->Layout();
	bSizer43111117->Fit( m_ColorModeSceneToggleButton );
	bSizer35->Add( m_ColorModeSceneToggleButton, 0, wxEXPAND | wxALL, 0 );
	
	m_ColorModeLayerToggleButton = new Helium::Editor::EditorButton( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer43111118;
	bSizer43111118 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ColorModeLayerToggleText = new wxStaticText( m_ColorModeLayerToggleButton, wxID_ANY, _("Layer Coloring"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ColorModeLayerToggleText->Wrap( -1 );
	bSizer43111118->Add( m_ColorModeLayerToggleText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_ColorModeLayerToggleBitmap = new Helium::Editor::DynamicBitmap( m_ColorModeLayerToggleButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer43111118->Add( m_ColorModeLayerToggleBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_ColorModeLayerToggleButton->SetSizer( bSizer43111118 );
	m_ColorModeLayerToggleButton->Layout();
	bSizer43111118->Fit( m_ColorModeLayerToggleButton );
	bSizer35->Add( m_ColorModeLayerToggleButton, 0, wxEXPAND | wxALL, 0 );
	
	m_ColorModeTypeToggleButton = new Helium::Editor::EditorButton( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer43111119;
	bSizer43111119 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ColorModeTypeToggleText = new wxStaticText( m_ColorModeTypeToggleButton, wxID_ANY, _("Type Coloring"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ColorModeTypeToggleText->Wrap( -1 );
	bSizer43111119->Add( m_ColorModeTypeToggleText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_ColorModeTypeToggleBitmap = new Helium::Editor::DynamicBitmap( m_ColorModeTypeToggleButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer43111119->Add( m_ColorModeTypeToggleBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_ColorModeTypeToggleButton->SetSizer( bSizer43111119 );
	m_ColorModeTypeToggleButton->Layout();
	bSizer43111119->Fit( m_ColorModeTypeToggleButton );
	bSizer35->Add( m_ColorModeTypeToggleButton, 0, wxEXPAND | wxALL, 0 );
	
	m_ColorModeScaleToggleButton = new Helium::Editor::EditorButton( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer43111120;
	bSizer43111120 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ColorModeScaleToggleText = new wxStaticText( m_ColorModeScaleToggleButton, wxID_ANY, _("Scale Coloring"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ColorModeScaleToggleText->Wrap( -1 );
	bSizer43111120->Add( m_ColorModeScaleToggleText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_ColorModeScaleToggleBitmap = new Helium::Editor::DynamicBitmap( m_ColorModeScaleToggleButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer43111120->Add( m_ColorModeScaleToggleBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_ColorModeScaleToggleButton->SetSizer( bSizer43111120 );
	m_ColorModeScaleToggleButton->Layout();
	bSizer43111120->Fit( m_ColorModeScaleToggleButton );
	bSizer35->Add( m_ColorModeScaleToggleButton, 0, wxEXPAND | wxALL, 0 );
	
	m_ColorModeScaleGradientToggleButton = new Helium::Editor::EditorButton( m_ToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer43111121;
	bSizer43111121 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ColorModeScaleGradientToggleText = new wxStaticText( m_ColorModeScaleGradientToggleButton, wxID_ANY, _("Scale Gradient Coloring"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ColorModeScaleGradientToggleText->Wrap( -1 );
	bSizer43111121->Add( m_ColorModeScaleGradientToggleText, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_ColorModeScaleGradientToggleBitmap = new Helium::Editor::DynamicBitmap( m_ColorModeScaleGradientToggleButton, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer43111121->Add( m_ColorModeScaleGradientToggleBitmap, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_ColorModeScaleGradientToggleButton->SetSizer( bSizer43111121 );
	m_ColorModeScaleGradientToggleButton->Layout();
	bSizer43111121->Fit( m_ColorModeScaleGradientToggleButton );
	bSizer35->Add( m_ColorModeScaleGradientToggleButton, 0, wxEXPAND | wxALL, 0 );
	
	m_ToolPanel->SetSizer( bSizer35 );
	m_ToolPanel->Layout();
	bSizer35->Fit( m_ToolPanel );
	bSizer21->Add( m_ToolPanel, 0, wxEXPAND | wxALL, 1 );
	
	m_ViewContainerPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* m_ViewSizer;
	m_ViewSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ViewContainerPanel->SetSizer( m_ViewSizer );
	m_ViewContainerPanel->Layout();
	m_ViewSizer->Fit( m_ViewContainerPanel );
	bSizer21->Add( m_ViewContainerPanel, 1, wxEXPAND, 1 );
	
	this->SetSizer( bSizer21 );
	this->Layout();
}

ViewPanelGenerated::~ViewPanelGenerated()
{
}
