#include "KeyArray.h"

#include <algorithm>

using namespace Helium;
using namespace Helium::Inspect;


///////////////////////////////////////////////////////////////////////////////
// Comparison operator for sorting the list of keys.
// 
bool KeyCompare( KeyPtr elem1, KeyPtr elem2 )
{
   return elem1->GetLocation() < elem2->GetLocation();
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
KeyArray::KeyArray()
: m_Selected( Key::InvalidKey )
, m_FreezeCount( 0 )
{
}

///////////////////////////////////////////////////////////////////////////////
// Deletes all the keys in the array, one by one.
// 
void KeyArray::Clear()
{
  if ( m_Keys.size() > 0 )
  {
    Freeze();
    ClearSelection();

    while ( m_Keys.size() > 0 )
    {
      DeleteKey( 0 );
    }
    Thaw();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Notifies listeners that a bunch of operations are about to occur on this
// key array so that they can optimize their work and wait until the operation
// is finished.  You can call this function repeatedly and only the first call
// will result in an event being fired.  For every call to Freeze,
// there should be a matching call to Thaw.
// 
void KeyArray::Freeze()
{
  if ( ++m_FreezeCount == 1 )
  {
    m_Freeze.Raise( KeyArgs( this, NULL ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Notifies listeners that a batch of operations has just completed on this
// array.  See above comments on Freeze for more info.
// 
void KeyArray::Thaw()
{
  HELIUM_ASSERT( m_FreezeCount > 0 );
  if ( --m_FreezeCount == 0 )
  {
    m_Thaw.Raise( KeyArgs( this, NULL ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Creates a key at the specified location and the specified color.
// 
u32 KeyArray::CreateKey( const Math::Color3& color, float location )
{
  const u32 numKeys = static_cast< u32 >( m_Keys.size() );
  u32 insertIndex = numKeys;
  for ( u32 keyIndex = 0; keyIndex < numKeys; ++keyIndex )
  {
    if ( m_Keys[keyIndex]->GetLocation() > location )
    {
      insertIndex = keyIndex;
      break;
    }
  }

  KeyPtr key = new Key( color, location );
  m_Keys.insert( m_Keys.begin() + insertIndex, key );

  m_KeyCreated.Raise( KeyArgs( this, key ) );

  return insertIndex;
}

///////////////////////////////////////////////////////////////////////////////
// Deletes the key at the specified index.  This may change the selection if
// the selected key is the one that is deleted.  Returns true if a valid key
// was specified and could be deleted.
// 
bool KeyArray::DeleteKey( u32 index )
{
  if ( index != Key::InvalidKey && index < m_Keys.size() )
  {
    if ( index == m_Selected )
    {
      ClearSelection();
    }

    // Hold pointer to key so it doesn't get deleted
    KeyPtr key = m_Keys[index];
    m_Keys.erase( m_Keys.begin() + index );
    m_KeyDeleted.Raise( KeyArgs( this, key ) );

    // Fix the selected key
    if ( index < m_Selected )
    {
      SelectKey( m_Selected - 1 );
    }

    return true;
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Moves the specified key to a new location.  If scrubbing is true, the 
// "moving" event will be fired, otherwise, the "moved" event will be fired.
// Returns the new index of the key since moving a key might reorder the list.
// 
u32 KeyArray::MoveKey( u32 index, float newLocation, bool scrubbing )
{
  if ( index != Key::InvalidKey && index < m_Keys.size() )
  {
    bool wasSelected = index == m_Selected;
    Key* key = m_Keys[index];
    const float error = 0.0009f;
    if ( scrubbing && ( ::abs( m_Keys[index]->GetLocation() - newLocation ) > error ) )
    {
      key->m_Location = newLocation;
      Sort();
      m_KeyMoving.Raise( KeyArgs( this, key ) );
    }
    else
    {
      key->m_Location = newLocation;
      Sort();
      m_KeyMoved.Raise( KeyArgs( this, key ) );
    }

    index = FindKey( key );
    if ( wasSelected )
    {
      SelectKey( index );
    }
  }

  return index;
}

///////////////////////////////////////////////////////////////////////////////
// Changes the color of a key at the specified index.
// 
void KeyArray::ChangeKeyColor( u32 index, const Math::Color3& color )
{
  if ( index != Key::InvalidKey && index < m_Keys.size() )
  {
    if ( m_Keys[index]->GetColor() != color )
    {
      m_Keys[index]->m_Color = color;
      m_KeyColorChanged.Raise( KeyArgs( this, m_Keys[index] ) );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns a pointer to the key at the specified index, or NULL if there is no
// key at that index.
// 
const Key* KeyArray::GetKey( u32 index ) const
{
  Key* key = NULL;
  if ( index != Key::InvalidKey && index < m_Keys.size() )
  {
    key = m_Keys[index];
  }
  return key;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the number of keys managed by this array.
// 
u32 KeyArray::GetCount() const
{
  return static_cast< u32 >( m_Keys.size() );
}

///////////////////////////////////////////////////////////////////////////////
// Selects the key at the specified index.  The selected key will be drawn
// differently from the other keys.  Returns true if the key was successfully
// selected.
// 
bool KeyArray::SelectKey( u32 index )
{
  if ( ( index == Key::InvalidKey || index < m_Keys.size() ) && index != m_Selected )
  {
    m_Selected = index;
    Key* selection = NULL;
    if ( m_Selected != Key::InvalidKey )
    {
      selection = m_Keys[m_Selected];
    }
    m_KeySelectionChanged.Raise( KeyArgs( this, selection ) );
    return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Clears the selected key.
// 
void KeyArray::ClearSelection()
{
  SelectKey( Key::InvalidKey );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the index of the key that is currently selected.
// 
u32 KeyArray::GetSelectedKey() const
{
  return m_Selected;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the index of the first key that is closest to the specified location.
// The tolerance value specifies how close the key needs to be to count as a
// match.  A tolerance of 0.0f means that it must be an exact match.  A 
// tolerance of 1.0f means to search all the keys and return the closest one to
// the specified location.
// 
u32 KeyArray::FindKeyAtLocation( float loc, float tolerance )
{
  Math::Clamp( tolerance, 0.0f, 1.0f );
  float difference = FLT_MAX;
  u32 found = Key::InvalidKey;
  V_KeyPtr::const_iterator keyItr = m_Keys.begin();
  V_KeyPtr::const_iterator keyEnd = m_Keys.end();
  for ( u32 keyIndex = 0; keyItr != keyEnd; ++keyItr, ++keyIndex )
  {
    float current = fabs( ( *keyItr )->GetLocation() - loc );
    if ( current <= tolerance && current < difference )
    {
      found = keyIndex;
      difference = current;
    }
  }
  return found;
}

///////////////////////////////////////////////////////////////////////////////
// Makes sure the keys are sorted in ascending order based on their location
// values.
// 
void KeyArray::Sort()
{
  std::sort( m_Keys.begin(), m_Keys.end(), KeyCompare );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the index of the specified key.
// 
u32 KeyArray::FindKey( const Key* key ) const
{
  V_KeyPtr::const_iterator keyItr = m_Keys.begin();
  V_KeyPtr::const_iterator keyEnd = m_Keys.end();
  for ( u32 keyIndex = 0; keyItr != keyEnd; ++keyItr, ++keyIndex )
  {
    if ( key == ( *keyItr ) )
    {
      return keyIndex;
    }
  }
  return Key::InvalidKey;
}
