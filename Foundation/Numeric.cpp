#include "Numeric.h"

using namespace Helium;

static void Test( bool clamp )
{
    // 32->32 int
    {
        int32_t source = NumericLimits<int32_t>::Maximum;
        uint32_t dest = 0;
        RangeCast( source, dest, clamp );
        HELIUM_ASSERT( dest == NumericLimits<int32_t>::Maximum );
    }

    // 32->32 int
    {
        uint32_t source = NumericLimits<uint32_t>::Maximum;
        int32_t dest = 0;
        RangeCast( source, dest, clamp );
        HELIUM_ASSERT( clamp ? dest == NumericLimits<int32_t>::Maximum : dest == 0 );
    }

    // 32->64 int
    {
        int32_t source = NumericLimits<int32_t>::Maximum;
        int64_t dest = 0;
        RangeCast( source, dest, clamp );
        HELIUM_ASSERT( dest == NumericLimits<int32_t>::Maximum );
    }

    // 32->64 float
    {
        float32_t source = NumericLimits<float32_t>::Maximum;
        float64_t dest = 0;
        RangeCast( source, dest, clamp );
        HELIUM_ASSERT( dest == NumericLimits<float32_t>::Maximum );
    }

    // 64->32 int
    {
        int64_t source = NumericLimits<int64_t>::Maximum;
        int32_t dest = 0;
        RangeCast( source, dest, clamp );
        HELIUM_ASSERT( clamp ? dest == NumericLimits<int32_t>::Maximum : dest == 0 );
    }

    // 64->32 float
    {
        float64_t source = NumericLimits<float64_t>::Maximum;
        float32_t dest = 0;
        RangeCast( source, dest, clamp );
        HELIUM_ASSERT( clamp ? dest == NumericLimits<float32_t>::Maximum : dest == 0 );
    }

    // int64_t->float32_t
    {
        int64_t source = NumericLimits<int64_t>::Maximum;
        float32_t dest = 0;
        RangeCast( source, dest, clamp );
        float32_t test = (float32_t)NumericLimits<int64_t>::Maximum;
        HELIUM_ASSERT( dest == test );
    }

    // float64_t->int32_t
    {
        float64_t source = NumericLimits<float64_t>::Maximum;
        int32_t dest = 0;
        RangeCast( source, dest, clamp );
        HELIUM_ASSERT( clamp ? dest == NumericLimits<int32_t>::Maximum : dest == 0 );
    }
}

struct NumericTest
{
    NumericTest()
    {
        Test(true);
        Test(false);
    }
} g_Test;