#include "Precompile.h"
#include "EditorFrame.h"

#include "DocumentManager.h"

#include "Application/RCS/RCS.h"
#include "Application/UI/FileDialog.h"
#include "Platform/Process.h"
#include "Platform/Exception.h"

using namespace Helium;
using namespace Helium::Editor;


// Static UI event table
BEGIN_EVENT_TABLE( EditorFrame, Frame )
END_EVENT_TABLE()


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
EditorFrame::EditorFrame( EditorType editorType, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: Frame( parent, id, title, pos, size, style, name )
, m_EditorType( editorType )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
EditorFrame::~EditorFrame()
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns the type of this editor.
// 
EditorTypes::EditorType EditorFrame::GetEditorType() const
{
  return m_EditorType;
}


///////////////////////////////////////////////////////////////////////////////
// Used by Frame::PrefixPreferenceKey
//
const tstring& EditorFrame::GetPreferencePrefix() const
{
  if ( m_PreferencePrefix.empty() )
  {
    const Reflect::Enumeration* editorTypesEnum = Reflect::GetEnumeration< EditorTypes::EditorType >();
    HELIUM_ASSERT( editorTypesEnum ); 
    editorTypesEnum->GetElementLabel( GetEditorType(), m_PreferencePrefix );
  }
  return m_PreferencePrefix;
}

void EditorFrame::OnPropertiesCreated( const PropertiesCreatedArgs& args )
{
  PostCommand( new PropertiesCreatedCommand( args.m_PropertiesManager, args.m_SelectionId, args.m_Controls ) );
}
