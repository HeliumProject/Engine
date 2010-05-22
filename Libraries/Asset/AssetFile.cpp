#include "AssetFile.h"

#include "AssetClass.h"
#include "ArtFileAttribute.h"
#include "ColorMapAttribute.h"

#include "CacheDB.h"

#include "Attribute/AttributeHandle.h"
#include "Common/String/Utilities.h"
#include "File/File.h"
#include "File/Resolver.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ExtensionSpecs.h"
#include "Finder/FinderSpec.h"
#include "Finder/ProjectSpecs.h"
#include "RCS/RCS.h"

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

AssetFile::AssetFile( File::Reference& file )
{
    Init();

    SetFileReference( file );
}

AssetFile::~AssetFile()
{
}

/////////////////////////////////////////////////////////////////////////////
AssetFilePtr AssetFile::FindAssetFile( const std::string& path, bool useCacheDB )
{
    NOC_ASSERT( !path.empty() );

    File::Reference file( path );
    file.Resolve();

    AssetFilePtr assetFile = new AssetFile( file );

    if ( useCacheDB )
    {
        // select the file from the cache
        Asset::GlobalCacheDB()->SelectAssetByHash( file.GetHash(), assetFile );
    }

    return assetFile;
}

/////////////////////////////////////////////////////////////////////////////
void AssetFile::Init()
{
    m_ShortName               = "";
    m_ModifierSpec            = NULL;
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
        FileSystem::GetLeaf( GetFilePath(), m_ShortName );
        FileSystem::StripExtension( m_ShortName );
    }
    return m_ShortName;
}

/////////////////////////////////////////////////////////////////////////////
const Finder::ModifierSpec* AssetFile::GetModifierSpec()
{
    if ( !m_ModifierSpec )
    {
        try
        {
            m_ModifierSpec = Finder::GetFileExtensionSpec( GetFilePath() );
        }
        catch( Finder::Exception & )
        {
            // try again below
        }

        if (!m_ModifierSpec )
        {
            std::string extension = FileSystem::GetExtension( GetFilePath(), 1 );
            try
            {
                m_ModifierSpec = Finder::GetFileExtensionSpec( extension );
            }
            catch( Finder::Exception & )
            {
            }
        }
    }
    return m_ModifierSpec;
}

/////////////////////////////////////////////////////////////////////////////
// Gets and caches the file extension
const std::string& AssetFile::GetExtension()
{
    if ( m_Extension.empty() )
    {
        FileSystem::GetExtension( GetFilePath(), m_Extension );
    }
    return m_Extension;
}

/////////////////////////////////////////////////////////////////////////////
// Gets and caches the file type
const std::string& AssetFile::GetFileType()
{
    if ( m_FileType.empty() )
    {
        if ( GetModifierSpec() )
        {
            m_FileType = GetModifierSpec()->GetUIName();
        }
        else
        {
            m_FileType = FileSystem::GetExtension( GetFilePath(), 1 );

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
    }
    return m_FileType;
}

/////////////////////////////////////////////////////////////////////////////
u64 AssetFile::GetSize()
{
    if ( m_Size == 0 )
    {
        struct _stati64 fileStats;
        if ( FileSystem::GetStats64( GetFilePath(), fileStats ) )
        {
            m_Size = fileStats.st_size;
        }
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
    return Asset::AssetClass::LoadAssetClass( *(assetFile->GetFileReference()) );
}

/////////////////////////////////////////////////////////////////////////////
void AssetFile::AddAttribute( const std::string& attrName, const std::string& attrValue, bool canAppend )
{
    Nocturnal::Insert<M_string>::Result inserted = m_Attributes.insert( M_string::value_type( attrName, attrValue ) );
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
void AssetFile::AddDependency( const File::ReferencePtr& fileRef )
{
    m_Dependencies.insert( fileRef );
}

/////////////////////////////////////////////////////////////////////////////
void AssetFile::SetDependencies( const File::S_Reference& dependencies )
{
    m_Dependencies.clear();
    m_Dependencies = dependencies;
}

/////////////////////////////////////////////////////////////////////////////
void AssetFile::GetDependenciesOfType( M_AssetFiles* assetFiles, i32 type, File::S_Reference& dependencies )
{
    File::S_Reference visited;
    return GetDependenciesOfType( assetFiles, type, dependencies, visited, 0 );
}

void AssetFile::GetDependenciesOfType( M_AssetFiles* assetFiles, i32 type, File::S_Reference& dependencies, File::S_Reference& visited, u32 depth )
{
    ++depth;

    if ( this->HasDependencies() )
    {
        const File::S_Reference& assetDependencies = GetDependencies();
        File::S_Reference::const_iterator itr = assetDependencies.begin();
        File::S_Reference::const_iterator end = assetDependencies.end();
        for ( ; itr != end; ++itr )
        {
            File::ReferencePtr fileRef = (*itr);
            fileRef->Resolve();

            if ( FileSystem::HasExtension( fileRef->GetPath(), FinderSpecs::Extension::REFLECT_BINARY.GetExtension() ) )
            {
                if ( visited.find( fileRef ) == visited.end() )
                {
                    visited.insert( fileRef );

                    M_AssetFiles::iterator foundFile = assetFiles->find( fileRef->GetHash() );
                    if ( foundFile != assetFiles->end() )
                    {
                        AssetFile* assetDependency = foundFile->second;
                        AssetClassPtr assetClass = AssetFile::GetAssetClass( assetDependency );
                        if ( assetClass && assetClass->HasType( type ) )
                        {
                            dependencies.insert( fileRef );
                        }

                        File::S_Reference deps;
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


