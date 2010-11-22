#include "Platform/String.h"

#ifndef __GNUC__

bool Helium::ConvertString( const std::string& src, std::wstring& dest )
{
    dest.resize( src.length() );
    mbstowcs( const_cast<wchar_t*>( dest.data() ), src.c_str(), src.length() );
    return true;
}

bool Helium::ConvertString( const std::wstring& src, std::string& dest )
{
    dest.resize( src.length() );
    wcstombs( const_cast<char*>( dest.data() ), src.c_str(), src.length() );
    return true;
}

#endif

tstring Helium::GetEncoding()
{
    tstringstream str;
    str << TXT("UTF-");
    str << sizeof(tchar_t) * 8;
    return str.str();
}