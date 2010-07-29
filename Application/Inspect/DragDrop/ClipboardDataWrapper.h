#pragma once

#include "ReflectClipboardData.h"

namespace Inspect
{
  /////////////////////////////////////////////////////////////////////////////
  // Helper class to package the ReflectClipboardData format into a single
  // item that can be serialized and stored on the clipboard.
  // 
  class ClipboardDataWrapper : public Reflect::Element
  {
  public:
    ReflectClipboardDataPtr m_Data;

  public:
    // Runtime Type Info
    REFLECT_DECLARE_CLASS( ClipboardDataWrapper, Reflect::Element );
    static void InitializeType();
    static void CleanupType();

  public:
    ClipboardDataWrapper();
    virtual ~ClipboardDataWrapper();

    static void EnumerateClass( Reflect::Compositor<ClipboardDataWrapper>& comp );
  };
  typedef Helium::SmartPtr< ClipboardDataWrapper > ClipboardDataWrapperPtr;
}