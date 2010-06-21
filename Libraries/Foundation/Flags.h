#pragma once

namespace Nocturnal
{
    template< class T >
    inline bool HasFlags( const T& bitMap, const T flag )
    {
        return ( ( bitMap & flag ) == flag );
    }

    template< class T >
    inline void SetFlag( T& bitMap, const T flag, const bool setIt = true )
    {
        if ( setIt )
        {
            bitMap |= flag;
        }
        else // unset
        {
            bitMap &= ~flag;
        }
    }
}