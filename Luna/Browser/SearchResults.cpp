#include "Precompile.h"

#include "SearchResults.h"

#include "Pipeline/Asset/AssetFile.h"
#include "Pipeline/Asset/AssetFolder.h"
#include "Pipeline/Asset/AssetClass.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Ctor/Dtor
SearchResults::SearchResults( u32 browserSearchID )
: m_BrowserSearchID( browserSearchID )
{
}

SearchResults::SearchResults( const SearchResults* results )
{
    if ( results )
    {
        m_BrowserSearchID = results->m_BrowserSearchID;
        m_AssetFiles = results->m_AssetFiles;
        m_LookupByHash = results->m_LookupByHash;
        m_Folders = results->m_Folders;
    }
}

SearchResults::~SearchResults()
{
    Clear();
}

///////////////////////////////////////////////////////////////////////////////
void SearchResults::Clear()
{
    m_AssetFiles.clear();
    m_LookupByHash.clear();
    m_Folders.clear();
}

///////////////////////////////////////////////////////////////////////////////
bool SearchResults::HasResults() const
{
    return ( !m_AssetFiles.empty() || !m_Folders.empty() );
}

///////////////////////////////////////////////////////////////////////////////
const Asset::V_AssetFiles& SearchResults::GetFiles() const
{
    return m_AssetFiles;
}

///////////////////////////////////////////////////////////////////////////////
Asset::AssetFile* SearchResults::FindFileByHash( const u64& hash ) const
{
    Asset::AssetFile* file = NULL;
    Asset::M_AssetFiles::const_iterator found = m_LookupByHash.find( hash );
    if ( found != m_LookupByHash.end() )
    {
        file = found->second;
    }
    return file;
}

///////////////////////////////////////////////////////////////////////////////
bool SearchResults::AddFile( Asset::AssetFilePtr assetFile )
{
    Asset::AssetFile* foundFile = FindFileByHash( assetFile->GetPath().Hash() );
    if ( foundFile )
    {
        // the file already exists, make sure it's up-to-date
    }
    else
    {
        // insert the file
        m_AssetFiles.push_back( assetFile );
        m_LookupByHash[ assetFile->GetPath().Hash() ] = m_AssetFiles.back();

        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
bool SearchResults::RemoveFile( Asset::AssetFilePtr assetFile )
{
    // save ourselves a little time by looking up the file by ID
    Asset::AssetFile* foundFile = FindFileByHash( assetFile->GetPath().Hash() );
    if ( foundFile )
    {
        Asset::V_AssetFiles::iterator itr = m_AssetFiles.begin();
        Asset::V_AssetFiles::iterator end = m_AssetFiles.end();
        for ( ; itr != end; ++itr )
        {
            if ( (*itr)->GetPath().Hash() == assetFile->GetPath().Hash() )
            {
                break;
            }
        }

        if ( itr != end )
        {
            m_AssetFiles.erase( itr );
            m_LookupByHash.erase( assetFile->GetPath().Hash() );
            return true;
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
bool SearchResults::AddFolder( Asset::AssetFolderPtr folder )
{
    Asset::V_AssetFolders::iterator itr = m_Folders.begin();
    Asset::V_AssetFolders::iterator end = m_Folders.begin();
    for ( ; itr != end; ++itr )
    {
        if ( (*itr)->GetFullPath() == folder->GetFullPath() )
        {
            return false;
        }
    }

    if ( itr == end )
    {
        m_Folders.push_back( folder );
        return true;
    }

    return false;
}

bool SearchResults::RemoveFolder( const tstring& fullPath )
{
    Asset::V_AssetFolders::iterator itr = m_Folders.begin();
    Asset::V_AssetFolders::iterator end = m_Folders.begin();
    for ( ; itr != end; ++itr )
    {
        if ( (*itr)->GetFullPath() == fullPath )
        {
            break;
        }
    }

    if ( itr != end )
    {
        m_Folders.erase( itr );
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
const Asset::V_AssetFolders& SearchResults::GetFolders() const
{
    return m_Folders;
}
