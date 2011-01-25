/* LiteSQL - Split implementation
 * 
 * The list of contributors at http://litesql.sf.net/ 
 * 
 * See LICENSE for copyright information. */
//#include "compatibility.hpp"
#include "litesql/split.hpp"
#include <string>
#include <string.h>
#include <cstring>
#include <cstdlib>

using namespace litesql;    

Split::Split(const LITESQL_String& s, const LITESQL_String& delim) {
  LITESQL_Char * buf = _tcsdup((LITESQL_Char*) s.c_str());
    LITESQL_Char * ptr = buf;
    int len = delim.size();
    std::vector<LITESQL_Char*> pointers;
    pointers.push_back(ptr);
    while((ptr = _tcsstr(ptr, delim.c_str()))) {
        *ptr = '\0';
        ptr += len;
        pointers.push_back(ptr);
    }
    for (std::vector<LITESQL_Char*>::iterator i = pointers.begin();
         i != pointers.end();
         ++i)
        push_back(LITESQL_String(*i));

    free(buf);
}
Split Split::slice(int start, int end) const {
    Split data;
    if (start < 0)
        start = start+size();
    if (end < 0)
        end = end+size();
    if (start >= static_cast<int>(size()))
        start = size()-1;
    if (end >= static_cast<int>(size()))
        end = size()-1;
    if (start >= end)
        return data;
    for (int i = start; i < end; i++)
        data.push_back(this->operator[](i));
    return data;
}

LITESQL_String Split::join(const std::vector<LITESQL_String>& strings,const LITESQL_String& delim){
    LITESQL_String res;
    for (const_iterator it = strings.begin(); it != strings.end(); it++) {
        if (it != strings.begin())
          res.append(delim);
        res.append(*it);
    }
    return res;
}

LITESQL_String Split::join(const LITESQL_String& delim) const {
  return join(*this,delim);  
}

Split & Split::extend(const std::vector<LITESQL_String> & s) {
  reserve(size()+s.size());
  for (std::vector<LITESQL_String>::const_iterator it = s.begin();it != s.end(); it++)
        push_back(*it);
    return *this;
}
