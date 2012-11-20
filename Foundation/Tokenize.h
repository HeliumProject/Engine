#pragma once 

#include "Foundation/Regex.h"

#include <string>
#include <vector>
#include <set>

#include <regex>

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

namespace Helium
{
    template< typename T >
    void Tokenize( const tstring& str, std::vector< T >& tokens, const tstring delimiters )
    {
        tregex splitPattern(delimiters); 

        std::tr1::sregex_token_iterator i(str.begin(), str.end(), splitPattern, -1); 
        std::tr1::sregex_token_iterator end; 

        for(; i != end; i++)
        {
            if( (*i).matched)
            {
                T temp; 
                tstringstream inStream(*i); 
                inStream >> temp; 

                tokens.push_back(temp);
            }
        }
    }

    template< typename T >
    void Tokenize( const tstring& str, std::set< T >& tokens, const tstring delimiters )
    {
        std::tr1::regex splitPattern(delimiters); 

        std::tr1::sregex_token_iterator i(str.begin(), str.end(), splitPattern, -1); 
        std::tr1::sregex_token_iterator end; 

        for(; i != end; i++)
        {
            if( (*i).matched)
            {
                T temp; 
                tstringstream inStream(*i); 
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
    inline void Tokenize( const tstring& str, std::vector< tstring >& tokens, const tstring delimiters )
    {
        tregex splitPattern(delimiters); 

        tsregex_token_iterator i(str.begin(), str.end(), splitPattern, -1); 
        tsregex_token_iterator end; 

        for(; i != end; i++)
        {
            if( (*i).matched)
            {
                tokens.push_back(*i); 
            }
        }
    }

    template<>
    inline void Tokenize( const tstring& str, std::set< tstring >& tokens, const tstring delimiters )
    {
        tregex splitPattern(delimiters); 

        tsregex_token_iterator i(str.begin(), str.end(), splitPattern, -1); 
        tsregex_token_iterator end; 

        for(; i != end; i++)
        {
            if( (*i).matched )
            {
                tokens.insert( *i ); 
            }
        }
    }
}