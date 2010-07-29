#pragma once

#include <iostream>
#include <iomanip>
#include <string>

#include <vector>
#include <set>
#include <map>
#include <hash_map>

#include "Platform/Types.h"

#include "Foundation/API.h"
#include "Foundation/Memory/Endian.h"

typedef u64 tuid;
typedef std::set< tuid > S_tuid;
typedef std::vector< tuid > V_tuid;
typedef std::map< tuid, tuid > M_tuid;
typedef std::map< tuid, u32 > M_tuidu32;

#define TUID_HEX_FORMAT TXT( "0x%016I64X" )
#define TUID_INT_FORMAT TXT( "%I64u" )

namespace Helium
{
    class GUID;

    class FOUNDATION_API TUID
    {
    protected:
        tuid m_ID;

    public:
        // The null ID
        static const tuid Null;

        // Generates a unique ID
        static tuid Generate();
        static void Generate( TUID& uid );
        static void Generate( tuid& uid );

        TUID(); // Null ID
        TUID( tuid id );
        TUID( const TUID &id );
        TUID( const tstring& id );

        TUID& operator=( const TUID &rhs );
        bool operator==( const TUID &rhs ) const;
        bool operator==( const tuid &rhs ) const;
        bool operator!=( const TUID &rhs ) const;
        bool operator!=( const tuid &rhs ) const;
        bool operator<( const TUID &rhs ) const;

        // Interop with tuid
        operator tuid() const;

        void ToString(tstring& id) const;
        bool FromString( const tstring& str );

        void ToGUID(GUID& id) const;
        void FromGUID(const GUID& id);

        // Resets the ID to be the null ID
        void Reset();

        static inline tostream& HexFormat( tostream& base )
        {
            return base << TXT( "0x" ) << std::setfill( TXT( '0' ) ) << std::setw(16) << std::right << std::hex << std::uppercase;
        }

        friend FOUNDATION_API tostream& operator<<( tostream& stream, const TUID& id );
        friend FOUNDATION_API tistream& operator>>( tistream& stream, TUID& id );
    };

    inline TUID::TUID()
        : m_ID( 0x0 )
    {

    }

    inline TUID::TUID( tuid id )
        : m_ID( id )
    {

    }

    inline TUID::TUID(const TUID &id)
        : m_ID( id.m_ID )
    {

    }

    inline TUID::TUID( const tstring& id )
    {
        FromString( id );
    }

    inline TUID& TUID::operator=(const TUID &rhs)
    {
        m_ID = rhs.m_ID;
        return *this;
    }

    inline bool TUID::operator==(const TUID &rhs) const
    {
        return m_ID == rhs.m_ID;
    }

    inline bool TUID::operator==( const tuid& rhs ) const
    {
        return m_ID == rhs;
    }

    inline bool TUID::operator!=(const TUID &rhs) const
    {
        return m_ID != rhs.m_ID;
    }

    inline bool TUID::operator!=( const tuid &rhs ) const
    {
        return m_ID != rhs;
    }

    inline bool TUID::operator<(const TUID &rhs) const
    {
        return m_ID < rhs.m_ID;
    }

    inline TUID::operator tuid() const
    {
        return m_ID;
    }

    inline void TUID::Reset()
    {
        m_ID = 0x0;
    }

    FOUNDATION_API inline tostream& operator<<(tostream& stream, const TUID& id)
    {
        tstring s;
        id.ToString(s);
        stream << s;
        return stream;
    }

    FOUNDATION_API inline tistream& operator>>(tistream& stream, TUID& id)
    {
        tstring s;
        stream >> s;
        id.FromString(s.c_str());
        return stream;
    }


    // 
    // Hashing class for storing UIDs as keys to a hash_map.
    // 

    class TUIDHasher : public stdext::hash_compare< u64 >
    {
    public:
        size_t operator()( const TUID& tuid ) const
        {
            return stdext::hash_compare< u64 >::operator()( 0 );
        }

        bool operator()( const TUID& tuid1, const TUID& tuid2 ) const
        {
            return stdext::hash_compare< u64 >::operator()( tuid1, tuid2 );
        }
    };

    typedef stdext::hash_map< TUID, TUID, TUIDHasher > HM_TUID;
    typedef stdext::hash_map< TUID, u32, TUIDHasher > HM_TUIDU32;
    typedef std::vector<TUID> V_TUID;
    typedef std::set<TUID> S_TUID;

    template<> inline void Swizzle<TUID>(TUID& val, bool swizzle)
    {
        // operator tuid() const will handle the conversion into the other swizzle func
        val = ConvertEndian(val, swizzle);
    }
}
