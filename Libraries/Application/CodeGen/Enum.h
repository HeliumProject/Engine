#pragma once

#include "Application/API.h"

#include "Node.h"
#include "EnumValue.h"

namespace CodeGen
{
  class Enum;
  typedef Nocturnal::SmartPtr< Enum > EnumPtr;

  /////////////////////////////////////////////////////////////////////////////
  // Class representing a C++ enumeration.
  // 
  class APPLICATION_API Enum : public Node
  {
  private:
    std::string m_Name;
    OS_EnumValueSmartPtr m_Values;
    bool m_HasAssignedValues;

  public:
    Enum( const std::string& name );
    ~Enum();

    bool AddValue( const std::string& name );
    bool AddValue( const std::string& name, i32 value );
    u32 GetMaxValue();
    bool HasAssignedValues() const;
    const OS_EnumValueSmartPtr& GetValues() const;
    const EnumValue* FindValue( const std::string& name ) const;
    const EnumValue* FindValue( i32 value ) const;
    bool Contains( const std::string& name ) const;
    bool operator<( const Enum& rhs ) const;  // Enumerations are sorted by their name
    bool operator==( const Enum& rhs ) const; // Enumerations with the same name are equal
    EnumPtr Clone() const;
  };

  typedef std::vector< EnumPtr > V_EnumSmartPtr;
  typedef Nocturnal::OrderedSet< EnumPtr > OS_EnumSmartPtr;
}
