#include "Precompile.h"
#include "SceneOutlinerState.h"

// Using
using namespace Editor;

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
i32 SceneOutlinerState::GetHorizontalScrollBarPos() const
{
  return m_HorizScrollPos;
}

///////////////////////////////////////////////////////////////////////////////
// Saves the specified value as the horizontal scroll bar postion.
// 
void SceneOutlinerState::SetHorizontalScrollBarPos( i32 pos )
{
  m_HorizScrollPos = pos;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the position of the vertical scroll bar in the tree control.
// 
i32 SceneOutlinerState::GetVerticalScrollBarPos() const
{
  return m_VertScrollPos;
}

///////////////////////////////////////////////////////////////////////////////
// Saves the specified value as the vertical scroll bar postion.
// 
void SceneOutlinerState::SetVerticalScrollBarPos( i32 pos )
{
  m_VertScrollPos = pos;
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified object to the list of objects that are expanded in the
// tree.  Should be called when a tree item is expanded.
// 
void SceneOutlinerState::AddExpandedObject( Object* object )
{
  m_ExpandedObjects.insert( object );
}

///////////////////////////////////////////////////////////////////////////////
// Removes the specified object from the list of expanded object in the tree.
// Should be called when a tree item is collapsed.
// 
void SceneOutlinerState::RemoveExpandedObject( Object* object )
{
  m_ExpandedObjects.erase( object );
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified object is in the list of expanded objects.
// 
bool SceneOutlinerState::IsExpanded( Object* object )
{
  return m_ExpandedObjects.find( object ) != m_ExpandedObjects.end();
}

///////////////////////////////////////////////////////////////////////////////
// Returns the list of expanded objects.
// 
const S_Objects& SceneOutlinerState::GetExpandedObjects() const
{
  return m_ExpandedObjects;
}
