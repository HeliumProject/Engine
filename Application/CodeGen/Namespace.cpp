#include "Namespace.h"

using namespace Helium::CodeGen;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
Namespace::Namespace( const std::string& name )
: Node( name )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
Namespace::~Namespace()
{
}

///////////////////////////////////////////////////////////////////////////////
// Adds an enumeration to the end of the enumeration list in this namespace.
// 
bool Namespace::AppendEnum( const EnumPtr& enumPtr, std::string& error )
{
  if ( !m_Enums.Append( enumPtr ) )
  {
    error = "There is already an enumeration named '" + enumPtr->GetShortName() + "' in the namespace '" + GetShortName() + "'.";
    return false;
  }

  enumPtr->SetParent( this );

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Inserts the specified enum at the front of the enum list.
// 
bool Namespace::PrependEnum( const EnumPtr& enumPtr, std::string& error )
{
  if ( !m_Enums.Prepend( enumPtr ) )
  {
    error = "There is already an enumeration named '" + enumPtr->GetShortName() + "' in the namespace '" + GetShortName() + "'.";
    return false;
  }

  enumPtr->SetParent( this );

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Returns all the enumerations that are in this namespace.
// 
const OS_EnumSmartPtr& Namespace::GetEnums() const
{
  return m_Enums;
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified namespace as a child of this one.
// 
void Namespace::AddNamespace( const NamespacePtr& namespacePtr )
{
  namespacePtr->SetParent( this );
  m_NestedNamespaces.push_back( namespacePtr );
}

///////////////////////////////////////////////////////////////////////////////
// Returns all the namespaces that are nested within this one.
// 
const V_NamespaceSmartPtr& Namespace::GetNamespaces() const
{
  return m_NestedNamespaces;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if this namespace's full name matches the one specified, or if
// any nested namespaces or enums within this one match.
// 
bool Namespace::Contains( const std::string& name ) const
{
  NameTypes::NameType nameType = GetNameType( name );

  if ( GetName( nameType ) == name )
  {
    return true;
  }

  V_NamespaceSmartPtr::const_iterator namespaceItr = m_NestedNamespaces.begin();
  V_NamespaceSmartPtr::const_iterator namespaceEnd = m_NestedNamespaces.end();
  for ( ; namespaceItr != namespaceEnd; ++namespaceItr )
  {
    const NamespacePtr& namespacePtr = *namespaceItr;
    if ( namespacePtr->Contains( name ) )
    {
      return true;
    }
  }

  OS_EnumSmartPtr::Iterator enumItr = m_Enums.Begin();
  OS_EnumSmartPtr::Iterator enumEnd = m_Enums.End();
  for ( ; enumItr != enumEnd; ++enumItr )
  {
    const EnumPtr& enumPtr = *enumItr;
    if ( enumPtr->Contains( name ) )
    {
      return true;
    }
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if this namespace has the specified full name, otherwise, 
// recursively searches nested namespaces for the match.  Returns NULL if the
// namespace could not be found.
// 
const Namespace* Namespace::FindNamespace( const std::string& name ) const
{
  NameTypes::NameType nameType = GetNameType( name );

  if ( GetName( nameType ) == name )
  {
    return this;
  }

  const Namespace* found = NULL;
  V_NamespaceSmartPtr::const_iterator namespaceItr = m_NestedNamespaces.begin();
  V_NamespaceSmartPtr::const_iterator namespaceEnd = m_NestedNamespaces.end();
  for ( ; namespaceItr != namespaceEnd && !found; ++namespaceItr )
  {
    const NamespacePtr& namespacePtr = *namespaceItr;
    found = namespacePtr->FindNamespace( name );
    // If found, will break out of loop
  }

  return found;
}

///////////////////////////////////////////////////////////////////////////////
// Looks for the specified enum within all namespaces contained by this object.
// 
const Enum* Namespace::FindEnum( const std::string& name ) const
{
  NameTypes::NameType nameType = GetNameType( name );

  OS_EnumSmartPtr::Iterator enumItr = m_Enums.Begin();
  OS_EnumSmartPtr::Iterator enumEnd = m_Enums.End();
  for ( ; enumItr != enumEnd; ++enumItr )
  {
    const EnumPtr& enumPtr = *enumItr;
    if ( enumPtr->GetName( nameType ) == name )
    {
      return enumPtr.Ptr();
    }
  }
  
  const Enum* found = NULL;
  V_NamespaceSmartPtr::const_iterator namespaceItr = m_NestedNamespaces.begin();
  V_NamespaceSmartPtr::const_iterator namespaceEnd = m_NestedNamespaces.end();
  for ( ; namespaceItr != namespaceEnd && !found; ++namespaceItr )
  {
    const NamespacePtr& namespacePtr = *namespaceItr;
    found = namespacePtr->FindEnum( name );
  }

  return found;
}

///////////////////////////////////////////////////////////////////////////////
// Check all contained enumerations (including those nested in sub-namespaces)
// looking for the enumeration value with the specified name.
// 
const EnumValue* Namespace::FindEnumValue( const std::string& name ) const
{
  const EnumValue* found = NULL;
  NameTypes::NameType nameType = GetNameType( name );

  OS_EnumSmartPtr::Iterator enumItr = m_Enums.Begin();
  OS_EnumSmartPtr::Iterator enumEnd = m_Enums.End();
  for ( ; enumItr != enumEnd; ++enumItr )
  {
    const EnumPtr& enumPtr = *enumItr;
    found = enumPtr->FindValue( name );
    if ( found )
    {
      return found;
    }
  }
  
  V_NamespaceSmartPtr::const_iterator namespaceItr = m_NestedNamespaces.begin();
  V_NamespaceSmartPtr::const_iterator namespaceEnd = m_NestedNamespaces.end();
  for ( ; namespaceItr != namespaceEnd && !found; ++namespaceItr )
  {
    const NamespacePtr& namespacePtr = *namespaceItr;
    found = namespacePtr->FindEnumValue( name );
    return found;
  }

  return found;
}
