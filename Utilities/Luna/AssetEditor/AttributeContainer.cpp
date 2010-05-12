#include "Precompile.h"
#include "AttributeContainer.h"

#include "AssetClass.h"
#include "AttributeWrapper.h"
#include "AttributeNode.h"

#include "Core/PropertiesManager.h"

// Using
using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::AttributeContainer );

///////////////////////////////////////////////////////////////////////////////
// Static initialization for all Luna::AttributeContainer types.
// 
void AttributeContainer::InitializeType()
{
  Reflect::RegisterClass<Luna::AttributeContainer>( "Luna::AttributeContainer" );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup of all Luna::AttributeContainer types.
// 
void AttributeContainer::CleanupType()
{
  Reflect::UnregisterClass<Luna::AttributeContainer>();
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AttributeContainer::AttributeContainer( Luna::AssetManager* assetManager, Luna::AssetClass* assetClass )
: Luna::AssetNode( assetManager )
, m_Asset( assetClass )
{
  SetName( "Attributes" );
  SetIcon( "attribute_attributes_16.png" );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
AttributeContainer::~AttributeContainer()
{
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to provide a typed list of attributes that are in this container.
// 
bool AttributeContainer::AddChild( AssetNodePtr child, Luna::AssetNode* beforeSibling )
{
  bool added = false;
  if ( child->HasType( Reflect::GetType< Luna::AttributeNode >() ) )
  {
    // Maintain our internal list of attributes.
    if ( __super::AddChild( child, beforeSibling ) )
    {
      Luna::AttributeNode* attributeNode = Reflect::DangerousCast< Luna::AttributeNode >( child );
      Luna::AttributeWrapper* attribute = attributeNode->GetAttribute();
      bool inserted = m_Attributes.insert( M_AttributeNodeDumbPtr::value_type( attribute->GetSlot(), attributeNode ) ).second; 
      NOC_ASSERT( inserted );
      added = true;
    }
  }
  return added;
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to maintain a typed list of attributes that belong to this container.
// 
bool AttributeContainer::RemoveChild( AssetNodePtr child )
{
  if ( child->HasType( Reflect::GetType< Luna::AttributeNode >() ) )
  {
    // Maintain our internal list of attributes
    Luna::AttributeNode* attributeNode = Reflect::DangerousCast< Luna::AttributeNode >( child );
    Luna::AttributeWrapper* attribute = attributeNode->GetAttribute();
    M_AttributeNodeDumbPtr::iterator found = m_Attributes.find( attribute->GetSlot() );
    if ( found != m_Attributes.end() )
    {
      if ( attribute == found->second->GetAttribute() )
      {
        m_Attributes.erase( found );
      }
      else
      {
        // Not sure if this is a problem or not...
        NOC_BREAK();
      }
    }
  }

  return __super::RemoveChild( child );
}

///////////////////////////////////////////////////////////////////////////////
// Returns an ordered list of the attributes that are in this container.
// 
const M_AttributeNodeDumbPtr& AttributeContainer::GetAttributes() const
{
  return m_Attributes;
}

///////////////////////////////////////////////////////////////////////////////
// Overrides Selectable.  Connects the persistent data attributes to the 
// property sheet UI.
// 
void AttributeContainer::ConnectProperties( EnumerateElementArgs& args )
{
  for each ( const M_AttributeNodeDumbPtr::value_type& pair in m_Attributes )
  {
    args.EnumerateElement( pair.second->GetAttribute()->GetPackage< Attribute::AttributeBase >() );
  }
}

