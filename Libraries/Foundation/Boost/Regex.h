#pragma once

#include <string> 
#include <sstream>
#include <boost/regex.hpp>

//--------------------------------------------------------------
// helper functions for using boost::regex
// 
// some of these could be made template functions on their return types
// and moved to std::strstream; if we did that we would need to decide what
// kind of error semantics we want. currently this is just adopted from 
// existing code that requires this functionality, and has WEAK error checking
// to say the least. 
// 
// currently they are templated on their input result
// good candidate result types are: 
//   boost::smatch (when you match a std::string)
//   boost::cmatch (when you match a const char*) 
// 
// this templatization should be transparent to the user, since 
// cmatch and smatch are not ambiguous at all. 
// 

namespace Nocturnal
{
  template <class MatchT>
  inline std::string BoostMatchResultAsString( const boost::match_results<MatchT>& results, int i )
  {
    return std::string ( results[i].first, results[i].second ); 
  }

  template <class T, class MatchT>
  inline T BoostMatchResult( const boost::match_results<MatchT>& results, int i )
  {
    std::istringstream str ( BoostMatchResultAsString<MatchT>(results, i) );

    T result;
    str >> result;
    NOC_ASSERT( !str.fail() );

    return result;
  }
}
