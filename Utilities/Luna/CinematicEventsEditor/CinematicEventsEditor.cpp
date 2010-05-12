#include "Precompile.h"
#include "CinematicEventsEditor.h"

#include "CinematicMainPanel.h"
#include "CinematicActiveEventsPanel.h"
#include "CinematicTimelinePanel.h"
#include "CinematicPropertyPanel.h"
#include "CinematicEventsEditorIDs.h"
#include "CinematicPreferences.h"
#include "Browser/BrowserToolBar.h"

#include "Asset/AssetClass.h"
#include "Asset/AssetInit.h"

#include "File/Manager.h"
#include "FileBrowser/FileBrowser.h"
#include "FileSystem/FileSystem.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ExtensionSpecs.h"
#include "Finder/Finder.h"
#include "Console/Console.h"
#include "Editor/SessionManager.h"
#include "UIToolKit/FileDialog.h"
#include "UIToolKit/ImageManager.h"
#include "UIToolKit/MenuMRU.h"
#include "Symbol/SymbolBuilder.h"

// Using
using namespace Luna;
using namespace Undo;
using namespace Reflect;
using namespace UIToolKit;
using namespace Asset;

// Static event table
BEGIN_EVENT_TABLE( CinematicEventsEditor, Editor )
  EVT_MENU_OPEN( OnMenuOpen )
  EVT_MENU( wxID_OPEN, OnOpen )
  EVT_MENU( CinematicEventsEditorIDs::SearchForFile, OnFind )
  EVT_MENU( wxID_SAVE, OnSave )
  EVT_MENU( wxID_CLOSE, OnClose )
  EVT_MENU( wxID_EXIT, OnExit )
  EVT_MENU( wxID_UNDO, OnUndo )
  EVT_MENU( wxID_REDO, OnRedo )
  EVT_MENU( wxID_CUT, OnCut )
  EVT_MENU( wxID_COPY, OnCopy )
  EVT_MENU( wxID_PASTE, OnPaste )
  EVT_MENU( CinematicEventsEditorIDs::SelectAll, OnSelectAll )
  EVT_CLOSE( OnExiting )
  EVT_CHAR( OnChar )
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
CinematicEventsEditor::CinematicEventsEditor()
: Editor( EditorTypes::CinematicEvents, NULL, wxID_ANY, wxT( "Luna Cinematic Events Editor" ), wxDefaultPosition, wxSize( 800, 600 ), wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER )
, m_Manager( this )
, m_MRU( new UIToolKit::MenuMRU( 20, this ) )
, m_StandardToolBar( NULL )
, m_MenuFile( new wxMenu() )
, m_MenuPanels( new wxMenu() )
, m_MenuItemOpenRecent( NULL )
, m_MenuMRU( new wxMenu() )
, m_MainPanel( new CinematicMainPanel( m_Manager, new CinematicPanel( this ) ) )
, m_ActiveEventsPanel( new CinematicActiveEventsPanel( m_Manager, new CinematicActivePanel( this ) ) )
, m_TimelinePanel( new CinematicEventsTimelinePanel( m_Manager, new CinematicTimelinePanel( this ) ) )
, m_PropertyPanel( new CinematicPropertyPanel( m_Manager, new wxPanel( this ) ) )
{
  wxIconBundle iconBundle;
  wxIcon tempIcon;
  tempIcon.CopyFromBitmap( UIToolKit::GlobalImageManager().GetBitmap( "cinematic_events_editor_128.png" ) );
  iconBundle.AddIcon( tempIcon );
  tempIcon.CopyFromBitmap( UIToolKit::GlobalImageManager().GetBitmap( "cinematic_events_editor_64.png" ) );
  iconBundle.AddIcon( tempIcon );
  tempIcon.CopyFromBitmap( UIToolKit::GlobalImageManager().GetBitmap( "cinematic_events_editor_32.png" ) );
  iconBundle.AddIcon( tempIcon );
  tempIcon.CopyFromBitmap( UIToolKit::GlobalImageManager().GetBitmap( "cinematic_events_editor_16.png" ) );
  iconBundle.AddIcon( tempIcon );
  SetIcons( iconBundle );

  // Toolbars
  m_StandardToolBar = new wxToolBar( this, -1, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER | wxTB_HORZ_TEXT );
  m_StandardToolBar->SetToolBitmapSize( wxSize( 16,16));
  m_StandardToolBar->AddTool( wxID_OPEN, wxT( "Open" ), wxArtProvider::GetBitmap( wxART_FILE_OPEN, wxART_OTHER, wxSize( 16, 16 ) ) );
  m_StandardToolBar->AddTool( CinematicEventsEditorIDs::SearchForFile, wxT( "Find..." ), wxArtProvider::GetBitmap( wxART_FIND, wxART_OTHER, wxSize( 16, 16 ) ) );
  m_StandardToolBar->AddTool( wxID_SAVE, wxT( "Save" ), wxArtProvider::GetBitmap( wxART_FILE_SAVE, wxART_OTHER, wxSize( 16, 16 ) ) );
  m_StandardToolBar->AddSeparator();
  m_StandardToolBar->AddTool( wxID_CUT, wxT( "Cut" ), wxArtProvider::GetBitmap( wxART_CUT, wxART_OTHER, wxSize( 16, 16 ) ) );
  m_StandardToolBar->AddTool( wxID_COPY, wxT( "Copy" ), wxArtProvider::GetBitmap( wxART_COPY, wxART_OTHER, wxSize(16, 16 ) ) );
  m_StandardToolBar->AddTool( wxID_PASTE, wxT( "Paste" ), wxArtProvider::GetBitmap( wxART_PASTE, wxART_OTHER, wxSize(16, 16 ) ) );
  m_StandardToolBar->AddSeparator();
  m_StandardToolBar->AddTool( wxID_UNDO, wxT( "Undo" ), wxArtProvider::GetBitmap( wxART_UNDO, wxART_OTHER, wxSize(16, 16 ) ) );
  m_StandardToolBar->AddTool( wxID_REDO, wxT( "Redo" ), wxArtProvider::GetBitmap( wxART_REDO, wxART_OTHER, wxSize(16, 16 ) ) );

  m_StandardToolBar->Realize();

  m_BrowserToolBar = new BrowserToolBar( this );
  m_BrowserToolBar->Realize();

  // Attach everything to the frame manager
  {
    wxAuiPaneInfo info; 
    info.Name( wxT( "standardtoolbar" ) ); 
    info.DestroyOnClose( false ); 
    info.Caption( wxT( "Standard Toolbar" ) ); 
    info.ToolbarPane(); 
    info.Top(); 
    info.LeftDockable( false ); 
    info.RightDockable( false ); 

    m_FrameManager.AddPane( m_StandardToolBar, info ); 
  }

  {
    m_FrameManager.AddPane( m_BrowserToolBar, m_BrowserToolBar->GetAuiPaneInfo( 2 ) ); 
  }

  {
    wxAuiPaneInfo info; 
    info.Name( wxT( "main" ) ); 
    info.DestroyOnClose( false ); 
    info.Caption( wxT( "Main" ) ); 
    info.CenterPane(); 
      
    m_FrameManager.AddPane( m_MainPanel->GetPanel(), info ); 
  
  }

  // properties panel is on top of the active events panel, by default
  {
    wxAuiPaneInfo info; 
    info.Name( wxT( "properties" ) ); 
    info.DestroyOnClose( false ); 
    info.Caption( wxT( "Selection Properties" ) ); 
    info.Right(); 
    info.Layer( 1 ); 
    info.Position( 1 ); 
    info.MinSize( 300, 300 ); 

    m_FrameManager.AddPane( m_PropertyPanel->GetPanel(), info ); 
  }


  {
    wxAuiPaneInfo info; 
    info.Name( wxT( "activeevents" ) ); 
    info.DestroyOnClose( false ); 
    info.Caption( wxT( "Active Events" ) ); 
    info.Right(); 
    info.Layer( 1 ); 
    info.Position( 2 ); 
    info.MinSize( 200, 200 ); 

    m_FrameManager.AddPane( m_ActiveEventsPanel->GetPanel(), info ); 
  }


  {
    wxAuiPaneInfo info; 
    info.Name( wxT( "timeline" ) );
    info.DestroyOnClose( false ); 
    info.Caption( wxT( "Timeline" ) ); 
    info.Bottom(); 
    info.Layer( 0 ); 
    info.Position( 1 ); 

    m_FrameManager.AddPane( m_TimelinePanel->GetPanel(), info ); 
  }
  
  // Menus
  wxMenuBar* menuBar = new wxMenuBar();

  // File menu
  m_MenuFile->Append( wxID_OPEN, "Open\tCtrl+O" );
  m_MenuItemOpenRecent = m_MenuFile->AppendSubMenu( m_MenuMRU, "Open Recent" );
  m_MenuFile->Append( CinematicEventsEditorIDs::SearchForFile, "Find..." );
  m_MenuFile->AppendSeparator();
  m_MenuFile->Append( wxID_SAVE, "Save\tCtrl+S" );
  m_MenuFile->AppendSeparator();
  m_MenuFile->Append( wxID_CLOSE, "Close" );
  m_MenuFile->AppendSeparator();
  m_MenuFile->Append( wxID_EXIT, "Exit" );
  menuBar->Append( m_MenuFile, "File" );


  // Edit menu
  wxMenu* editMenu = new wxMenu();
  editMenu->Append( wxID_UNDO, "Undo\tCtrl+Z" );
  editMenu->Append( wxID_REDO, "Redo\tCtrl+Y" );
  editMenu->AppendSeparator();
  editMenu->Append( wxID_CUT, "Cut\tCtrl+X" );
  editMenu->Append( wxID_COPY, "Copy\tCtrl+C" );
  editMenu->Append( wxID_PASTE, "Paste\tCtrl+V" );
  editMenu->AppendSeparator();
  editMenu->Append( CinematicEventsEditorIDs::SelectAll, "Select All\tCtrl+A" );
  menuBar->Append( editMenu, "Edit" );

  // Create menu items for all the panels so that they can be shown and hidden.  The base
  // class has functions to do this for us.
  CreatePanelsMenu( m_MenuPanels );
  menuBar->Append( m_MenuPanels, "Panels" );

  // Status bar and menu bar
  CreateStatusBar();
  SetMenuBar( menuBar );

  // Restore layout if any
  CinematicEditorPreferences()->GetCinematicEditorWindowSettings()->ApplyToWindow( this, &m_FrameManager, true );

  // MRU callback
  m_MRU->AddItemSelectedListener( UIToolKit::MRUSignature::Delegate( this, &CinematicEventsEditor::OnMRUMenuItem ) );
  m_MRU->FromVector( CinematicEditorPreferences()->GetMRU()->GetPaths() );

  // Disable certain toolbar buttons (they'll enable when appropriate)
  m_StandardToolBar->EnableTool( wxID_SAVE, false );
  m_MenuFile->Enable( wxID_SAVE, false );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
CinematicEventsEditor::~CinematicEventsEditor()
{
  m_MRU->RemoveItemSelectedListener( UIToolKit::MRUSignature::Delegate( this, &CinematicEventsEditor::OnMRUMenuItem ) );

  // Save preferences and MRU
  V_string mruPaths;
  m_MRU->ToVector( mruPaths );
  CinematicEditorPreferences()->GetMRU()->SetPaths( mruPaths );
  CinematicEditorPreferences()->SavePreferences();

  delete m_MainPanel;
  delete m_TimelinePanel;
  delete m_ActiveEventsPanel;
  delete m_PropertyPanel;
}

///////////////////////////////////////////////////////////////////////////////
// Static creator function for making an Asset Editor.
// 
static Editor* CreateCinematicEventsEditor()
{
  return new CinematicEventsEditor();
}

///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void CinematicEventsEditor::InitializeEditor()
{
  static Finder::FilterSpec s_Filter( "CinematicEventsEditor::s_Filter", "Cinematic" );
  s_Filter.AddSpec( FinderSpecs::Asset::CINEMATIC_DECORATION );
  SessionManager::GetInstance()->RegisterEditor( new EditorInfo( EditorTypes::CinematicEvents, &CreateCinematicEventsEditor, &s_Filter ) );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void CinematicEventsEditor::CleanupEditor()
{
}

///////////////////////////////////////////////////////////////////////////////
// Save current window state to the preferences.
// 
void CinematicEventsEditor::SaveWindowState()
{
  CinematicEditorPreferences()->GetCinematicEditorWindowSettings()->SetFromWindow( this, &m_FrameManager );
}

DocumentManager* CinematicEventsEditor::GetDocumentManager()
{
  return &m_Manager;
}

///////////////////////////////////////////////////////////////////////////////
// Overridden from Editor.  Opens the specified file (which should be an 
// asset class) and returns a pointer to the editor file.
// 
void CinematicEventsEditor::PerformOpen( const std::string& path )
{
  if ( !path.empty() && FileSystem::Exists( path ) )
  {
    std::string error;
    DocumentPtr doc = m_Manager.OpenPath( path, error );
    if ( doc )
    {
      doc->AddDocumentModifiedListener( DocumentChangedSignature::Delegate( this, &CinematicEventsEditor::OnDocumentModified ) );
      doc->AddDocumentSavedListener( DocumentChangedSignature::Delegate( this, &CinematicEventsEditor::OnDocumentModified ) );
      doc->AddDocumentClosedListener( DocumentChangedSignature::Delegate( this, &CinematicEventsEditor::OnDocumentClosed ) );
    }
    else if ( !error.empty() )
    {
      m_MRU->Remove( path );
      wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, this );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Called when a menu is about to be opened.  Enables and disables items in the
// menu as appropriate.
// 
void CinematicEventsEditor::OnMenuOpen( wxMenuEvent& args )
{
  args.Skip();
  if ( args.GetMenu() == m_MenuFile )
  {
    m_MenuFile->Enable( m_MenuItemOpenRecent->GetId(), !m_MRU->GetItems().Empty() );
    m_MRU->PopulateMenu( m_MenuMRU );
  }
  else if ( args.GetMenu() == m_MenuPanels )
  {
    UpdatePanelsMenu( m_MenuPanels );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when one of the items on the "most recently used" menu is 
// clicked on.  Attempts to open the file.
// 
void CinematicEventsEditor::OnMRUMenuItem( const UIToolKit::MRUArgs& args )
{
  PerformOpen( args.m_Item );
}

///////////////////////////////////////////////////////////////////////////////
// Callback when the user causes a UI event to open a file.  Prompts for the
// file to open, and opens it.
// 
void CinematicEventsEditor::OnOpen( wxCommandEvent& args )
{
  FileDialog browserDlg( this, "Open", Finder::ProjectAssets().c_str(), "", "", FileDialogStyles::DefaultOpen | FileDialogStyles::ShowAllFilesFilter );

  browserDlg.SetFilter( FinderSpecs::Asset::CINEMATIC_DECORATION.GetDialogFilter() );

  if ( browserDlg.ShowModal() == wxID_OK )
  {
    PerformOpen( browserDlg.GetPath().c_str() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback when the user causes a UI event to find a file. 
// creates a FileBrowser dialog.
// 
void CinematicEventsEditor::OnFind( wxCommandEvent& args )
{
  File::FileBrowser browserDlg( this, -1, "Open" );

  browserDlg.SetFilter( FinderSpecs::Asset::CINEMATIC_DECORATION );

  if ( browserDlg.ShowModal() == wxID_OK )
  {
    PerformOpen( browserDlg.GetPath() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// UI callback when the users requests that an asset class be closed.
// 
void CinematicEventsEditor::OnClose( wxCommandEvent& args )
{
  m_Manager.Close();
}


///////////////////////////////////////////////////////////////////////////////
// Called when a user selects "Exit" from the File menu. Creates a close window
// event which should be processed by the OnExisting Editor function.
// 
void CinematicEventsEditor::OnExit( wxCommandEvent& args )
{
  wxCloseEvent closeEvent( wxEVT_CLOSE_WINDOW );
  GetEventHandler()->AddPendingEvent( closeEvent );
}

///////////////////////////////////////////////////////////////////////////////
// Saves the current moby's animation data
// 
void CinematicEventsEditor::OnSave( wxCommandEvent& args )
{
  std::string error;
  if ( !m_Manager.Save( error ) && !error.empty() )
  {
    wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, this );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Called when the UI initiates an Undo operation.
// 
void CinematicEventsEditor::OnUndo( wxCommandEvent& args )
{
  m_Manager.GetUndoQueue().Undo();
}

///////////////////////////////////////////////////////////////////////////////
// Called when the UI initiates a Redo operation.  
// 
void CinematicEventsEditor::OnRedo( wxCommandEvent& args )
{
  m_Manager.GetUndoQueue().Redo();
}

///////////////////////////////////////////////////////////////////////////////
// Called when the Cut button or menu item is selected.
// 
void CinematicEventsEditor::OnCut( wxCommandEvent& args )
{
  m_Manager.Cut();
}

///////////////////////////////////////////////////////////////////////////////
// Called when the Copy button or menu item is selected.
// 
void CinematicEventsEditor::OnCopy( wxCommandEvent& args )
{
  m_Manager.Copy();
}

///////////////////////////////////////////////////////////////////////////////
// Called when the Paste button or menu item is selected.
// 
void CinematicEventsEditor::OnPaste( wxCommandEvent& args )
{
  m_Manager.Paste();
}

///////////////////////////////////////////////////////////////////////////////
// Keyboard shortcuts.
// 
void CinematicEventsEditor::OnChar( wxKeyEvent& event )
{
  event.Skip();

  if ( !m_Manager.GetCurrentClip().ReferencesObject() )
  {
    return;
  }

  switch (event.KeyCode())
  {
  case WXK_INSERT:
    m_MainPanel->AddEvent();
    break;

  case WXK_DELETE:
    m_Manager.DeleteSelected();
    break;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Closes all files and exits the app.
// 
void CinematicEventsEditor::OnExiting( wxCloseEvent &args )
{
  if ( !m_Manager.Close() )
  {
    if ( args.CanVeto() )
    {
      args.Veto();
      return;
    }
  }

  args.Skip();
}

void CinematicEventsEditor::OnSelectAll( wxCommandEvent& e )
{
  m_Manager.SelectAll();
}

void CinematicEventsEditor::OnDocumentModified( const DocumentChangedArgs& args )
{
  bool doAnyDocsNeedSaved = false;
  OS_DocumentSmartPtr::Iterator docItr = m_Manager.GetDocuments().Begin();
  OS_DocumentSmartPtr::Iterator docEnd = m_Manager.GetDocuments().End();
  for ( ; docItr != docEnd; ++docItr )
  {
    if ( ( *docItr )->IsModified() )
    {
      doAnyDocsNeedSaved = true;
      break;
    }
  }
  
  m_StandardToolBar->EnableTool( wxID_SAVE, doAnyDocsNeedSaved );
  m_MenuFile->Enable( wxID_SAVE, doAnyDocsNeedSaved );
}

void CinematicEventsEditor::OnDocumentClosed( const DocumentChangedArgs& args )
{
  OnDocumentModified( args );

  const Document* doc = args.m_Document;
  doc->RemoveDocumentModifiedListener( DocumentChangedSignature::Delegate( this, &CinematicEventsEditor::OnDocumentModified ) );
  doc->RemoveDocumentSavedListener( DocumentChangedSignature::Delegate( this, &CinematicEventsEditor::OnDocumentModified ) );
  doc->RemoveDocumentClosedListener( DocumentChangedSignature::Delegate( this, &CinematicEventsEditor::OnDocumentClosed ) );
}
