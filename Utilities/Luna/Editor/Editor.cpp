#include "Precompile.h"
#include "Editor.h"

#include "DocumentManager.h"
#include "SessionManager.h"

#include "FileSystem/FileSystem.h"
#include "Finder/LunaSpecs.h"
#include "RCS/RCS.h"
#include "UIToolKit/FileDialog.h"
#include "Windows/Process.h"
#include "Windows/Error.h"

// Using
using namespace Luna;


// Static UI event table
BEGIN_EVENT_TABLE( Editor, Frame )
END_EVENT_TABLE()


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
Editor::Editor( EditorType editorType, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: Frame( parent, id, title, pos, size, style, name )
, m_EditorType( editorType )
{
  SessionManager::GetInstance()->RegisterEditorInstance( this );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
Editor::~Editor()
{
  SessionManager::GetInstance()->UnregisterEditorInstance( this );
}

///////////////////////////////////////////////////////////////////////////////
// Returns appropriate state container to save session information to.  Derived
// classes should NOC_OVERRIDE this function and return a new EditorState derived
// class as needed.
// 
EditorStatePtr Editor::GetSessionState() 
{ 
  return new EditorState(); 
}

///////////////////////////////////////////////////////////////////////////////
// Saves the current session (a list of all open files), possibly prompting
// the user on where to save the session if it has never been saved before.
// 
void Editor::PromptSaveSession( bool forceSaveAs )
{
  static std::string sessionPath;
  if ( sessionPath.empty() || forceSaveAs )
  {
    UIToolKit::FileDialog fileDialog( this, "Save Session As...", "", "", "", wxSAVE | wxOVERWRITE_PROMPT );
    fileDialog.SetFilter( FinderSpecs::Luna::SESSION_DECORATION.GetDialogFilter() );
    if ( fileDialog.ShowModal() == wxID_OK )
    {
      // If new file name, save
      sessionPath = fileDialog.GetPath().c_str();
    }
  }

  if ( !sessionPath.empty() )
  {
    SessionManager::GetInstance()->SaveSession( sessionPath, GetEditorType() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Prompts the user to choose a session file to open.  
// 
void Editor::PromptLoadSession()
{
  UIToolKit::FileDialog fileDialog( this, "Open Session..." );
  fileDialog.AddFilter( FinderSpecs::Luna::SESSION_DECORATION.GetDialogFilter() );
  fileDialog.SetFilterIndex( FinderSpecs::Luna::SESSION_DECORATION.GetDialogFilter() );
  if ( fileDialog.ShowModal() == wxID_OK )
  {
    std::string path( fileDialog.GetPath().c_str() );
    
    if ( FileSystem::Exists( path ) )
    {
      SessionManager::GetInstance()->LoadSession( path );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Populate the EditorState with the list of currently open documents
// 
bool Editor::SaveSession( const EditorStatePtr& state )
{
  const OS_DocumentSmartPtr& documents = GetDocumentManager()->GetDocuments();
  
  OS_DocumentSmartPtr::Iterator itr = documents.Begin();
  OS_DocumentSmartPtr::Iterator end = documents.End();
  for ( ; itr != end; ++itr )
  {
      state->m_OpenFileRefs.insert( new File::Reference( (*itr)->GetFileReference() ) );
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
bool Editor::LoadSession( const EditorStatePtr& state )
{
  std::string error;
  bool openedAllFiles = true;

  for ( File::S_Reference::iterator itr = state->m_OpenFileRefs.begin(), end = state->m_OpenFileRefs.end(); itr != end; ++itr )
  {
    error.clear();
    (*itr)->Resolve();
    openedAllFiles &= GetDocumentManager()->OpenPath( (*itr)->GetPath(), error ) != NULL;
  }
  return openedAllFiles;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the type of this editor.
// 
EditorTypes::EditorType Editor::GetEditorType() const
{
  return m_EditorType;
}


///////////////////////////////////////////////////////////////////////////////
// Used by Frame::PrefixPreferenceKey
//
const std::string& Editor::GetPreferencePrefix() const
{
  if ( m_PreferencePrefix.empty() )
  {
    const Reflect::Enumeration* editorTypesEnum = Reflect::GetEnumeration< EditorTypes::EditorType >();
    NOC_ASSERT( editorTypesEnum ); 
    editorTypesEnum->GetElementLabel( GetEditorType(), m_PreferencePrefix );
  }
  return m_PreferencePrefix;
}

///////////////////////////////////////////////////////////////////////////////
// Uses p4win to display the revision history for the specified file.  Error
// checking is performed and messages are displayed as modal dialogs to this
// editor.
//
void Editor::RevisionHistory( const std::string& path )
{
  if ( path.empty() )
  {
    return;
  }

  std::string clean( path );
  FileSystem::CleanName( clean );
  if ( !RCS::PathIsManaged( clean ) )
  {
    std::string msg = std::string( "The path '" ) + clean + "' is not under revision control. Unable to display revision history.";
    wxMessageBox( msg.c_str(), "Warning", wxCENTER | wxICON_WARNING | wxOK, this );
    return;
  }

  std::string win32Name( clean );
  FileSystem::Win32Name( clean, win32Name );
  std::string command = std::string( "p4win.exe -H \"" ) + win32Name + std::string( "\"" );

  try 
  {
    Windows::Execute( command );
  }
  catch ( const Windows::Exception& e )
  {
    std::string error = e.Get();
    error += "\nMake sure that you have p4win properly installed.";
    wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, this );
    return;
  }
}

void Editor::OnPropertiesCreated( const PropertiesCreatedArgs& args )
{
  PostCommand( new PropertiesCreatedCommand( args.m_PropertiesManager, args.m_SelectionId, args.m_Controls ) );
}
