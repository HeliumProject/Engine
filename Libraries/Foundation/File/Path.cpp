#include "Path.h"

#include "Platform/Types.h"
#include "Platform/Stat.h"
#include "Platform/Path.h"

#include "Foundation/Exception.h"
#include "Foundation/Checksum/Crc32.h"
#include "Foundation/Checksum/MD5.h"
#include "Foundation/Checksum/MurmurHash2.h"

#include <algorithm>
#include <sstream>

using namespace Nocturnal;

void Path::Init( const char* path )
{
    m_Path = path;

    std::replace( m_Path.begin(), m_Path.end(), Platform::PathSeparator, s_InternalPathSeparator );
}

Path::Path( const char* path )
{
    Init( path );
}

Path::Path( const std::string& path )
{
    Init( path.c_str() );
}

Path::Path( const Path& path )
{
    Init( path.m_Path.c_str() );
}

Path& Path::operator=( const Path& rhs )
{
    Init( rhs.m_Path.c_str() );
    return *this;
}

bool Path::operator==( const Path& rhs ) const
{
    return m_Path == rhs.m_Path;
}

bool Path::operator<( const Path& rhs ) const
{
    return m_Path < rhs.m_Path;
}

void Path::Normalize( std::string& path )
{
    toLower( path );
    std::replace( path.begin(), path.end(), Platform::PathSeparator, s_InternalPathSeparator );
}

void Path::MakeNative( std::string& path )
{
    std::replace( path.begin(), path.end(), s_InternalPathSeparator, Platform::PathSeparator );
}

bool Path::Exists( const std::string& path )
{
    Path native( path );
    Platform::Stat stat;
    return native.Stat( stat );
}

bool Path::Stat( Platform::Stat& stat ) const
{
    return Platform::StatPath( Native().c_str(), stat );
}

bool Path::IsAbsolute( const std::string& path )
{
    return Platform::IsAbsolute( path.c_str() );
}

bool Path::IsUnder( const std::string& location, const std::string& path )
{
    return ( path.find( location ) == 0 );
}

bool Path::IsFile() const
{
    if ( *(m_Path.rbegin()) == s_InternalPathSeparator )
    {
        return false;
    }

    Platform::Stat stat;
    if ( !Platform::StatPath( Native().c_str(), stat ) )
    {
        return false;
    }

    return ( stat.m_Mode & Platform::ModeFlags::File ) == Platform::ModeFlags::File;
}

bool Path::IsDirectory() const
{
    if ( *(m_Path.rbegin()) == s_InternalPathSeparator )
    {
        return true;
    }

    Platform::Stat stat;
    if ( !Platform::StatPath( Native().c_str(), stat ) )
    {
        return false;
    }

    return ( stat.m_Mode & Platform::ModeFlags::Directory ) == Platform::ModeFlags::Directory;
}

bool Path::Writable() const
{
    Platform::Stat stat;
    if ( !Platform::StatPath( Native().c_str(), stat ) )
    {
        return false;
    }

    return ( stat.m_Mode & Platform::ModeFlags::Write ) == Platform::ModeFlags::Write;
}

bool Path::Readable() const
{
    Platform::Stat stat;
    if ( !Platform::StatPath( Native().c_str(), stat ) )
    {
        return false;
    }

    return ( stat.m_Mode & Platform::ModeFlags::Read ) == Platform::ModeFlags::Read;
}

bool Path::ChangedSince( u64 lastTime ) const
{
    Platform::Stat stat;
    if ( !Platform::StatPath( Native().c_str(), stat ) )
    {
        return false;
    }

    return ( stat.m_ModifiedTime > lastTime );
}

u64 Path::ModifiedTime() const
{
    Platform::Stat stat;
    if ( Stat( stat ) )
    {
        return stat.m_ModifiedTime;
    }

    return 0;
}

u64 Path::CreatedTime() const
{
    Platform::Stat stat;
    if ( Stat( stat ) )
    {
        return stat.m_CreatedTime;
    }

    return 0;
}

u64 Path::AccessTime() const
{
    Platform::Stat stat;
    if ( Stat( stat ) )
    {
        return stat.m_AccessTime;
    }

    return 0;
}


bool Path::MakePath() const
{
    Path dir( Directory() );
    return Platform::MakePath( dir.Native().c_str() );
}

bool Path::Create() const
{
    if ( !MakePath() )
    {
        return false;
    }

    FILE *f = fopen( Native().c_str(), "w" );

    if ( !f )
    {
        return false;
    }

    fclose(f);
    return true;
}

bool Path::Copy( const Nocturnal::Path& target, bool overwrite ) const
{
    return Platform::Copy( Native().c_str(), target.Native().c_str(), overwrite );
}

bool Path::Move( const Nocturnal::Path& target ) const 
{
    return Platform::Move( Native().c_str(), target.Native().c_str() );
}

bool Path::Delete() const
{
    return Platform::Delete( Native().c_str() );
}

const std::string& Path::Get() const
{
    return m_Path;
}

const std::string& Path::Set( const std::string& path )
{
    Init( path.c_str() );
    return m_Path;
}

void Path::Split( std::string& directory, std::string& filename ) const
{
    directory = Directory();
    filename = Filename();
}

void Path::Split( std::string& directory, std::string& filename, std::string& extension ) const
{
    Split( directory, filename );
    extension = Extension();
}

std::string Path::Basename() const
{
    return m_Path.substr( m_Path.rfind( '/' ) + 1, m_Path.rfind( '.' ) + 1 );
}

std::string Path::Filename() const
{
    return m_Path.substr( m_Path.rfind( '/' ) + 1 );
}

std::string Path::Directory() const
{
    return m_Path.substr( 0, m_Path.rfind( '/' ) + 1 );
}

std::string Path::Extension() const
{
    return m_Path.substr( m_Path.rfind( '.' ) + 1 );
}

std::string Path::Native() const
{
    std::string native = m_Path;
    Path::MakeNative( native );    
    return native;
}

std::string Path::Absolute() const
{
    std::string full;
    Platform::GetFullPath( Native().c_str(), full );
    return full;
}

std::string Path::Normalized() const
{
    std::string normalized = m_Path;
    Path::Normalize( normalized );
    return normalized;
}

u64 Path::Hash() const
{
    return Nocturnal::MurmurHash2( m_Path );
}

std::string Path::Signature()
{
    return Nocturnal::MD5( m_Path );
}

void Path::ReplaceExtension( const std::string& newExtension )
{
    m_Path.replace( m_Path.rfind( '.' ) + 1, newExtension.length(), newExtension );
}

bool Path::Exists() const
{
    std::string absolute = Absolute();
    Path::MakeNative( absolute );
    return Path::Exists( absolute );
}

bool Path::IsAbsolute() const
{
    return Path::IsAbsolute( m_Path );
}

bool Path::IsUnder( const std::string& location )
{
    return Path::IsUnder( location, m_Path );
}

size_t Path::length() const
{
    return m_Path.length();
}

bool Path::empty() const
{
    return m_Path.empty();
}

const char* Path::c_str() const
{
    return m_Path.c_str();
}

std::string Path::FileCRC()
{
    u32 crc = Nocturnal::FileCrc32( m_Path.c_str() );

    std::stringstream str;
    str << std::hex << std::uppercase << crc;
    return str.str();
}

bool Path::VerifyFileCRC( const std::string& hash )
{
    return FileCRC().compare( hash ) == 0;
}

std::string Path::FileMD5()
{
    return Nocturnal::FileMD5( m_Path.c_str() );
}

bool Path::VerifyFileMD5( const std::string& hash )
{
    return FileMD5().compare( hash ) == 0;
}
