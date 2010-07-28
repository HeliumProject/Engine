/* LiteSQL 
 * 
 * The list of contributors at http://litesql.sf.net/ 
 * 
 * See LICENSE for copyright information. */

/** \file utils.hpp
    includes LITESQL_String.hpp and split.hpp */
#ifndef _litesql_utils_hpp
#define _litesql_utils_hpp
#include "litesql_char.hpp"
#include <assert.h>
#include "litesql/string.hpp"
#include "litesql/split.hpp"
namespace litesql {
template <class T>
const T& min(const T& v1, const T& v2) {
    return (v1 < v2) ? (v1) : (v2);
}
}
#endif