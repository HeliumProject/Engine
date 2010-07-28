#pragma once

#include "Platform/Types.h"

#include <string> 
#include <sstream>
#include <regex>

//--------------------------------------------------------------
// helper functions for using std::tr1::regex
// 
// some of these could be made template functions on their return types
// and moved to tstringstream; if we did that we would need to decide what
// kind of error semantics we want. currently this is just adopted from 
// existing code that requires this functionality, and has WEAK error checking
// to say the least. 
// 
// currently they are templated on their input result
// good candidate result types are: 
//   std::tr1::smatch (when you match a tstring)
//   std::tr1::cmatch (when you match a const tchar*) 
// 
// this templatization should be transparent to the user, since 
// cmatch and smatch are not ambiguous at all. 
// 

typedef std::tr1::basic_regex<tchar, std::tr1::regex_traits<tchar> > tregex;
typedef std::tr1::match_results<const tchar*> tcmatch;
typedef std::tr1::match_results<tstring::const_iterator> tsmatch;
typedef std::tr1::regex_token_iterator< const tchar*> tcregex_token_iterator;
typedef std::tr1::regex_token_iterator< tstring::const_iterator> tsregex_token_iterator;
typedef std::tr1::regex_iterator< const tchar* > tcregex_iterator;
typedef std::tr1::regex_iterator< tstring::const_iterator > tsregex_iterator;

namespace Nocturnal
{
    template <class MatchT>
    inline tstring MatchResultAsString( const std::tr1::match_results<MatchT>& results, int i )
    {
        return tstring ( results[i].first, results[i].second ); 
    }

    template <class T, class MatchT>
    inline T MatchResult( const std::tr1::match_results<MatchT>& results, int i )
    {
        tistringstream str ( MatchResultAsString<MatchT>(results, i) );

        T result;
        str >> result;
        NOC_ASSERT( !str.fail() );

        return result;
    }
}
