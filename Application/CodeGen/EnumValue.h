#pragma once

#include "Foundation/Container/OrderedSet.h"

#include "Application/API.h"
#include "Node.h"

namespace CodeGen
{
  // Forwards
  class Enum;
  typedef Helium::SmartPtr< Enum > EnumPtr;

  /////////////////////////////////////////////////////////////////////////////
  // Class representing a C++ enumeration value.
  // 
  class APPLICATION_API EnumValue : public Node
  {
  private:
    i32 m_Value;

  public:
    EnumValue( const std::string& name, i32 value, const EnumPtr& enumPtr );
    ~EnumValue();

    i32 GetValue() const;
  };

  typedef Helium::SmartPtr< EnumValue > EnumValuePtr;
  typedef std::vector< EnumValuePtr > V_EnumValueSmartPtr;
  typedef Helium::OrderedSet< EnumValuePtr > OS_EnumValueSmartPtr;
}
