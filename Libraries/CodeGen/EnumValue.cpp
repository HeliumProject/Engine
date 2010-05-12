#include "EnumValue.h"
#include "Enum.h"

using namespace CodeGen;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
EnumValue::EnumValue( const std::string& name, i32 value, const EnumPtr& enumPtr )
: Node( name )
, m_Value( value )
{
  SetParent( enumPtr );
}
  
///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
EnumValue::~EnumValue()
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns the value associated with this enumeration element.
// 
i32 EnumValue::GetValue() const
{
  return m_Value;
}
