bool Helium::ConvertString( const char* src, char* dest, size_t destCount )
{
	size_t srcCount = strlen( src );
	memcpy( dest, src, ( srcCount < destCount ? srcCount : destCount ) * sizeof( *src ) );
	return true;
}

bool Helium::ConvertString( const std::string& src, std::string& dest )
{
	dest = src;
	return true;
}

bool Helium::ConvertString( const wchar_t* src, wchar_t* dest, size_t destCount )
{
	size_t srcCount = wcslen( src );
	memcpy( dest, src, ( srcCount < destCount ? srcCount : destCount ) * sizeof( *src ) );
	return true;
}

bool Helium::ConvertString( const std::wstring& src, std::wstring& dest )
{
	dest = src;
	return true;
}