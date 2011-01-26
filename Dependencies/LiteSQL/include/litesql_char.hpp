#pragma once

#ifndef _litesql_char_hpp
#define _litesql_char_hpp
//
// String types
//

#include <string>
#include <fstream>
#include <sstream>
#include <strstream>
#include <iostream>
#include <stdio.h>

#ifdef WIN32
# include <tchar.h>
#endif

#ifdef LITESQL_UNICODE
typedef wchar_t LITESQL_Char;
#define LITESQL_L(s) L##s
#define LITESQL_cin std::wcin
#define LITESQL_cout std::wcout
#define LITESQL_cerr std::wcerr
#define LITESQL_clog std::wclog
#else
typedef char LITESQL_Char;
#define LITESQL_L(s) s
#define LITESQL_cin std::cin
#define LITESQL_cout std::cout
#define LITESQL_cerr std::cerr
#define LITESQL_clog std::clog
#endif

typedef std::basic_string<LITESQL_Char> LITESQL_String;

typedef std::basic_istream<LITESQL_Char, std::char_traits<LITESQL_Char> > LITESQL_iStream;
typedef std::basic_ostream<LITESQL_Char, std::char_traits<LITESQL_Char> > LITESQL_oStream;
typedef std::basic_iostream<LITESQL_Char, std::char_traits<LITESQL_Char> > LITESQL_ioStream;

typedef std::basic_ifstream<LITESQL_Char, std::char_traits<LITESQL_Char> > LITESQL_ifStream;
typedef std::basic_ofstream<LITESQL_Char, std::char_traits<LITESQL_Char> > LITESQL_ofSstream;
typedef std::basic_fstream<LITESQL_Char, std::char_traits<LITESQL_Char> > LITESQL_fSstream;

typedef std::basic_stringstream<LITESQL_Char, std::char_traits<LITESQL_Char>, std::allocator<LITESQL_Char> > LITESQL_Stringstream;
typedef std::basic_istringstream<LITESQL_Char, std::char_traits<LITESQL_Char>, std::allocator<LITESQL_Char> > LITESQL_iStringstream;
typedef std::basic_ostringstream<LITESQL_Char, std::char_traits<LITESQL_Char>, std::allocator<LITESQL_Char> > LITESQL_oStringstream;


inline bool LITESQL_ConvertChar(char src, char& dest)
{
    dest = src;
    return true;
}

inline bool LITESQL_ConvertString(const std::string& src, std::string& dest)
{
    dest = src;
    return true;
}

#ifndef __GNUC__
    inline bool LITESQL_ConvertChar(wchar_t src, wchar_t& dest)
    {
        dest = src;
        return true;
    }

    bool LITESQL_ConvertChar(char src, wchar_t& dest);
    bool LITESQL_ConvertChar(wchar_t src, char& dest);

    inline bool LITESQL_ConvertString(const std::wstring& src, std::wstring& dest)
    {
        dest = src;
        return true;
    }

    bool LITESQL_ConvertString(const std::string& src, std::wstring& dest);
    bool LITESQL_ConvertString(const std::wstring& src, std::string& dest);
    
#endif

#endif