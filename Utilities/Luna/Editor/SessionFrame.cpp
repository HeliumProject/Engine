#include "Precompile.h"
#include "SessionFrame.h"

#include "ApplicationPreferences.h"
#include "SubmitChangesDialog.h"
#include "EditorChooser.h"
#include "SessionManager.h"

#include "Browser/BrowserToolBar.h"
#include "FileSystem/FileSystem.h"
#include "Finder/ExtensionSpecs.h"
#include "Finder/LunaSpecs.h"
#include "UIToolKit/FileDialog.h"
#include "UIToolKit/ImageManager.h"

// Using
using namespace Luna;
using namespace UIToolKit;

// Static event table
BEGIN_EVENT_TABLE(SessionFrame, wxFrame)
EVT_MENU( wxID_OPEN, OnOpen )
EVT_MENU( wxID_SAVE, OnSaveSession )
EVT_MENU( wxID_SAVEAS, OnSaveSessionAs )
EVT_MENU( wxID_EXIT, OnExit )
END_EVENT_TABLE()

// Constants
static const char* s_FrameTitle = "Luna Session Manager";

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
SessionFrame::SessionFrame()
: Frame( NULL, wxID_ANY, wxT( s_FrameTitle ), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER )
, m_FileMenu( NULL )
, m_ToolBar( NULL )
{
  SetTitle( Frame::GetEShellTitle( s_FrameTitle ).c_str() );

  wxIconBundle iconBundle;
  wxIcon tempIcon;
  tempIcon.CopyFromBitmap( UIToolKit::GlobalImageManager().GetBitmap( "session_manager_128.png" ) );
  iconBundle.AddIcon( tempIcon );
  tempIcon.CopyFromBitmap( UIToolKit::GlobalImageManager().GetBitmap( "session_manager_64.png" ) );
  iconBundle.AddIcon( tempIcon );
  tempIcon.CopyFromBitmap( UIToolKit::GlobalImageManager().GetBitmap( "session_manager_32.png" ) );
  iconBundle.AddIcon( tempIcon );
  tempIcon.CopyFromBitmap( UIToolKit::GlobalImageManager().GetBitmap( "session_manager_16.png" ) );
  iconBundle.AddIcon( tempIcon );
  SetIcons( iconBundle );

  // Menus
  wxMenuBar* menuBar = new wxMenuBar();

  // File menu
  m_FileMenu = new wxMenu();
  m_FileMenu->Append( wxID_OPEN, "Open" );
  wxMenuItem* saveAllItem = m_FileMenu->Append( wxID_ANY, "Save All" );
  Connect( saveAllItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SessionFrame::OnSaveAll ), NULL, this );
  m_FileMenu->Append( wxID_SAVE, "Save Session" );
  m_FileMenu->Append( wxID_SAVEAS, "Save Session As..." );
  m_FileMenu->AppendSeparator();
  wxMenuItem* commitItem = m_FileMenu->Append( wxID_ANY, "Commit...", "Check all files in default changelist into revision control." );
  Connect( commitItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SessionFrame::OnCheckin ), NULL, this );
  m_FileMenu->AppendSeparator();
  m_FileMenu->Append( wxID_EXIT, "Exit" );
  menuBar->Append( m_FileMenu, "File" );

  // Toolbars
  m_ToolBar = new wxToolBar( this, -1, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER | wxTB_HORZ_TEXT );
  m_ToolBar->SetToolBitmapSize( wxSize( 16,16 ) );
  m_ToolBar->AddTool( wxID_OPEN, wxT( "Open" ), wxArtProvider::GetBitmap( wxART_FILE_OPEN, wxART_OTHER, wxSize( 16, 16 ) ) );
  m_ToolBar->AddTool( saveAllItem->GetId(), wxT( "Save All" ), UIToolKit::GlobalImageManager().GetBitmap( "save_all_16.png" ) );
  m_ToolBar->Realize();
  
  BrowserToolBar* browserToolBar = new BrowserToolBar( this );
  browserToolBar->Realize();

  m_FrameManager.AddPane(m_ToolBar, wxAuiPaneInfo().
    Name(wxT("standard")).Caption(wxT("Standard")).
    ToolbarPane().Top().Gripper(false).Floatable(false));

  m_FrameManager.AddPane(browserToolBar, wxAuiPaneInfo().
    Name(wxT("browser")).Caption(wxT("Browser")).
    ToolbarPane().Top().Row(2).Gripper(false).Floatable(false));

  wxPanel* editorChooserPanel = new wxPanel( this );
  editorChooserPanel->SetSizer( new wxBoxSizer( wxVERTICAL ) );

  EditorChooser* editorChooser = new EditorChooser( editorChooserPanel );
  editorChooserPanel->GetSizer()->Add( editorChooser, 1, wxEXPAND );

  m_FrameManager.AddPane(editorChooserPanel, wxAuiPaneInfo().Name(wxT("editor_chooser")).CenterPane());

  // Size dialog based on panel's size
  wxSize offset( 10, 126 ); // Accommodate borders, tabs, menus
  SetSize( editorChooser->GetSize() + offset ); 
  SetMinSize( editorChooser->GetMinSize() + offset );
  
  Layout();

  // Attach everything to the frame
  CreateStatusBar();
  SetMenuBar( menuBar );

  GetApplicationPreferences()->GetSessionFrameSettings()->ApplyToWindow( this );
  
  m_FrameManager.Update();
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
SessionFrame::~SessionFrame()
{
  // This was suppose to be called automatically by Frame, but  this window
  // appears to be special in that the OnExiting callback is never called for
  // it.
  SaveWindowState(); 
}

///////////////////////////////////////////////////////////////////////////////
// Apply the current window settings to the data that is saved in the preferences.
// 
void SessionFrame::SaveWindowState()
{
  GetApplicationPreferences()->GetSessionFrameSettings()->SetFromWindow( this );
}

///////////////////////////////////////////////////////////////////////////////
// Used by Frame::PrefixPreferenceKey
// 
const std::string& SessionFrame::GetPreferencePrefix() const
    {
  static const std::string preferencesPrefix = "SessionManager";
  return preferencesPrefix;
}

///////////////////////////////////////////////////////////////////////////////
// If there is currently a session file specified in the session manager, this
// file will be saved.  If there is no file specified, the user will be prompted
// as to whether or not they want to save a session file.  Then, if the user 
// chooses to save a session file, they will be prompted for the location.
// 
void SessionFrame::PromptSaveSession()
{
  const std::string& sessionPath = SessionManager::GetInstance()->GetSessionPath();
  if ( !sessionPath.empty() )
  {
    SessionManager::GetInstance()->SaveSession( sessionPath );
  }
  else
  {
    const wxString& msg( "Would you like to save your session?" );
    const wxString& title( "Save Session?" );
    if ( wxMessageBox( msg, title, wxYES_NO | wxCENTER | wxICON_QUESTION, this ) == wxYES )
    {
      OnSaveSessionAs( wxCommandEvent() );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// UI callback for when this frame generates an open command event.  Allows the
// user to open a session file, or any other file which will be opened with the
// appropriate editor.
// 
void SessionFrame::OnOpen( wxCommandEvent& args )
{
  std::string path;
  FileDialog fileDialog( this, "Open..." );
  fileDialog.AddFilter( FinderSpecs::Luna::SESSION_DECORATION.GetDialogFilter() );
  fileDialog.AddFilter( FinderSpecs::Extension::REFLECT_BINARY.GetDialogFilter() );
  fileDialog.SetFilterIndex( FinderSpecs::Luna::SESSION_DECORATION.GetDialogFilter() );
  if ( fileDialog.ShowModal() == wxID_OK )
  {
    path = fileDialog.GetPath().c_str();
    SessionManager::GetInstance()->Edit( path );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Save all currently open documents in all editors.
// 
void SessionFrame::OnSaveAll( wxCommandEvent& args )
{
  SessionManager::GetInstance()->SaveAllOpenDocuments();
  PromptSaveSession();
}

///////////////////////////////////////////////////////////////////////////////
// UI callback for when this frame generates a save command event.  Prompts to 
// save a session file.
// 
void SessionFrame::OnSaveSession( wxCommandEvent& args )
{
  std::string sessionPath = SessionManager::GetInstance()->GetSessionPath();
  if ( sessionPath.empty() )
  {
    OnSaveSessionAs( args );
  }
  else
  {
    SessionManager::GetInstance()->SaveSession( sessionPath );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user chooses the "save as" option in the UI.  Prompts
// the user to save a session file to a new location.
// 
void SessionFrame::OnSaveSessionAs( wxCommandEvent& args )
{
  // Prompt
  FileDialog fileDialog( this, "Save Session As...", "", "", "", wxSAVE | wxOVERWRITE_PROMPT );
  fileDialog.SetFilter( FinderSpecs::Luna::SESSION_DECORATION.GetDialogFilter() );
  if ( fileDialog.ShowModal() == wxID_OK )
  {
    // If new file name, save
    std::string savePath = fileDialog.GetPath().c_str();
    if ( !savePath.empty() )
    {
      SessionManager::GetInstance()->SaveSession( savePath );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Called when the "commit" menu item is selected.
// 
void SessionFrame::OnCheckin( wxCommandEvent& args )
{
  if ( SessionManager::GetInstance()->SaveAllOpenDocuments() )
  {
    // Show the commit dialog
    SubmitChangesDialog dlg( this );
    dlg.ShowModal();
  }
  else
  {
    // There were errors, notify the user
    wxMessageBox( "Failed to save all open documents.", "Unable to Commit", wxCENTER | wxICON_WARNING | wxOK, this );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Closes this window.
// 
void SessionFrame::OnExit( wxCommandEvent& args )
{
  Close();
}
