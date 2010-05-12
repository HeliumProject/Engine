#include "Windows/Windows.h"

#include "AssetClass.h"
#include "EngineTypeInfo.h"
#include "Exceptions.h"

#include "Attribute/Attribute.h"

#include "EntityAsset.h"
#include "ShaderAsset.h"
#include "LevelAsset.h"
#include "TexturePackBase.h"

#include "FileSystem/FileSystem.h"
#include "File/Manager.h"
#include "Finder/Finder.h"
#include "Console/Console.h"
#include "Windows/Thread.h"
#include "Reflect/Version.h"
#include "Reflect/Object.h"

#include <memory>

using namespace Reflect;
using namespace Asset;

REFLECT_DEFINE_ABSTRACT( AssetClass );

void AssetClass::EnumerateClass( Reflect::Compositor<AssetClass>& comp )
{
  Reflect::Field* fieldDescription = comp.AddField( &AssetClass::m_Description, "m_Description" );

  Reflect::Field* fieldAssetClassID = comp.AddField( &AssetClass::m_AssetClassID, "m_AssetClassID", Reflect::FieldFlags::FileID | Reflect::FieldFlags::Discard | Reflect::FieldFlags::Hide );
}

AssetClass::AssetClass()
  : m_AssetClassID( TUID::Null )
  , m_EngineType ( EngineTypes::Null )
{

}

std::string AssetClass::GetFilePath() const
{
  std::string assetPath;

  if ( m_AssetClassID != TUID::Null )
  {
    File::GlobalManager().GetPath( m_AssetClassID, assetPath );
  }

  return assetPath;
}

std::string AssetClass::GetBuiltDir() const
{
  return Finder::GetBuiltFolder( m_AssetClassID );
}

std::string AssetClass::GetDataDir() const
{
  std::string assetPath;
  File::GlobalManager().GetPath( m_AssetClassID, assetPath );

  FileSystem::StripLeaf( assetPath );
  return assetPath;
}

const std::string& AssetClass::GetFullName() const
{
  if ( m_FullName.empty() )
  {
    m_FullName = GetFilePath();
    FileSystem::StripPrefix( File::GlobalManager().GetManagedAssetsRoot(), m_FullName );
  }

  return m_FullName;
}

std::string AssetClass::GetQualifiedName( const AssetClass* assetClass )
{
  EngineType engineType = assetClass->GetEngineType();

  if ( ( engineType != EngineTypes::Level )
    && ( engineType != EngineTypes::Cinematic )
    && ( engineType != EngineTypes::Movie ) )
  {
    throw Exception( "GetQualifiedName used incorrectly, please use GetFullName for assets of this type." );
  }

  std::string qualifiedName;
  
  qualifiedName = assetClass->GetFilePath();
  FileSystem::StripPrefix( File::GlobalManager().GetManagedAssetsRoot(), qualifiedName );

  // strip off leaf
  FileSystem::StripLeaf( qualifiedName );

  // erase the /
  qualifiedName.erase( qualifiedName.size() - 1 );

  // strip off the first directory
  std::string::size_type pos = qualifiedName.find( "/" );
  if ( pos != std::string::npos )
  {
    qualifiedName = qualifiedName.substr( pos + 1 );
  }

  return qualifiedName;
}

const std::string& AssetClass::GetShortName() const
{
  if ( m_ShortName.empty() )
  {
    m_ShortName = GetFilePath();
    FileSystem::GetLeaf( m_ShortName, m_ShortName );

    // strip off extension
    while ( FileSystem::HasExtension( m_ShortName ) )
    {
      FileSystem::StripExtension( m_ShortName );
    }
  }

  return m_ShortName;
}

EngineType AssetClass::GetEngineType() const
{
  // if we have not been classified yet
  if ( m_EngineType == EngineTypes::Null )
  {
    // classify this
    Classify();
  }

  return m_EngineType;
}

std::string AssetClass::GetEngineTypeName() const
{
  std::string engineTypeName( "Unknown" );
  GetEngineTypeName( this, engineTypeName );
  return engineTypeName;
}

void AssetClass::GetEngineTypeName( const Asset::AssetClass* assetClass, std::string& engineTypeName )
{
  if ( assetClass )
  {
    const Reflect::Enumeration* info = Reflect::Registry::GetInstance()->GetEnumeration( "EngineType" );
    if ( info && info->GetElementLabel( assetClass->GetEngineType(), engineTypeName ) )
    {
      return;
    }
  }
  engineTypeName = "Unknown";
}

// EngineTypeInfo funcitons
const std::string& AssetClass::GetEngineTypeBuilderDLL( const EngineType engineType )
{
  return Asset::GetEngineTypeBuilderDLL( engineType );
}

const std::string& AssetClass::GetEngineTypeIcon( const EngineType engineType )
{
  return Asset::GetEngineTypeIcon( engineType );
}


void AssetClass::AttributeChanged( const Attribute::AttributeBase* attr )
{
  __super::AttributeChanged( attr );

  // we just changed, so re-classify if we have been classified before
  if (m_EngineType != EngineTypes::Null)
  {
    Classify();
  }
}

void AssetClass::SetAttribute( const Attribute::AttributePtr& attr, bool validate )
{
  __super::SetAttribute( attr, validate );

  if ( m_EngineType != EngineTypes::Null )
  {
    Classify();
  }
}

void AssetClass::RemoveAttribute( i32 typeID )
{
  __super::RemoveAttribute( typeID );

  if ( m_EngineType != EngineTypes::Null )
  {
    Classify();
  }
}

void AssetClass::Serialize( const AssetVersionPtr &version )
{
  std::string filePath;
  File::GlobalManager().GetPath( m_AssetClassID, filePath );

  Reflect::Archive::ToFile( this, filePath, version );

  m_Modified = false;
}

/////////////////////////////////////////////////////////////////////////////
// Override this function to provide logic that makes sure an asset class is
// valid.  Fill out the error message provided so that it can be reported
// to the user.  This function is intended to be used by the UI to make sure
// that an asset is valid before it is built (validation is done during save).
// 
bool AssetClass::ValidateClass( std::string& error ) const
{
  error.clear();
  return true;
}

bool AssetClass::ValidateCompatible( const Attribute::AttributePtr &attr, std::string& error ) const
{
  if ( attr->GetAttributeUsage() == Attribute::AttributeUsages::Instance )
  {
    error = "The " + attr->GetClass()->m_UIName + " attribute can only be added to an instance of an asset.";
    return false;
  }

  return __super::ValidateCompatible( attr, error );
}

void AssetClass::LoadFinished()
{

}

bool AssetClass::IsBuildable() const
{
  return false;
}

bool AssetClass::IsViewable() const
{
  return false;
}

void AssetClass::CopyTo(const Reflect::ElementPtr& destination) 
{
  // Restore the Asset Class ID after performing the copy
  AssetClass* destinationAsset = Reflect::ObjectCast< AssetClass >( destination );
  tuid destID = destinationAsset->m_AssetClassID;
  __super::CopyTo( destination );
  destinationAsset->m_AssetClassID = destID;
}

typedef stdext::hash_map< tuid, AssetClassPtr > M_AssetClass;

M_AssetClass g_AssetClassCache;

CRITICAL_SECTION g_AssetClassCacheSection;

void AssetClass::InitializeCache()
{
  InitializeCriticalSection( &g_AssetClassCacheSection );
}

void AssetClass::CleanupCache()
{
  g_AssetClassCache.clear();
  DeleteCriticalSection( &g_AssetClassCacheSection );
}

void AssetClass::InvalidateCache( tuid assetClassID )
{
  Windows::TakeSection section (g_AssetClassCacheSection);
  g_AssetClassCache.erase( assetClassID );
}

void AssetClass::InvalidateCache()
{
  Windows::TakeSection section (g_AssetClassCacheSection);
  g_AssetClassCache.clear();
}

tuid AssetClass::FindAuthoritativeTuid( tuid assetClassID )
{
  File::ManagedFilePtr managedFile = File::GlobalManager().GetManagedFile( assetClassID );
  if ( !managedFile.ReferencesObject() )
  {
    throw Exception( "Could not locate an asset with id: "TUID_HEX_FORMAT" when attempting to load asset class.\n", assetClassID );
  }
  
  return managedFile->m_Id;
}

AssetClassPtr AssetClass::FindAssetClass( tuid assetClassID, bool useCache )
{
  // check to see if the tuid is in the cache (only authoritative tuids are in the cache)
  if ( useCache )
  {
    Windows::TakeSection section (g_AssetClassCacheSection);

    M_AssetClass::iterator found = g_AssetClassCache.find( assetClassID );
    if ( found != g_AssetClassCache.end() )
    {
      return found->second;
    }
  }

  // determine the authoritative tuid
  File::ManagedFilePtr managedFile = File::GlobalManager().GetManagedFile( assetClassID );
  if ( !managedFile.ReferencesObject() )
  {
    throw UnableToLocateAssetClassException( assetClassID );
  }

  // this is now our authoritative tuid
  assetClassID = managedFile->m_Id;
  
  // the tuid passed in may not have been authoritative; check the cache again
  if ( useCache )
  {
    Windows::TakeSection section (g_AssetClassCacheSection);

    M_AssetClass::iterator found = g_AssetClassCache.find( assetClassID );
    if ( found != g_AssetClassCache.end() )
    {
      return found->second;
    }
  }

  // load actual class instance from disk
  AssetClassPtr assetClass = NULL;
  try
  {
    assetClass = Reflect::Archive::FromFile< AssetClass >( managedFile->m_Path );
  }
  catch ( const Nocturnal::Exception& exception )
  {
    Console::Warning( "%s\n", exception.what() );
  }

  // check that we got it
  if( assetClass == NULL )
  {
    // we failed
    throw UnableToLoadAssetClassException( assetClassID, managedFile->m_Path );
  }

  // save the class id on the instance to it can write itself back out
  assetClass->m_AssetClassID = assetClassID;

  if ( useCache )
  {
    Windows::TakeSection section (g_AssetClassCacheSection);

    // put the recently loaded AssetClass into the cache (this should be authoritative tuids only)
    g_AssetClassCache[assetClassID] = assetClass;
  }

  assetClass->LoadFinished();

  // success
  return assetClass;
}

void AssetClass::Classify() const
{
  EngineType previousType = m_EngineType;

  m_EngineType = AssetClass::Classify( this );

  if ( m_EngineType != previousType )
  {
    m_EngineTypeChanged.Raise( EngineTypeChangeArgs( this, previousType ) );
  }
}

// EngineTypeInfo funcitons
static const EntityAssetPtr& GetEngineTypeEntityAsset( const EngineType engineType )
{
  const EngineTypeInfo& engineTypeInfo = GetEngineTypeInfo( engineType );
  NOC_ASSERT( engineTypeInfo.IsValid() && engineTypeInfo.m_EntityAsset != NULL );
  return engineTypeInfo.m_EntityAsset;
}

EngineType AssetClass::Classify( const AssetClass* assetClass )
{
  if ( assetClass->HasType( Reflect::GetType<LevelAsset>() ) )
    return EngineTypes::Level;

  if ( assetClass->HasType( Reflect::GetType<ShaderAsset>() ) )
    return EngineTypes::Shader;

  if ( assetClass->HasType( Reflect::GetType<TexturePackBase>() ) )
    return EngineTypes::TexturePack; 

  // you probably won't hit this because Shrub doesn't require very much in the
  // IsSubset rule, so probably if you think it's going to hit this return, 
  // really it will hit the Shrub return
  // 
  return EngineTypes::Null;
}

static int ScoreEngineType( const AssetClass* assetClass, const AssetClass* engineClass )
{
  int score = 0;

  Attribute::M_Attribute::const_iterator itor = assetClass->GetAttributes().begin();
  Attribute::M_Attribute::const_iterator end = assetClass->GetAttributes().end();
  for( ; itor != end; ++itor )
  {
    if( engineClass->ContainsAttribute( itor->second->GetType() ) )
      ++score;


  }

  // not sure if we want to do this since the global engine type "templates" contain the
  // maximum number of attributes that something can have and still be classified as that type
  itor = engineClass->GetAttributes().begin();
  end = engineClass->GetAttributes().end();
  for( ; itor != end; ++itor )
  {
    if( !assetClass->ContainsAttribute( itor->second->GetType() ) )
      --score;
  }

  return score;
}

AssetClassPtr AssetClass::NearestEngineType( const Attribute::AttributeBase* addedAttribute )
{
  AssetClassPtr nearestType;

  // this should probably just be a global array?
  V_AssetClass engineClasses;
  engineClasses.push_back( GetEngineTypeEntityAsset( EngineTypes::Ufrag ) );
  engineClasses.push_back( GetEngineTypeEntityAsset( EngineTypes::Shrub ) );
  engineClasses.push_back( GetEngineTypeEntityAsset( EngineTypes::Tie ) );
  engineClasses.push_back( GetEngineTypeEntityAsset( EngineTypes::Moby ) );

  int score = 0;
  int maxScore = 0;
  bool foundFirstCandidate = false;

  V_AssetClass::iterator itor = engineClasses.begin();
  V_AssetClass::iterator end  = engineClasses.end();

  for( ; itor != end; ++itor )
  {
    // if the engine type doesn't even contain the specified attribute, don't consider it
    if( addedAttribute && !(*itor)->ContainsAttribute( addedAttribute->GetType() ) )
    {
      continue;
    }
    else
    {
      score = ScoreEngineType( this, *itor );

      // if it's the first potential candidate for scoring, set the maxscore to the score
      if( !foundFirstCandidate )
      {
        foundFirstCandidate = true;
        maxScore = score;
        nearestType = *itor;
      }
      if( score > maxScore )
      {
        maxScore = score;
        nearestType = *itor;
      }
    }
  }
  return nearestType;
}
