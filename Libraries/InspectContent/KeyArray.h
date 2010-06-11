#pragma once

#include "API.h"
#include "Key.h"
#include "Foundation/Automation/Event.h"

namespace Inspect
{
  /////////////////////////////////////////////////////////////////////////////
  // Arguments and events for when the key array is changed.
  // 
  struct KeyArgs
  {
    KeyArray* m_KeyArray;
    Key* m_Key;

    KeyArgs( KeyArray* keyArray, Key* key )
      : m_KeyArray( keyArray )
      , m_Key( key )
    {
    }
  };
  typedef Nocturnal::Signature< void, const KeyArgs& > KeySignature;


  /////////////////////////////////////////////////////////////////////////////
  // Manages a list of keys.  Events are fired when keys are changed.
  // 
  class INSPECTCONTENT_API KeyArray : public Nocturnal::RefCountBase< KeyArray >
  {
  private:
    V_KeyPtr m_Keys;
    u32 m_Selected;
    i32 m_FreezeCount;

  public:
    KeyArray();
    
    void Clear();

    void Freeze();
    void Thaw();

    u32 CreateKey( const Math::Color3& color, float location );
    bool DeleteKey( u32 index );
    
    u32 MoveKey( u32 index, float newLocation, bool scrubbing );
    void ChangeKeyColor( u32 index, const Math::Color3& color );
    
    const Key* GetKey( u32 index ) const;
    u32 GetCount() const;

    bool SelectKey( u32 index );
    void ClearSelection();
    u32 GetSelectedKey() const;

    u32 FindKeyAtLocation( float loc, float tolerance = 0.001 );

  private:
    void Sort();
    u32 FindKey( const Key* key ) const;

    // Events
  private:
    KeySignature::Event m_KeyCreated;
  public:
    void AddKeyCreatedListener( const KeySignature::Delegate& listener )
    {
      m_KeyCreated.Add( listener );
    }
    void RemoveKeyCreatedListener( const KeySignature::Delegate& listener )
    {
      m_KeyCreated.Remove( listener );
    }

  private:
    KeySignature::Event m_KeyDeleted;
  public:
    void AddKeyDeletedListener( const KeySignature::Delegate& listener )
    {
      m_KeyDeleted.Add( listener );
    }
    void RemoveKeyDeletedListener( const KeySignature::Delegate& listener )
    {
      m_KeyDeleted.Remove( listener );
    }

  private:
    KeySignature::Event m_Freeze;
  public:
    void AddFreezeListener( const KeySignature::Delegate& listener )
    {
      m_Freeze.Add( listener );
    }
    void RemoveFreezeListener( const KeySignature::Delegate& listener )
    {
      m_Freeze.Remove( listener );
    }

  private:
    KeySignature::Event m_Thaw;
  public:
    void AddThawListener( const KeySignature::Delegate& listener )
    {
      m_Thaw.Add( listener );
    }
    void RemoveThawListener( const KeySignature::Delegate& listener )
    {
      m_Thaw.Remove( listener );
    }

  private:
    KeySignature::Event m_KeySelectionChanged;
  public:
    void AddKeySelectionChangedListener( const KeySignature::Delegate& listener )
    {
      m_KeySelectionChanged.Add( listener );
    }
    void RemoveKeySelectionChangedListener( const KeySignature::Delegate& listener )
    {
      m_KeySelectionChanged.Remove( listener );
    }

  private:
    KeySignature::Event m_KeyMoving;
  public:
    void AddKeyMovingListener( const KeySignature::Delegate& listener )
    {
      m_KeyMoving.Add( listener );
    }
    void RemoveKeyMovingListener( const KeySignature::Delegate& listener )
    {
      m_KeyMoving.Remove( listener );
    }

  private:
    KeySignature::Event m_KeyMoved;
  public:
    void AddKeyMovedListener( const KeySignature::Delegate& listener )
    {
      m_KeyMoved.Add( listener );
    }
    void RemoveKeyMovedListener( const KeySignature::Delegate& listener )
    {
      m_KeyMoved.Remove( listener );
    }

  private:
    KeySignature::Event m_KeyColorChanged;
  public:
    void AddKeyColorChangedListener( const KeySignature::Delegate& listener )
    {
      m_KeyColorChanged.Add( listener );
    }
    void RemoveKeyColorChangedListener( const KeySignature::Delegate& listener )
    {
      m_KeyColorChanged.Remove( listener );
    }
  };
  typedef Nocturnal::SmartPtr< KeyArray > KeyArrayPtr;
}
