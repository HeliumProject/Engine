#include "Precompile.h"

#include "SessionManager.h"

#include "DocumentManager.h"
#include "Editor.h"
#include "SessionState.h"
#include "SessionVersion.h"

#include "Console/Console.h"
#include "FileSystem/FileSystem.h"
#include "Finder/ExtensionSpecs.h"
#include "Finder/LunaSpecs.h"
#include "Inspect/InspectInit.h"
#include "Windows/Process.h"
#include "RCS/RCS.h"

// Using
using Nocturnal::Insert; 
using namespace Luna;


///////////////////////////////////////////////////////////////////////////////
// Constructor - Private.  This is a singleton class, use GetInstance to access it.
// 
SessionManager::SessionManager()
{
  Inspect::g_EditFilePath.Add( Inspect::EditFilePathSignature::Delegate( this, &SessionManager::PropertiesPanelEdit ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
SessionManager::~SessionManager()
{
  Inspect::g_EditFilePath.Remove( Inspect::EditFilePathSignature::Delegate( this, &SessionManager::PropertiesPanelEdit ) );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the one and only instance of this class.
// 
SessionManager* SessionManager::GetInstance()
{
  static SessionManager theSessionManager;
  return &theSessionManager;
}

///////////////////////////////////////////////////////////////////////////////
// Loads the session information for all editors from a file.
// 
void SessionManager::LoadSession( const std::string& file )
{
  // Close any files and editors that are already open
  bool areDocsOpen = false;
  M_EditorDumbPtr::const_iterator editorItr = m_RunningEditors.begin();
  M_EditorDumbPtr::const_iterator editorEnd = m_RunningEditors.end();
  for ( ; editorItr != editorEnd && !areDocsOpen; ++editorItr )
  {
    Editor* editor = editorItr->second;
    areDocsOpen = editor->GetDocumentManager()->GetDocuments().Size() > 0; // breaks loop
  }

  bool continueLoad = true;
  if ( areDocsOpen )
  {
    if ( wxMessageBox( "Would you like to close any documents that are currently open in any running Luna editors?",
      "Close documents?", wxCENTER | wxICON_QUESTION | wxYES_NO ) == wxYES )
    {
      M_EditorDumbPtr::const_iterator editorItr = m_RunningEditors.begin();
      M_EditorDumbPtr::const_iterator editorEnd = m_RunningEditors.end();
      for ( ; editorItr != editorEnd; ++editorItr )
      {
        Editor* editor = editorItr->second;
        continueLoad &= editor->GetDocumentManager()->CloseAll();
      }
    }
  }

  if ( continueLoad )
  {
    // Load the session information from the file
    SessionStatePtr session;
    
    try
    {
      session = Reflect::Archive::FromFile< SessionState >( file );
    }
    catch ( const Nocturnal::Exception& e )
    {
      Console::Error( "Failed to load session from %s: %s\n", file.c_str(), e.what() );
    }

    if ( session.ReferencesObject() && session->m_EditorStates.size() > 0 )
    {
      const Reflect::Enumeration* enumInfo = Reflect::Registry::GetInstance()->GetEnumeration( Reflect::GetType< EditorTypes::EditorType >() );
      NOC_ASSERT( enumInfo );

      // Open an editor for each state and load the state
      M_EditorState::const_iterator itr = session->m_EditorStates.begin();
      M_EditorState::const_iterator end = session->m_EditorStates.end();
      for ( ; itr != end; ++itr )
      {
        const std::string& enumStr = itr->first;
        Reflect::M_StrEnumerationElement::const_iterator found = enumInfo->m_ElementsByName.find( enumStr );

        if ( found != enumInfo->m_ElementsByName.end() )
        {
          const Reflect::EnumerationElementPtr& enumElem = found->second;
          EditorTypes::EditorType editorType = static_cast< EditorTypes::EditorType >( enumElem->m_Value );
          Editor* editor = LaunchEditor( editorType );
          if ( editor )
          {
            editor->LoadSession( itr->second );
          }
        }
        else
        {
          NOC_BREAK();
        }
      }

      m_Path = file;
    }
  }
  else
  {
    wxMessageBox( "Errors were encountered while trying to close documents that were already open.", "Error", wxCENTER | wxICON_ERROR | wxOK, NULL );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Saves the states of all the editors to a file.
// 
void SessionManager::SaveSession( const std::string& file, const EditorType saveEditorType )
{
#pragma TODO( "Editor refactor" )
  SessionStatePtr state = new SessionState();

  if ( m_RunningEditors.size() > 0 )
  {
    const Reflect::Enumeration* editorTypeEnum = Reflect::Registry::GetInstance()->GetEnumeration( "EditorType" );
    NOC_ASSERT( editorTypeEnum );

    M_EditorDumbPtr::iterator itr = m_RunningEditors.begin();
    M_EditorDumbPtr::iterator end = m_RunningEditors.end();
    for ( ; itr != end; ++itr )
    {
      EditorType editorType = itr->first;
      if ( ( saveEditorType == EditorTypes::Invalid )
        || ( editorType == saveEditorType ) )
      {
        Reflect::M_ValueEnumerationElement::const_iterator found = editorTypeEnum->m_ElementsByValue.find( editorType );
        if ( found != editorTypeEnum->m_ElementsByValue.end() )
        {
          Editor* editor = itr->second;
          const Reflect::EnumerationElementPtr& enumElem = found->second;
          const std::string& enumStr = enumElem->m_Name;

          state->m_EditorStates[ enumStr ] = editor->GetSessionState();
          editor->SaveSession( state->m_EditorStates[ enumStr ] );
        }
        else
        {
          // unknown editor type
          NOC_BREAK();
        }
      }
    }

    NOC_ASSERT( !state->m_EditorStates.empty() );

    Reflect::Archive::ToFile( state, file, new SessionVersion() );
    m_Path = file;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns the path to the file containing the session state, which was most 
// recently saved or loaded.
// 
const std::string& SessionManager::GetSessionPath() const
{
  return m_Path;
}

///////////////////////////////////////////////////////////////////////////////
// Register an editor with the session manager.  
// 
void SessionManager::RegisterEditor( const EditorInfoPtr& info )
{
  Nocturnal::Insert< M_EditorInfo >::Result inserted = m_RegisteredEditors.insert( M_EditorInfo::value_type( info->GetType(), info ) );

  // If you hit this, you have attempted to register an editor with the same 
  // type more than once.
  NOC_ASSERT( inserted.second );
}

///////////////////////////////////////////////////////////////////////////////
// When an editor is created, it should register itself with the Session
// Manager by calling this function.
// 
void SessionManager::RegisterEditorInstance( Editor* editor )
{
  Nocturnal::Insert< M_EditorDumbPtr >::Result inserted = m_RunningEditors.insert( M_EditorDumbPtr::value_type( editor->GetEditorType(), editor ) );

  // If you hit this, you attempted to register the same editor instance more 
  // than once.
  NOC_ASSERT( inserted.second );
}

///////////////////////////////////////////////////////////////////////////////
// When an editor is deleted, it should unregister itself with the Session 
// Manager.
// 
void SessionManager::UnregisterEditorInstance( Editor* editor )
{
  m_RunningEditors.erase( editor->GetEditorType() );
}

///////////////////////////////////////////////////////////////////////////////
// Launches the specified editor, or if the editor is already launched, shows 
// the editor.
// 
Editor* SessionManager::LaunchEditor( EditorTypes::EditorType whichEditor )
{
  Editor* editor = NULL;

  M_EditorDumbPtr::const_iterator foundRunning = m_RunningEditors.find( whichEditor );
  if ( foundRunning != m_RunningEditors.end() )
  {
    editor = foundRunning->second;
  }
  else
  {
    M_EditorInfo::const_iterator foundRegistered = m_RegisteredEditors.find( whichEditor );
    if ( foundRegistered != m_RegisteredEditors.end() )
    {
      const EditorInfo* info = foundRegistered->second;
      editor = info->Create();
    }
  }

  if ( editor )
  {
    if ( !editor->IsShown() )
    {
      editor->Show();
    }
    else if ( editor->IsIconized() )
    {
      editor->Iconize( false );
    }
    editor->Raise();
  }

  return editor;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the number of currently running Luna Editors.
// 
u32 SessionManager::GetRunningEditorCount() const
{
  return static_cast< u32 >( m_RunningEditors.size() );
}

///////////////////////////////////////////////////////////////////////////////
// Saves all files currently open in each of the running editors.
// 
bool SessionManager::SaveAllOpenDocuments()
{
  bool savedAll = true;
  std::string error;
  M_EditorDumbPtr::const_iterator editorItr = m_RunningEditors.begin();
  M_EditorDumbPtr::const_iterator editorEnd = m_RunningEditors.end();
  for ( ; editorItr != editorEnd; ++editorItr )
  {
    Editor* editor = editorItr->second;
    error.clear();
    savedAll &= editor->GetDocumentManager()->SaveAll( error );
  }
  return savedAll;
}

///////////////////////////////////////////////////////////////////////////////
// Saves all files currently open in each of the running editors.  This will
// also optionally show/suppress error prompts.
// 
bool SessionManager::SaveAllOpenDocuments( Editor* currentEditor, bool& showPrompts )
{
  V_string errorMessages;
  
  SessionManager::M_EditorDumbPtr::const_iterator editorItr = m_RunningEditors.begin();
  SessionManager::M_EditorDumbPtr::const_iterator editorEnd = m_RunningEditors.end();
  for ( ; editorItr != editorEnd; ++editorItr )
  {
    DocumentManager* documentManager = editorItr->second->GetDocumentManager();
    OS_DocumentSmartPtr::Iterator docItr = documentManager->GetDocuments().Begin();
    OS_DocumentSmartPtr::Iterator docEnd = documentManager->GetDocuments().End();
    for ( ; docItr != docEnd; ++docItr )
    {
      DocumentPtr document = *docItr;
      if ( !document->IsModified() )
      {
        continue;
      }

      if ( !RCS::PathIsManaged( document->GetFilePath() ) )
      {
        std::string errorMessage;
        if ( !documentManager->Save( document, errorMessage ) )
        {
          errorMessages.push_back( errorMessage );
        }
        continue;
      }

      if ( documentManager->IsCheckedOut( document ) )
      {
        std::string errorMessage;
        if ( !documentManager->Save( document, errorMessage ) )
        {
          errorMessages.push_back( errorMessage );
        }
        continue;
      }

      if ( documentManager->IsUpToDate( document ) )
      {
        std::string msg = std::string( "File '" ) + document->GetFileName() + "' has been changed, but is not checked out.  Would you like to check out and save this file?";
        if ( wxYES == wxMessageBox( msg.c_str(), "Check out and save?", wxYES_NO | wxCENTER | wxICON_QUESTION, editorItr->second ) )
        {
          if ( documentManager->CheckOut( document ) )
          {
            std::string errorMessage;
            if ( !documentManager->Save( document, errorMessage ) )
            {
              errorMessages.push_back( errorMessage );
            }
          }
          else
          {
            std::string errorMessage = std::string( "Failed to check out '" ) + document->GetFileName() + "'.";
            errorMessages.push_back( errorMessage );
          }  
        }
        
        continue;
      }
      
      std::string errorMessage = std::string( "Unfortunately, the file '" ) + document->GetFileName() + "' has been modified in revsion control since you opened it.\n\nYou cannot save the changes you have made.\n\nTo fix this:\n1) Close the file\n2) Get updated assets\n3) Make your changes again\n\nSorry for the inconvenience.";
      errorMessages.push_back( errorMessage );
    }
  }
  
  const u32 maxErrors = 3;
  u32 numErrors = (u32) errorMessages.size();
  if ( numErrors && showPrompts )
  {
    std::string errorMessage( "Errors occurred while saving!" );
    if ( numErrors > maxErrors )
    {
      char numErrorsString[ 256 ] = { 0 };
      sprintf( numErrorsString, "  Only %d out of the %d errors are shown below:\n\n", maxErrors, numErrors );
      errorMessage += std::string( numErrorsString );
      numErrors = maxErrors;
    }
    else
    {
      errorMessage += "  The errors are shown below:\n\n";
    }
    
    for ( u32 i = 0; i < numErrors; ++i )
    {
      errorMessage += "--------------------------------------------------\n";
      errorMessage += errorMessages[ i ] + "\n";
    }

    errorMessage += "--------------------------------------------------\n";
    errorMessage += "\nWould you like to ignore this message in the future?";

    showPrompts = wxMessageBox( errorMessage.c_str(), "Cannot save", wxYES_NO | wxCENTER | wxICON_QUESTION, currentEditor ) == wxNO;
    return false;
  }
  
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Opens the specified file in one of the following:
// If the file is a session, it will be opened in the session frame.
// If the file ends in .rb, it will be opened in a Luna Editor.
// Else the file will be started from a command prompt.
// 
void SessionManager::Edit( const std::string& file )
{
  if ( CheckOut( file, true ) )
  {
    if ( FileSystem::HasExtension( file, FinderSpecs::Luna::SESSION_DECORATION.GetDecoration() ) )
    {
      LoadSession( file );
    }
    else if ( FileSystem::HasExtension( file, FinderSpecs::Extension::REFLECT_BINARY.GetExtension() ) )
    {
      std::string error;
      EditorType editorType = FindEditorForFile( file );
      if ( editorType != EditorTypes::Invalid )
      {
        Editor* editor = LaunchEditor( editorType );
        if ( editor && editor->GetDocumentManager() )
        {
          editor->GetDocumentManager()->OpenPath( file, error );
        }
      }
      else
      {
        error = "Unable to determine appropriate Luna editor for file '" + file + "'.";
      }

      if ( !error.empty() )
      {
        wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, NULL );
        return;
      }
    }
    else if ( FileSystem::HasExtension( file, FinderSpecs::Extension::GRAPH_SHADER.GetExtension() ) )
    {
      Windows::Execute( std::string ("fragmentshader.exe \"") + file + "\"", true );
    }
    else if ( FileSystem::HasExtension( file, FinderSpecs::Extension::MAYA_BINARY.GetExtension() ) )
    {
      // Make sure that we launch the correct version of Maya
      Windows::Execute( std::string ("maya.exe -file \"") + file + "\"", true );
    }
    else
    {
      Windows::Execute( std::string ("cmd.exe /c start \"\" \"") + file + "\"", true );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Checks out the specified file for edit
// 
bool SessionManager::CheckOut( const std::string& file, bool prompt )
{
  if ( file.empty() )
  {
    std::stringstream errorStream;
    errorStream << "No file path was specified.\n";
    wxMessageBox( errorStream.str().c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, NULL );
    return false;
  }

  bool fileExists = false;
  try 
  {
    fileExists = FileSystem::Exists( file );
  }
  catch ( const Nocturnal::Exception& e )
  {
    // Do nothing, this is an invalid file path, so we can't launch it
    std::stringstream errorStream;
    errorStream << "Invalid file specified (" << file << "):\n" << e.Get();
    wxMessageBox( errorStream.str().c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, NULL );
    return false;
  }

  if ( !fileExists )
  {
    std::stringstream errorStream;
    errorStream << "Invalid file specified (" << file << "):\nFile does not exist on disk.";
    wxMessageBox( errorStream.str().c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, NULL );
    return false;
  }

  try
  {
    if ( !RCS::PathIsManaged( file ) )
    {
      return true;
    }
  }
  catch ( const Nocturnal::Exception& )
  {
    return false;
  }

  DocumentPtr tempDoc = new Document( file );
  DocumentManager tempDocMngr;

  if ( prompt )
  {
    return tempDocMngr.QueryCheckOut( tempDoc );
  }

  return tempDocMngr.CheckOut( tempDoc );
}

///////////////////////////////////////////////////////////////////////////////
// Iterates over the registered editors and returns the first one that can 
// handle opening the specified file (determined by looking at the file 
// extension).
// 
EditorTypes::EditorType SessionManager::FindEditorForFile( const std::string& path )
{
  EditorType editorType = EditorTypes::Invalid;

  std::string extension = FileSystem::GetExtension( path );

  if ( !extension.empty() )
  {
    M_EditorInfo::const_iterator itr = m_RegisteredEditors.begin();
    M_EditorInfo::const_iterator end = m_RegisteredEditors.end();
    for ( ; itr != end; ++itr )
    {
      const EditorInfo* editorInfo = itr->second;
      if ( editorInfo->GetFilterSpec()->IsExtensionValid( extension ) )
      {
        editorType = editorInfo->GetType();
        break;
      }
    }
  }

  return editorType;
}

///////////////////////////////////////////////////////////////////////////////
// WHERE DID THIS COME FROM?  CAN WE GET RID OF IT?
// 
void SessionManager::PropertiesPanelEdit( const Inspect::EditFilePathArgs& args )
{
  Edit( args.m_File );
}

