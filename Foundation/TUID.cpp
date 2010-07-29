#include "TUID.h"
#include "GUID.h"

#include "Platform/Exception.h"
#include "Foundation/Memory/Endian.h"
#include "Platform/Windows/Windows.h"

#include <time.h>
#include <iphlpapi.h>
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace Helium;

#pragma comment ( lib, "iphlpapi.lib" )

const tuid TUID::Null = 0x0;

static u64 s_CachedMacBits64 = 0; ///< cached 48-bit MAC address, centered in u64
static u64 s_GUIDConvertConstant = 0xDEADDEADDEADDEAD;

// GUID interop
struct guid
{
    u32   data0;
    u16   data1;
    u16   data2;
    u8    data3[8];

    guid()
    {
        memset(this, 0, sizeof(guid));
    }
};

bool isAlpha( int c )
{
    return ( std::isalpha( c ) != 0 );
}

void TUID::ToGUID( Helium::GUID& id ) const
{
    static guid null;
    if ( m_ID == 0x0 )
    {
        memset( &id, 0, sizeof( guid ) );
    }
    else
    {
        u64* tempPtr = (u64*)&id;

        // set the 'high order' guid bytes to our constant so we can identify it later
        tempPtr[ 0 ] = s_GUIDConvertConstant;

        // endian convert the data bytes so standard GUID printing routines generate the same characters as hexidecimal TUID printing
        tempPtr[ 1 ] = ConvertEndian( m_ID, true );
    }
}

void TUID::ToString(tstring& id) const
{
    tostringstream str;
    str << TUID::HexFormat << m_ID;
    id = str.str();
}

void TUID::FromGUID( const Helium::GUID& id )
{
    static guid null;

    if ( memcmp( &id, &null, sizeof( guid ) ) == 0 )
    {
        m_ID = Null;
    }
    else
    {
        u64* tempPtr = (u64*)&id;

        // if our GUID has been converted FROM a tuid (it has been upsampled)
        if ( tempPtr[ 0 ] == s_GUIDConvertConstant )
        {
            // copy it back and endian convert to account for byte reorder noted below
            m_ID = ConvertEndian( tempPtr[ 1 ], true );
        }
        else
        {
            // we're going to xor the top and bottom halves of the guid into the tuid
            m_ID = ( ( ( ( u64 ) id.Data1 ) << 32 ) | ( id.Data2 << 16 ) | ( id.Data3 ) ) ^ ( *(u64*)( &id.Data4 ) );
        }
    }
}

bool TUID::FromString( const tstring& str )
{
    tstringstream stream;
    tstring::const_iterator alphaIt = std::find_if( str.begin(), str.end(), isAlpha );

    if ( alphaIt != str.end() )
    {
        if ( str.length() >= 2 && ( str[1] == 'x' || str[1] == 'X' ) )
        {
            stream << str;
            stream >> std::hex >> m_ID;
        }
        else
        {
            // this is icky, pretend that they entered a hex tuid without the prefix
            tstring prefixedStr = tstring( TXT( "0x" ) ) + str;

            stream << prefixedStr;
            stream >> std::hex >> m_ID;
        }
    }
    else
    {
        // this is also icky, but it might actually be a decimal TUID
        stream << str;
        stream >> m_ID;
    }

    return ( !stream.fail() && stream.eof() );
}

tuid TUID::Generate()
{
    tuid uid;
    Generate( uid );
    return uid;
}

void TUID::Generate( TUID& uid )
{
    Generate( uid.m_ID );
}

void TUID::Generate( tuid& uid )
{
    uid = (tuid)TUID::Null;

    // fetches the MAC address (if it has not already been set)
    if ( s_CachedMacBits64 == 0 )
    {
        IP_ADAPTER_INFO adapterInfo[16];
        DWORD bufLength = sizeof( adapterInfo );
        DWORD status = GetAdaptersInfo( adapterInfo, &bufLength );
        if ( status != ERROR_SUCCESS )
        {
            throw Helium::Exception( TXT( "Could not get network adapter info to seed TUID generation." ) );
        }

        // cache the appropriate bits
        u64 bits = 0;
        u64 tempByte = 0;
        for ( i32 address_byte = 5; address_byte >= 0; --address_byte )
        {
            tempByte = adapterInfo[0].Address[ address_byte ];
            bits |= tempByte << ( 8 * address_byte );
        }

        s_CachedMacBits64 |= (bits << 8); // shift left 8 to center
    }

    uid |= s_CachedMacBits64;

    u64 timeBits = 0;

    // get the clock ticks
    LARGE_INTEGER ticks;
    BOOL result = QueryPerformanceCounter( &ticks );
    if ( !result )
    {
        throw Helium::Exception( TXT( "Could not obtain performance counter ticks to generate TUID." ) );
    }
    timeBits = ticks.LowPart;
    timeBits = timeBits << 32; // shift left

    // get the system time
    time_t systemTime;
    time( &systemTime );
    timeBits |= systemTime; // concat with clock ticks

    uid ^= timeBits;
}