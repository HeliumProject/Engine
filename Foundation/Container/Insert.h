#pragma once

#include <utility>

//
// This is a cross platform solution for microsofts _Pairib typedef,
//  which is very convenient but not present in STLPort
// 
// typedef std::map<tstring, int> M_StrToInt;
//
// M_StrToInt m_Map;
//
// Insert<M_strToInt>::Result result = m_Map.insert( M_StrToInt::valueType("test", 1)); 
//

namespace Helium
{
    template <typename Container>
    struct Insert
    {
        typedef std::pair<typename Container::iterator, bool> Result;
    };
}
