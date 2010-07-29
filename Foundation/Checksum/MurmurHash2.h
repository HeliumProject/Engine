#pragma once

#include "Platform/Types.h"

namespace Helium
{

    // 64-bit hash for 64-bit platforms

    inline u64 MurmurHash64A ( const void * key, u64 len, unsigned int seed )
    {
        const u64 m = 0xc6a4a7935bd1e995;
        const int r = 47;

        u64 h = seed ^ (len * m);

        const u64 * data = (const u64 *)key;
        const u64 * end = data + (len/8);

        while(data != end)
        {
            u64 k = *data++;

            k *= m; 
            k ^= k >> r; 
            k *= m; 

            h ^= k;
            h *= m; 
        }

        const unsigned char* data2 = (const unsigned char*)data;

        switch(len & 7)
        {
        case 7: h ^= u64(data2[6]) << 48;
        case 6: h ^= u64(data2[5]) << 40;
        case 5: h ^= u64(data2[4]) << 32;
        case 4: h ^= u64(data2[3]) << 24;
        case 3: h ^= u64(data2[2]) << 16;
        case 2: h ^= u64(data2[1]) << 8;
        case 1: h ^= u64(data2[0]);
            h *= m;
        };

        h ^= h >> r;
        h *= m;
        h ^= h >> r;

        return h;
    }

    inline u64 MurmurHash2( const tstring& data )
    {
        return MurmurHash64A( data.data(), (u64)data.length(), 42 );
    }
}