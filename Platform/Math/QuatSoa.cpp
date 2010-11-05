//----------------------------------------------------------------------------------------------------------------------
// QuatSoa.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

//#include "CorePch.h"
#include "Platform/Math/QuatSoa.h"

namespace Helium
{
    const QuatSoa QuatSoa::IDENTITY(
        Simd::LoadZeros(),
        Simd::LoadZeros(),
        Simd::LoadZeros(),
        Simd::SetSplatF32( 1.0f ) );
}
