#include "Precompile.h"

#include "AnimationEventsEditor.h"
#include "AnimationMainPanel.h"
#include "AnimationActiveEventsPanel.h"
#include "AnimationTimelinePanel.h"
#include "AnimationPropertyPanel.h"
#include "AnimationEventsPreferences.h"
#include "AnimationEventsEditorIDs.h"
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
#include "Symbol/SymbolBuilder.h"

// Using
using namespace Luna;
using namespace Undo;
using namespace Reflect;
using namespace UIToolKit;
using namespace Asset;

// Static event table
BEGIN_EVENT_TABLE( AnimationEventsEditor, Editor )
  EVT_MENU_OPEN( OnMenuOpen )
  EVT_MENU( wxID_OPEN, OnOpen )
  EVT_MENU( AnimationEventsEditorIDs::SearchForFile, OnFind )
  EVT_MENU( wxID_SAVE, OnSave )
  EVT_MENU( wxID_CLOSE, OnClose )
  EVT_MENU( wxID_EXIT, OnExit )
  EVT_MENU( wxID_UNDO, OnUndo )
  EVT_MENU( wxID_REDO, OnRedo )
  EVT_MENU( wxID_CUT, OnCut )
  EVT_MENU( wxID_COPY, OnCopy )
  EVT_MENU( wxID_PASTE, OnPaste )
  EVT_MENU( AnimationEventsEditorIDs::SelectAll, OnSelectAll )
  EVT_CLOSE( OnExiting )
  EVT_CHAR( OnChar )
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AnimationEventsEditor::AnimationEventsEditor()
: Editor( EditorTypes::AnimationEvents, NULL, wxID_ANY, wxT( "Luna Animation Events Editor" ), wxDefaultPosition, wxSize( 800, 600 ), wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER )
, m_Manager( this )
, m_MRU( new UIToolKit::MenuMRU( 20, this ) )
, m_StandardToolBar( NULL )
, m_MenuFile( new wxMenu() )
, m_MenuPanels( new wxMenu() )
, m_MenuItemOpenRecent( NULL )
, m_MenuMRU( new wxMenu() )
, m_MainPanel( new AnimationEventsMainPanel( m_Manager, new AnimationPanel( this ) ) )
, m_ActiveEventsPanel( new AnimationActiveEventsPanel( m_Manager, new AnimationActivePanel( this ) ) )
, m_TimelinePanel( new AnimationEventsTimelinePanel( m_Manager, new AnimationTimelinePanel( this ) ) )
, m_PropertyPanel( new AnimationEventsPropertyPanel( m_Manager, new wxPanel( this ) ) )
{
  wxIconBundle iconBundle;
  wxIcon tempIcon;
  tempIcon.CopyFromBitmap( UIToolKit::GlobalImageManager().GetBitmap( "events_editor_64.png" ) );
  iconBundle.AddIcon( tempIcon );
  tempIcon.CopyFromBitmap( UIToolKit::GlobalImageManager().GetBitmap( "events_editor_32.png" ) );
  iconBundle.AddIcon( tempIcon );
  tempIcon.CopyFromBitmap( UIToolKit::GlobalImageManager().GetBitmap( "events_editor_16.png" ) );
  iconBundle.AddIcon( tempIcon );
  SetIcons( iconBundle );

  // Toolbars
  m_StandardToolBar = new wxToolBar( this, -1, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER | wxTB_HORZ_TEXT );
  m_StandardToolBar->SetToolBitmapSize( wxSize( 16,16));
  m_StandardToolBar->AddTool( wxID_OPEN, wxT( "Open" ), wxArtProvider::GetBitmap( wxART_FILE_OPEN, wxART_OTHER, wxSize( 16, 16 ) ) );
  m_StandardToolBar->AddTool( AnimationEventsEditorIDs::SearchForFile, wxT( "Find..." ), wxArtProvider::GetBitmap( wxART_FIND, wxART_OTHER, wxSize( 16, 16 ) ) );
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
  m_MenuFile->Append( AnimationEventsEditorIDs::SearchForFile, "Find..." );
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
  editMenu->Append( AnimationEventsEditorIDs::SelectAll, "Select All\tCtrl+A" );
  menuBar->Append( editMenu, "Edit" );

  // Create menu items for all the panels so that they can be shown and hidden.  The base
  // class has functions to do this for us.
  CreatePanelsMenu( m_MenuPanels );
  menuBar->Append( m_MenuPanels, "Panels" );

  // Status bar and menu bar
  CreateStatusBar();
  SetMenuBar( menuBar );

  // Restore layout if any
  AnimationEventsEditorPreferences()->GetAnimationEventsWindowSettings()->ApplyToWindow( this, &m_FrameManager, true );

  // MRU callback
  m_MRU->AddItemSelectedListener( UIToolKit::MRUSignature::Delegate( this, &AnimationEventsEditor::OnMRUMenuItem ) );
  m_MRU->FromVector( AnimationEventsEditorPreferences()->GetMRU()->GetPaths() );
  
  // Disable certain toolbar buttons (they'll enable when appropriate)
  m_StandardToolBar->EnableTool( wxID_SAVE, false );
  m_MenuFile->Enable( wxID_SAVE, false );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
AnimationEventsEditor::~AnimationEventsEditor()
{
  m_MRU->RemoveItemSelectedListener( UIToolKit::MRUSignature::Delegate( this, &AnimationEventsEditor::OnMRUMenuItem ) );

  // Save preferences and MRU
  V_string mruPaths;
  m_MRU->ToVector( mruPaths );
  AnimationEventsEditorPreferences()->GetMRU()->SetPaths( mruPaths );
  AnimationEventsEditorPreferences()->SavePreferences();

  delete m_MainPanel;
  delete m_TimelinePanel;
  delete m_ActiveEventsPanel;
  delete m_PropertyPanel;
}

///////////////////////////////////////////////////////////////////////////////
// Static creator function for making an Animation Events Editor.
// 
static Editor* CreateAnimationEventsEditor()
{
  return new AnimationEventsEditor();
}

///////////////////////////////////////////////////////////////////////////////
// Static initialization function.
// 
void AnimationEventsEditor::InitializeEditor()
{
  static Finder::FilterSpec s_Filter( "AnimationEventsEditor::s_Filter", "Entity" );
  s_Filter.AddSpec( FinderSpecs::Asset::ENTITY_DECORATION );
  SessionManager::GetInstance()->RegisterEditor( new EditorInfo( EditorTypes::AnimationEvents, &CreateAnimationEventsEditor, &s_Filter ) );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup function.
// 
void AnimationEventsEditor::CleanupEditor()
{
}

///////////////////////////////////////////////////////////////////////////////
// Save current window settings to the preferences.
// 
void AnimationEventsEditor::SaveWindowState()
{
  AnimationEventsEditorPreferences()->GetAnimationEventsWindowSettings()->SetFromWindow( this, &m_FrameManager );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the document manager for this editor.
// 
DocumentManager* AnimationEventsEditor::GetDocumentManager() 
{
  return &m_Manager;
}

///////////////////////////////////////////////////////////////////////////////
// Open the specified file.
// 
void AnimationEventsEditor::PerformOpen( const std::string& path )
{
  if ( !path.empty() && FileSystem::Exists( path ) )
  {
    std::string error;
    DocumentPtr doc = m_Manager.OpenPath( path, error );
    if ( doc )
    {
      doc->AddDocumentModifiedListener( DocumentChangedSignature::Delegate( this, &AnimationEventsEditor::OnDocumentModified ) );
      doc->AddDocumentSavedListener( DocumentChangedSignature::Delegate( this, &AnimationEventsEditor::OnDocumentModified ) );
      doc->AddDocumentClosedListener( DocumentChangedSignature::Delegate( this, &AnimationEventsEditor::OnDocumentClosed ) );
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
void AnimationEventsEditor::OnMenuOpen( wxMenuEvent& args )
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
void AnimationEventsEditor::OnMRUMenuItem( const UIToolKit::MRUArgs& args )
{
  PerformOpen( args.m_Item );
}

///////////////////////////////////////////////////////////////////////////////
// Callback when the user causes a UI event to open a file.  Prompts for the
// file to open, and opens it.
// 
void AnimationEventsEditor::OnOpen( wxCommandEvent& args )
{
  FileDialog browserDlg( this, "Open", Finder::ProjectAssets().c_str(), "", "", FileDialogStyles::DefaultOpen | FileDialogStyles::ShowAllFilesFilter );

  browserDlg.SetFilter( FinderSpecs::Asset::ENTITY_DECORATION.GetDialogFilter() );

  if ( browserDlg.ShowModal() == wxID_OK )
  {
    PerformOpen( browserDlg.GetPath().c_str() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback when the user causes a UI event to find a file. 
// creates a FileBrowser dialog.
// 
void AnimationEventsEditor::OnFind( wxCommandEvent& args )
{
  File::FileBrowser browserDlg( this, -1, "Open" );

  browserDlg.SetFilter( FinderSpecs::Asset::ENTITY_DECORATION );

  if ( browserDlg.ShowModal() == wxID_OK )
  {
    PerformOpen( browserDlg.GetPath() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// UI callback when the users requests that an asset class be closed.
// 
void AnimationEventsEditor::OnClose( wxCommandEvent& args )
{
  m_Manager.Close();
}

///////////////////////////////////////////////////////////////////////////////
// Called when a user selects "Exit" from the File menu.  Fires an event to 
// shutdown this window.
// 
void AnimationEventsEditor::OnExit( wxCommandEvent& args )
{
  wxCloseEvent closeEvent( wxEVT_CLOSE_WINDOW );
  GetEventHandler()->AddPendingEvent( closeEvent );
}


///////////////////////////////////////////////////////////////////////////////
// Saves the current moby's animation data
// 
void AnimationEventsEditor::OnSave( wxCommandEvent& args )
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
void AnimationEventsEditor::OnUndo( wxCommandEvent& args )
{
  m_Manager.GetUndoQueue().Undo();
}

///////////////////////////////////////////////////////////////////////////////
// Called when the UI initiates a Redo operation.  
// 
void AnimationEventsEditor::OnRedo( wxCommandEvent& args )
{
  m_Manager.GetUndoQueue().Redo();
}

///////////////////////////////////////////////////////////////////////////////
// Called when the Cut button or menu item is selected.
// 
void AnimationEventsEditor::OnCut( wxCommandEvent& args )
{
  m_Manager.Cut();
}

///////////////////////////////////////////////////////////////////////////////
// Called when the Copy button or menu item is selected.
// 
void AnimationEventsEditor::OnCopy( wxCommandEvent& args )
{
  m_Manager.Copy();
}

///////////////////////////////////////////////////////////////////////////////
// Called when the Paste button or menu item is selected.
// 
void AnimationEventsEditor::OnPaste( wxCommandEvent& args )
{
  m_Manager.Paste();
}

///////////////////////////////////////////////////////////////////////////////
// Keyboard shortcuts.
// 
void AnimationEventsEditor::OnChar( wxKeyEvent& event )
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
void AnimationEventsEditor::OnExiting( wxCloseEvent &args )
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

void AnimationEventsEditor::OnSelectAll( wxCommandEvent& e )
{
  m_Manager.SelectAll();
}

void AnimationEventsEditor::OnDocumentModified( const DocumentChangedArgs& args )
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

void AnimationEventsEditor::OnDocumentClosed( const DocumentChangedArgs& args )
{
  OnDocumentModified( args );

  const Document* doc = args.m_Document;
  doc->RemoveDocumentModifiedListener( DocumentChangedSignature::Delegate( this, &AnimationEventsEditor::OnDocumentModified ) );
  doc->RemoveDocumentSavedListener( DocumentChangedSignature::Delegate( this, &AnimationEventsEditor::OnDocumentModified ) );
  doc->RemoveDocumentClosedListener( DocumentChangedSignature::Delegate( this, &AnimationEventsEditor::OnDocumentClosed ) );
}
