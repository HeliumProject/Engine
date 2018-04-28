#include "Precompile.h"

#include "MathSimd/QuatSoa.h"

const Helium::Simd::QuatSoa Helium::Simd::QuatSoa::IDENTITY(
    Simd::LoadZeros(),
    Simd::LoadZeros(),
    Simd::LoadZeros(),
    Simd::SetSplatF32( 1.0f ) );
