#include "litesql_char.hpp"

#ifndef __GNUC__

bool LITESQL_ConvertChar(char src, wchar_t& dest)
{
    return mbtowc(&dest, &src, 1) > 0;
}

bool LITESQL_ConvertChar(wchar_t src, char& dest)
{
    return wctomb(&dest, src) > 0;
}

bool LITESQL_ConvertString(const std::string& src, std::wstring& dest)
{
    dest.resize(src.length());
    mbstowcs(const_cast<wchar_t*>(dest.data()), src.c_str(), src.length());
    return true;
}

bool LITESQL_ConvertString(const std::wstring& src, std::string& dest)
{
    dest.resize(src.length());
    wcstombs(const_cast<char*>(dest.data()), src.c_str(), src.length());
    return true;
}

#endif
