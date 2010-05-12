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

#pragma once

#include "API.h"
#include "Exceptions.h"
#include "Common/Types.h"

#include <set>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>

// TUID identifier
typedef u64 tuid;
typedef std::set< tuid > S_tuid;
typedef std::vector< tuid > V_tuid;
typedef std::map< tuid, tuid > M_tuid;
typedef std::map< tuid, u32 > M_tuidu32;

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

namespace TUID
{
  // Used for undefined or invalid TUID
  const static tuid Null = 0;

  TUID_API tuid Generate();
  TUID_API void Generate( tuid &tuid );
  TUID_API bool Parse( const std::string& str, tuid& id );

  TUID_API void Downsample( const tuid &tuid, u16 &output );
  TUID_API void Downsample( const tuid &tuid, u32 &output );

  TUID_API void Deconstruct( const tuid &t, u32 &bottomHalf, u32 &topHalf );
  TUID_API tuid Reconstruct( u32 bottomHalf, u32 topHalf );
  TUID_API tuid Reconstruct( i32 bottomHalf, i32 topHalf );

  inline std::ostream& HexFormat(std::ostream& base)
  {
    return base << "0x" << std::setfill('0') << std::setw(16) << std::right << std::hex << std::uppercase;
  }

  TUID_API void Downsample( const guid& g, tuid& outputTuid );
  TUID_API void Upsample( const tuid& t, guid& outputGuid );
}

#define TUID_HEX_FORMAT "0x%016I64X"
#define TUID_INT_FORMAT "%I64u" // was "%016I64u"
