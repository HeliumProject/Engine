#include "Precompile.h"
#include "ComponentWrapper.h"

#include "AssetNode.h"
#include "FieldFactory.h"
#include "PersistentDataFactory.h"
#include "Core/PropertiesManager.h"

#include "Pipeline/Asset/AssetFlags.h"
#include "Foundation/Component/Component.h"

// Using
using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::ComponentWrapper );

///////////////////////////////////////////////////////////////////////////////
// Static initialization for all Luna::ComponentWrapper types.
// 
void ComponentWrapper::InitializeType()
{
  Reflect::RegisterClass<Luna::ComponentWrapper>( TXT( "Luna::ComponentWrapper" ) );
  PersistentDataFactory::GetInstance()->Register( Reflect::GetType< Component::ComponentBase >(), &ComponentWrapper::CreateComponent );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup of all Luna::ComponentWrapper types.
// 
void ComponentWrapper::CleanupType()
{
  Reflect::UnregisterClass<Luna::ComponentWrapper>();
}

///////////////////////////////////////////////////////////////////////////////
// Static creator function. 
// 
Luna::PersistentDataPtr ComponentWrapper::CreateComponent( Reflect::Element* attribute, Luna::AssetManager* assetManager )
{
  return new Luna::ComponentWrapper( Reflect::AssertCast< Component::ComponentBase >( attribute ), assetManager );
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ComponentWrapper::ComponentWrapper( Component::ComponentBase* attribute, Luna::AssetManager* assetManager )
: Luna::PersistentData( attribute, assetManager )
, m_AssetClass( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ComponentWrapper::~ComponentWrapper()
{
}

///////////////////////////////////////////////////////////////////////////////
// Sets the asset class that this attribute belongs to.
// 
void ComponentWrapper::SetAssetClass( Luna::AssetClass* asset )
{
  m_AssetClass = asset;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the asset class that this attribute belongs to.
// 
Luna::AssetClass* ComponentWrapper::GetAssetClass() const
{
  return m_AssetClass;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the name of this attribute.
// 
tstring ComponentWrapper::GetName() const
{
  const Component::ComponentBase* package = GetPackage< Component::ComponentBase >();
  return package->GetClass()->m_UIName;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the persistent data type ID.
// 
i32 ComponentWrapper::GetSlot() const
{
  const Component::ComponentBase* package = GetPackage< Component::ComponentBase >();
  return package->GetSlot();
}

////////////////////////////////////////////////////////////////////////////////
// we don't always have the SmallIcon property set, because not all 
// attributes inherit from a common base class that knows about AssetProperties
// 
// additionally, some parts of the UI may not have an Luna::ComponentWrapper yet, but just
// want to query the icon for the attribute 
// 
static tstring g_DefaultIconName = TXT( "null.png" ); 

const tstring& ComponentWrapper::GetComponentIcon(const Component::ComponentBase* attribute)
{
  const tstring& icon = attribute->GetClass()->GetProperty( Asset::AssetProperties::SmallIcon );
  if(icon.empty())
  {
    return g_DefaultIconName; 
  }
  else
  {
    return icon; 
  }
}


///////////////////////////////////////////////////////////////////////////////
// Returns the icon to use with this attribute.
// 
const tstring& ComponentWrapper::GetIcon() const
{
  return Luna::ComponentWrapper::GetComponentIcon( GetPackage< Component::ComponentBase >() ); 
}

///////////////////////////////////////////////////////////////////////////////
// Creates child nodes of this attribute, under the specified parent.
// 
void ComponentWrapper::CreateChildren( Luna::AssetNode* parentNode )
{
  Luna::FieldFactory::GetInstance()->CreateChildFieldNodes( parentNode, GetPackage< Reflect::Element >(), m_AssetManager );
}

///////////////////////////////////////////////////////////////////////////////
// Derived classes can NOC_OVERRIDE this function to add their own context menu
// items.
// 
void ComponentWrapper::PopulateContextMenu( ContextMenuItemSet& menu )
{
}

///////////////////////////////////////////////////////////////////////////////
// Derived classes can NOC_OVERRIDE this function to determin what shows up
// the generic iterated properties panel
//
void ComponentWrapper::ConnectProperties( EnumerateElementArgs& args )
{
  args.EnumerateElement( GetPackage< Reflect::Element >() );
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
Undo::CommandPtr ComponentWrapper::OverwriteSettings( Component::ComponentBase* src )
{
  Undo::CommandPtr cmd;

  Component::ComponentBase* pkg = GetPackage< Component::ComponentBase >();
  if ( !pkg->Equals( src ) )
  {
    // MAKE THIS A COMMAND
    src->CopyTo( pkg );
    pkg->RaiseChanged();
  }

  return cmd;
}

