#pragma once

#include <string>
#include <algorithm>

#include "Platform/Assert.h"
#include "Foundation/Exception.h"
#include "Foundation/Memory/SmartPtr.h"

#include "API.h"
#include "Base.h"
#include "TypeID.h"

namespace Reflect
{
  //
  // Type, fully qualified type information
  //

  namespace ReservedTypes
  {
    enum ReservedType
    {
      Invalid  = -1,
      First,
      Any,
    };
  }
  typedef ReservedTypes::ReservedType ReservedType;

  class REFLECT_API Type NOC_ABSTRACT : public Base
  {
  public:
    REFLECTION_BASE( ReflectionTypes::Type );

    //
    // Fields
    //

    std::string                   m_ShortName;          // the name of this type in the codebase, this is not necessarily unique (templates share the same short name)
    std::string                   m_FullName;           // the fully qualified name of this type in the codebase (this is super-long if its a template type)
    std::string                   m_UIName;             // the friendly name for the field, its optional (will use the short name if not specified)

    i32                           m_TypeID;             // the unique id of this type
    u32                           m_Size;               // the size of the object in bytes

    mutable std::vector<void**>   m_Pointers;           // cached pointers to this type
    mutable std::vector<i32*>     m_IDs;                // cached ids to this type

  protected:
    Type();
    virtual ~Type();

    // assign unique type id
    static i32 AssignTypeID();

  public:
    void TrackPointer(void** pointer) const;
    void RemovePointer(void** pointer) const;

    void TrackID(i32* id) const;
    void RemoveID(i32* id) const;
  };

  template <class T>
  class TypeTracker
  {
  public:
    TypeTracker()
      : m_Type ( NULL )
      , m_Data ( NULL )
    {
       
    }

    ~TypeTracker()
    {
      if(m_Type)
      {
        m_Type->RemovePointer( (void**)&m_Type ); 
        m_Type->RemovePointer( (void**)m_Data );
      }
    }

    void Set( const Type* type, T* data )
    {
      m_Type = type; 
      m_Data = data; 

      // we are a reference to the type. if the type is deleted, we need to know
      // or else we will crash below when trying to access the type
      m_Type->TrackPointer( (void**)&m_Type ); 
      m_Type->TrackPointer( (void**)m_Data );
    }

  private:
    const Type* m_Type;
    T* m_Data;
  };

  class IDTracker
  {
  public:
    IDTracker()
      : m_Type( NULL )
      , m_Data( NULL )
    {
      
    }

    ~IDTracker()
    {
      if(m_Type)
      {
        m_Type->RemovePointer( (void**) &m_Type ); 
        m_Type->RemoveID( m_Data );
      }
      
    }

    void Set( const Type* type, i32* data )
    {
      m_Type = type; 
      m_Data = data; 

      // we are a reference to the type. if the type is deleted, we need to know
      // or else we will crash below when trying to access the type
      m_Type->TrackPointer( (void**) &m_Type ); 
      m_Type->TrackID( m_Data );
    }


  private:
    const Type* m_Type;
    i32* m_Data;
  };
}