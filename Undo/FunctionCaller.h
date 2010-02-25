#pragma once

#include "Common/Memory/SmartPtr.h"

namespace Undo
{
  /////////////////////////////////////////////////////////////////////////////
  // Pure NOC_ABSTRACT base class for calling an arbitrary function.  This is used
  // by the ExistenceCommand to call arbitrary add and remove functions.
  // 
  class FunctionCaller : public Nocturnal::RefCountBase<FunctionCaller>
  {
  public:
    FunctionCaller()
    {
    }
    
    virtual ~FunctionCaller() 
    {
    }

    virtual void operator()() = 0;
  };
  typedef Nocturnal::SmartPtr< FunctionCaller > FunctionCallerPtr;


  /////////////////////////////////////////////////////////////////////////////
  // Calls a function on an object, passing in an item by const reference.
  // 
  template< class T, class V >
  class MemberFunctionConstRef : public FunctionCaller
  {
  public:
    typedef void ( T::*ConstRef )( const V& );

  private:
    T* m_Container;
    V m_Item;
    ConstRef m_Function;

  public:
    MemberFunctionConstRef( T* container, const V& item, ConstRef func )
      : m_Container( container )
      , m_Item( item )
      , m_Function( func )
    {
    }

    virtual void operator()() NOC_OVERRIDE
    {
      ( m_Container->*m_Function )( m_Item );
    }
  };

  /////////////////////////////////////////////////////////////////////////////
  // Calls a function on an object, passing in an item by const reference and
  // maintaining the return value.
  // 
  template< class T, class V, class R >
  class MemberFunctionConstRefReturn : public FunctionCaller
  {
  public:
    typedef R ( T::*ConstRefWithReturn )( const V& );

  private:
    T* m_Container;
    V m_Item;
    ConstRefWithReturn m_Function;

  public:
    MemberFunctionConstRefReturn( T* container, const V& item, ConstRefWithReturn func )
      : m_Container( container )
      , m_Item( item )
      , m_Function( func )
    {
    }

    virtual void operator()() NOC_OVERRIDE
    {
      ( m_Container->*m_Function )( m_Item );
    }
  };

  // Add others as needed...
}