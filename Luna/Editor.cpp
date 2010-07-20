#include "Precompile.h"
#include "Editor.h"

#include "DocumentManager.h"

#include "Application/RCS/RCS.h"
#include "Application/UI/FileDialog.h"
#include "Platform/Process.h"
#include "Platform/Exception.h"

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
const tstring& Editor::GetPreferencePrefix() const
{
  if ( m_PreferencePrefix.empty() )
  {
    const Reflect::Enumeration* editorTypesEnum = Reflect::GetEnumeration< EditorTypes::EditorType >();
    NOC_ASSERT( editorTypesEnum ); 
    editorTypesEnum->GetElementLabel( GetEditorType(), m_PreferencePrefix );
  }
  return m_PreferencePrefix;
}

void Editor::OnPropertiesCreated( const PropertiesCreatedArgs& args )
{
  PostCommand( new PropertiesCreatedCommand( args.m_PropertiesManager, args.m_SelectionId, args.m_Controls ) );
}
