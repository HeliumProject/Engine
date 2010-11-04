//----------------------------------------------------------------------------------------------------------------------
// QuatSoa.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"
#include "Core/QuatSoa.h"

namespace Lunar
{
    const QuatSoa QuatSoa::IDENTITY(
        Helium::Simd::LoadZeros(),
        Helium::Simd::LoadZeros(),
        Helium::Simd::LoadZeros(),
        Helium::Simd::SetSplatF32( 1.0f ) );
}
