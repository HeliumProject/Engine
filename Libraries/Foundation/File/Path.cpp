#include "Path.h"

#include "Platform/Types.h"
#include "Foundation/Exception.h"
#include "Platform/Windows/Windows.h"

#include "Foundation/Checksum/MD5.h"
#include "Foundation/Checksum/MurmurHash2.h"

#include <algorithm>
#include <sys/stat.h>

using namespace Nocturnal;

void Path::Init( const char* path )
{
    m_Path = path;

    for( int platform = 1; platform < Platforms::Count; ++platform )
    {
        std::replace( m_Path.begin(), m_Path.end(), s_PlatformSeparators[ platform ], s_PlatformSeparators[ Platforms::Internal ] );
    }
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
    for( int platform = 1; platform < Platforms::Count; ++platform )
    {
        std::replace( path.begin(), path.end(), s_PlatformSeparators[ platform ], s_PlatformSeparators[ Platforms::Internal ] );
    }
}

void Path::MakeNative( std::string& path )
{
#pragma TODO( "Make 'native' not just mean 'windows'." )
    Platform platform = Platforms::Windows;
    std::replace( path.begin(), path.end(), s_PlatformSeparators[ Platforms::Internal ], s_PlatformSeparators[ platform ] );
}

bool Path::Exists( const std::string& path )
{
    struct stat statInfo;
    return ( stat( path.c_str(), &statInfo ) == 0 );
}

bool Path::IsAbsolute( const std::string& path )
{
#ifdef _WINDOWS_
    if ( path.length() > 1 )
    {
        if ( path[ 1 ] == ':' )
            return true;

        if ( path[ 0 ] == '\\' && path[ 1 ] == '\\' )
            return true;
    }
#endif

    return false;
}

bool Path::IsUnder( const std::string& location, const std::string& path )
{
    return ( path.find( location ) == 0 );
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
    std::string path = Native();
    char* fullPath = new char[ MAX_PATH ];
    u32 result = ::GetFullPathName( path.c_str(), MAX_PATH, fullPath, NULL );

    if ( result > MAX_PATH )
    {
        delete fullPath;
        fullPath = new char[ result ];
        result = ::GetFullPathName( path.c_str(), result, fullPath, NULL );
    }

    if ( result == 0 )
    {
        delete fullPath;
        throw Nocturnal::Exception( "Could not get absolute path for path '%s'.", path.c_str() );
    }

    path = fullPath;
    delete fullPath;
    return path;
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
