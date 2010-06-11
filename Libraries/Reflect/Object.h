#pragma once

#include "API.h"
#include "Class.h"
#include "Exceptions.h"

#include "Platform/Types.h"
#include "Platform/Assert.h"
#include "Foundation/Memory/SmartPtr.h"

namespace Reflect
{
  //
  // Reflect::Object is a reference counted and type checked NOC_ABSTRACT base class
  //

  class REFLECT_API Object NOC_ABSTRACT
  {
  private:
    mutable i32 m_RefCount;

  protected:
    Object();

  private:
    Object(const Object& rhs);

  public:
    virtual ~Object();


    //
    // Memory
    //

    void* operator new(size_t bytes);
    void operator delete(void *ptr, size_t bytes);


    //
    // RTTI
    //

    // Returns the type id for this instance
    virtual i32 GetType() const;

    // Deduces type membership for this instance
    virtual bool HasType(i32 type) const;

    // Retrieves the RTTI data for this instance
    virtual const Reflect::Class* GetClass() const;

    // Enumerates member data (stub)
    static void EnumerateClass( Reflect::Compositor<Element>& comp );


    //
    // Reference Counting
    //

  public:
    int GetRefCount() const;
    void IncrRefCount() const;
    void DecrRefCount() const;
  };


  //
  // DangerousCast does not type checking
  //

  template<class __Derived>
  inline __Derived* DangerousCast(Reflect::Object* base)
  {
    return static_cast<__Derived*>(base);
  }

  template<class __Derived>
  inline const __Derived* ConstDangerousCast(const Reflect::Object* base)
  {
    return static_cast<const __Derived*>(base);
  }

  //
  // AssertCast type checks in debug and asserts if failure, does no type checking in release
  //

  template<class __Derived>
  inline __Derived* AssertCast(Reflect::Object* base)
  {
    if ( base != NULL )
    {
      NOC_ASSERT( base->HasType(GetClass<__Derived>()->m_TypeID) );
    }

    return DangerousCast<__Derived>(base);
  }

  template<class __Derived>
  inline const __Derived* ConstAssertCast(const Reflect::Object* base)
  {
    if ( base != NULL )
    {
      NOC_ASSERT( base->HasType(GetClass<__Derived>()->m_TypeID) );
    }

    return ConstDangerousCast<__Derived>(base);
  }

  //
  // TryCast type checks and throws if failure
  //

  template<class __Derived>
  inline __Derived* TryCast(Reflect::Object* base)
  {
    if ( base != NULL && !base->HasType(GetClass<__Derived>()->m_TypeID) )
    {
      throw CastException ( "Object of type '%s' cannot be cast to type '%s'",
        base->GetClass()->m_ShortName.c_str(),
        GetClass<__Derived>()->m_ShortName.c_str() );
    }

    return DangerousCast<__Derived>(base);
  }

  template<class __Derived>
  inline const __Derived* ConstTryCast(const Reflect::Object* base)
  {
    if ( base != NULL && !base->HasType(GetClass<__Derived>()->m_TypeID) )
    {
      throw CastException ( "Object of type '%s' cannot be cast to type '%s'",
        base->GetClass()->m_ShortName.c_str(),
        GetClass<__Derived>()->m_ShortName.c_str() );
    }

    return ConstDangerousCast<__Derived>(base);
  }

  //
  // ObjectCast always type checks and returns null if failure
  //

  template<class __Derived>
  inline __Derived* ObjectCast(Reflect::Object* base)
  {
    if ( base != NULL && base->HasType(GetClass<__Derived>()->m_TypeID) )
    {
      return DangerousCast<__Derived>(base);
    }
    else
    {
      return NULL;
    }
  }

  template<class __Derived>
  inline const __Derived* ConstObjectCast(const Reflect::Object* base)
  {
    if ( base != NULL && base->HasType(GetClass<__Derived>()->m_TypeID) )
    {
      return ConstDangerousCast<__Derived>(base);
    }
    else
    {
      return NULL;
    }
  }
}