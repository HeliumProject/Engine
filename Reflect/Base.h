#pragma once

#include "API.h"

#include "Common/Types.h"
#include "Common/Container/Insert.h"
#include "Windows/Atomic.h"

//
// Obviously the reflection system itself can't use the same type checking as the code
//  its reflecting upon, so define a simple type checking system just for the reflection classes
//

#define REFLECTION_BASE(__Type) \
  const static int ReflectionTypeID = __Type; \
  virtual int GetReflectionType () const { return __Type; } \
  virtual bool HasReflectionType (int id) const { return __Type == id; }

#define REFLECTION_TYPE(__Type) \
  const static int ReflectionTypeID = __Type; \
  virtual int GetReflectionType () const NOC_OVERRIDE { return __Type; } \
  virtual bool HasReflectionType (int id) const NOC_OVERRIDE { return __Type == id || __super::HasReflectionType(id); }

namespace Reflect
{
  //
  // All types have to belong to this enum
  //

  namespace ReflectionTypes
  {
    enum ReflectionType
    {
      Invalid,
      Type,
      Enumeration,
      Composite,
      Structure,
      Class,
      Field,
      ElementField,
      EnumerationField,
    };
  }
  typedef ReflectionTypes::ReflectionType ReflectionType;


  //
  // This lets us safely cast between reflection class pointers
  //

  class REFLECT_API Base : public ::Windows::AtomicRefCountBase
  {
  public:
    REFLECTION_BASE(ReflectionTypes::Invalid);

    mutable M_string m_Properties;

    template<class T>
    inline void SetProperty( const std::string& key, const T& value )
    {
      std::ostringstream str;
      str << value;

      if ( !str.fail() )
      {
        SetProperty<std::string>( key, str.str() );
      }
    }

    template<>
    inline void SetProperty( const std::string& key, const std::string& value )
    {
      m_Properties[key] = value;
    }

    template<class T>
    inline bool GetProperty( const std::string& key, T& value ) const
    {
      std::string strValue;
      bool result = GetProperty<std::string>( key, strValue );

      if ( result )
      {
        std::istringstream str( strValue );
        str >> value;
        return !str.fail();
      }

      return false;
    }

    template<>
    inline bool GetProperty( const std::string& key, std::string& value ) const
    {
      M_string::const_iterator found = m_Properties.find( key ); 
      if ( found != m_Properties.end() )
      {
        value = found->second;
        return true;
      }

      return false;
    }

    inline const std::string& GetProperty( const std::string& key ) const
    {
      M_string::const_iterator found = m_Properties.find( key );
      if ( found != m_Properties.end() )
      {
        return found->second;
      }

      static std::string empty;
      return empty;
    }
  }; 

  template<typename T>
  T* ReflectionCast(Base* type)
  {
    return (type && type->HasReflectionType( T::ReflectionTypeID )) ? static_cast<T*>(type) : NULL;
  }

  template<typename T>
  const T* ReflectionCast(const Base* type)
  {
    return (type && type->HasReflectionType( T::ReflectionTypeID )) ? static_cast<const T*>(type) : NULL;
  }
}