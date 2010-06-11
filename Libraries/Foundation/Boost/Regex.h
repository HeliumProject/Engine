#pragma once

#include "Platform/Types.h" 

#include <boost/regex.hpp>
#include <string> 
#include <stdlib.h> 

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

template <class ResultType>
inline std::string ResultAsString(ResultType& results, int i)
{
  const char* random = &*results[i].first; 

  std::string temp( results[i].first, results[i].second); 
  return temp; 
}

template <class ResultType>
inline int ResultAsInt(ResultType& results, int i)
{
  std::string resultString = ResultAsString(results, i); 

  return atoi( resultString.c_str() ); 
}

template <class ResultType>
inline u64 ResultAsU64(ResultType& results, int i)
{
  return _strtoui64( ResultAsString(results, i).c_str(), NULL, 10); 
}

