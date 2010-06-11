#include "Precompile.h"
#include "EditorInfo.h"
#include "Platform/Assert.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
EditorInfo::EditorInfo( EditorType type, CreateEditorFunc createFunc, const Finder::FilterSpec* filter )
: m_Type( type )
, m_CreateFunc( createFunc )
, m_FilterSpec( filter )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
EditorInfo::~EditorInfo()
{
}

///////////////////////////////////////////////////////////////////////////////
// Uses the create function to create and return a new instance of the editor
// represented by this class.
// 
Editor* EditorInfo::Create() const
{
  NOC_ASSERT( m_CreateFunc );
  return m_CreateFunc();
}

///////////////////////////////////////////////////////////////////////////////
// Returns the type of this editor.
// 
EditorTypes::EditorType EditorInfo::GetType() const
{
  return m_Type;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the filter spec identifying which file types this editor can work 
// with.
// 
const Finder::FilterSpec* EditorInfo::GetFilterSpec() const
{
  return m_FilterSpec;
}
