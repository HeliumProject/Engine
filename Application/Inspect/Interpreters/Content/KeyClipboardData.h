#pragma once

#include "Key.h"

namespace Inspect
{
  class KeyClipboardData : public Reflect::Element
  {
    // Members
  public:
    V_KeyPtr m_Keys;

    // RTTI
  public:
    REFLECT_DECLARE_CLASS( KeyClipboardData, Reflect::Element );
    static void EnumerateClass( Reflect::Compositor< KeyClipboardData >& comp );
    
    // Methods
  public:
    KeyClipboardData();
  };
  typedef Helium::SmartPtr< KeyClipboardData > KeyClipboardDataPtr;
}
