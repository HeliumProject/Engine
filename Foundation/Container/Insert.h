#pragma once

#include "Foundation/Container/Pair.h"

#include <utility>

//
// This is a cross platform solution for microsofts _Pairib typedef,
//  which is very convenient but not present in STLPort
// 
// typedef Map<tstring, int> M_StrToInt;
//
// M_StrToInt m_Map;
//
// Insert<M_strToInt>::Result result = m_Map.Insert( M_StrToInt::ValueType("test", 1)); 
//

namespace Helium
{
    template< typename Container >
    struct StdInsert
    {
        typedef std::pair< typename Container::iterator, bool > Result;
    };

    template< typename Container >
    struct Insert
    {
        typedef Pair< typename Container::Iterator, bool > Result;
    };
}
