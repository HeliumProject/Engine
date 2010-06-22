#include "FileInfo.h"

#include "Pipeline/API.h"
#include "Pipeline/Dependencies/Dependencies.h"
#include "Pipeline/Dependencies/DependenciesExceptions.h"

#include "Foundation/Flags.h"

#include "Foundation/Log.h"

#include "Application/SQL/SQL.h"

#include <cryptlib.h>
#include <filters.h>

using namespace Dependencies;

/////////////////////////////////////////////////////////////////////////////
FileInfo::FileInfo() : DependencyInfo()
{
}


/////////////////////////////////////////////////////////////////////////////
FileInfo::FileInfo( DependencyGraph* owner, const std::string& path, const std::string& typeName, const GraphConfigs graphConfigs ) 
: DependencyInfo( owner, path, typeName, graphConfigs )
{
}


/////////////////////////////////////////////////////////////////////////////
void FileInfo::SetInfo()
{
    m_VersionRowID = SQL::InvalidRowID;

    {
        m_LastModified = m_Path.ModifiedTime();
        m_Size         = m_Path.Size();
    }
}




/////////////////////////////////////////////////////////////////////////////
// Determines if the file has been modified on disk since its info was cached.
//
// Returns TRUE if:
//  - file does not exist on disc
//  - the last modified time and size have changed
//  - the cached md5 is not the same as the md5 of the file on disk
//
bool WasFileModifiedOnDisk( const FileInfo& file )
{
    // file does not exist on disk
    if ( !file.m_Path.Exists() )
    {
        //Log::Bullet wasFileModifiedBullet( Log::Streams::Debug, Log::Levels::Verbose, "File was modified: file does not exist on disk\n" );
        return true;
    }

    // file size has changed
    if ( file.m_Size != file.m_Path.Size() )
    {
        //Log::Bullet wasFileModifiedBullet( Log::Streams::Debug, Log::Levels::Verbose, "File was modified: file size has changed\n" );
        return true;
    }

    // Return false, and early out if the last modified time has changed
    if ( file.m_LastModified == file.m_Path.ModifiedTime() )
    {
        return false;
    }

    // Only compare the MD5 and signature if:
    //  - the file size has NOT changed
    //  - the last modified time HAS changed
    //  - and m_MD5 cache is not empty
    // (otherwise it's very unlikely that the MD5 is the same)
    if ( !file.m_MD5.empty() )
    {
        std::string curFileMD5 = file.m_Path.FileMD5();

        //the cached md5 is not the same as the md5 of the file on disk
        if ( file.m_MD5 == curFileMD5 )
        {
            return false;
        }
        else
        {
            //Log::Bullet wasFileModifiedBullet( Log::Streams::Debug, Log::Levels::Verbose, "File was modified: the cached md5 is not the same as the md5 of the file on disk\n" );
            return true;
        }
    }

    //Log::Bullet wasFileModifiedBullet( Log::Streams::Debug, Log::Levels::Verbose, "File was modified: default fall-through; assume true, there was no cached MD5 on file\n" );
    return true;
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine if the file has changed on disk since the MD5 was generated.
//
bool FileInfo::IsMD5Valid()
{
    if ( m_MD5.empty() )
    {
        m_IsMD5Valid = false;
    }
    else if ( !m_IsMD5Valid )
    {
        m_IsMD5Valid = !WasFileModifiedOnDisk( *this );
    }

    return m_IsMD5Valid;
}

/////////////////////////////////////////////////////////////////////////////
// Used force generate a new MD5
//
void FileInfo::GenerateMD5()
{
    m_MD5 = m_Path.FileMD5();
    m_IsMD5Valid = true;
}

/////////////////////////////////////////////////////////////////////////////
// Determines if the file was modified since it was last cached.
//
// Returns TRUE if:
//  - the FileSpec no longer exits
//  - the file does not currently exist in the GraphDB (invalid RowID)
//  - expected FormatVersion has changed (builder updated)
//  - the file has changed on disk
//    Determines if the file has been modified on disk since its info was cached.
//    TRUE if:
//    * file does not exist on disc
//    * the last modified time and size have changed
//    * the cached md5 is not the same as the md5 of the file on disk
//
bool FileInfo::WasModified()
{
    // the file does not currently exist in the GraphDB (invalid RowID)
    if ( m_VersionRowID == SQL::InvalidRowID || m_RowID == SQL::InvalidRowID )
    {
        //Log::Bullet wasFileModifiedBullet( Log::Streams::Debug, Log::Levels::Verbose, "File was modified: the file does not currently exist in the GraphDB (invalid RowID)\n" );
        return true;
    }
    // expected FormatVersion has changed (builder updated)
    else if ( m_TypeName.empty() || ( m_OwnerGraph && ( m_FormatVersion != m_OwnerGraph->GetFormatVersion( m_TypeName ) ) ) )
    {
        //Log::Bullet wasFileModifiedBullet( Log::Streams::Debug, Log::Levels::Verbose, "File was modified: expected FormatVersion has changed (builder updated)\n" );
        return true;
    }
    // the file was modified on disk
    else if ( WasFileModifiedOnDisk( *this ) )
    {
        return true;
    }

    return false;
}


/////////////////////////////////////////////////////////////////////////////
FileInfo::~FileInfo()
{

}



/////////////////////////////////////////////////////////////////////////////
bool FileInfo::AppendToSignature( CryptoPP::HashFilter* hashFilter, std::vector< std::string >& trace)
{

    bool dirtyFile = false;
    if ( !m_Path.Exists() )
    {
        Log::Bullet cacheGetGraph( Log::Streams::Debug, Log::Levels::Verbose, "CreateSignature is skipping optional input dependency (%s)\n", m_Path.c_str() );
        return dirtyFile;
    }

    std::string md5;

    if ( !m_AlternateSignatureGenerationPath.empty() )
    {
        Nocturnal::Path path( m_AlternateSignatureGenerationPath );
        md5 = path.FileMD5();
    }
    else
    {
        if ( !IsMD5Valid() )
        {
            GenerateMD5();
            dirtyFile = true;
        }

        md5 = m_MD5;
    }

    {
        hashFilter->Put( ( byte const* ) md5.data(), md5.size() );
    }

    trace.push_back( md5 );
    trace.back() += " (" + ( !m_AlternateSignatureGenerationPath.empty() ? m_AlternateSignatureGenerationPath : m_Path ) + ")";

    return dirtyFile;
}

void FileInfo::CacheCopy( DependencyInfo& rhs )
{
    __super::CacheCopy( rhs );

    FileInfo* fileInfo = dynamic_cast< FileInfo* >(&rhs);

    if( fileInfo )
        m_AlternateSignatureGenerationPath = m_AlternateSignatureGenerationPath;
}

/////////////////////////////////////////////////////////////////////////////
// Copy from one file to this
void FileInfo::CopyFrom( const DependencyInfo &rhs )
{
    if ( this != &rhs )
    {

        __super::CopyFrom( rhs );
        const FileInfo* fileInfo = dynamic_cast< const FileInfo* >(&rhs);
        if( fileInfo )
        {
            m_AlternateSignatureGenerationPath = fileInfo->m_AlternateSignatureGenerationPath;
        }

    }

}

/////////////////////////////////////////////////////////////////////////////
// Determines if two dependencies are equal
bool FileInfo::IsEqual( const DependencyInfo &rhs ) const
{
    bool eq = true;

    eq = eq && __super::IsEqual( rhs );

    const FileInfo* fileInfo = dynamic_cast< const FileInfo* >(&rhs);
    if( fileInfo )
    {
        eq = eq && ( m_AlternateSignatureGenerationPath== fileInfo->m_AlternateSignatureGenerationPath );
    }    

    return eq;
}

bool FileInfo::Exists()
{
    return m_Path.Exists();
}
