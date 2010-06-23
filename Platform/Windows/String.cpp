#include "Platform/String.h"

#pragma TODO("Implement Error Checking")

bool Platform::ConvertChar( char src, wchar_t& dest )
{
    mbtowc( &dest, &src, 1 );
    return true;
}

bool Platform::ConvertChar( wchar_t src, char& dest )
{
    wctomb( &dest, src );
    return true;
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