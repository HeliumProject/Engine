#pragma once

#include <iostream>
#include <string>

#include <vector>
#include <set>
#include <map>
#include <hash_map>

#include "Platform/Types.h"

#include "Foundation/API.h"
#include "Foundation/Memory/Endian.h"

typedef uint64_t tuid;

namespace Helium
{
    //
    // Wrapper for Microsoft GUID/Uuid - requires RPCRT4.LIB
    //

    class FOUNDATION_API GUID
    {
    public:
        // The null ID
        static const GUID Null;

        uint32_t Data1;
        uint16_t Data2;
        uint16_t Data3;
        uint8_t Data4[ 8 ];


        GUID(); // Null ID
        GUID( const GUID &id );
        GUID& operator=( const GUID &rhs );

        bool operator==( const GUID &rhs ) const;
        bool operator!=( const GUID &rhs ) const;
        bool operator<( const GUID &rhs ) const;

        void ToTUID( tuid& id ) const;
        void FromTUID( tuid id );

        void ToString( tstring& id ) const;
        bool FromString( const tstring& id );

        // Resets the ID to be kNullID
        void Reset();

        // Generates a unique ID
        static GUID Generate();

        // Generates a unique ID
        static void Generate( GUID& uid );

        friend FOUNDATION_API tostream& operator<<( tostream& stream, const GUID& id );
        friend FOUNDATION_API tistream& operator>>( tistream& stream, GUID& id );
    };

    FOUNDATION_API inline tostream& operator<<( tostream& stream, const GUID& id )
    {
        tstring s;
        id.ToString( s );
        stream << s;
        return stream;
    }

    FOUNDATION_API inline tistream& operator>>( tistream& stream, GUID& id )
    {
        tstring s;
        stream >> s;
        id.FromString( s.c_str() );
        return stream;
    }


    // 
    // Hashing class for storing UIDs as keys to a hash_map.
    // 

    class GUIDHasher : public stdext::hash_compare< GUID >
    {
    public:
        size_t operator()( const GUID& guid ) const
        {
            // Based on hashing algorithm found at:
            // http://source.winehq.org/source/tools/widl/write_msft.c
            size_t hash = 0;

            const short* data = reinterpret_cast<const short*>( &guid );
            hash ^= *data++;
            hash ^= *data++;
            hash ^= *data++;
            hash ^= *data++;
            hash ^= *data++;
            hash ^= *data++;
            hash ^= *data++;
            hash ^= *data;

            return hash & 0x1f;
        }

        bool operator()( const GUID& guid1, const GUID& guid2 ) const
        {
            return guid1 < guid2;
        }
    };

    typedef stdext::hash_map< GUID, GUID, GUIDHasher > HM_GUID;
    typedef stdext::hash_map< GUID, uint32_t, GUIDHasher > HM_GUIDU32;
    typedef std::vector< GUID > V_GUID;
    typedef std::set< GUID > S_GUID;  

    template<> inline void Swizzle< GUID >( GUID& val, bool swizzle )
    {
        val.Data1 = ConvertEndian(val.Data1, swizzle);
        val.Data2 = ConvertEndian(val.Data2, swizzle);
        val.Data3 = ConvertEndian(val.Data3, swizzle);
    }
}