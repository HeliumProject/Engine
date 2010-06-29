#pragma once

#include <string> 
#include <sstream>
#include <boost/regex.hpp>

//--------------------------------------------------------------
// helper functions for using boost::regex
// 
// some of these could be made template functions on their return types
// and moved to tstringstream; if we did that we would need to decide what
// kind of error semantics we want. currently this is just adopted from 
// existing code that requires this functionality, and has WEAK error checking
// to say the least. 
// 
// currently they are templated on their input result
// good candidate result types are: 
//   boost::smatch (when you match a tstring)
//   boost::cmatch (when you match a const tchar*) 
// 
// this templatization should be transparent to the user, since 
// cmatch and smatch are not ambiguous at all. 
// 

typedef boost::basic_regex<tchar, boost::regex_traits<tchar> > tregex;
typedef boost::match_results<const tchar*> tcmatch;
typedef boost::match_results<tstring::const_iterator> tsmatch;
typedef boost::regex_token_iterator< const tchar*> tcregex_token_iterator;
typedef boost::regex_token_iterator< tstring::const_iterator> tsregex_token_iterator;
typedef boost::regex_iterator< const tchar* > tcregex_iterator;
typedef boost::regex_iterator< tstring::const_iterator > tsregex_iterator;


namespace Nocturnal
{
    template <class MatchT>
    inline tstring BoostMatchResultAsString( const boost::match_results<MatchT>& results, int i )
    {
        return tstring ( results[i].first, results[i].second ); 
    }

    template <class T, class MatchT>
    inline T BoostMatchResult( const boost::match_results<MatchT>& results, int i )
    {
        tistringstream str ( BoostMatchResultAsString<MatchT>(results, i) );

        T result;
        str >> result;
        NOC_ASSERT( !str.fail() );

        return result;
    }
}
