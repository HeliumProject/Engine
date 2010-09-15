///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  4 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Precompile.h"


#include "wx/tglbtn.h"

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
	this->Connect( ID_NewScene, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnNewScene ) );
	this->Connect( ID_NewEntity, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnNewEntity ) );
	this->Connect( ID_NewProject, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnNewProject ) );
	this->Connect( ID_Open, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnOpen ) );
	this->Connect( ID_Close, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnClose ) );
	this->Connect( ID_SaveAll, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnSaveAll ) );
	this->Connect( ID_Import, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnImport ) );
	this->Connect( ID_ImportFromClipboard, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnImport ) );
	this->Connect( ID_Export, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnExport ) );
	this->Connect( ID_ExportToClipboard, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnExport ) );
	this->Connect( ID_Exit, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnExit ) );
	this->Connect( wxID_UNDO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnUndo ) );
	this->Connect( wxID_REDO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnRedo ) );
	this->Connect( wxID_CUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnCut ) );
	this->Connect( wxID_COPY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnCopy ) );
	this->Connect( wxID_PASTE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnPaste ) );
	this->Connect( wxID_DELETE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnDelete ) );
	this->Connect( ID_SelectAll, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnSelectAll ) );
	this->Connect( ID_InvertSelection, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnInvertSelection ) );
	this->Connect( ID_Parent, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnParent ) );
	this->Connect( ID_Unparent, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnUnparent ) );
	this->Connect( ID_Group, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnGroup ) );
	this->Connect( ID_Ungroup, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnUngroup ) );
	this->Connect( ID_Center, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnCenter ) );
	this->Connect( ID_Duplicate, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnDuplicate ) );
	this->Connect( ID_SmartDuplicate, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnSmartDuplicate ) );
	this->Connect( ID_CopyTransform, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnCopyTransform ) );
	this->Connect( ID_PasteTransform, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnPasteTransform ) );
	this->Connect( ID_SnapToCamera, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnSnapToCamera ) );
	this->Connect( ID_SnapCameraTo, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnSnapCameraTo ) );
	this->Connect( ID_WalkUp, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnPickWalk ) );
	this->Connect( ID_WalkForward, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnPickWalk ) );
	this->Connect( ID_Settings, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnSettings ) );
	this->Connect( ID_About, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameGenerated::OnAbout ) );
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
	m_DirectoryNotebook->AddPage( m_TypesPanel, _("Types"), false );
	
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
	
	m_HelpRichText = new wxRichTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0|wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS );
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
	
	m_CreateNewLayerFromSelectionButton = new wxBitmapButton( m_LayerManagementPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer36->Add( m_CreateNewLayerFromSelectionButton, 0, wxALL, 2 );
	
	m_CreateNewLayerButton = new wxBitmapButton( m_LayerManagementPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer36->Add( m_CreateNewLayerButton, 0, wxALL, 2 );
	
	m_DeleteSelectedLayersButton = new wxBitmapButton( m_LayerManagementPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer36->Add( m_DeleteSelectedLayersButton, 0, wxALL, 2 );
	
	m_staticline4 = new wxStaticLine( m_LayerManagementPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	bSizer36->Add( m_staticline4, 0, wxEXPAND | wxALL, 5 );
	
	m_AddSelectionToLayerButton = new wxBitmapButton( m_LayerManagementPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer36->Add( m_AddSelectionToLayerButton, 0, wxALL, 2 );
	
	m_RemoveSelectionFromLayerButton = new wxBitmapButton( m_LayerManagementPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer36->Add( m_RemoveSelectionFromLayerButton, 0, wxALL, 2 );
	
	m_staticline5 = new wxStaticLine( m_LayerManagementPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	bSizer36->Add( m_staticline5, 0, wxEXPAND | wxALL, 5 );
	
	m_SelectLayerMembersButton = new wxBitmapButton( m_LayerManagementPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer36->Add( m_SelectLayerMembersButton, 0, wxALL, 2 );
	
	m_SelectLayersButton = new wxBitmapButton( m_LayerManagementPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer36->Add( m_SelectLayersButton, 0, wxALL, 2 );
	
	m_LayerManagementPanel->SetSizer( bSizer36 );
	m_LayerManagementPanel->Layout();
	bSizer36->Fit( m_LayerManagementPanel );
	bSizer19->Add( m_LayerManagementPanel, 0, wxEXPAND | wxALL, 0 );
	
	this->SetSizer( bSizer19 );
	this->Layout();
	
	// Connect Events
	m_CreateNewLayerFromSelectionButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LayersPanelGenerated::OnNewLayerFromSelection ), NULL, this );
	m_CreateNewLayerButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LayersPanelGenerated::OnNewLayer ), NULL, this );
	m_DeleteSelectedLayersButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LayersPanelGenerated::OnDeleteLayer ), NULL, this );
	m_AddSelectionToLayerButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LayersPanelGenerated::OnAddSelectionToLayer ), NULL, this );
	m_RemoveSelectionFromLayerButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LayersPanelGenerated::OnRemoveSelectionFromLayer ), NULL, this );
	m_SelectLayerMembersButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LayersPanelGenerated::OnSelectLayerMembers ), NULL, this );
	m_SelectLayersButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LayersPanelGenerated::OnSelectLayer ), NULL, this );
}

LayersPanelGenerated::~LayersPanelGenerated()
{
	// Disconnect Events
	m_CreateNewLayerFromSelectionButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LayersPanelGenerated::OnNewLayerFromSelection ), NULL, this );
	m_CreateNewLayerButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LayersPanelGenerated::OnNewLayer ), NULL, this );
	m_DeleteSelectedLayersButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LayersPanelGenerated::OnDeleteLayer ), NULL, this );
	m_AddSelectionToLayerButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LayersPanelGenerated::OnAddSelectionToLayer ), NULL, this );
	m_RemoveSelectionFromLayerButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LayersPanelGenerated::OnRemoveSelectionFromLayer ), NULL, this );
	m_SelectLayerMembersButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LayersPanelGenerated::OnSelectLayerMembers ), NULL, this );
	m_SelectLayersButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LayersPanelGenerated::OnSelectLayer ), NULL, this );
	
}

ProjectPanelGenerated::ProjectPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxVERTICAL );
	
	m_ProjectManagementPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer36;
	bSizer36 = new wxBoxSizer( wxHORIZONTAL );
	
	m_AddFile = new wxBitmapButton( m_ProjectManagementPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer36->Add( m_AddFile, 0, wxALL, 2 );
	
	m_Delete = new wxBitmapButton( m_ProjectManagementPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer36->Add( m_Delete, 0, wxALL, 2 );
	
	m_ProjectManagementPanel->SetSizer( bSizer36 );
	m_ProjectManagementPanel->Layout();
	bSizer36->Fit( m_ProjectManagementPanel );
	bSizer24->Add( m_ProjectManagementPanel, 0, wxEXPAND | wxALL, 2 );
	
	m_DataViewCtrl = new wxDataViewCtrl ( this, wxID_ANY );
	bSizer24->Add( m_DataViewCtrl, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer24 );
	this->Layout();
	
	// Connect Events
	m_AddFile->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectPanelGenerated::OnAddPath ), NULL, this );
	m_Delete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectPanelGenerated::OnDelete ), NULL, this );
}

ProjectPanelGenerated::~ProjectPanelGenerated()
{
	// Disconnect Events
	m_AddFile->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectPanelGenerated::OnAddPath ), NULL, this );
	m_Delete->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectPanelGenerated::OnDelete ), NULL, this );
	
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
	
	m_ToolsPanel = new wxPanel( m_MainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* m_ToolsPanelSizer;
	m_ToolsPanelSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_ToolsPanel->SetSizer( m_ToolsPanelSizer );
	m_ToolsPanel->Layout();
	m_ToolsPanelSizer->Fit( m_ToolsPanel );
	bSizer27->Add( m_ToolsPanel, 1, wxEXPAND | wxALL, 0 );
	
	m_PlayButton = new wxBitmapButton( m_MainPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer27->Add( m_PlayButton, 0, wxALL, 2 );
	
	m_PauseButton = new wxBitmapButton( m_MainPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer27->Add( m_PauseButton, 0, wxALL, 2 );
	
	m_StopButton = new wxBitmapButton( m_MainPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer27->Add( m_StopButton, 0, wxALL, 2 );
	
	
	bSizer27->Add( 20, 0, 1, wxEXPAND, 5 );
	
	m_VaultSearchBox = new wxSearchCtrl( m_MainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 400,-1 ), wxTE_PROCESS_ENTER );
	#ifndef __WXMAC__
	m_VaultSearchBox->ShowSearchButton( true );
	#endif
	m_VaultSearchBox->ShowCancelButton( false );
	m_VaultSearchBox->SetMinSize( wxSize( 300,-1 ) );
	
	bSizer27->Add( m_VaultSearchBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2 );
	
	m_MainPanel->SetSizer( bSizer27 );
	m_MainPanel->Layout();
	bSizer27->Fit( m_MainPanel );
	bSizer26->Add( m_MainPanel, 0, wxEXPAND | wxALL, 0 );
	
	m_ToolsPropertiesPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_ToolsPropertiesPanel->SetMinSize( wxSize( -1,50 ) );
	
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

TypesPanelGenerated::TypesPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer30;
	bSizer30 = new wxBoxSizer( wxVERTICAL );
	
	this->SetSizer( bSizer30 );
	this->Layout();
}

TypesPanelGenerated::~TypesPanelGenerated()
{
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
	
	m_OptionsButton = new wxBitmapButton( m_NavigationPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_OptionsButton->SetToolTip( _("Vault Settings...") );
	
	m_OptionsButton->SetToolTip( _("Vault Settings...") );
	
	bSizer34->Add( m_OptionsButton, 0, wxALL, 5 );
	
	m_NavigationPanel->SetSizer( bSizer34 );
	m_NavigationPanel->Layout();
	bSizer34->Fit( m_NavigationPanel );
	bSizer33->Add( m_NavigationPanel, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ResultsPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	bSizer24->Add( m_ResultsPanel, 1, wxEXPAND | wxALL, 5 );
	
	bSizer33->Add( bSizer24, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer33 );
	this->Layout();
	
	// Connect Events
	m_SearchCtrl->Connect( wxEVT_COMMAND_SEARCHCTRL_CANCEL_BTN, wxCommandEventHandler( VaultPanelGenerated::OnSearchCancelButtonClick ), NULL, this );
	m_SearchCtrl->Connect( wxEVT_COMMAND_SEARCHCTRL_SEARCH_BTN, wxCommandEventHandler( VaultPanelGenerated::OnSearchGoButtonClick ), NULL, this );
	m_SearchCtrl->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( VaultPanelGenerated::OnSearchTextEnter ), NULL, this );
	m_OptionsButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VaultPanelGenerated::OnVaultSettingsButtonClick ), NULL, this );
}

VaultPanelGenerated::~VaultPanelGenerated()
{
	// Disconnect Events
	m_SearchCtrl->Disconnect( wxEVT_COMMAND_SEARCHCTRL_CANCEL_BTN, wxCommandEventHandler( VaultPanelGenerated::OnSearchCancelButtonClick ), NULL, this );
	m_SearchCtrl->Disconnect( wxEVT_COMMAND_SEARCHCTRL_SEARCH_BTN, wxCommandEventHandler( VaultPanelGenerated::OnSearchGoButtonClick ), NULL, this );
	m_SearchCtrl->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( VaultPanelGenerated::OnSearchTextEnter ), NULL, this );
	m_OptionsButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VaultPanelGenerated::OnVaultSettingsButtonClick ), NULL, this );
	
}

ViewPanelGenerated::ViewPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxVERTICAL );
	
	m_ToolPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxHORIZONTAL );
	
	m_FrameOriginButton = new wxBitmapButton( m_ToolPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer35->Add( m_FrameOriginButton, 0, wxALL, 2 );
	
	m_FrameSelectedButton = new wxBitmapButton( m_ToolPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer35->Add( m_FrameSelectedButton, 0, wxALL, 2 );
	
	
	bSizer35->Add( 10, 0, 0, wxEXPAND, 0 );
	
	m_PreviousViewButton = new wxBitmapButton( m_ToolPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer35->Add( m_PreviousViewButton, 0, wxALL, 2 );
	
	m_NextViewButton = new wxBitmapButton( m_ToolPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer35->Add( m_NextViewButton, 0, wxALL, 2 );
	
	
	bSizer35->Add( 10, 0, 0, wxEXPAND, 0 );
	
	m_HighlightModeToggleButton = new wxBitmapToggleButton( m_ToolPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer35->Add( m_HighlightModeToggleButton, 0, wxALL, 2 );
	
	
	bSizer35->Add( 10, 0, 0, wxEXPAND, 0 );
	
	m_OrbitCameraToggleButton = new wxBitmapToggleButton( m_ToolPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer35->Add( m_OrbitCameraToggleButton, 0, wxALL, 2 );
	
	m_FrontCameraToggleButton = new wxBitmapToggleButton( m_ToolPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer35->Add( m_FrontCameraToggleButton, 0, wxALL, 2 );
	
	m_SideCameraToggleButton = new wxBitmapToggleButton( m_ToolPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer35->Add( m_SideCameraToggleButton, 0, wxALL, 2 );
	
	m_TopCameraToggleButton = new wxBitmapToggleButton( m_ToolPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer35->Add( m_TopCameraToggleButton, 0, wxALL, 2 );
	
	
	bSizer35->Add( 10, 0, 0, wxEXPAND, 0 );
	
	m_ShowAxesToggleButton = new wxBitmapToggleButton( m_ToolPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer35->Add( m_ShowAxesToggleButton, 0, wxALL, 2 );
	
	m_ShowGridToggleButton = new wxBitmapToggleButton( m_ToolPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer35->Add( m_ShowGridToggleButton, 0, wxALL, 2 );
	
	m_ShowBoundsToggleButton = new wxBitmapToggleButton( m_ToolPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer35->Add( m_ShowBoundsToggleButton, 0, wxALL, 2 );
	
	m_ShowStatisticsToggleButton = new wxBitmapToggleButton( m_ToolPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer35->Add( m_ShowStatisticsToggleButton, 0, wxALL, 2 );
	
	
	bSizer35->Add( 10, 0, 0, wxEXPAND, 0 );
	
	m_FrustumCullingToggleButton = new wxBitmapToggleButton( m_ToolPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer35->Add( m_FrustumCullingToggleButton, 0, wxALL, 2 );
	
	m_BackfaceCullingToggleButton = new wxBitmapToggleButton( m_ToolPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer35->Add( m_BackfaceCullingToggleButton, 0, wxALL, 2 );
	
	
	bSizer35->Add( 10, 0, 0, wxEXPAND, 0 );
	
	m_WireframeShadingToggleButton = new wxBitmapToggleButton( m_ToolPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer35->Add( m_WireframeShadingToggleButton, 0, wxALL, 2 );
	
	m_MaterialShadingToggleButton = new wxBitmapToggleButton( m_ToolPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer35->Add( m_MaterialShadingToggleButton, 0, wxALL, 2 );
	
	
	bSizer35->Add( 10, 0, 0, wxEXPAND, 0 );
	
	m_ColorModeSceneToggleButton = new wxBitmapToggleButton( m_ToolPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer35->Add( m_ColorModeSceneToggleButton, 0, wxALL, 2 );
	
	m_ColorModeLayerToggleButton = new wxBitmapToggleButton( m_ToolPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer35->Add( m_ColorModeLayerToggleButton, 0, wxALL, 2 );
	
	m_ColorModeNodeTypeToggleButton = new wxBitmapToggleButton( m_ToolPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer35->Add( m_ColorModeNodeTypeToggleButton, 0, wxALL, 2 );
	
	m_ColorModeScaleToggleButton = new wxBitmapToggleButton( m_ToolPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer35->Add( m_ColorModeScaleToggleButton, 0, wxALL, 2 );
	
	m_ColorModeScaleGradientToggleButton = new wxBitmapToggleButton( m_ToolPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer35->Add( m_ColorModeScaleGradientToggleButton, 0, wxALL, 2 );
	
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
	
	// Connect Events
	m_FrameOriginButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnFrameOrigin ), NULL, this );
	m_FrameSelectedButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnFrameSelected ), NULL, this );
	m_PreviousViewButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnPreviousView ), NULL, this );
	m_NextViewButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnNextView ), NULL, this );
	m_HighlightModeToggleButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnHighlightMode ), NULL, this );
	m_OrbitCameraToggleButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnViewCameraChange ), NULL, this );
	m_FrontCameraToggleButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnViewCameraChange ), NULL, this );
	m_SideCameraToggleButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnViewCameraChange ), NULL, this );
	m_TopCameraToggleButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnViewCameraChange ), NULL, this );
	m_ShowAxesToggleButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnViewChange ), NULL, this );
	m_ShowGridToggleButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnViewChange ), NULL, this );
	m_ShowBoundsToggleButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnViewChange ), NULL, this );
	m_ShowStatisticsToggleButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnViewChange ), NULL, this );
	m_FrustumCullingToggleButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnViewChange ), NULL, this );
	m_BackfaceCullingToggleButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnViewChange ), NULL, this );
	m_WireframeShadingToggleButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnViewChange ), NULL, this );
	m_MaterialShadingToggleButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnViewChange ), NULL, this );
	m_ColorModeSceneToggleButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnColorMode ), NULL, this );
	m_ColorModeLayerToggleButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnColorMode ), NULL, this );
	m_ColorModeNodeTypeToggleButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnColorMode ), NULL, this );
	m_ColorModeScaleToggleButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnColorMode ), NULL, this );
	m_ColorModeScaleGradientToggleButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnColorMode ), NULL, this );
}

ViewPanelGenerated::~ViewPanelGenerated()
{
	// Disconnect Events
	m_FrameOriginButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnFrameOrigin ), NULL, this );
	m_FrameSelectedButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnFrameSelected ), NULL, this );
	m_PreviousViewButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnPreviousView ), NULL, this );
	m_NextViewButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnNextView ), NULL, this );
	m_HighlightModeToggleButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnHighlightMode ), NULL, this );
	m_OrbitCameraToggleButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnViewCameraChange ), NULL, this );
	m_FrontCameraToggleButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnViewCameraChange ), NULL, this );
	m_SideCameraToggleButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnViewCameraChange ), NULL, this );
	m_TopCameraToggleButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnViewCameraChange ), NULL, this );
	m_ShowAxesToggleButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnViewChange ), NULL, this );
	m_ShowGridToggleButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnViewChange ), NULL, this );
	m_ShowBoundsToggleButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnViewChange ), NULL, this );
	m_ShowStatisticsToggleButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnViewChange ), NULL, this );
	m_FrustumCullingToggleButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnViewChange ), NULL, this );
	m_BackfaceCullingToggleButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnViewChange ), NULL, this );
	m_WireframeShadingToggleButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnViewChange ), NULL, this );
	m_MaterialShadingToggleButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnViewChange ), NULL, this );
	m_ColorModeSceneToggleButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnColorMode ), NULL, this );
	m_ColorModeLayerToggleButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnColorMode ), NULL, this );
	m_ColorModeNodeTypeToggleButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnColorMode ), NULL, this );
	m_ColorModeScaleToggleButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnColorMode ), NULL, this );
	m_ColorModeScaleGradientToggleButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewPanelGenerated::OnColorMode ), NULL, this );
	
}
