#include "PlatformPch.h"
#include "Platform/String.h"

bool Helium::ConvertString( const char* src, wchar_t* dest, size_t destCount )
{
	return ::MultiByteToWideChar( CP_UTF8, 0, src, -1, dest, (int)destCount ) != 0;
}

bool Helium::ConvertString( const wchar_t* src, char* dest, size_t destCount )
{
	return ::WideCharToMultiByte( CP_UTF8, 0, src, -1, dest, (int)destCount, "?", NULL ) != 0;
}

size_t Helium::GetConvertedStringLength( const char* src )
{
	return ::MultiByteToWideChar( CP_UTF8, 0, src, -1, NULL, 0 );
}

size_t Helium::GetConvertedStringLength( const wchar_t* src )
{
	return ::WideCharToMultiByte( CP_UTF8, 0, src, -1, NULL, 0, NULL, NULL );
}

bool Helium::ConvertString( const std::string& src, std::wstring& dest )
{
	size_t length = GetConvertedStringLength( src.c_str() );
	dest.resize( length );
	return ConvertString( src.c_str(), const_cast<wchar_t*>( dest.data() ), dest.length() );
}

bool Helium::ConvertString( const std::wstring& src, std::string& dest )
{
	size_t length = GetConvertedStringLength( src.c_str() );
	dest.resize( length );
	return ConvertString( src.c_str(), const_cast<char*>( dest.data() ), dest.length() );
}
