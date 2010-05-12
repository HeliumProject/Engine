#include "AssetTemplate.h"

#include "AssetFlags.h"

#include "Attribute/AttributeCollection.h"
#include "Common/CommandLine.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Finder/FinderSpec.h"
#include "Reflect/Archive.h"

#include <boost/regex.hpp> 

// Using
using namespace Asset;

REFLECT_DEFINE_CLASS( AssetTemplate );

void AssetTemplate::EnumerateClass( Reflect::Compositor<AssetTemplate>& comp )
{
  Reflect::Field* fieldName = comp.AddField( &AssetTemplate::m_Name, "m_Name" );
  Reflect::Field* fieldDescription = comp.AddField( &AssetTemplate::m_Description, "m_Description" );
  Reflect::Field* fieldModifierSpec = comp.AddField( &AssetTemplate::m_ModifierSpec, "m_ModifierSpec" );

  Reflect::Field* fieldDefaultAddSubDir = comp.AddField( &AssetTemplate::m_DefaultAddSubDir, "m_DefaultAddSubDir" );
  Reflect::Field* fieldShowSubDirCheckbox = comp.AddField( &AssetTemplate::m_ShowSubDirCheckbox, "m_ShowSubDirCheckbox" );
  Reflect::Field* fieldAboutDirSettings = comp.AddField( &AssetTemplate::m_AboutDirSettings, "m_AboutDirSettings" );
  Reflect::Field* fieldDefaultRoot = comp.AddField( &AssetTemplate::m_DefaultRoot, "m_DefaultRoot" );
  Reflect::Field* fieldDefaultFormat = comp.AddField( &AssetTemplate::m_DefaultFormat, "m_DefaultFormat" );
  Reflect::Field* fieldDirectoryPatterns = comp.AddField( &AssetTemplate::m_DirectoryPatterns, "m_DirectoryPatterns" );

  Reflect::Field* fieldRequiredAttributes = comp.AddField( &AssetTemplate::m_RequiredAttributes, "m_RequiredAttributes" );
  Reflect::Field* fieldOptionalAttributes = comp.AddField( &AssetTemplate::m_OptionalAttributes, "m_OptionalAttributes" );
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AssetTemplate::AssetTemplate( const Reflect::Composite* composite )
: m_Name( "Asset" )
, m_Description( "Default Description - Ask a tools programmer to add more useful info here!" )
, m_DefaultAddSubDir( false )
, m_ShowSubDirCheckbox( false )
, m_RequiredAttributes( new Attribute::AttributeCollection() )
, m_OptionalAttributes( new Attribute::AttributeCollection() )
{
  if ( composite )
  {
    m_Name = composite->m_UIName;
    composite->GetProperty( AssetProperties::LongDescription, m_Description );

    std::string specName;
    if ( composite->GetProperty( AssetProperties::RootFolderSpec, specName ) )
    {
      m_DefaultRoot = Finder::GetFolderSpec( specName )->GetRelativeFolder();
    }

    composite->GetProperty( AssetProperties::ModifierSpec, m_ModifierSpec );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
AssetTemplate::~AssetTemplate()
{
}

///////////////////////////////////////////////////////////////////////////////
// 
bool AssetTemplate::ValidateDirectory( const std::string& directory ) const
{
  if ( m_DirectoryPatterns.empty() || Nocturnal::GetCmdLineFlag( "asset_admin" ) )
  {
    return true;
  }
  
  if ( !FileSystem::HasPrefix( Finder::ProjectAssets(), directory ) )
  {
    return false;
  }

  std::string relativeDir = directory;
  FileSystem::StripPrefix( Finder::ProjectAssets(), relativeDir );

  std::string pattern;
  V_string::const_iterator itr = m_DirectoryPatterns.begin();
  V_string::const_iterator end = m_DirectoryPatterns.end();
  for ( ; itr != end; ++itr )
  {
    pattern = (*itr);

    while ( !pattern.empty() && 
      ( *pattern.rbegin() == '/' || *pattern.rbegin() == '\\' ) )
    {
      pattern.erase( pattern.size() - 1 );
    }

    if ( pattern.empty() )
    {
      return true;
    }

    pattern += "\\/.*";
    const boost::regex matchValidDirectory( pattern, boost::match_single_line );    
    boost::smatch matchResult;
    if ( boost::regex_match( relativeDir, matchResult, matchValidDirectory ) )
    {
      return true;
    }
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified attribute to the list of required attributes that must
// be present when this template creates an asset class.
// 
bool AssetTemplate::AddRequiredAttribute( const Attribute::AttributePtr& attribute )
{
  bool isOk = false;
  if ( !m_RequiredAttributes->ContainsAttribute( attribute->GetType() ) )
  {
    m_RequiredAttributes->SetAttribute( attribute, false );
    isOk = true;
  }
  return isOk;
}

bool AssetTemplate::AddRequiredAttribute( const i32 typeID )
{
  bool isOk = false;
  if ( !m_RequiredAttributes->ContainsAttribute( typeID ) )
  {
    Attribute::AttributePtr attribute = Reflect::AssertCast< Attribute::AttributeBase >( Reflect::Registry::GetInstance()->CreateInstance( typeID ) );
    m_RequiredAttributes->SetAttribute( attribute, false );
    isOk = true;
  }
  return isOk;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the list of attributes that must be present on assets that are 
// created with this template.
// 
const Attribute::M_Attribute& AssetTemplate::GetRequiredAttributes() const
{
  return m_RequiredAttributes->GetAttributes();
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified attribute as an optional one.  
// 
bool AssetTemplate::AddOptionalAttribute( const Attribute::AttributePtr& attribute )
{
  bool isOk = false;
  if ( !m_OptionalAttributes->ContainsAttribute( attribute->GetType() ) )
  {
    m_OptionalAttributes->SetAttribute( attribute, false );
    isOk = true;
  }
  return isOk;
}

bool AssetTemplate::AddOptionalAttribute( const i32 typeID )
{
  bool isOk = false;
  if ( !m_OptionalAttributes->ContainsAttribute( typeID ) )
  {
    Attribute::AttributePtr attribute = Reflect::AssertCast< Attribute::AttributeBase >( Reflect::Registry::GetInstance()->CreateInstance( typeID ) );
    m_OptionalAttributes->SetAttribute( attribute, false );
    isOk = true;
  }
  return isOk;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the list of optional attributes for this template.  These attributes
// can be present when creating an asset using this template.
// 
const Attribute::M_Attribute& AssetTemplate::GetOptionalAttributes() const
{
  return m_OptionalAttributes->GetAttributes();
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