#include "File.h"

#include "Windows/Windows.h"

#include "../Checksum/MD5.h"
#include "../Checksum/Crc32.h"


#include <sys/stat.h>
#include <sstream>

using namespace Nocturnal;

void SplitDirectories( const std::string& directory, V_string& output )
{
    std::string::size_type start = 0; 
    std::string::size_type end = 0; 
    while ( ( end = directory.find( '/', start ) ) != std::string::npos )
    { 
        output.push_back( directory.substr( start, end - start ) ); 
        start = end + 1;
    }
    output.push_back( directory.substr( start ) ); 
}

void File::Update()
{
    struct _stati64 stats;
    if ( _stati64( m_Path.Native().c_str(), &stats ) == 0 )
    {
        m_LastModTime = stats.st_mtime;
    }
}

bool File::Exists() const
{
    struct stat statInfo;
    return ( stat( m_Path.Native().c_str(), &statInfo ) == 0 );
}

bool File::MakePath()
{
    std::string directory = m_Path.Directory();

    NOC_ASSERT( !directory.empty() );

    V_string directories;
    SplitDirectories( directory, directories );

    struct stat statInfo;
    std::string currentDirectory;
    currentDirectory.reserve( MAX_PATH );
    currentDirectory = directories[ 0 ];
    for( V_string::const_iterator itr = directories.begin() + 1, end = directories.end(); itr != end; ++itr )
    {
        if ( ( (*currentDirectory.rbegin()) != ':' ) && ( stat( currentDirectory.c_str(), &statInfo ) != 0 ) )
        {
            if ( !CreateDirectory( currentDirectory.c_str(), NULL ) )
            {
                return false;
            }
        }

        currentDirectory += std::string( "\\" ) + *itr;
    }

    return true;
}

bool File::Create()
{
    if ( !MakePath() )
    {
        return false;
    }

    FILE *f = fopen( m_Path.c_str(), "w" );

    // if we couldn't open the file for some reason, throw
    if ( !f )
    {
        return false;
    }

    fclose(f);
    return true;
}

bool File::Copy( const std::string& target, bool overwrite )
{
    return ( TRUE == ::CopyFile( m_Path.Native().c_str(), target.c_str(), overwrite ? FALSE : TRUE ) );
}

bool File::Move( const std::string& target )
{
    if ( TRUE == ::MoveFile( m_Path.Native().c_str(), target.c_str() ) )
    {
        m_Path.Set( target.c_str() );
        return true;
    }

    return false;
}

bool File::Delete()
{
    return ( TRUE == ::DeleteFile( m_Path.Native().c_str() ) );
}

bool File::GetAttributes( u32& attributes ) const
{
    attributes = GetFileAttributesA( m_Path.Native().c_str() ); 
    if ( attributes == INVALID_FILE_ATTRIBUTES )
    {
        return false;
    }
    return true;
}

bool File::IsWritable() const
{
    u32 attributes = GetFileAttributesA( m_Path.Native().c_str() );
    return ( ( attributes != INVALID_FILE_ATTRIBUTES ) && !( attributes & FILE_ATTRIBUTE_READONLY ) );
}

bool File::IsFile() const
{
    u32 attributes = GetFileAttributesA( m_Path.Native().c_str() );
    return ( ( attributes != INVALID_FILE_ATTRIBUTES ) && !( attributes & FILE_ATTRIBUTE_DIRECTORY ) );
}

bool File::IsFolder() const 
{
    u32 attributes = GetFileAttributesA( m_Path.Native().c_str() );
    return ( ( attributes != INVALID_FILE_ATTRIBUTES ) && ( attributes & FILE_ATTRIBUTE_DIRECTORY ) );
}

bool File::HasChanged() const
{
    return HasChangedSince( m_LastModTime );
}

bool File::HasChangedSince( u64 lastTime ) const
{
    struct _stati64 stats;
    GetStats64( stats );
    return ( stats.st_mtime > lastTime );
}

bool File::GetStats( struct _stat& stats ) const
{
    return ( _stat( m_Path.Native().c_str(), &stats ) == 0 );
}

bool File::GetStats64( struct _stati64& stats ) const
{
    return ( _stati64( m_Path.Native().c_str(), &stats ) == 0 );
}

u64 File::GetCreatedTime()
{
    struct _stati64 stats;
    if ( GetStats64( stats ) )
    {
        return stats.st_ctime;
    }
    return 0;
}

u64 File::GetModifiedTime()
{
    struct _stati64 stats;
    if ( GetStats64( stats ) )
    {
        return stats.st_mtime;
    }
    return 0;
}

u64 File::Size()
{
    struct _stati64 stats;
    if ( GetStats64( stats ) )
    {
        return stats.st_size;
    }
    return 0;
}

std::string File::CRC()
{
    u32 crc = Nocturnal::FileCrc32( m_Path.c_str() );

    std::stringstream str;
    str << std::hex << std::uppercase << crc;
    return str.str();
}

bool File::VerifyCRC( const std::string &hashString )
{
    return CRC().compare( hashString ) == 0;
}

std::string File::MD5()
{
    return Nocturnal::FileMD5( m_Path.c_str() );
}

bool File::VerifyMD5( const std::string &hashString )
{
    return MD5().compare( hashString ) == 0;
}
