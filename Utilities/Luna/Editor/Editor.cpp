#include "Precompile.h"
#include "Editor.h"

#include "DocumentManager.h"

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
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
Editor::~Editor()
{
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
