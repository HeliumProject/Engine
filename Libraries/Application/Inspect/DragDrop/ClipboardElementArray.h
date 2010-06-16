#pragma once

#include "Application/API.h"
#include "ReflectClipboardData.h"
#include "Reflect/SimpleSerializer.h"
#include "Reflect/ElementArraySerializer.h"

namespace Inspect
{
  /////////////////////////////////////////////////////////////////////////////
  // Stores an array of elements.  You can specify that all elements derive
  // from a certain base class.
  // 
  class APPLICATION_API ClipboardElementArray : public ReflectClipboardData
  {
  public:
    std::string m_CommonBaseClass;
    Reflect::V_Element m_Elements;

  public:
    // Runtime Type Info
    REFLECT_DECLARE_CLASS( ClipboardElementArray, ReflectClipboardData );
    static void InitializeType();
    static void CleanupType();

  public:
    ClipboardElementArray();
    virtual ~ClipboardElementArray();
    i32 GetCommonBaseTypeID() const;
    void SetCommonBaseTypeID( i32 typeID );
    bool Add( const Reflect::ElementPtr& item );
    virtual bool Merge( const ReflectClipboardData* source ) NOC_OVERRIDE;

  protected:
    bool CanAdd( const Reflect::ElementPtr& item ) const;

  public:
    static void EnumerateClass( Reflect::Compositor<ClipboardElementArray>& comp );
  };
  typedef Nocturnal::SmartPtr< ClipboardElementArray > ClipboardElementArrayPtr;
}