#include "Platform/Simd.h"
#include "Platform/Assert.h"

#pragma TODO( "LUNAR MERGE - Remove HELIUM_EPSILON definition here or update as appropriate once Platform/Math/MathCommon.h is merged over." )
#define HELIUM_EPSILON ( 1.0e-8f )

namespace Helium
{
    const SimdVector Simd::EPSILON = Simd::SetSplatF32( HELIUM_EPSILON );
}
