#include "FoundationPch.h"
#include "TUID.h"

#include "Platform/Exception.h"
#include "Foundation/Endian.h"

#include <time.h>
#include <iphlpapi.h>
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace Helium;

#pragma comment ( lib, "iphlpapi.lib" )

const tuid TUID::Null = 0x0;

static uint64_t s_CachedMacBits64 = 0; ///< cached 48-bit MAC address, centered in uint64_t

bool isAlpha( int c )
{
    return ( std::isalpha( c ) != 0 );
}

void TUID::ToString(tstring& id) const
{
    tostringstream str;
    str << TUID::HexFormat << m_ID;
    id = str.str();
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
        uint64_t bits = 0;
        uint64_t tempByte = 0;
        for ( int32_t address_byte = 5; address_byte >= 0; --address_byte )
        {
            tempByte = adapterInfo[0].Address[ address_byte ];
            bits |= tempByte << ( 8 * address_byte );
        }

        s_CachedMacBits64 |= (bits << 8); // shift left 8 to center
    }

    uid |= s_CachedMacBits64;

    uint64_t timeBits = 0;

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