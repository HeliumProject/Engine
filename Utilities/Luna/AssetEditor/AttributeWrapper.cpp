#include "Precompile.h"
#include "AttributeWrapper.h"

#include "AssetNode.h"
#include "FieldFactory.h"
#include "PersistentDataFactory.h"
#include "Core/PropertiesManager.h"

#include "Asset/AssetFlags.h"
#include "Attribute/Attribute.h"

// Using
using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::AttributeWrapper );

///////////////////////////////////////////////////////////////////////////////
// Static initialization for all Luna::AttributeWrapper types.
// 
void AttributeWrapper::InitializeType()
{
  Reflect::RegisterClass<Luna::AttributeWrapper>( "Luna::AttributeWrapper" );
  PersistentDataFactory::GetInstance()->Register( Reflect::GetType< Attribute::AttributeBase >(), &AttributeWrapper::CreateAttribute );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup of all Luna::AttributeWrapper types.
// 
void AttributeWrapper::CleanupType()
{
  Reflect::UnregisterClass<Luna::AttributeWrapper>();
}

///////////////////////////////////////////////////////////////////////////////
// Static creator function. 
// 
Luna::PersistentDataPtr AttributeWrapper::CreateAttribute( Reflect::Element* attribute, Luna::AssetManager* assetManager )
{
  return new Luna::AttributeWrapper( Reflect::AssertCast< Attribute::AttributeBase >( attribute ), assetManager );
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AttributeWrapper::AttributeWrapper( Attribute::AttributeBase* attribute, Luna::AssetManager* assetManager )
: Luna::PersistentData( attribute, assetManager )
, m_AssetClass( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
AttributeWrapper::~AttributeWrapper()
{
}

///////////////////////////////////////////////////////////////////////////////
// Sets the asset class that this attribute belongs to.
// 
void AttributeWrapper::SetAssetClass( Luna::AssetClass* asset )
{
  m_AssetClass = asset;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the asset class that this attribute belongs to.
// 
Luna::AssetClass* AttributeWrapper::GetAssetClass() const
{
  return m_AssetClass;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the name of this attribute.
// 
std::string AttributeWrapper::GetName() const
{
  const Attribute::AttributeBase* package = GetPackage< Attribute::AttributeBase >();
  return package->GetClass()->m_UIName;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the persistent data type ID.
// 
i32 AttributeWrapper::GetSlot() const
{
  const Attribute::AttributeBase* package = GetPackage< Attribute::AttributeBase >();
  return package->GetSlot();
}

////////////////////////////////////////////////////////////////////////////////
// we don't always have the SmallIcon property set, because not all 
// attributes inherit from a common base class that knows about AssetProperties
// 
// additionally, some parts of the UI may not have an Luna::AttributeWrapper yet, but just
// want to query the icon for the attribute 
// 
static std::string g_DefaultIconName = "null_16.png"; 

const std::string& AttributeWrapper::GetAttributeIcon(const Attribute::AttributeBase* attribute)
{
  const std::string& icon = attribute->GetClass()->GetProperty( Asset::AssetProperties::SmallIcon );
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
const std::string& AttributeWrapper::GetIcon() const
{
  return Luna::AttributeWrapper::GetAttributeIcon( GetPackage< Attribute::AttributeBase >() ); 
}

///////////////////////////////////////////////////////////////////////////////
// Creates child nodes of this attribute, under the specified parent.
// 
void AttributeWrapper::CreateChildren( Luna::AssetNode* parentNode )
{
  Luna::FieldFactory::GetInstance()->CreateChildFieldNodes( parentNode, GetPackage< Reflect::Element >(), m_AssetManager );
}

///////////////////////////////////////////////////////////////////////////////
// Derived classes can NOC_OVERRIDE this function to add their own context menu
// items.
// 
void AttributeWrapper::PopulateContextMenu( ContextMenuItemSet& menu )
{
}

///////////////////////////////////////////////////////////////////////////////
// Derived classes can NOC_OVERRIDE this function to determin what shows up
// the generic iterated properties panel
//
void AttributeWrapper::ConnectProperties( EnumerateElementArgs& args )
{
  args.EnumerateElement( GetPackage< Reflect::Element >() );
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
Undo::CommandPtr AttributeWrapper::OverwriteSettings( Attribute::AttributeBase* src )
{
  Undo::CommandPtr cmd;

  Attribute::AttributeBase* pkg = GetPackage< Attribute::AttributeBase >();
  if ( !pkg->Equals( src ) )
  {
    // MAKE THIS A COMMAND
    src->CopyTo( pkg );
    pkg->RaiseChanged();
  }

  return cmd;
}

