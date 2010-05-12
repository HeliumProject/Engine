////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2008 Insomniac Games
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the Insomniac Open License
// as published by Insomniac Games.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even an implied warranty. See the
// Insomniac Open License for more details.
//
// You should have received a copy of the Insomniac Open License
// along with this code; if not, write to the Insomniac Games
// 2255 N. Ontario St Suite 550, Burbank, CA 91504, or email
// nocturnal@insomniacgames.com, or visit
// http://nocturnal.insomniacgames.com.
////////////////////////////////////////////////////////////////////////////

#include "TUID.h"

#include "Common/Memory/Endian.h"
#include "Windows/Windows.h"
#include <time.h>
#include <iphlpapi.h>
#include <assert.h>
#include <sstream>

#include <algorithm>
#include <cctype>

//
// This module is used to generate and handle 64-bit, Tiny Unique IDentifier (TUID)
//

using namespace TUID;

static u64 CachedMacBits64 = 0; ///< cached 48-bit MAC address, centered in u64
static u64 GUIDConvertConstant = 0xDEADDEADDEADDEAD;

bool isAlpha( int c )
{
  return ( std::isalpha( c ) != 0 );
}

tuid TUID::Generate()
{
  tuid newTuid;
  Generate( newTuid );
  return newTuid;
}

/** Generate creates a 64 bit TUID.
Each character represents a byte
<ul>
<li> M - mac address </li>
<li> T - system time </li>
<li> P - performance ticks </li>
</ul>
tuid = 0MMMMMM0 xor PPPPTTTT
@param id tuid reference
*/
void TUID::Generate(tuid &id)
{
  id = Null;

  // fetches the MAC address (if it has not already been set)
  if ( CachedMacBits64 == 0 )
  {
    IP_ADAPTER_INFO adapterInfo[16];
    DWORD bufLength = sizeof(adapterInfo);
    DWORD status = GetAdaptersInfo( adapterInfo, &bufLength);
    if ( status != ERROR_SUCCESS )
    {
      throw TUID::GenerationFailedException();
    }

    // cache the appropriate bits
    u64 bits = 0;
    u64 tempByte = 0;
    for ( i32 address_byte = 5; address_byte >= 0; --address_byte )
    {
      tempByte = adapterInfo[0].Address[ address_byte ];
      bits |= tempByte << ( 8 * address_byte );
    }

    CachedMacBits64 |= (bits << 8); // shift left 8 to center
  }

  id |= CachedMacBits64;

  u64 timeBits = 0;

  // get the clock ticks
  LARGE_INTEGER ticks;
  BOOL result = QueryPerformanceCounter( &ticks );
  if ( !result )
  {
    throw TUID::GenerationFailedException();
  }
  timeBits = ticks.LowPart;
  timeBits = timeBits << 32; // shift left

  // get the system time
  time_t systemTime;
  time( &systemTime );
  timeBits |= systemTime; // concat with clock ticks

  id ^= timeBits;
}

// try to parse a tuid from a string (the string can be hex formatted with a leading 0x)
bool TUID::Parse( const std::string& str, tuid& id )
{
  std::stringstream stream;

  std::string::const_iterator alphaIt = std::find_if( str.begin(), str.end(), isAlpha );

  if ( alphaIt != str.end() )
  {
    if ( str.length() >= 2 && ( str[1] == 'x' || str[1] == 'X' ) )
    {
      stream << str;
      stream >> std::hex >> id;
    }
    else
    {
      // this is icky, pretend that they entered a hex tuid without the prefix
      std::string prefixedStr = std::string( "0x" ) + str;

      stream << prefixedStr;
      stream >> std::hex >> id;
    }
  }
  else
  {
    // this is also icky, but it might actually be a decimal TUID
    stream << str;
    stream >> id;
  }

  return ( !stream.fail() && stream.eof() && id != TUID::Null );
}

/** Downsample samples a TUID into a smaller container.
      This version samples every 4th bit into a u16.
*/
void TUID::Downsample( const tuid &tuid, u16 &output )
{
  // samples every 4th bit of the tuid into the u16
  output = 0;
  for( int i = 0; i < 16; ++i )
  {
    output |= ( 0x1 & ( tuid >> (i * 4) ) ) << i;
  }
}

/** Downsample samples a TUID into a smaller container.
      This version samples every other bit into a u32.
*/
void TUID::Downsample( const tuid &tuid, u32 &output )
{
  // samples every other bit in the tuid into the u32
  output = 0;
  for( int i = 0; i < 32; ++i )
  {
    output |= ( 0x1 & ( tuid >> (i * 2) ) )<< i;
  }
}

void TUID::Deconstruct( const tuid &t, u32 &bottomHalf, u32 &topHalf )
{
  bottomHalf = (u32) t;
  topHalf = (u32) ( t >> 32 );
}

tuid TUID::Reconstruct( u32 bottomHalf, u32 topHalf )
{
  return (tuid) ( ( (u64)topHalf << 32 ) | bottomHalf );
}

tuid TUID::Reconstruct( i32 bottomHalf, i32 topHalf )
{
  return Reconstruct( (u32) bottomHalf, (u32) topHalf );
}

void TUID::Downsample( const guid& g, tuid& outputTuid )
{
  static guid null;

  if ( memcmp(&g, &null, sizeof(guid)) == 0 )
  {
    outputTuid = 0x0;
  }
  else
  {
    u64* tempPtr = (u64*)&g;

    // if our GUID has been converted FROM a tuid (it has been upsampled)
    if (tempPtr[0] == GUIDConvertConstant)
    {
      // copy it back and endian convert to account for byte reorder noted below
      outputTuid = ConvertEndian(tempPtr[1], true);
    }
    else
    {
      // we're going to xor the top and bottom halves of the guid into the tuid
      outputTuid = ( ( ( ( u64 ) g.data0 ) << 32 ) | ( g.data1 << 16 ) | ( g.data2 ) ) ^ ( *(u64*)( &g.data3 ) );
    }
  }
}

void TUID::Upsample( const tuid& t, guid& outputGuid )
{
  if ( t == 0x0 )
  {
    memset(&outputGuid, 0, sizeof(guid));
  }
  else
  {
    u64* tempPtr = (u64*)&outputGuid;

    // set the 'high order' guid bytes to our constant so we can identify it later
    tempPtr[0] = GUIDConvertConstant;

    // endian convert the data bytes so standard GUID printing routines generate the same characters as hexidecimal TUID printing
    tempPtr[1] = ConvertEndian(t, true);
  }
}
