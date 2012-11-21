#pragma once

namespace Helium
{
    template< class T >
    inline bool HasFlags( const T& bitMap, const T flag );

    template< class T >
    inline void SetFlag( T& bitMap, const T flag, const bool setIt = true );
}

#include "Foundation/Flags.inl"