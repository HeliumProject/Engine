#include "EditorPch.h"
#include "SceneOutlinerState.h"

using namespace Helium;
using namespace Helium::Editor;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
SceneOutlinerState::SceneOutlinerState()
{
  Reset();
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
SceneOutlinerState::~SceneOutlinerState()
{
}

///////////////////////////////////////////////////////////////////////////////
// Resets all internal members.
// 
void SceneOutlinerState::Reset()
{
  m_HorizScrollPos = 0;
  m_VertScrollPos = 0;
  m_ExpandedObjects.clear();
}

///////////////////////////////////////////////////////////////////////////////
// Returns the position of the horizonal scroll bar in the tree control.
// 
int32_t SceneOutlinerState::GetHorizontalScrollBarPos() const
{
  return m_HorizScrollPos;
}

///////////////////////////////////////////////////////////////////////////////
// Saves the specified value as the horizontal scroll bar postion.
// 
void SceneOutlinerState::SetHorizontalScrollBarPos( int32_t pos )
{
  m_HorizScrollPos = pos;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the position of the vertical scroll bar in the tree control.
// 
int32_t SceneOutlinerState::GetVerticalScrollBarPos() const
{
  return m_VertScrollPos;
}

///////////////////////////////////////////////////////////////////////////////
// Saves the specified value as the vertical scroll bar postion.
// 
void SceneOutlinerState::SetVerticalScrollBarPos( int32_t pos )
{
  m_VertScrollPos = pos;
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified object to the list of objects that are expanded in the
// tree.  Should be called when a tree item is expanded.
// 
void SceneOutlinerState::AddExpandedObject( Reflect::Object* object )
{
  m_ExpandedObjects.insert( object );
}

///////////////////////////////////////////////////////////////////////////////
// Removes the specified object from the list of expanded object in the tree.
// Should be called when a tree item is collapsed.
// 
void SceneOutlinerState::RemoveExpandedObject( Reflect::Object* object )
{
  m_ExpandedObjects.erase( object );
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified object is in the list of expanded objects.
// 
bool SceneOutlinerState::IsExpanded( Reflect::Object* object )
{
  return m_ExpandedObjects.find( object ) != m_ExpandedObjects.end();
}

///////////////////////////////////////////////////////////////////////////////
// Returns the list of expanded objects.
// 
const std::set< Reflect::Object* >& SceneOutlinerState::GetExpandedObjects() const
{
  return m_ExpandedObjects;
}
