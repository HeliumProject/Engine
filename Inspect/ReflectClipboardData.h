#pragma once

#include "API.h"
#include "reflect/Serializers.h"

namespace Inspect
{
  /////////////////////////////////////////////////////////////////////////////
  // Base class for any data that can be stored on the clipboard.
  // 
  class INSPECT_API ReflectClipboardData NOC_ABSTRACT : public Reflect::Element
  {
  public:
    // Runtime Type Info
    REFLECT_DECLARE_ABSTRACT( ReflectClipboardData, Reflect::Element );
    static void InitializeType();
    static void CleanupType();

  public:
    ReflectClipboardData();
    virtual ~ReflectClipboardData();

    // Derived classes must implement this function to combine their data
    // with another object's (and return true if successful).
    virtual bool Merge( const ReflectClipboardData* source ) = 0;

    static void EnumerateClass( Reflect::Compositor<ReflectClipboardData>& comp );
  };
  typedef Nocturnal::SmartPtr< ReflectClipboardData > ReflectClipboardDataPtr;
}