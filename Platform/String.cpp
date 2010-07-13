#include "Platform/String.h"

#ifndef __GNUC__

bool Platform::ConvertChar( char src, wchar_t& dest )
{
    return mbtowc( &dest, &src, 1 ) > 0;
}

bool Platform::ConvertChar( wchar_t src, char& dest )
{
    return wctomb( &dest, src ) > 0;
}

bool Platform::ConvertString( const std::string& src, std::wstring& dest )
{
    dest.resize( src.length() );
    mbstowcs( const_cast<wchar_t*>( dest.data() ), src.c_str(), src.length() );
    return true;
}

bool Platform::ConvertString( const std::wstring& src, std::string& dest )
{
    dest.resize( src.length() );
    wcstombs( const_cast<char*>( dest.data() ), src.c_str(), src.length() );
    return true;
}

#endif
