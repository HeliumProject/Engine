/* LiteSQL 
 * 
 * The list of contributors at http://litesql.sf.net/ 
 * 
 * See LICENSE for copyright information. */

/** \file split.hpp
    contains class Split */
#ifndef litesql_split_hpp
#define litesql_split_hpp
#include "litesql_char.hpp"
#include <string>
#include <vector>
namespace litesql {
/** splits and joins strings. Modelled with Python's strings in mind. */
class Split : public std::vector<LITESQL_String> {
public:
    /** empty split */
    Split() {}
    /** init with reserved size (only reserved)  */
    Split(size_t initial_size) { reserve(initial_size);}
    /** from LITESQL_String vector */
    Split(std::vector<LITESQL_String> data) 
        : std::vector<LITESQL_String>(data) {}
    /** from LITESQL_String. Split to parts using delimeter */
    Split(const LITESQL_String& s, const LITESQL_String& delim= LITESQL_L(" "));

    /** returns a part of strings 
     \param start starting index
     \param end ending index
     Indexes can be negative; actual index is calculated from the end of 
     Split then.*/
    Split slice(int start, int end) const;
    /** returns strings joined with delimiter */
    LITESQL_String join(const LITESQL_String& delim) const;

    static LITESQL_String join(const std::vector<LITESQL_String>& strings,const LITESQL_String& delim= LITESQL_L(" "));
    /** adds contents of another split to the end */
    Split & extend(const std::vector<LITESQL_String> & s);
};
}
#endif