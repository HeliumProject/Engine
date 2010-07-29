/* LiteSQL - LITESQL_String utilities' implementation
 * 
 * The list of contributors at http://litesql.sf.net/ 
 * 
 * See LICENSE for copyright information. */
#include "litesql_char.hpp"
#include "litesql/utils.hpp"

#include <ctype.h>
#include <cstdlib>

namespace litesql {

bool startsWith(const LITESQL_String& what, const LITESQL_String& with) {
    if (what.size() < with.size())
        return false;
    if (what.substr(0, with.size()) == with)
        return true;
    return false;
}
bool endsWith(const LITESQL_String& what, const LITESQL_String& with) {
    if (what.size() < with.size())
        return false;
    if (what.substr(what.size()-with.size(), what.size()) == with)
        return true;
    return false;
}

LITESQL_String toLower(LITESQL_String s) {
    for (unsigned int i = 0; i < s.size(); i++)
        s[i] = tolower(s[i]);
    return s;	
}
LITESQL_String toUpper(LITESQL_String s) {
    for (unsigned int i = 0; i < s.size(); i++)
        s[i] = toupper(s[i]);
    return s;	
}

LITESQL_String capitalize(const LITESQL_String& s) {
    if (s.empty())
        return s;
    LITESQL_Char buf[2] = {toupper(s[0]), 0};
    return LITESQL_String(buf) + s.substr(1, s.size());
}
LITESQL_String decapitalize(const LITESQL_String& s) {
    if (s.empty())
        return s;
    LITESQL_Char buf[2] = {tolower(s[0]), 0};
    return LITESQL_String(buf) + s.substr(1, s.size());
}

LITESQL_String rstrip(const LITESQL_String& s) {
    if (s.empty())
        return s;
    int pos = s.size()-1;
    while (1) {
        if (isspace(s[pos]) && pos > 0)
            pos--;
        else
            break;
    }
    return s.substr(0, pos+1);
}
LITESQL_String lstrip(const LITESQL_String& s) {
    unsigned int pos = 0;
    while (1) {
        if (isspace(s[pos]) && pos < s.size()-1)
            pos++;
        else
            break;
    }
    return s.substr(pos, s.size());
}
LITESQL_String replace(const LITESQL_String& s, const LITESQL_String& what, const LITESQL_String& with) {
    Split parts(s, what);
    return parts.join(with);
}
int hexToInt(const LITESQL_String& s) {
    int res = 0;
	
    for (size_t i = 0; i < s.size(); i++) {
        int multiplier = 1;
        int exp = (s.size() - 1 - i);
        while (exp-- > 0)
            multiplier *= 16;
        int ch = s[i];
        if (ch >= '0' && ch <= '9')
            res += multiplier * (ch - '0');
        else if (ch >= 'a' && ch <= 'z')
            res += multiplier * (ch - 'a');
        else if (ch >= 'A' && ch <= 'Z')
            res += multiplier * (ch - 'A');
    }
    return res;
}
int atoi(const LITESQL_String &s) {
    return _tstoi(s.c_str());
}
LITESQL_String operator*(int amount, const LITESQL_String &s) {
    LITESQL_String res;
    for(;amount>0;amount--)
        res += s;
    return res;
}	
LITESQL_String operator*(const LITESQL_String &s, int amount) {
    return amount * s;
}
LITESQL_String escapeSQL(const LITESQL_String &str)
{ 
    LITESQL_String tmp;
    if (str ==  LITESQL_L("NULL"))
        return  LITESQL_L("NULL");
    
    tmp = replace(str,  LITESQL_L("'NULL'"),  LITESQL_L("NULL"));
    return  LITESQL_L("'") + replace(tmp,  LITESQL_L("'"),  LITESQL_L("''")) +  LITESQL_L("'");
} 
}

