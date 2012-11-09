#include "FoundationPch.h"
#include "FilePath.h"

#include "Platform/Exception.h"
#include "Platform/File.h"
#include "Platform/Types.h"

#include "Foundation/String.h"
#include "Foundation/Crc32.h"
#include "Foundation/MD5.h"
	
#include <algorithm>
#include <sstream>

using namespace Helium;

void FilePath::Init( const tchar_t* path )
{
	m_Path = path;

	std::replace( m_Path.begin(), m_Path.end(), Helium::PathSeparator, s_InternalPathSeparator );
}

FilePath::FilePath( const tchar_t* path )
{
	Init( path );
}

FilePath::FilePath( const tstring& path )
{
	Init( path.c_str() );
}

FilePath::FilePath( const FilePath& path )
{
	Init( path.m_Path.c_str() );
}

const tchar_t* FilePath::operator*() const
{
	static const tchar_t emptyString[] = { TXT( '\0' ) };

	const tchar_t* pString = m_Path.c_str();

	return ( pString ? pString : emptyString );
}

FilePath& FilePath::operator=( const FilePath& rhs )
{
	Init( rhs.m_Path.c_str() );
	return *this;
}

bool FilePath::operator==( const FilePath& rhs ) const
{
	return CaseInsensitiveCompareString( m_Path.c_str(), rhs.m_Path.c_str() ) == 0;
}

bool FilePath::operator<( const FilePath& rhs ) const
{
	return CaseInsensitiveCompareString( m_Path.c_str(), rhs.m_Path.c_str() ) < 0;
}

Helium::FilePath FilePath::operator+( const tchar_t* rhs ) const
{
	return Helium::FilePath( Get() + rhs );
}

Helium::FilePath FilePath::operator+( const tstring& rhs ) const
{
	return Helium::FilePath( Get() + rhs );
}

Helium::FilePath FilePath::operator+( const Helium::FilePath& rhs ) const
{
	// you shouldn't use this on an absolute path
	HELIUM_ASSERT( !rhs.IsAbsolute() );
	return rhs.GetAbsolutePath( *this );
}

Helium::FilePath& FilePath::operator+=( const tchar_t* rhs )
{
	Set( Get() + rhs );
	return *this;
}

Helium::FilePath& FilePath::operator+=( const tstring& rhs )
{
	Set( Get() + rhs );
	return *this;
}

Helium::FilePath& FilePath::operator+=( const Helium::FilePath& rhs )
{
	// you shouldn't use this on an absolute path
	HELIUM_ASSERT( !rhs.IsAbsolute() );
	*this = rhs.GetAbsolutePath( *this );
	return *this;
}

void FilePath::Normalize( tstring& path )
{
	toLower( path );
	std::replace( path.begin(), path.end(), Helium::PathSeparator, s_InternalPathSeparator );
}

void FilePath::MakeNative( tstring& path )
{
	std::replace( path.begin(), path.end(), s_InternalPathSeparator, Helium::PathSeparator );
}

void FilePath::GuaranteeSeparator( tstring& path )
{
	if ( !path.empty() && *path.rbegin() != s_InternalPathSeparator )
	{
		path += s_InternalPathSeparator;
	}
}

bool FilePath::Exists( const tstring& path )
{
	Status stat;
	return stat.Read( path.c_str() );
}

bool FilePath::IsAbsolute( const tchar_t* path )
{
	return Helium::IsAbsolute( path );
}

bool FilePath::IsUnder( const tstring& location, const tstring& path )
{
	return CaseInsensitiveCompareString( location.c_str(), path.c_str(), location.length() ) == 0;
}

bool FilePath::IsFile() const
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

bool FilePath::IsDirectory() const
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

bool FilePath::Writable() const
{
	Status stat;
	if ( stat.Read( m_Path.c_str() ) )
	{
		return true;
	}

	return ( stat.m_Mode & Helium::StatusModes::Write ) == Helium::StatusModes::Write;
}

bool FilePath::Readable() const
{
	Status stat;
	if ( stat.Read( m_Path.c_str() ) )
	{
		return false;
	}

	return ( stat.m_Mode & Helium::StatusModes::Read ) == Helium::StatusModes::Read;
}

bool FilePath::MakePath() const
{
#pragma TODO( "FIXME: This seems excessive, but Helium::MakePath expects native separators" )
	tstring dir = Directory();
	FilePath::MakeNative( dir );
	return Helium::MakePath( dir.c_str() );
}

bool FilePath::Create() const
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

bool FilePath::Copy( const Helium::FilePath& target, bool overwrite ) const
{
	return Helium::Copy( m_Path.c_str(), target.m_Path.c_str(), overwrite );
}

bool FilePath::Move( const Helium::FilePath& target ) const 
{
	return Helium::Move( m_Path.c_str(), target.m_Path.c_str() );
}

bool FilePath::Delete() const
{
	return Helium::Delete( m_Path.c_str() );
}

const tstring& FilePath::Get() const
{
	return m_Path;
}

const tstring& FilePath::Set( const tstring& path )
{
	Init( path.c_str() );
	return m_Path;
}

void FilePath::Clear()
{
	Set( TXT( "" ) );
}

void FilePath::TrimToExisting()
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

void FilePath::Split( tstring& directory, tstring& filename ) const
{
	directory = Directory();
	filename = Filename();
}

void FilePath::Split( tstring& directory, tstring& filename, tstring& extension ) const
{
	Split( directory, filename );
	extension = Extension();
}

tstring FilePath::Basename() const
{
	tstring basename = Filename();
	size_t pos = basename.rfind( TXT( '.' ) );

	if ( pos != tstring::npos )
	{
		return basename.substr( 0, pos );
	}

	return basename;
}

tstring FilePath::Filename() const
{
	size_t pos = m_Path.rfind( s_InternalPathSeparator );
	if ( pos != tstring::npos )
	{
		return m_Path.substr( pos + 1 );
	}

	return m_Path;
}

tstring FilePath::Directory() const
{
	size_t pos = m_Path.rfind( s_InternalPathSeparator );
	if ( pos != tstring::npos )
	{
		return m_Path.substr( 0, pos + 1 );
	}

	return TXT( "" );
}

std::vector< tstring > FilePath::DirectoryAsVector() const
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

tstring FilePath::Extension() const
{
	tstring filename = Filename();
	size_t pos = filename.rfind( TXT( '.' ) );
	if ( pos != tstring::npos )
	{
		return filename.substr( pos + 1 );
	}

	return TXT( "" );
}

tstring FilePath::FullExtension() const
{
	tstring filename = Filename();
	size_t pos = filename.find_first_of( TXT( '.' ) );
	if ( pos != tstring::npos )
	{
		return filename.substr( pos + 1 );
	}

	return TXT( "" );
}

void FilePath::RemoveExtension()
{
	size_t slash = m_Path.find_last_of( s_InternalPathSeparator );
	size_t pos = m_Path.find_last_of( TXT( '.' ), slash == tstring::npos ? 0 : slash );
	if ( pos != tstring::npos )
	{
		m_Path.erase( pos );
	}
}

void FilePath::RemoveFullExtension()
{
	size_t slash = m_Path.find_last_of( s_InternalPathSeparator );
	size_t pos = m_Path.find_first_of( TXT( '.' ), slash == tstring::npos ? 0 : slash );
	if ( pos != tstring::npos )
	{
		m_Path.erase( pos );
	}
}

void FilePath::ReplaceExtension( const tstring& newExtension )
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

void FilePath::ReplaceFullExtension( const tstring& newExtension )
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

bool FilePath::HasExtension( const tchar_t* extension ) const
{
	size_t len = StringLength( extension );

	if ( m_Path.length() - len < 0 )
	{
		return false;
	}

	return CaseInsensitiveCompareString( m_Path.c_str() + ( m_Path.length() - len ), extension ) == 0;
}

tstring FilePath::Native() const
{
	tstring native = m_Path;
	FilePath::MakeNative( native );    
	return native;
}

tstring FilePath::Absolute() const
{
	tstring full;
	Helium::GetFullPath( m_Path.c_str(), full );
	return full;
}

tstring FilePath::Normalized() const
{
	tstring normalized = m_Path;
	FilePath::Normalize( normalized );
	return normalized;
}

tstring FilePath::Signature()
{
	tstring temp = m_Path;
	Normalize( temp );
	return Helium::MD5( temp );
}

Helium::FilePath FilePath::GetAbsolutePath( const Helium::FilePath& basisPath ) const
{
	HELIUM_ASSERT( !IsAbsolute() ); // shouldn't call this on an already-absolute path

	tstring newPathtstring;
	Helium::GetFullPath( tstring( basisPath.Directory() + m_Path ).c_str(), newPathtstring );
	return Helium::FilePath( newPathtstring );
}

Helium::FilePath FilePath::GetRelativePath( const Helium::FilePath& basisPath ) const
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
	return Helium::FilePath( newPathtstring );
}

bool FilePath::Exists() const
{
	return FilePath::Exists( m_Path );
}

bool FilePath::IsAbsolute() const
{
	return FilePath::IsAbsolute( m_Path.c_str() );
}

bool FilePath::IsUnder( const tstring& location ) const
{
	return FilePath::IsUnder( location, m_Path );
}

size_t FilePath::length() const
{
	return m_Path.length();
}

bool FilePath::empty() const
{
	return m_Path.empty();
}

const tchar_t* FilePath::c_str() const
{
	return m_Path.c_str();
}

tstring FilePath::FileMD5() const
{
	return Helium::FileMD5( m_Path.c_str() );
}

bool FilePath::VerifyFileMD5( const tstring& hash ) const
{
	return FileMD5().compare( hash ) == 0;
}
