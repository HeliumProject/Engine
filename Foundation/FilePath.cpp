#include "FoundationPch.h"
#include "FilePath.h"

#include "Platform/Exception.h"
#include "Platform/File.h"
#include "Platform/Status.h"
#include "Platform/Types.h"

#include "Foundation/String.h"
#include "Foundation/Crc32.h"
#include "Foundation/MD5.h"
	
#include <algorithm>
#include <sstream>

using namespace Helium;

void Path::Init( const tchar_t* path )
{
	m_Path = path;

	std::replace( m_Path.begin(), m_Path.end(), Helium::PathSeparator, s_InternalPathSeparator );
}

Path::Path( const tchar_t* path )
{
	Init( path );
}

Path::Path( const tstring& path )
{
	Init( path.c_str() );
}

Path::Path( const Path& path )
{
	Init( path.m_Path.c_str() );
}

const tchar_t* Path::operator*() const
{
	static const tchar_t emptyString[] = { TXT( '\0' ) };

	const tchar_t* pString = m_Path.c_str();

	return ( pString ? pString : emptyString );
}

Path& Path::operator=( const Path& rhs )
{
	Init( rhs.m_Path.c_str() );
	return *this;
}

bool Path::operator==( const Path& rhs ) const
{
	return CaseInsensitiveCompareString( m_Path.c_str(), rhs.m_Path.c_str() ) == 0;
}

bool Path::operator<( const Path& rhs ) const
{
	return CaseInsensitiveCompareString( m_Path.c_str(), rhs.m_Path.c_str() ) < 0;
}

Helium::Path Path::operator+( const tchar_t* rhs ) const
{
	return Helium::Path( Get() + rhs );
}

Helium::Path Path::operator+( const tstring& rhs ) const
{
	return Helium::Path( Get() + rhs );
}

Helium::Path Path::operator+( const Helium::Path& rhs ) const
{
	// you shouldn't use this on an absolute path
	HELIUM_ASSERT( !rhs.IsAbsolute() );
	return rhs.GetAbsolutePath( *this );
}

Helium::Path& Path::operator+=( const tchar_t* rhs )
{
	Set( Get() + rhs );
	return *this;
}

Helium::Path& Path::operator+=( const tstring& rhs )
{
	Set( Get() + rhs );
	return *this;
}

Helium::Path& Path::operator+=( const Helium::Path& rhs )
{
	// you shouldn't use this on an absolute path
	HELIUM_ASSERT( !rhs.IsAbsolute() );
	*this = rhs.GetAbsolutePath( *this );
	return *this;
}

void Path::Normalize( tstring& path )
{
	toLower( path );
	std::replace( path.begin(), path.end(), Helium::PathSeparator, s_InternalPathSeparator );
}

void Path::MakeNative( tstring& path )
{
	std::replace( path.begin(), path.end(), s_InternalPathSeparator, Helium::PathSeparator );
}

void Path::GuaranteeSeparator( tstring& path )
{
	if ( !path.empty() && *path.rbegin() != s_InternalPathSeparator )
	{
		path += s_InternalPathSeparator;
	}
}

bool Path::Exists( const tstring& path )
{
	Status stat;
	return stat.Read( path.c_str() );
}

bool Path::IsAbsolute( const tchar_t* path )
{
	return Helium::IsAbsolute( path );
}

bool Path::IsUnder( const tstring& location, const tstring& path )
{
	return CaseInsensitiveCompareString( location.c_str(), path.c_str(), location.length() ) == 0;
}

bool Path::IsFile() const
{
	if ( *(m_Path.rbegin()) == s_InternalPathSeparator )
	{
		return false;
	}

	Status stat;
	if ( !stat.Read( m_Path.c_str() ) )
	{
		return false;
	}

	return !( stat.m_Mode & Helium::StatusModes::Directory );
}

bool Path::IsDirectory() const
{
	if ( *(m_Path.rbegin()) == s_InternalPathSeparator )
	{
		return true;
	}

	Status stat;
	if ( !stat.Read( m_Path.c_str() ) )
	{
		return false;
	}

	return ( stat.m_Mode & Helium::StatusModes::Directory );
}

bool Path::Writable() const
{
	Status stat;
	if ( stat.Read( m_Path.c_str() ) )
	{
		return true;
	}

	return ( stat.m_Mode & Helium::StatusModes::Write ) == Helium::StatusModes::Write;
}

bool Path::Readable() const
{
	Status stat;
	if ( stat.Read( m_Path.c_str() ) )
	{
		return false;
	}

	return ( stat.m_Mode & Helium::StatusModes::Read ) == Helium::StatusModes::Read;
}

bool Path::MakePath() const
{
#pragma TODO( "FIXME: This seems excessive, but Helium::MakePath expects native separators" )
	tstring dir = Directory();
	Path::MakeNative( dir );
	return Helium::MakePath( dir.c_str() );
}

bool Path::Create() const
{
	if ( !MakePath() )
	{
		return false;
	}

	File f;
	if ( !f.Open( m_Path.c_str(), FileModes::MODE_WRITE, true ) )
	{
		return false;
	}
	f.Close();
	
	return true;
}

bool Path::Copy( const Helium::Path& target, bool overwrite ) const
{
	return Helium::Copy( m_Path.c_str(), target.m_Path.c_str(), overwrite );
}

bool Path::Move( const Helium::Path& target ) const 
{
	return Helium::Move( m_Path.c_str(), target.m_Path.c_str() );
}

bool Path::Delete() const
{
	return Helium::Delete( m_Path.c_str() );
}

const tstring& Path::Get() const
{
	return m_Path;
}

const tstring& Path::Set( const tstring& path )
{
	Init( path.c_str() );
	return m_Path;
}

void Path::Clear()
{
	Set( TXT( "" ) );
}

void Path::TrimToExisting()
{
	if ( !Exists() )
	{
		Set( Directory() );
	}

	while ( !m_Path.empty() && !Exists() )
	{
		std::vector< tstring > directories = DirectoryAsVector();
		tstring newDir;
		for( std::vector< tstring >::const_iterator itr = directories.begin(), end = directories.end(); itr != end && itr != end - 1; ++itr )
		{
			newDir += *itr + s_InternalPathSeparator;
		}

		Set( newDir );
	}
}

void Path::Split( tstring& directory, tstring& filename ) const
{
	directory = Directory();
	filename = Filename();
}

void Path::Split( tstring& directory, tstring& filename, tstring& extension ) const
{
	Split( directory, filename );
	extension = Extension();
}

tstring Path::Basename() const
{
	tstring basename = Filename();
	size_t pos = basename.rfind( TXT( '.' ) );

	if ( pos != tstring::npos )
	{
		return basename.substr( 0, pos );
	}

	return basename;
}

tstring Path::Filename() const
{
	size_t pos = m_Path.rfind( s_InternalPathSeparator );
	if ( pos != tstring::npos )
	{
		return m_Path.substr( pos + 1 );
	}

	return m_Path;
}

tstring Path::Directory() const
{
	size_t pos = m_Path.rfind( s_InternalPathSeparator );
	if ( pos != tstring::npos )
	{
		return m_Path.substr( 0, pos + 1 );
	}

	return TXT( "" );
}

std::vector< tstring > Path::DirectoryAsVector() const
{
	tistringstream iss( Directory() );
	std::vector< tstring > out;
	do
	{ 
		tstring tmp;
		std::getline( iss, tmp, s_InternalPathSeparator );
		if ( !iss )
		{
			break;
		}
		out.push_back( tmp ); 
	} while( iss );

	return out;
}

tstring Path::Extension() const
{
	tstring filename = Filename();
	size_t pos = filename.rfind( TXT( '.' ) );
	if ( pos != tstring::npos )
	{
		return filename.substr( pos + 1 );
	}

	return TXT( "" );
}

tstring Path::FullExtension() const
{
	tstring filename = Filename();
	size_t pos = filename.find_first_of( TXT( '.' ) );
	if ( pos != tstring::npos )
	{
		return filename.substr( pos + 1 );
	}

	return TXT( "" );
}

void Path::RemoveExtension()
{
	size_t slash = m_Path.find_last_of( s_InternalPathSeparator );
	size_t pos = m_Path.find_last_of( TXT( '.' ), slash == tstring::npos ? 0 : slash );
	if ( pos != tstring::npos )
	{
		m_Path.erase( pos );
	}
}

void Path::RemoveFullExtension()
{
	size_t slash = m_Path.find_last_of( s_InternalPathSeparator );
	size_t pos = m_Path.find_first_of( TXT( '.' ), slash == tstring::npos ? 0 : slash );
	if ( pos != tstring::npos )
	{
		m_Path.erase( pos );
	}
}

void Path::ReplaceExtension( const tstring& newExtension )
{
	size_t slash = m_Path.find_last_of( s_InternalPathSeparator );
	size_t offset = m_Path.rfind( TXT( '.' ) );
	if ( offset != tstring::npos && ( offset > ( slash != tstring::npos ? slash : 0 ) ) )
	{
		m_Path.replace( offset + 1, newExtension.length(), newExtension );
	}
	else
	{
		m_Path += TXT( '.' ) + newExtension;
	}
}

void Path::ReplaceFullExtension( const tstring& newExtension )
{
	size_t slash = m_Path.find_last_of( s_InternalPathSeparator );
	size_t offset = m_Path.find_first_of( TXT( '.' ), slash == tstring::npos ? 0 : slash );
	if ( offset != tstring::npos )
	{
		m_Path.replace( offset + 1, newExtension.length(), newExtension );
	}
	else
	{
		m_Path += TXT( '.' ) + newExtension;
	}
}

bool Path::HasExtension( const tchar_t* extension ) const
{
	size_t len = StringLength( extension );

	if ( m_Path.length() - len < 0 )
	{
		return false;
	}

	return CaseInsensitiveCompareString( m_Path.c_str() + ( m_Path.length() - len ), extension ) == 0;
}

tstring Path::Native() const
{
	tstring native = m_Path;
	Path::MakeNative( native );    
	return native;
}

tstring Path::Absolute() const
{
	tstring full;
	Helium::GetFullPath( m_Path.c_str(), full );
	return full;
}

tstring Path::Normalized() const
{
	tstring normalized = m_Path;
	Path::Normalize( normalized );
	return normalized;
}

tstring Path::Signature()
{
	tstring temp = m_Path;
	Normalize( temp );
	return Helium::MD5( temp );
}

Helium::Path Path::GetAbsolutePath( const Helium::Path& basisPath ) const
{
	HELIUM_ASSERT( !IsAbsolute() ); // shouldn't call this on an already-absolute path

	tstring newPathtstring;
	Helium::GetFullPath( tstring( basisPath.Directory() + m_Path ).c_str(), newPathtstring );
	return Helium::Path( newPathtstring );
}

Helium::Path Path::GetRelativePath( const Helium::Path& basisPath ) const
{
	std::vector< tstring > targetDirectories = this->DirectoryAsVector();
	std::vector< tstring > baseDirectories = basisPath.DirectoryAsVector();

	size_t i = 0;
	while( targetDirectories.size() > i && baseDirectories.size() > i && ( targetDirectories[ i ] == baseDirectories[ i ] ) )
	{
		++i;
	}

	if ( i == 0 )
	{
		return *this;
	}

	tstring newPathtstring;
	for ( size_t j = 0; j < ( baseDirectories.size() - i ); ++j )
	{
		newPathtstring += tstring( TXT( ".." ) ) + s_InternalPathSeparator;
	}

	for ( size_t j = i; j < targetDirectories.size(); ++j )
	{
		newPathtstring += targetDirectories[ j ] + s_InternalPathSeparator;
	}

	newPathtstring += Filename();
	return Helium::Path( newPathtstring );
}

bool Path::Exists() const
{
	return Path::Exists( m_Path );
}

bool Path::IsAbsolute() const
{
	return Path::IsAbsolute( m_Path.c_str() );
}

bool Path::IsUnder( const tstring& location ) const
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

const tchar_t* Path::c_str() const
{
	return m_Path.c_str();
}

tstring Path::FileMD5() const
{
	return Helium::FileMD5( m_Path.c_str() );
}

bool Path::VerifyFileMD5( const tstring& hash ) const
{
	return FileMD5().compare( hash ) == 0;
}
