int Helium::NaturalCompareString( tchar_t const* a, tchar_t const* b )
{
	return strnatcmp( a, b );
}

int Helium::CaseInsensitiveNaturalCompareString( tchar_t const* a, tchar_t const* b )
{
	return strinatcmp( a, b );
}

bool Helium::NaturalStringComparitor::operator()( const tstring& str1, const tstring& str2 ) const
{
	return ( strnatcmp( str1.c_str(), str2.c_str() ) < 0 );
}

bool Helium::CaseInsensitiveNaturalStringComparitor::operator()( const tstring& str1, const tstring& str2 ) const
{
	return ( strinatcmp( str1.c_str(), str2.c_str() ) < 0 );
}