#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <hash_map>

#include "API.h"
#include "Platform/Types.h"
#include "Foundation/Memory/Endian.h"

typedef u64 tuid;

namespace Nocturnal
{
    namespace UID
    {
        //
        // Wrapper for Microsoft GUID/Uuid - requires RPCRT4.LIB
        //

        class UID_API GUID
        {
        public:
            // The null ID
            static const GUID Null;

            u32 Data1;
            u16 Data2;
            u16 Data3;
            u8 Data4[ 8 ];

            
            GUID(); // Null ID
            GUID( const GUID &id );
            GUID& operator=( const GUID &rhs );

            bool operator==( const GUID &rhs ) const;
            bool operator!=( const GUID &rhs ) const;
            bool operator<( const GUID &rhs ) const;

            void ToTUID( tuid& id ) const;
            void FromTUID( tuid id );

            void ToString( std::string& id ) const;
            bool FromString( const std::string& id );

            // Resets the ID to be kNullID
            void Reset();

            // Generates a unique ID
            static GUID Generate();

            // Generates a unique ID
            static void Generate( GUID& uid );

            friend UID_API std::ostream& operator<<( std::ostream& stream, const GUID& id );
            friend UID_API std::istream& operator>>( std::istream& stream, GUID& id );
        };

        UID_API inline std::ostream& operator<<( std::ostream& stream, const GUID& id )
        {
            std::string s;
            id.ToString( s );
            stream << s;
            return stream;
        }

        UID_API inline std::istream& operator>>( std::istream& stream, GUID& id )
        {
            std::string s;
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
        typedef stdext::hash_map< GUID, u32, GUIDHasher > HM_GUIDU32;
        typedef std::vector< GUID > V_GUID;
        typedef std::set< GUID > S_GUID;  
    }

    template<> inline void Swizzle< UID::GUID >( UID::GUID& val, bool swizzle )
    {
        val.Data1 = ConvertEndian(val.Data1, swizzle);
        val.Data2 = ConvertEndian(val.Data2, swizzle);
        val.Data3 = ConvertEndian(val.Data3, swizzle);
    }
}