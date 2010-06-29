#include "Precompile.h"
#include "CollectionManager.h"

#include "Foundation/Boost/Regex.h"
#include "Foundation/Checksum/MD5.h"
#include "Foundation/Flags.h"
#include "Foundation/String/Utilities.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/Reflect/Version.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
/// class CollectionManager
///////////////////////////////////////////////////////////////////////////////
REFLECT_DEFINE_CLASS( CollectionManager )
void CollectionManager::EnumerateClass( Reflect::Compositor<CollectionManager>& comp )
{
    Reflect::Field* fieldCollectionFilePaths = comp.AddField( &CollectionManager::m_CollectionFilePaths, "m_CollectionFilePaths" );
}

/////////////////////////////////////////////////////////////////////////////
CollectionManager::CollectionManager()
{
    AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &CollectionManager::OnAssetCollectionsChanged ) );
}

/////////////////////////////////////////////////////////////////////////////
CollectionManager::~CollectionManager()
{
    RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &CollectionManager::OnAssetCollectionsChanged ) );

    ClearCollectionMap();
    m_CollectionFilePaths.clear();
}

///////////////////////////////////////////////////////////////////////////////
void CollectionManager::PreSerialize()
{
    __super::PreSerialize();

    SaveAllCollections();
}

///////////////////////////////////////////////////////////////////////////////
void CollectionManager::PostDeserialize()
{
    __super::PostDeserialize();

    std::set< tstring > copyCollectionFilePaths = m_CollectionFilePaths;

    ClearCollectionMap();
    for ( std::set< tstring >::const_iterator itr = copyCollectionFilePaths.begin(), end = copyCollectionFilePaths.end(); itr != end; ++itr )
    {
        Nocturnal::Path collectionPath( *itr );

        AssetCollectionPtr collection = AssetCollection::LoadFrom( collectionPath );

        if ( collection )
        {
            AddCollectionToMap( collection );
        }
        else
        {
            // TODO debug warning
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the manager already contains the specified collection.
bool CollectionManager::ContainsCollection( AssetCollection* collection ) const
{
    if ( !collection )
    {
        return false;
    }

    return ( FindCollection( collection->GetPath().Get() ) == collection );
}

///////////////////////////////////////////////////////////////////////////////
// Checks that collection name is unique
bool CollectionManager::IsCollectionNameAvailable( AssetCollection* collection, const tstring& name, tstring& errors )
{
    AssetCollection* foundCollection = FindCollection( name );
    if ( collection && foundCollection && (*collection) != (*foundCollection) )
    {
        errors = TXT( "Collection names need to be unique, you already have a collection called: " ) + name;
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
void CollectionManager::GetUniqueName( tstring& name, const tchar* baseName )
{
    // try the base name first
    name = baseName;

    int numTries = 0;
    while ( FindCollection( name.c_str() ) )
    {
        ++numTries;
        tostringstream str ( name );
        str << baseName << TXT( " " ) << numTries;
        name = str.str();
    }
}

///////////////////////////////////////////////////////////////////////////////
bool CollectionManager::AddCollection( AssetCollection* collection )
{
    if ( !collection )
        return false;

    Nocturnal::Insert<M_AssetCollections>::Result inserted = AddCollectionToMap( collection );

    return inserted.second;
}

///////////////////////////////////////////////////////////////////////////////
bool CollectionManager::RenameCollection( AssetCollection* collection, const tstring& name, tstring& errors )
{
    if ( !collection )
        return false;

    if ( !AssetCollection::IsValidCollectionName( name, errors ) )
    {
        return false;
    }

    if ( !IsCollectionNameAvailable( collection, name, errors ) )
    {
        return false;
    }

    tstring oldName = collection->GetName();
    
    tstring oldFilePath = collection->GetPath().Get();

    AssetCollection* foundCollection = FindCollection( oldName );
    if ( !foundCollection )
    {
        errors = TXT( "" );
        return false;
    }

    // only do the work if we have to
    if ( oldName != name )
    {
        foundCollection->SetName( name );

        tstring filePath;
        AssetCollection::CreateFilePath( name, filePath );
        
        Nocturnal::Path path( filePath );
        foundCollection->SetPath( path );

        // erase the old reference from the map
        if ( path.Exists() )
        {
            path.Delete();
        }

        RaiseChanged( GetClass()->FindField( &CollectionManager::m_AssetCollections ) );    
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
void CollectionManager::DeleteCollection( AssetCollection* collection )
{
    if ( !collection )
        return;

    tstring filePath = collection->GetPath().Get();

    CloseCollection( collection );

    Nocturnal::Path path( filePath );
    if ( !filePath.empty() && path.Exists() )
    {
        path.Delete();
    }
}

///////////////////////////////////////////////////////////////////////////////
AssetCollection* CollectionManager::OpenCollection( const tstring& path, bool copyLocal )
{
    Nocturnal::Path filePath( path );

    AssetCollectionPtr loadCollection = AssetCollection::LoadFrom( filePath );
    if ( loadCollection )
    {
        tstring name;
        GetUniqueName( name, ( loadCollection->GetName().empty() ? TXT( "Imported Collection" ) : loadCollection->GetName().c_str() ) );
        loadCollection->SetName( name );

        u32 flags = loadCollection->GetFlags();
        if ( copyLocal )
        {
            tstring filePath;
            AssetCollection::CreateFilePath( name, filePath );

            Nocturnal::Path localPath( filePath );

            loadCollection->SetPath( localPath );

            flags |= AssetCollectionFlags::CanRename | AssetCollectionFlags::CanHandleDragAndDrop;
        }
        loadCollection->SetFlags( flags );

        // Append the newly imported collection
        Nocturnal::Insert<M_AssetCollections>::Result inserted = AddCollectionToMap( loadCollection );

        return inserted.first->second;
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
void CollectionManager::CloseCollection( AssetCollection* collection )
{
    if ( !collection )
        return;

    RemoveCollectionFromMap( collection );
}

///////////////////////////////////////////////////////////////////////////////
AssetCollection* CollectionManager::ImportCollection( const tstring& path )
{
    return OpenCollection( path, true );
}

///////////////////////////////////////////////////////////////////////////////
AssetCollection* CollectionManager::FindCollection( const tstring& name ) const
{
    for ( M_AssetCollections::const_iterator itr = m_AssetCollections.begin(), end = m_AssetCollections.end(); itr != end; ++itr )
    {    
        AssetCollection* collection = itr->second;
        if ( collection->GetName() == name )
        {
            return collection;
        }
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
bool CollectionManager::ImportIntoStaticCollection( AssetCollection* collection, const tstring& path )
{
    if ( !collection )
        return false;

    AssetCollection* foundCollection = FindCollection( collection->GetPath().Get() );
    if ( foundCollection )
    {
        Nocturnal::Path filePath( path );

        AssetCollectionPtr loadCollection = AssetCollection::LoadFrom( filePath );
        if ( loadCollection )
        {
            if ( foundCollection->AddAssets( loadCollection->GetAssetPaths() ) )
            {
                Dirty( foundCollection );

                RaiseChanged( GetClass()->FindField( &CollectionManager::m_AssetCollections ) );

                return true;
            }
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
bool CollectionManager::SaveCollection( AssetCollection* collection, const tstring& path )
{
    NOC_ASSERT( collection );

    bool result = false;

    tstring outputFile = path.empty() ? collection->GetPath().Get() : path;

    if ( !collection->ReadOnly()
        && !collection->IsTemporary()
        && !outputFile.empty() )
    {
        // Only save dirty files
        std::set< u64 >::iterator findCollection = m_DirtyCollectionHashes.find( collection->GetPath().Hash() );
        if ( findCollection != m_DirtyCollectionHashes.end() )
        {
            result = AssetCollection::SaveTo( collection, outputFile );
            if ( path.empty() ) // only unset dirty if they didn't pass in a different path to save to
            {
                m_DirtyCollectionHashes.erase( collection->GetPath().Hash() );
            }
        }

        m_CollectionFilePaths.insert( collection->GetPath().Get() );
    }
    else
    {
        // TODO debug warning
        result = false;
    }

    return result;
}


///////////////////////////////////////////////////////////////////////////////
void CollectionManager::SaveAllCollections()
{
    m_CollectionFilePaths.clear();
    for ( M_AssetCollections::const_iterator itr = m_AssetCollections.begin(), end = m_AssetCollections.end(); itr != end; ++itr )
    {    
        AssetCollection* collection = itr->second;
        SaveCollection( collection );
    }

    m_DirtyCollectionHashes.clear();
}

///////////////////////////////////////////////////////////////////////////////
void CollectionManager::Dirty( const AssetCollection* collection )
{
    if ( !collection )
        return;

    m_DirtyCollectionHashes.insert( collection->GetPath().Hash() );
}

///////////////////////////////////////////////////////////////////////////////
Nocturnal::Insert<M_AssetCollections>::Result CollectionManager::AddCollectionToMap( AssetCollection* collection )
{
    Nocturnal::Insert<M_AssetCollections>::Result inserted = m_AssetCollections.insert(
        M_AssetCollections::value_type( collection->GetPath().Hash(), collection ) );

    NOC_ASSERT( inserted.second );

    if ( inserted.second )
    {
        collection->InitializeCollection();
        collection->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &CollectionManager::OnCollectionModified ) );

        Dirty( collection );
        m_CollectionAdded.Raise( collection );
        RaiseChanged( GetClass()->FindField( &CollectionManager::m_AssetCollections ) );
    }

    return inserted;
}

///////////////////////////////////////////////////////////////////////////////
bool CollectionManager::RemoveCollectionFromMap( AssetCollection* collection )
{
    if ( !collection )
        return false;

    const tstring collectionName = collection->GetName();

    m_CollectionRemoving.Raise( collection );

    collection->CleanupCollection();
    collection->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &CollectionManager::OnCollectionModified ) );

    if ( m_AssetCollections.erase( collection->GetPath().Hash() ) > 0 )
    {
        m_DirtyCollectionHashes.erase( collection->GetPath().Hash() );
        RaiseChanged( GetClass()->FindField( &CollectionManager::m_AssetCollections ) );
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
void CollectionManager::ClearCollectionMap()
{
    if ( !m_AssetCollections.empty() )
    {
        m_ClearAll.Raise( NULL );

        for ( M_AssetCollections::const_iterator itr = m_AssetCollections.begin(), end = m_AssetCollections.end(); itr != end; ++itr )
        {    
            AssetCollection* collection = itr->second;
            collection->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &CollectionManager::OnCollectionModified ) );
        }

        m_AssetCollections.clear();
        m_DirtyCollectionHashes.clear();
        RaiseChanged( GetClass()->FindField( &CollectionManager::m_AssetCollections ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
void CollectionManager::OnAssetCollectionsChanged( const Reflect::ElementChangeArgs& args )
{
    SaveAllCollections();
}

///////////////////////////////////////////////////////////////////////////////
void CollectionManager::OnCollectionModified( const Reflect::ElementChangeArgs& args )
{
    const AssetCollection* collection = Reflect::ConstObjectCast<AssetCollection>( args.m_Element );

    if ( collection )
    {
        Dirty( collection );
    }
}