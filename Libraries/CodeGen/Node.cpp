#include "Node.h"
#include "Common/Assert.h"

using namespace CodeGen;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
Node::Node( const std::string& name )
: m_Parent( NULL )
, m_ShortName( name )
, m_FullName( "" )
, m_IsFullNameDirty( true )
{
  // Never specify a full name.  The full name is calculated based upon the
  // node's parents.
  NOC_ASSERT( GetNameType( name ) == NameTypes::ShortName );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
Node::~Node()
{
}

///////////////////////////////////////////////////////////////////////////////
// Gets either the short name or the full name.
// 
const std::string& Node::GetName( NameType which ) const
{
  if ( which == NameTypes::ShortName )
  {
    return GetShortName();
  }

  return GetFullName();
}

///////////////////////////////////////////////////////////////////////////////
// Returns the name of this node.
// 
const std::string& Node::GetShortName() const
{
  return m_ShortName;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the full name of the node (qualified with "::" to separate parents).
// 
const std::string& Node::GetFullName() const
{
  if ( m_IsFullNameDirty )
  {
    if ( GetParent() )
    {
      m_FullName = GetParent()->GetFullName() + "::" + m_ShortName;
    }
    else
    {
      m_FullName = m_ShortName;
    }

    m_IsFullNameDirty = false;
  }

  return m_FullName;
}

///////////////////////////////////////////////////////////////////////////////
// Makes this node nested inside the specified parent.
// 
void Node::SetParent( Node* parent )
{
  if ( parent != m_Parent )
  {
    m_Parent = parent;
    m_IsFullNameDirty = true;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns the parent node of this one.
// 
const Node* Node::GetParent() const
{
  return m_Parent;
}

///////////////////////////////////////////////////////////////////////////////
// Static convenience function to convert a full name to a short one.
// 
std::string CodeGen::GetShortName( const std::string& fullName )
{
  std::string shortName( fullName );

  std::string::size_type index = shortName.size();
  const char* previous = NULL;
  std::string::reverse_iterator rItr = shortName.rbegin();
  std::string::reverse_iterator rEnd = shortName.rend();
  for ( ; rItr != rEnd; ++rItr, --index )
  {
    if ( previous && *previous == ':' )
    {
      if ( *rItr == ':' )
      {
        ++index;
        break;
      }
    }

    if ( *rItr == ':' )
    {
      previous = &( *rItr );
    }
    else
    {
      previous = NULL;
    }
  }

  if ( index != shortName.size() )
  {
    shortName = shortName.substr( index );
  }

  return shortName;
}

