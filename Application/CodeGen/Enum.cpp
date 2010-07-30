#include "Enum.h"

using namespace Helium::CodeGen;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
Enum::Enum( const std::string& name )
: Node( name )
, m_HasAssignedValues( false )
{
}
  
///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
Enum::~Enum()
{
}

///////////////////////////////////////////////////////////////////////////////
// Adds a new element to this enumeration (with the specified name).  A default
// value will be assigned to this element based upon its position within the
// enumeration.
// 
bool Enum::AddValue( const std::string& name )
{
  i32 value = m_Values.Empty() ? 0 : GetMaxValue() + 1;
  return m_Values.Append( new EnumValue( name, value, this ) );
}

///////////////////////////////////////////////////////////////////////////////
// Adds a new element to this enumeration with the specified name and value.
// 
bool Enum::AddValue( const std::string& name, i32 value )
{
  m_HasAssignedValues = true;
  return m_Values.Append( new EnumValue( name, value, this ) );
}

///////////////////////////////////////////////////////////////////////////////
// Gets the max value used in this enum
// 
u32 Enum::GetMaxValue()
{
  u32 highestValue = 0;
  CodeGen::OS_EnumValueSmartPtr::Iterator valIt = m_Values.Begin();
  CodeGen::OS_EnumValueSmartPtr::Iterator valEnd = m_Values.End();
  for ( ; valIt != valEnd; ++valIt )
  {
    if ( (*valIt)->GetValue() > (i32)highestValue )
    {
      highestValue = (*valIt)->GetValue();
    }
  }

  return highestValue;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if any of this enumeration's elements had values explicitly
// assigned to them.
// 
bool Enum::HasAssignedValues() const
{
  return m_HasAssignedValues;
}

///////////////////////////////////////////////////////////////////////////////
// Returns all the values that belong to this enumeration.
// 
const OS_EnumValueSmartPtr& Enum::GetValues() const
{
  return m_Values;
}

///////////////////////////////////////////////////////////////////////////////
// Searches through all the enumeration elements, looking for the one with
// the specified name.
// 
const EnumValue* Enum::FindValue( const std::string& name ) const
{
  // Determine whether we were given a full name or a short name to look for.
  NameTypes::NameType nameType = GetNameType( name );

  const EnumValue* found = NULL;
  OS_EnumValueSmartPtr::Iterator itr = m_Values.Begin();
  OS_EnumValueSmartPtr::Iterator end = m_Values.End();
  for ( ; itr != end; ++itr )
  {
    const EnumValuePtr& value = *itr;
    if ( value->GetName( nameType ) == name )
    {
      found = value.Ptr();
      break;
    }
  }

  return found;
}

///////////////////////////////////////////////////////////////////////////////
// Searches through all the enumeration elements, looking for the one with
// the specified value.
// 
const EnumValue* Enum::FindValue( i32 value ) const
{
  const EnumValue* found = NULL;
  OS_EnumValueSmartPtr::Iterator itr = m_Values.Begin();
  OS_EnumValueSmartPtr::Iterator end = m_Values.End();
  for ( ; itr != end; ++itr )
  {
    const EnumValuePtr& enumValue = *itr;
    if ( enumValue->GetValue() == value )
    {
      found = enumValue.Ptr();
      break;
    }
  }

  return found;
}


///////////////////////////////////////////////////////////////////////////////
// Returns true if this enum has the specified name, or if any of the values 
// contained within this enum match the specified name.
// 
bool Enum::Contains( const std::string& name ) const
{
  NameTypes::NameType nameType = GetNameType( name );

  if ( GetName( nameType ) == name )
  {
    return true;
  }

  OS_EnumValueSmartPtr::Iterator valItr = m_Values.Begin();
  OS_EnumValueSmartPtr::Iterator valEnd = m_Values.End();
  for ( ; valItr != valEnd; ++valItr )
  {
    const EnumValuePtr& enumVal = *valItr;
    if ( enumVal->GetName( nameType ) == name )
    {
      return true;
    }
  }

  return false;
}
 
///////////////////////////////////////////////////////////////////////////////
// Enumerations are sorted by their names.
// 
bool Enum::operator<( const Enum& rhs ) const
{
  return GetShortName() < rhs.GetShortName();
}

///////////////////////////////////////////////////////////////////////////////
// Enumerations are equal if their names are the same.
// 
bool Enum::operator==( const Enum& rhs ) const
{
  return GetShortName() == rhs.GetShortName();
}

///////////////////////////////////////////////////////////////////////////////
// Creates and returns a new enumeration that is a clone of this one.  The
// new enumeration will have the same name and values as this enumeration.
// The parent pointer will not be copied to the clone (as this would mean
// that the parent would have two child enumerations with the same name and 
// that is probably not desirable).
// 
EnumPtr Enum::Clone() const
{
  EnumPtr clone = new Enum( GetShortName() );
  OS_EnumValueSmartPtr::Iterator itr = m_Values.Begin();
  OS_EnumValueSmartPtr::Iterator end = m_Values.End();
  for ( ; itr != end; ++itr )
  {
    const EnumValuePtr& val = *itr;
    if ( HasAssignedValues() )
    {
      clone->AddValue( val->GetShortName(), val->GetValue() );
    }
    else
    {
      clone->AddValue( val->GetShortName() );
    }
  }
  return clone;
}
