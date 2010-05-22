#include "Precompile.h"
#include "AssetCollection.h"

#include "Browser.h"

#include "Common/Boost/Regex.h"
#include "Common/Checksum/MD5.h"
#include "Common/Flags.h"
#include "Common/String/Utilities.h"
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
    Reflect::Field* fieldFlags = comp.AddField( &AssetCollection::m_Flags, "m_Flags" );
    Reflect::Field* fieldAssetReferences = comp.AddField( &AssetCollection::m_AssetReferences, "m_AssetReferences", Reflect::FieldFlags::FileRef );
}

/////////////////////////////////////////////////////////////////////////////
AssetCollection::AssetCollection()
: m_FreezeCount( 0 ) 
, m_Name( "" )
, m_Flags( AssetCollectionFlags::Default )
, m_spFileReference( NULL )
{
}

/////////////////////////////////////////////////////////////////////////////
AssetCollection::AssetCollection( const std::string& name, const u32 flags )
: m_FreezeCount( 0 ) 
, m_Name( name )
, m_Flags( AssetCollectionFlags::Default )
, m_spFileReference( NULL )
{
    SetFlags( flags );
}

/////////////////////////////////////////////////////////////////////////////
AssetCollection::~AssetCollection()
{
    m_AssetReferences.clear();
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
    FinderSpecs::Luna::ASSET_COLLECTION_RB_DECORATION.Modify( filePath );
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
    stream << " (" << GetAssetReferences().size() << " ";
    stream << ( ( GetAssetReferences().size() == 1 ) ? "item" : "items" );
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
void AssetCollection::SetAssetReferences( const File::S_Reference& references )
{
    m_AssetReferences.clear();
    m_AssetReferences = references;
    DirtyField( GetClass()->FindField( &AssetCollection::m_AssetReferences ) );
}

/////////////////////////////////////////////////////////////////////////////
bool AssetCollection::AddAsset( const File::Reference& fileRef )
{
    if ( !ContainsAsset( fileRef.GetHash() ) )
    {
        File::ReferencePtr ref = new File::Reference( fileRef );
        m_AssetReferences.insert( ref );

        DirtyField( GetClass()->FindField( &AssetCollection::m_AssetReferences ) );

        return true;
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////
bool AssetCollection::AddAssets( const File::S_Reference& assets )
{
    bool result = false;

    for ( File::S_Reference::const_iterator itr = assets.begin(), end = assets.end(); itr != end; ++itr )
    {
        const File::Reference ref = *(*itr);

        if ( !ContainsAsset( ref.GetHash() ) )
        {
            File::ReferencePtr newRef = new File::Reference( ref );
            m_AssetReferences.insert( newRef );
            result = true;
        }
    }

    if ( result )
    {
        DirtyField( GetClass()->FindField( &AssetCollection::m_AssetReferences ) );
    }

    return result;
}

/////////////////////////////////////////////////////////////////////////////
bool AssetCollection::RemoveAsset( const File::Reference& fileRef )
{
    if ( ContainsAsset( fileRef.GetHash() ) )
    {
        u64 passedId = fileRef.GetHash();
        for( File::S_Reference::const_iterator itr = m_AssetReferences.begin(), end = m_AssetReferences.end(); itr != end; ++itr )
        {
            if ( (*itr)->GetHash() == passedId )
            {
                m_AssetReferences.erase( itr );
                DirtyField( GetClass()->FindField( &AssetCollection::m_AssetReferences ) );
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
    for( File::S_Reference::const_iterator itr = m_AssetReferences.begin(), end = m_AssetReferences.end(); itr != end; ++itr )
    {
        if ( (*itr)->GetHash() == id )
        {
            return true;
        }
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////
void AssetCollection::ClearAssets()
{
    m_AssetReferences.clear();

    DirtyField( GetClass()->FindField( &AssetCollection::m_AssetReferences ) );
}

/////////////////////////////////////////////////////////////////////////////////
bool AssetCollection::operator<( const AssetCollection& rhs ) const
{
    return m_Name < rhs.m_Name;
}

/////////////////////////////////////////////////////////////////////////////
AssetCollectionPtr AssetCollection::LoadFrom( File::Reference& fileRef )
{
    if ( !fileRef.GetFile().Exists() )
    {
        Console::Warning( "Unable to read collection from file %s; Reason: File does not exist.\n", fileRef.GetPath().c_str() );
        return NULL;
    }

    // load actual class instance from disk
    AssetCollectionPtr assetCollection = NULL;
    try
    {
        assetCollection = Reflect::Archive::FromFile< AssetCollection >( fileRef.GetPath() );
    }
    catch ( const Nocturnal::Exception& ex )
    {
        Console::Error( "Unable to read asset collection from file %s; Reason: %s.\n", fileRef.GetPath().c_str(), ex.what() );
        return NULL;
    }

    if( assetCollection == NULL )
    {
        Console::Error( "Unable to read asset collection from file %s.\n", fileRef.GetPath().c_str() );
        return NULL;
    }

    u32 flags = assetCollection->GetFlags();
    if ( !fileRef.GetFile().IsWritable() )
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

