#include "AssetTemplate.h"

#include "AssetFlags.h"

#include "Pipeline/Component/ComponentCollection.h"
#include "Foundation/CommandLine.h"
#include "Foundation/Reflect/Archive.h"

#include <boost/regex.hpp> 

// Using
using namespace Asset;

REFLECT_DEFINE_CLASS( AssetTemplate );

void AssetTemplate::EnumerateClass( Reflect::Compositor<AssetTemplate>& comp )
{
  Reflect::Field* fieldName = comp.AddField( &AssetTemplate::m_Name, "m_Name" );
  Reflect::Field* fieldDescription = comp.AddField( &AssetTemplate::m_Description, "m_Description" );
  Reflect::Field* fieldFileFilter = comp.AddField( &AssetTemplate::m_FileFilter, "m_FileFilter" );

  Reflect::Field* fieldDefaultAddSubDir = comp.AddField( &AssetTemplate::m_DefaultAddSubDir, "m_DefaultAddSubDir" );
  Reflect::Field* fieldShowSubDirCheckbox = comp.AddField( &AssetTemplate::m_ShowSubDirCheckbox, "m_ShowSubDirCheckbox" );
  Reflect::Field* fieldDefaultFormat = comp.AddField( &AssetTemplate::m_DefaultFormat, "m_DefaultFormat" );

  Reflect::Field* fieldRequiredComponents = comp.AddField( &AssetTemplate::m_RequiredComponents, "m_RequiredComponents" );
  Reflect::Field* fieldOptionalComponents = comp.AddField( &AssetTemplate::m_OptionalComponents, "m_OptionalComponents" );
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AssetTemplate::AssetTemplate( const Reflect::Composite* composite )
: m_Name( "Asset" )
, m_Description( "Default Description - Ask a tools programmer to add more useful info here!" )
, m_DefaultAddSubDir( false )
, m_ShowSubDirCheckbox( false )
, m_RequiredComponents( new Component::ComponentCollection() )
, m_OptionalComponents( new Component::ComponentCollection() )
{
  if ( composite )
  {
    m_Name = composite->m_UIName;
    composite->GetProperty( AssetProperties::LongDescription, m_Description );
    composite->GetProperty( AssetProperties::FileFilter, m_FileFilter );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
AssetTemplate::~AssetTemplate()
{
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified attribute to the list of required attributes that must
// be present when this template creates an asset class.
// 
bool AssetTemplate::AddRequiredComponent( const Component::ComponentPtr& attribute )
{
  bool isOk = false;
  if ( !m_RequiredComponents->ContainsComponent( attribute->GetType() ) )
  {
    m_RequiredComponents->SetComponent( attribute, false );
    isOk = true;
  }
  return isOk;
}

bool AssetTemplate::AddRequiredComponent( const i32 typeID )
{
  bool isOk = false;
  if ( !m_RequiredComponents->ContainsComponent( typeID ) )
  {
    Component::ComponentPtr attribute = Reflect::AssertCast< Component::ComponentBase >( Reflect::Registry::GetInstance()->CreateInstance( typeID ) );
    m_RequiredComponents->SetComponent( attribute, false );
    isOk = true;
  }
  return isOk;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the list of attributes that must be present on assets that are 
// created with this template.
// 
const Component::M_Component& AssetTemplate::GetRequiredComponents() const
{
  return m_RequiredComponents->GetComponents();
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified attribute as an optional one.  
// 
bool AssetTemplate::AddOptionalComponent( const Component::ComponentPtr& attribute )
{
  bool isOk = false;
  if ( !m_OptionalComponents->ContainsComponent( attribute->GetType() ) )
  {
    m_OptionalComponents->SetComponent( attribute, false );
    isOk = true;
  }
  return isOk;
}

bool AssetTemplate::AddOptionalComponent( const i32 typeID )
{
  bool isOk = false;
  if ( !m_OptionalComponents->ContainsComponent( typeID ) )
  {
    Component::ComponentPtr attribute = Reflect::AssertCast< Component::ComponentBase >( Reflect::Registry::GetInstance()->CreateInstance( typeID ) );
    m_OptionalComponents->SetComponent( attribute, false );
    isOk = true;
  }
  return isOk;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the list of optional attributes for this template.  These attributes
// can be present when creating an asset using this template.
// 
const Component::M_Component& AssetTemplate::GetOptionalComponents() const
{
  return m_OptionalComponents->GetComponents();
}

///////////////////////////////////////////////////////////////////////////////
// Returns the list of templates for the give class with type id
//
void AssetTemplate::GetAssetTemplates( const i32 typeID, Reflect::V_Element& assetTemplates )
{
  assetTemplates.clear();

  std::string assetTemplatesStr = Reflect::Registry::GetInstance()->GetClass( typeID )->GetProperty( AssetProperties::AssetTemplates );

  if ( !assetTemplatesStr.empty() )
  {
    std::stringstream stream( assetTemplatesStr );
    Reflect::Archive::FromStream( stream, Reflect::ArchiveTypes::Binary, assetTemplates );
  }
  else
  {
    // create the default template
    AssetTemplatePtr defaultTemplate = new AssetTemplate( Reflect::Registry::GetInstance()->GetClass( typeID ) );    
    assetTemplates.push_back( defaultTemplate );  
  }
}