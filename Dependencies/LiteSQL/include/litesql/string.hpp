/* LiteSQL 
 * 
 * The list of contributors at http://litesql.sf.net/ 
 * 
 * See LICENSE for copyright information. */

/** \file LITESQL_String.hpp
    helpful LITESQL_String utils */
#ifndef litesql_string_hpp
#define litesql_string_hpp

#include "litesql_char.hpp"

#include <string>
#include <sstream>
namespace litesql {
/** returns LITESQL_String representation of passed parameter if it can be 
    written to LITESQL_oStringstream */
template <class T> 
LITESQL_String toString(T a) {
    LITESQL_oStringstream ost;
    ost << a;
    return ost.str();
}

/** returns true if 'what' starts with 'with' */
bool startsWith(const LITESQL_String& what, const LITESQL_String& with);
/** returns true if 'what' end with 'with' */
bool endsWith(const LITESQL_String& what, const LITESQL_String& with);
/** returns lower-case version of the LITESQL_String */
LITESQL_String toLower(LITESQL_String s);
/** returns upper-case version of the LITESQL_String */
LITESQL_String toUpper(LITESQL_String s);

LITESQL_String capitalize(const LITESQL_String& s);
LITESQL_String decapitalize(const LITESQL_String& s);


/** returns a LITESQL_String with no empty characters at the end of the LITESQL_String */
LITESQL_String rstrip(LITESQL_String s);
/** returns a LITESQL_String with no empty characters at the beginning of the LITESQL_String */
LITESQL_String lstrip(LITESQL_String s);
/** returns a copy LITESQL_String 's' where 'what' is replaced with 'with' */
LITESQL_String replace(const LITESQL_String& s, const LITESQL_String& what, const LITESQL_String& with);
/** converts LITESQL_String representation of a hex number to integer */
int hexToInt(const LITESQL_String& s);
/** LITESQL_String version of atoi */
int atoi(const LITESQL_String &s);
/** returns a LITESQL_String which is duplicated 'amount' times */
LITESQL_String operator*(int amount, const LITESQL_String &s);
/** returns a LITESQL_String which is duplicated 'amount' times */
LITESQL_String operator*(const LITESQL_String &s, int amount);
/** escapes ' characters so that they do not break SQL statements.
    Note: 'NULL' is escaped to NULL */
LITESQL_String escapeSQL(const LITESQL_String &str);
}

#endif














