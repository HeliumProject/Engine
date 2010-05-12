#include "Precompile.h"

#include "SearchResults.h"

#include "Asset/AssetFile.h"
#include "Asset/AssetFolder.h"
#include "Asset/AssetClass.h"
#include "FileSystem/FileSystem.h"

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
    m_LookupByID = results->m_LookupByID;
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
  m_LookupByID.clear();
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
Asset::AssetFile* SearchResults::FindFileByID( tuid id ) const
{
  Asset::AssetFile* file = NULL;
  Asset::M_AssetFiles::const_iterator found = m_LookupByID.find( id );
  if ( found != m_LookupByID.end() )
  {
    file = found->second;
  }
  return file;
}

///////////////////////////////////////////////////////////////////////////////
bool SearchResults::AddFile( Asset::AssetFilePtr assetFile )
{
  Asset::AssetFile* foundFile = FindFileByID( assetFile->GetFileID() );
  if ( foundFile )
  {
    // the file already exists, make sure it's up-to-date
  }
  else
  {
    // insert the file
    m_AssetFiles.push_back( assetFile );
    m_LookupByID[assetFile->GetFileID()] = m_AssetFiles.back();

    return true;
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
bool SearchResults::RemoveFile( Asset::AssetFilePtr assetFile )
{
  // save ourselves a little time by looking up the file by ID
  Asset::AssetFile* foundFile = FindFileByID( assetFile->GetFileID() );
  if ( foundFile )
  {
    Asset::V_AssetFiles::iterator itr = m_AssetFiles.begin();
    Asset::V_AssetFiles::iterator end = m_AssetFiles.end();
    for ( ; itr != end; ++itr )
    {
      if ( (*itr)->GetFileID() == assetFile->GetFileID() )
      {
        break;
      }
    }

    if ( itr != end )
    {
      m_AssetFiles.erase( itr );
      m_LookupByID.erase( assetFile->GetFileID() );
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

bool SearchResults::RemoveFolder( const std::string& fullPath )
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
