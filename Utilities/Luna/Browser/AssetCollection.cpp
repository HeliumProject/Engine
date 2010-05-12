#include "Precompile.h"
#include "AssetCollection.h"

#include "Browser.h"

#include "Common/Boost/Regex.h"
#include "Common/Checksum/MD5.h"
#include "Common/Flags.h"
#include "Common/String/Utilities.h"
#include "File/Manager.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Finder/LunaSpecs.h"
#include "Reflect/Element.h"
#include "Reflect/Version.h"

using namespace Luna;


///////////////////////////////////////////////////////////////////////////////
/// class AssetCollection
///////////////////////////////////////////////////////////////////////////////
REFLECT_DEFINE_CLASS( AssetCollection )
void AssetCollection::EnumerateClass( Reflect::Compositor<AssetCollection>& comp )
{
  Reflect::Field* fieldName = comp.AddField( &AssetCollection::m_Name, "m_Name" );
  Reflect::Field* fieldID = comp.AddField( &AssetCollection::m_ID, "m_ID", Reflect::FieldFlags::Force );
  Reflect::Field* fieldFlags = comp.AddField( &AssetCollection::m_Flags, "m_Flags" );
  Reflect::Field* fieldAssetIDs = comp.AddField( &AssetCollection::m_AssetIDs, "m_AssetIDs", Reflect::FieldFlags::FileID );
}

/////////////////////////////////////////////////////////////////////////////
AssetCollection::AssetCollection()
: m_FreezeCount( 0 ) 
, m_Name( "" )
, m_ID( TUID::Generate() )
, m_Flags( AssetCollectionFlags::Default )
, m_FilePath( "" )
{
}

/////////////////////////////////////////////////////////////////////////////
AssetCollection::AssetCollection( const std::string& name, const u32 flags )
: m_FreezeCount( 0 ) 
, m_Name( name )
, m_ID( TUID::Generate() )
, m_Flags( AssetCollectionFlags::Default )
, m_FilePath( "" )
{
  SetFlags( flags );
}

/////////////////////////////////////////////////////////////////////////////
AssetCollection::~AssetCollection()
{
  m_AssetIDs.clear();
}

///////////////////////////////////////////////////////////////////////////////
void AssetCollection::Freeze()
{
  ++m_FreezeCount;
}

///////////////////////////////////////////////////////////////////////////////
void AssetCollection::Thaw()
{
  NOC_ASSERT( m_FreezeCount > 0 );

  if ( --m_FreezeCount == 0 )
  {
    RaiseChanged();
  }
}

///////////////////////////////////////////////////////////////////////////////
void AssetCollection::DirtyField( const Reflect::Field* field )
{
  if ( m_FreezeCount == 0 )
  {
    RaiseChanged( field );
  }
}

//
// Static Helper functions
//

///////////////////////////////////////////////////////////////////////////////
void AssetCollection::CreateSignature( const std::string& str, std::string& signature )
{
  signature = str;
  toLower( signature );
  signature = Nocturnal::MD5( signature );
}

void AssetCollection::CreateSignature( tuid id, std::string& signature )
{
  std::stringstream stream;
  stream << TUID::HexFormat << id;
  signature = Nocturnal::MD5( stream.str() );
}

///////////////////////////////////////////////////////////////////////////////
void AssetCollection::CreateFilePath( const std::string name, std::string& filePath, const std::string& folder )
{
  filePath = !folder.empty() ? folder : FinderSpecs::Luna::PREFERENCES_FOLDER.GetFolder() + "collections/";  
  filePath += name;
  FinderSpecs::Luna::ASSET_COLLECTION_IRB_DECORATION.Modify( filePath );
}

/////////////////////////////////////////////////////////////////////////////
void AssetCollection::SetName( const std::string& name )
{
  if ( m_Name != name )
  {
    m_Name = name;
    DirtyField( GetClass()->FindField( &AssetCollection::m_Name ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
std::string AssetCollection::GetDisplayName() const
{
  std::stringstream stream;
  stream << GetName();
  stream << " (" << GetAssetIDs().size() << " ";
  stream << ( ( GetAssetIDs().size() == 1 ) ? "item" : "items" );
  stream << ")";

  return stream.str();
}

///////////////////////////////////////////////////////////////////////////////
std::string AssetCollection::GetQueryString() const
{
  std::string queryString = "collection: \"" + GetName() + "\"";
  return queryString;
}

///////////////////////////////////////////////////////////////////////////////
void AssetCollection::SetFilePath( const std::string& filePath )
{
  m_FilePath = filePath;
}

/////////////////////////////////////////////////////////////////////////////
const std::string& AssetCollection::GetFilePath() const
{
  if ( m_FilePath.empty() )
  {
    CreateFilePath( GetName(), m_FilePath );
  }
  return m_FilePath;
}

/////////////////////////////////////////////////////////////////////////////
void AssetCollection::SetFlags( const u32 flags )
{
  m_Flags = flags;
}

/////////////////////////////////////////////////////////////////////////////
bool AssetCollection::IsDynamic() const
{
  return Nocturnal::HasFlags<i32>( GetFlags(), AssetCollectionFlags::Dynamic );
}

/////////////////////////////////////////////////////////////////////////////
bool AssetCollection::IsTemporary() const
{
  return Nocturnal::HasFlags<i32>( GetFlags(), AssetCollectionFlags::Temporary );
}

/////////////////////////////////////////////////////////////////////////////
bool AssetCollection::CanRename() const
{
  return ( !ReadOnly() && Nocturnal::HasFlags<i32>( GetFlags(), AssetCollectionFlags::CanRename ) );
}

/////////////////////////////////////////////////////////////////////////////
bool AssetCollection::CanHandleDragAndDrop() const
{
  return Nocturnal::HasFlags<i32>( GetFlags(), AssetCollectionFlags::CanHandleDragAndDrop );
}

/////////////////////////////////////////////////////////////////////////////
bool AssetCollection::ReadOnly() const
{
  return Nocturnal::HasFlags<i32>( GetFlags(), AssetCollectionFlags::ReadOnly );
}

/////////////////////////////////////////////////////////////////////////////
void AssetCollection::SetAssetIDs( const S_tuid& ids )
{
  m_AssetIDs.clear();

  m_AssetIDs = ids;

  DirtyField( GetClass()->FindField( &AssetCollection::m_AssetIDs ) );
}

/////////////////////////////////////////////////////////////////////////////
bool AssetCollection::AddAssetID( tuid id )
{
  if ( !ContainsAssetID( id ) )
  {
    m_AssetIDs.insert( id );

    DirtyField( GetClass()->FindField( &AssetCollection::m_AssetIDs ) );

    return true;
  }

  return false;
}

/////////////////////////////////////////////////////////////////////////////
bool AssetCollection::AddAssetIDs( const S_tuid& ids )
{
  bool result = false;

  for ( S_tuid::const_iterator itr = ids.begin(), end = ids.end(); itr != end; ++itr )
  {
    if ( !ContainsAssetID( *itr ) )
    {
      m_AssetIDs.insert( *itr );
      result = true;
    }
  }

  if ( result )
  {
    DirtyField( GetClass()->FindField( &AssetCollection::m_AssetIDs ) );
  }

  return result;
}

/////////////////////////////////////////////////////////////////////////////
bool AssetCollection::RemoveAssetID( tuid id )
{
  if ( ContainsAssetID( id ) )
  {
    m_AssetIDs.erase( m_AssetIDs.find( id ) );

    DirtyField( GetClass()->FindField( &AssetCollection::m_AssetIDs ) );

    return true;
  }

  return false;
}

/////////////////////////////////////////////////////////////////////////////
// Queries the container for the existence of the specified asset
bool AssetCollection::ContainsAssetID( tuid id ) const
{
  if ( m_AssetIDs.find( id ) != m_AssetIDs.end() )
  {
    return true;
  }

  return false;
}

/////////////////////////////////////////////////////////////////////////////
void AssetCollection::ClearAssetIDs()
{
  m_AssetIDs.clear();

  DirtyField( GetClass()->FindField( &AssetCollection::m_AssetIDs ) );
}

/////////////////////////////////////////////////////////////////////////////////
bool AssetCollection::operator<( const AssetCollection& rhs ) const
{
  return GetID() < rhs.GetID();
}

/////////////////////////////////////////////////////////////////////////////////
bool AssetCollection::operator==( const AssetCollection& rhs ) const
{
  return GetID() == rhs.GetID();
}

/////////////////////////////////////////////////////////////////////////////////
bool AssetCollection::operator!=( const AssetCollection& rhs ) const
{
  return GetID() != rhs.GetID();
}

/////////////////////////////////////////////////////////////////////////////
AssetCollectionPtr AssetCollection::LoadFromFile( const std::string& path )
{
  if ( !FileSystem::Exists( path ) )
  {
    Console::Warning( "Unable to read collection from file %s; Reason: File does not exist.\n", path.c_str() );
    return NULL;
  }

  // load actual class instance from disk
  AssetCollectionPtr assetCollection = NULL;
  try
  {
    assetCollection = Reflect::Archive::FromFile< AssetCollection >( path );
  }
  catch ( const Nocturnal::Exception& ex )
  {
    Console::Error( "Unable to read asset collection from file %s; Reason: %s.\n", path.c_str(), ex.what() );
    return NULL;
  }

  if( assetCollection == NULL )
  {
    Console::Error( "Unable to read asset collection from file %s.\n", path.c_str() );
    return NULL;
  }

  assetCollection->SetFilePath( path );


  u32 flags = assetCollection->GetFlags();
  if ( !FileSystem::IsWritable( path ) )
  {
    flags &= ~AssetCollectionFlags::CanRename;
    flags &= ~AssetCollectionFlags::CanHandleDragAndDrop;
    flags |= AssetCollectionFlags::ReadOnly;
  }
  assetCollection->SetFlags( flags );


  return assetCollection;
}

/////////////////////////////////////////////////////////////////////////////
bool AssetCollection::SaveToFile( const AssetCollection* collection, const std::string& path )
{
  if ( path.empty() )
  {
    Console::Warning( "Unable to save asset collection '%s', no file path specified.\n", collection->GetName().c_str() );
    return false;
  }

  bool saved = false;
  try
  {
    Reflect::Archive::ToFile( collection, path );
    saved = true;
  }
  catch ( const Nocturnal::Exception& ex )
  {
    Console::Error( "Unable to save asset collection '%s', to '%s'; Reason: %s.\n", collection->GetName().c_str(), path.c_str(), ex.Get() );
    saved = false;
  }

  return saved;
}

///////////////////////////////////////////////////////////////////////////////
bool AssetCollection::IsValidCollectionName( const std::string& name, std::string& errors )
{
  const boost::regex matchValidName( "^[a-z0-9]{1}[\\w\\-\\(\\. ]{1,24}$", boost::regex::icase | boost::match_single_line );

  boost::smatch  matchResult;
  if ( !boost::regex_match( name, matchResult, matchValidName ) )
  {
    errors = "Collection names must have a lenght less than 25 characters, and can only contain alphanumeric characters, spaces and special characters: \'.\', \'_\', and \'-\'";
    return false;
  }

  return true;
}

