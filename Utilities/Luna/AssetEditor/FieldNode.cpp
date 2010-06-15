#include "Precompile.h"
#include "FieldNode.h"
#include "ContainerNode.h"
#include "SimpleFieldNode.h"
#include "Asset/AssetFlags.h"

using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::FieldNode );

///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void FieldNode::InitializeType()
{
  Reflect::RegisterClass<Luna::FieldNode>( "Luna::FieldNode" );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void FieldNode::CleanupType()
{
  Reflect::UnregisterClass<Luna::FieldNode>();
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
FieldNode::FieldNode( Luna::AssetManager* assetManager, Reflect::Element* element, const Reflect::Field* field )
: Luna::AssetNode( assetManager )
, m_Element( element )
, m_Field( field )
{
  // Set up the serializer
  m_Serializer = m_Field->CreateSerializer( m_Element );

  // Default name to display in the UI
  SetName( field->m_UIName );

  // Add listeners
  m_Element->AddChangedListener( Reflect::ElementChangeSignature::Delegate ( this, &FieldNode::OnElementChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
FieldNode::~FieldNode()
{
  // Remove listeners
  m_Element->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate ( this, &FieldNode::OnElementChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the element that this node's field belongs to.
// 
Reflect::Element* FieldNode::GetElement() const
{
  return m_Element;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the field that is wrapped by this node.
// 
const Reflect::Field* FieldNode::GetField() const
{
  return m_Field;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified field flag was found on this field.  See 
// FieldFlags.h, AssetFlags.h, etc.
// 
bool FieldNode::HasFlag( u32 flag ) const
{
  return ( m_Field->m_Flags & flag ) == flag;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the string representation of the data located in this field.  Note
// that this function may not be valid for all types of data (for example, an
// array of reflect elements does not provide a string representation of its
// data).
// 
std::string FieldNode::GetValue() const
{
  std::stringstream stream;
  stream << *m_Serializer;
  return stream.str();
}

///////////////////////////////////////////////////////////////////////////////
// This function is called when the field associated with this node is changed.
// Derived classes should NOC_OVERRIDE this function to do any work that is necessary.
// 
void FieldNode::HandleFieldChanged()
{
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void FieldNode::OnElementChanged( const Reflect::ElementChangeArgs& args )
{
  if ( args.m_Field == m_Field )
  {
    HandleFieldChanged();
  }
}
