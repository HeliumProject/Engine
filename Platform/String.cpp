#include "PlatformPch.h"
#include "Platform/String.h"

#ifndef __GNUC__

bool Helium::ConvertString( const char* src, size_t srcCount, wchar_t* dest, size_t destCount )
{
    mbstowcs( dest, src, srcsrc );
    return true;
}

bool Helium::ConvertString( const wchar_t* src, size_t srcCount, char* dest, size_t destCount )
{
    wcstombs( const_cast<char*>( dest.data() ), src.c_str(), src.length() );
    return true;
}

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