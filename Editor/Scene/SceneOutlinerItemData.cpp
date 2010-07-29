#include "Precompile.h"
#include "SceneOutlinerItemData.h"

// Using
using namespace Editor;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
SceneOutlinerItemData::SceneOutlinerItemData( Object* object )
: m_Object( object )
, m_CachedCount( 0 )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
SceneOutlinerItemData::~SceneOutlinerItemData()
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns the Object associated with this class.
// 
Object* SceneOutlinerItemData::GetObject() const
{
  return m_Object;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the Object associated with this class.
// 
void SceneOutlinerItemData::SetObject( Object* object )
{
  m_Object = object;
}

///////////////////////////////////////////////////////////////////////////////
// Tracks the basic label information for this node.
// 
void SceneOutlinerItemData::SetItemText( const tstring& text )
{
  m_ItemText = text; 
}

///////////////////////////////////////////////////////////////////////////////
// Tracks the basic label information for this node.
// 
const tstring& SceneOutlinerItemData::GetItemText()
{
  return m_ItemText;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the previously cached count.  Used to indicate how many child objects
// this not has.
// 
int SceneOutlinerItemData::GetCachedCount()
{
  return m_CachedCount; 
}

///////////////////////////////////////////////////////////////////////////////
// Stores the specified number.  Used to indicate how many child objects this
// node has.
// 
void SceneOutlinerItemData::SetCachedCount( int count )
{
  m_CachedCount = count; 
}
