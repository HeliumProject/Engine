#pragma once 

#include "Utilities.h"

#include <string>
#include <vector>
#include <set>

#include <boost/regex.hpp>

////////////////////////////////////////////////////////////////////////
//
// String tokenizer
//
//  This parses delmited values into a typed container
//   EG 1, 2, 3 with delimiter ',' -> std::vector<int> containing
//
// Special delimiters that need to be escaped:
//  - "|" (pipe) should be "\\|"
//
////////////////////////////////////////////////////////////////////////

template< typename T >
void Tokenize( const std::string& str, std::vector< T >& tokens, const std::string delimiters )
{
    boost::regex splitPattern(delimiters); 

    boost::sregex_token_iterator i(str.begin(), str.end(), splitPattern, -1); 
    boost::sregex_token_iterator end; 

    for(; i != end; i++)
    {
        if( (*i).matched)
        {
            T temp; 
            std::stringstream inStream(*i); 
            inStream >> temp; 

            tokens.push_back(temp);
        }
    }
}

template< typename T >
void Tokenize( const std::string& str, std::set< T >& tokens, const std::string delimiters )
{
    boost::regex splitPattern(delimiters); 

    boost::sregex_token_iterator i(str.begin(), str.end(), splitPattern, -1); 
    boost::sregex_token_iterator end; 

    for(; i != end; i++)
    {
        if( (*i).matched)
        {
            T temp; 
            std::stringstream inStream(*i); 
            inStream >> temp; 

            tokens.push_back(temp);
        }
    }
}

////////////////////////////////////////////////////////////////////////
//
// Specializations to handle strings
//
////////////////////////////////////////////////////////////////////////

template<>
inline void Tokenize( const std::string& str, std::vector< std::string >& tokens, const std::string delimiters )
{
    boost::regex splitPattern(delimiters); 

    boost::sregex_token_iterator i(str.begin(), str.end(), splitPattern, -1); 
    boost::sregex_token_iterator end; 

    for(; i != end; i++)
    {
        if( (*i).matched)
        {
            tokens.push_back(*i); 
        }
    }
}

template<>
inline void Tokenize( const std::string& str, std::set< std::string >& tokens, const std::string delimiters )
{
    boost::regex splitPattern(delimiters); 

    boost::sregex_token_iterator i(str.begin(), str.end(), splitPattern, -1); 
    boost::sregex_token_iterator end; 

    for(; i != end; i++)
    {
        if( (*i).matched)
        {
            tokens.insert( *i ); 
        }
    }
}
