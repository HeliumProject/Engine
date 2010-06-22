#include "Precompile.h"
#include "AssetCollection.h"

#include "Browser.h"

#include "Application/Preferences.h"
#include "Foundation/Boost/Regex.h"
#include "Foundation/Checksum/MD5.h"
#include "Foundation/Flags.h"
#include "Foundation/File/Path.h"
#include "Foundation/String/Utilities.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/Reflect/Version.h"

using namespace Luna;
using namespace Nocturnal;

///////////////////////////////////////////////////////////////////////////////
/// class AssetCollection
///////////////////////////////////////////////////////////////////////////////
REFLECT_DEFINE_CLASS( AssetCollection )
void AssetCollection::EnumerateClass( Reflect::Compositor<AssetCollection>& comp )
{
    Reflect::Field* fieldName = comp.AddField( &AssetCollection::m_Name, "m_Name" );
    Reflect::Field* fieldFlags = comp.AddField( &AssetCollection::m_Flags, "m_Flags" );
    Reflect::Field* fieldAssetPaths = comp.AddField( &AssetCollection::m_AssetPaths, "m_AssetPaths", Reflect::FieldFlags::Path );
}

/////////////////////////////////////////////////////////////////////////////
AssetCollection::AssetCollection()
: m_FreezeCount( 0 ) 
, m_Name( "" )
, m_Flags( AssetCollectionFlags::Default )
{
}

/////////////////////////////////////////////////////////////////////////////
AssetCollection::AssetCollection( const std::string& name, const u32 flags )
: m_FreezeCount( 0 ) 
, m_Name( name )
, m_Flags( AssetCollectionFlags::Default )
{
    SetFlags( flags );
}

/////////////////////////////////////////////////////////////////////////////
AssetCollection::~AssetCollection()
{
    m_AssetPaths.clear();
}

void AssetCollection::GetFileFilters( V_string& filters )
{
    std::string filterFormat = "Asset Collection (*.collection.%s)|*.collection.%s";

    S_string reflectExtensions;
    Reflect::Archive::GetExtensions( reflectExtensions );

    char filter[ 128 ];
    for ( S_string::const_iterator itr = reflectExtensions.begin(), end = reflectExtensions.end(); itr != end; ++itr )
    {
        sprintf_s( filter, 128, filterFormat.c_str(), (*itr).c_str(), (*itr).c_str() );
        filters.push_back( filter );
    }
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
    if ( folder.empty() )
    {
        Nocturnal::Path prefsPath;
        if ( !Application::GetPreferencesDirectory( prefsPath ) )
        {
            throw Nocturnal::Exception( "Could not get preferences directory." );
        }
        filePath = prefsPath.Get() + "collections/";
    }
    else
    {
        filePath = folder;
    }

    filePath += name;
    filePath += ".collection.rb";
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
    stream << " (" << GetAssetPaths().size() << " ";
    stream << ( ( GetAssetPaths().size() == 1 ) ? "item" : "items" );
    stream << ")";

    return stream.str();
}

///////////////////////////////////////////////////////////////////////////////
std::string AssetCollection::GetQueryString() const
{
    std::string queryString = "collection: \"" + GetName() + "\"";
    return queryString;
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
void AssetCollection::SetAssetReferences( const std::set< Nocturnal::Path >& references )
{
    m_AssetPaths.clear();
    m_AssetPaths = references;
    DirtyField( GetClass()->FindField( &AssetCollection::m_AssetPaths ) );
}

/////////////////////////////////////////////////////////////////////////////
bool AssetCollection::AddAsset( const Nocturnal::Path& path )
{
    if ( !ContainsAsset( path.Hash() ) )
    {
        m_AssetPaths.insert( path );

        DirtyField( GetClass()->FindField( &AssetCollection::m_AssetPaths ) );

        return true;
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////
bool AssetCollection::AddAssets( const std::set< Nocturnal::Path >& assets )
{
    bool result = false;

    for ( std::set< Nocturnal::Path >::const_iterator itr = assets.begin(), end = assets.end(); itr != end; ++itr )
    {
        const Nocturnal::Path& path = *itr;

        if ( !ContainsAsset( path.Hash() ) )
        {
            m_AssetPaths.insert( path );
            result = true;
        }
    }

    if ( result )
    {
        DirtyField( GetClass()->FindField( &AssetCollection::m_AssetPaths ) );
    }

    return result;
}

/////////////////////////////////////////////////////////////////////////////
bool AssetCollection::RemoveAsset( const Nocturnal::Path& path )
{
    if ( ContainsAsset( path.Hash() ) )
    {
        u64 passedId = path.Hash();
        for( std::set< Nocturnal::Path >::const_iterator itr = m_AssetPaths.begin(), end = m_AssetPaths.end(); itr != end; ++itr )
        {
            if ( (*itr).Hash() == passedId )
            {
                m_AssetPaths.erase( itr );
                DirtyField( GetClass()->FindField( &AssetCollection::m_AssetPaths ) );
                return true;
            }
        }
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////
// Queries the container for the existence of the specified asset
bool AssetCollection::ContainsAsset( u64 id ) const
{
    for( std::set< Nocturnal::Path >::const_iterator itr = m_AssetPaths.begin(), end = m_AssetPaths.end(); itr != end; ++itr )
    {
        if ( (*itr).Hash() == id )
        {
            return true;
        }
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////
void AssetCollection::ClearAssets()
{
    m_AssetPaths.clear();

    DirtyField( GetClass()->FindField( &AssetCollection::m_AssetPaths ) );
}

/////////////////////////////////////////////////////////////////////////////////
bool AssetCollection::operator<( const AssetCollection& rhs ) const
{
    return m_Name < rhs.m_Name;
}

/////////////////////////////////////////////////////////////////////////////
AssetCollectionPtr AssetCollection::LoadFrom( const Nocturnal::Path& path )
{
    if ( !path.Exists() )
    {
        Log::Warning( "Unable to read collection from file %s; Reason: File does not exist.\n", path.c_str() );
        return NULL;
    }

    // load actual class instance from disk
    AssetCollectionPtr assetCollection = NULL;
    try
    {
        assetCollection = Reflect::Archive::FromFile< AssetCollection >( path.c_str() );
    }
    catch ( const Nocturnal::Exception& ex )
    {
        Log::Error( "Unable to read asset collection from file %s; Reason: %s.\n", path.c_str(), ex.what() );
        return NULL;
    }

    if( assetCollection == NULL )
    {
        Log::Error( "Unable to read asset collection from file %s.\n", path.c_str() );
        return NULL;
    }

    u32 flags = assetCollection->GetFlags();
    Nocturnal::Path file( path );
    if ( !file.Writable() )
    {
        flags &= ~AssetCollectionFlags::CanRename;
        flags &= ~AssetCollectionFlags::CanHandleDragAndDrop;
        flags |= AssetCollectionFlags::ReadOnly;
    }
    assetCollection->SetFlags( flags );

    return assetCollection;
}

/////////////////////////////////////////////////////////////////////////////
bool AssetCollection::SaveTo( const AssetCollection* collection, const std::string& path )
{
    bool saved = false;
    try
    {
        Reflect::Archive::ToFile( collection, path );
        saved = true;
    }
    catch ( const Nocturnal::Exception& ex )
    {
        Log::Error( "Unable to save asset collection '%s', to '%s'; Reason: %s.\n", collection->GetName().c_str(), path.c_str(), ex.Get() );
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

