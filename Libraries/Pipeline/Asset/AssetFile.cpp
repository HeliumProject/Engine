#include "AssetFile.h"

#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/Asset/Components/ArtFileComponent.h"
#include "Pipeline/Asset/Components/ColorMapComponent.h"

#include "Pipeline/Component/ComponentHandle.h"
#include "Foundation/File/Path.h"
#include "Foundation/String/Utilities.h"
#include "Application/RCS/RCS.h"

using namespace Asset;

REFLECT_DEFINE_CLASS( AssetFile );
void AssetFile::EnumerateClass( Reflect::Compositor<AssetFile>& comp )
{
}

///////////////////////////////////////////////////////////////////////////////
/// class AssetFile
///////////////////////////////////////////////////////////////////////////////

AssetFile::AssetFile()
{
    Init();
}

AssetFile::AssetFile( Nocturnal::Path& path )
{
    Init();

    SetPath( path );
}

AssetFile::~AssetFile()
{
}

/////////////////////////////////////////////////////////////////////////////
AssetFilePtr AssetFile::FindAssetFile( const std::string& filePath, CacheDB* cache )
{
    NOC_ASSERT( !filePath.empty() );

    Nocturnal::Path path( filePath );

    AssetFilePtr assetFile = NULL;

    if ( cache )
    {
        // select the file from the cache
        cache->SelectAssetByHash( path.Hash(), assetFile );
    }

    if ( !assetFile )
    {
        assetFile = new AssetFile( path );
    }

    return assetFile;
}

/////////////////////////////////////////////////////////////////////////////
void AssetFile::Init()
{
    m_ShortName               = "";
    m_FileFilter              = "";
    m_Extension               = "";
    m_FileType                = "";
    m_AssetType               = Asset::AssetTypes::Null;
    m_Size                    = 0;
    m_Attributes.clear();
    m_Dependencies.clear();
    m_RowID                   = 0;
}

/////////////////////////////////////////////////////////////////////////////
const std::string& AssetFile::GetShortName()
{
    if ( m_ShortName.empty() )
    {
        Nocturnal::Path path( m_Path );
        path.RemoveExtension();
        m_ShortName = path.Filename();
    }
    return m_ShortName;
}

/////////////////////////////////////////////////////////////////////////////
const std::string& AssetFile::GetFileFilter()
{
    if ( m_FileFilter.empty() )
    {
        m_FileFilter = std::string( "*." ) + Nocturnal::Path( GetFilePath() ).Extension();
    }
    return m_FileFilter;
}

/////////////////////////////////////////////////////////////////////////////
// Gets and caches the file extension
const std::string& AssetFile::GetExtension()
{
    if ( m_Extension.empty() )
    {
        m_Extension = Nocturnal::Path( GetFilePath() ).Extension();
    }
    return m_Extension;
}

/////////////////////////////////////////////////////////////////////////////
// Gets and caches the file type
const std::string& AssetFile::GetFileType()
{
    if ( m_FileType.empty() )
    {
        m_FileType = Nocturnal::Path( GetFilePath() ).Extension();

        if ( !m_FileType.empty() && *m_FileType.begin() == '.' )
        {
            m_FileType.erase( 0, 1 );
        }

        if ( m_FileType.empty() )
        {
            m_FileType = "Unknown";
        }
        else
        {
            toUpper( m_FileType );
        }
    }
    return m_FileType;
}

/////////////////////////////////////////////////////////////////////////////
u64 AssetFile::GetSize()
{
    if ( m_Size == 0 )
    {
        m_Size = Nocturnal::Path( GetFilePath() ).Size();
    }
    return m_Size;
}

/////////////////////////////////////////////////////////////////////////////
AssetType AssetFile::GetAssetType()
{
    return m_AssetType;
}

/////////////////////////////////////////////////////////////////////////////
AssetClassPtr AssetFile::GetAssetClass( AssetFile* assetFile )
{
    NOC_ASSERT( assetFile );
    return Asset::AssetClass::LoadAssetClass( assetFile->GetPath() );
}

/////////////////////////////////////////////////////////////////////////////
void AssetFile::AddAttribute( const std::string& attrName, const std::string& attrValue, bool canAppend )
{
    Nocturnal::Insert<std::map< std::string, std::string >>::Result inserted = m_Attributes.insert( std::map< std::string, std::string >::value_type( attrName, attrValue ) );
    if ( !inserted.second && inserted.first->second != attrValue )
    {
        std::string& attributes = inserted.first->second;
        if ( canAppend && attributes.find( attrValue ) == std::string::npos )
        {
            // append it to the existing one
            attributes += attributes.empty() ? "" : ", ";
            attributes += attrValue;
        }
        else
        {
            // why are we inserting the same attribute value twice?
            NOC_BREAK();
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
void AssetFile::AddDependency( const Nocturnal::Path& path )
{
    m_Dependencies.insert( path );
}

/////////////////////////////////////////////////////////////////////////////
void AssetFile::SetDependencies( const std::set< Nocturnal::Path >& dependencies )
{
    m_Dependencies.clear();
    m_Dependencies = dependencies;
}

/////////////////////////////////////////////////////////////////////////////
void AssetFile::GetDependenciesOfType( M_AssetFiles* assetFiles, i32 type, std::set< Nocturnal::Path >& dependencies )
{
    std::set< Nocturnal::Path > visited;
    return GetDependenciesOfType( assetFiles, type, dependencies, visited, 0 );
}

void AssetFile::GetDependenciesOfType( M_AssetFiles* assetFiles, i32 type, std::set< Nocturnal::Path >& dependencies, std::set< Nocturnal::Path >& visited, u32 depth )
{
    ++depth;

    if ( this->HasDependencies() )
    {
        const std::set< Nocturnal::Path >& assetDependencies = GetDependencies();
        std::set< Nocturnal::Path >::const_iterator itr = assetDependencies.begin();
        std::set< Nocturnal::Path >::const_iterator end = assetDependencies.end();
        for ( ; itr != end; ++itr )
        {
            Nocturnal::Path path = (*itr);

            if ( path.Extension() == Reflect::Archive::GetExtension( Reflect::ArchiveTypes::Binary ) )
            {
                if ( visited.find( path ) == visited.end() )
                {
                    visited.insert( path );

                    M_AssetFiles::iterator foundFile = assetFiles->find( path.Hash() );
                    if ( foundFile != assetFiles->end() )
                    {
                        AssetFile* assetDependency = foundFile->second;
                        AssetClassPtr assetClass = AssetFile::GetAssetClass( assetDependency );
                        if ( assetClass && assetClass->HasType( type ) )
                        {
                            dependencies.insert( path );
                        }

                        std::set< Nocturnal::Path > deps;
                        assetDependency->GetDependenciesOfType( assetFiles, type, deps, visited, depth );
                        dependencies.insert( deps.begin(), deps.end() );
                    }
                }
            }
        }
    }

    if ( --depth == 0 )
    {
        visited.clear();
    }
}


