#include "Precompile.h"
#include "ComponentContainer.h"

#include "AssetClass.h"
#include "ComponentWrapper.h"
#include "ComponentNode.h"

#include "Core/PropertiesManager.h"

// Using
using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::ComponentContainer );

///////////////////////////////////////////////////////////////////////////////
// Static initialization for all Luna::ComponentContainer types.
// 
void ComponentContainer::InitializeType()
{
  Reflect::RegisterClass<Luna::ComponentContainer>( TXT( "Luna::ComponentContainer" ) );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup of all Luna::ComponentContainer types.
// 
void ComponentContainer::CleanupType()
{
  Reflect::UnregisterClass<Luna::ComponentContainer>();
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ComponentContainer::ComponentContainer( Luna::AssetManager* assetManager, Luna::AssetClass* assetClass )
: Luna::AssetNode( assetManager )
, m_Asset( assetClass )
{
  SetName( TXT( "Components" ) );
  SetIcon( TXT( "component_components.png" ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ComponentContainer::~ComponentContainer()
{
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to provide a typed list of components that are in this container.
// 
bool ComponentContainer::AddChild( AssetNodePtr child, Luna::AssetNode* beforeSibling )
{
  bool added = false;
  if ( child->HasType( Reflect::GetType< Luna::ComponentNode >() ) )
  {
    // Maintain our internal list of components.
    if ( __super::AddChild( child, beforeSibling ) )
    {
      Luna::ComponentNode* componentNode = Reflect::DangerousCast< Luna::ComponentNode >( child );
      Luna::ComponentWrapper* component = componentNode->GetComponent();
      bool inserted = m_Components.insert( M_ComponentNodeDumbPtr::value_type( component->GetSlot(), componentNode ) ).second; 
      NOC_ASSERT( inserted );
      added = true;
    }
  }
  return added;
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to maintain a typed list of components that belong to this container.
// 
bool ComponentContainer::RemoveChild( AssetNodePtr child )
{
  if ( child->HasType( Reflect::GetType< Luna::ComponentNode >() ) )
  {
    // Maintain our internal list of components
    Luna::ComponentNode* componentNode = Reflect::DangerousCast< Luna::ComponentNode >( child );
    Luna::ComponentWrapper* component = componentNode->GetComponent();
    M_ComponentNodeDumbPtr::iterator found = m_Components.find( component->GetSlot() );
    if ( found != m_Components.end() )
    {
      if ( component == found->second->GetComponent() )
      {
        m_Components.erase( found );
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
// Returns an ordered list of the components that are in this container.
// 
const M_ComponentNodeDumbPtr& ComponentContainer::GetComponents() const
{
  return m_Components;
}

///////////////////////////////////////////////////////////////////////////////
// Overrides Selectable.  Connects the persistent data components to the 
// property sheet UI.
// 
void ComponentContainer::ConnectProperties( EnumerateElementArgs& args )
{
  for each ( const M_ComponentNodeDumbPtr::value_type& pair in m_Components )
  {
    args.EnumerateElement( pair.second->GetComponent()->GetPackage< Component::ComponentBase >() );
  }
}

