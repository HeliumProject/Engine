#include "Precompile.h"
#include "AssetCollection.h"

#include "Vault.h"

#include "Foundation/Preferences.h"
#include "Foundation/Regex.h"
#include "Foundation/Checksum/MD5.h"
#include "Foundation/Flags.h"
#include "Foundation/File/Path.h"
#include "Foundation/String/Utilities.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/Reflect/Version.h"

using namespace Helium;
using namespace Helium::Editor;

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
, m_Name( TXT( "" ) )
, m_Flags( AssetCollectionFlags::Default )
{
}

/////////////////////////////////////////////////////////////////////////////
AssetCollection::AssetCollection( const tstring& name, const u32 flags )
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

void AssetCollection::GetFileFilters( std::vector< tstring >& filters )
{
    tstring filterFormat = TXT( "Asset Collection (*.collection.%s)|*.collection.%s" );

    std::set< tstring > reflectExtensions;
    Reflect::Archive::GetExtensions( reflectExtensions );

    tchar filter[ 128 ];
    for ( std::set< tstring >::const_iterator itr = reflectExtensions.begin(), end = reflectExtensions.end(); itr != end; ++itr )
    {
        _stprintf_s( filter, 128, filterFormat.c_str(), (*itr).c_str(), (*itr).c_str() );
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
    HELIUM_ASSERT( m_FreezeCount > 0 );

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
void AssetCollection::CreateSignature( const tstring& str, tstring& signature )
{
    signature = str;
    toLower( signature );
    signature = Helium::MD5( signature );
}

void AssetCollection::CreateSignature( tuid id, tstring& signature )
{
    tstringstream stream;
    stream << TUID::HexFormat << id;
    signature = Helium::MD5( stream.str() );
}

///////////////////////////////////////////////////////////////////////////////
void AssetCollection::CreateFilePath( const tstring name, tstring& filePath, const tstring& folder )
{
    if ( folder.empty() )
    {
        Helium::Path prefsPath;
        if ( !Helium::GetPreferencesDirectory( prefsPath ) )
        {
            throw Helium::Exception( TXT( "Could not get preferences directory." ) );
        }
        filePath = prefsPath.Get() + TXT( "collections/" );
    }
    else
    {
        filePath = folder;
    }

    filePath += name;
    filePath += TXT( ".collection.nrb" );
}

/////////////////////////////////////////////////////////////////////////////
void AssetCollection::SetName( const tstring& name )
{
    if ( m_Name != name )
    {
        m_Name = name;
        DirtyField( GetClass()->FindField( &AssetCollection::m_Name ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
tstring AssetCollection::GetDisplayName() const
{
    tstringstream stream;
    stream << GetName();
    stream << TXT( " (" ) << GetAssetPaths().size() << TXT( " " );
    stream << ( ( GetAssetPaths().size() == 1 ) ? TXT( "item" ) : TXT( "items" ) );
    stream << TXT( ")" );

    return stream.str();
}

///////////////////////////////////////////////////////////////////////////////
tstring AssetCollection::GetQueryString() const
{
    tstring queryString = TXT( "collection: \"" ) + GetName() + TXT( "\"" );
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
    return Helium::HasFlags<i32>( GetFlags(), AssetCollectionFlags::Dynamic );
}

/////////////////////////////////////////////////////////////////////////////
bool AssetCollection::IsTemporary() const
{
    return Helium::HasFlags<i32>( GetFlags(), AssetCollectionFlags::Temporary );
}

/////////////////////////////////////////////////////////////////////////////
bool AssetCollection::CanRename() const
{
    return ( !ReadOnly() && Helium::HasFlags<i32>( GetFlags(), AssetCollectionFlags::CanRename ) );
}

/////////////////////////////////////////////////////////////////////////////
bool AssetCollection::CanHandleDragAndDrop() const
{
    return Helium::HasFlags<i32>( GetFlags(), AssetCollectionFlags::CanHandleDragAndDrop );
}

/////////////////////////////////////////////////////////////////////////////
bool AssetCollection::ReadOnly() const
{
    return Helium::HasFlags<i32>( GetFlags(), AssetCollectionFlags::ReadOnly );
}

/////////////////////////////////////////////////////////////////////////////
void AssetCollection::SetAssetReferences( const std::set< Helium::Path >& references )
{
    m_AssetPaths.clear();
    m_AssetPaths = references;
    DirtyField( GetClass()->FindField( &AssetCollection::m_AssetPaths ) );
}

/////////////////////////////////////////////////////////////////////////////
bool AssetCollection::AddAsset( const Helium::Path& path )
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
bool AssetCollection::AddAssets( const std::set< Helium::Path >& assets )
{
    bool result = false;

    for ( std::set< Helium::Path >::const_iterator itr = assets.begin(), end = assets.end(); itr != end; ++itr )
    {
        const Helium::Path& path = *itr;

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
bool AssetCollection::RemoveAsset( const Helium::Path& path )
{
    if ( ContainsAsset( path.Hash() ) )
    {
        u64 passedId = path.Hash();
        for( std::set< Helium::Path >::const_iterator itr = m_AssetPaths.begin(), end = m_AssetPaths.end(); itr != end; ++itr )
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
    for( std::set< Helium::Path >::const_iterator itr = m_AssetPaths.begin(), end = m_AssetPaths.end(); itr != end; ++itr )
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
AssetCollectionPtr AssetCollection::LoadFrom( const Helium::Path& path )
{
    if ( !path.Exists() )
    {
        Log::Warning( TXT( "Unable to read collection from file %s; Reason: File does not exist.\n" ), path.c_str() );
        return NULL;
    }

    // load actual class instance from disk
    AssetCollectionPtr assetCollection = NULL;
    try
    {
        assetCollection = Reflect::Archive::FromFile< AssetCollection >( path.c_str() );
    }
    catch ( const Helium::Exception& ex )
    {
        Log::Error( TXT( "Unable to read asset collection from file %s; Reason: %s.\n" ), path.c_str(), ex.What() );
        return NULL;
    }

    if( assetCollection == NULL )
    {
        Log::Error( TXT( "Unable to read asset collection from file %s.\n" ), path.c_str() );
        return NULL;
    }

    u32 flags = assetCollection->GetFlags();
    Helium::Path file( path );
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
bool AssetCollection::SaveTo( const AssetCollection* collection, const tstring& path )
{
    bool saved = false;
    try
    {
        Reflect::Archive::ToFile( collection, path );
        saved = true;
    }
    catch ( const Helium::Exception& ex )
    {
        Log::Error( TXT( "Unable to save asset collection '%s', to '%s'; Reason: %s.\n" ), collection->GetName().c_str(), path.c_str(), ex.Get() );
        saved = false;
    }

    return saved;
}

///////////////////////////////////////////////////////////////////////////////
bool AssetCollection::IsValidCollectionName( const tstring& name, tstring& errors )
{
    const tregex matchValidName( TXT( "^[a-z0-9]{1}[\\w\\-\\(\\. ]{1,24}$" ), std::tr1::regex::icase );

    tsmatch  matchResult;
    if ( !std::tr1::regex_match( name, matchResult, matchValidName ) )
    {
        errors = TXT( "Collection names must have a lenght less than 25 characters, and can only contain alphanumeric characters, spaces and special characters: \'.\', \'_\', and \'-\'" );
        return false;
    }

    return true;
}

