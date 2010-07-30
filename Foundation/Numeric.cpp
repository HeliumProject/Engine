#include "Numeric.h"

using namespace Helium;

static void Test( bool clamp )
{
    // 32->32 int
    {
        i32 source = NumericLimits<i32>::Maximum;
        u32 dest = 0;
        RangeCast( source, dest, clamp );
        HELIUM_ASSERT( dest == NumericLimits<i32>::Maximum );
    }

    // 32->32 int
    {
        u32 source = NumericLimits<u32>::Maximum;
        i32 dest = 0;
        RangeCast( source, dest, clamp );
        HELIUM_ASSERT( clamp ? dest == NumericLimits<i32>::Maximum : dest == 0 );
    }

    // 32->64 int
    {
        i32 source = NumericLimits<i32>::Maximum;
        i64 dest = 0;
        RangeCast( source, dest, clamp );
        HELIUM_ASSERT( dest == NumericLimits<i32>::Maximum );
    }

    // 32->64 float
    {
        f32 source = NumericLimits<f32>::Maximum;
        f64 dest = 0;
        RangeCast( source, dest, clamp );
        HELIUM_ASSERT( dest == NumericLimits<f32>::Maximum );
    }

    // 64->32 int
    {
        i64 source = NumericLimits<i64>::Maximum;
        i32 dest = 0;
        RangeCast( source, dest, clamp );
        HELIUM_ASSERT( clamp ? dest == NumericLimits<i32>::Maximum : dest == 0 );
    }

    // 64->32 float
    {
        f64 source = NumericLimits<f64>::Maximum;
        f32 dest = 0;
        RangeCast( source, dest, clamp );
        HELIUM_ASSERT( clamp ? dest == NumericLimits<f32>::Maximum : dest == 0 );
    }

    // i64->f32
    {
        i64 source = NumericLimits<i64>::Maximum;
        f32 dest = 0;
        RangeCast( source, dest, clamp );
        f32 test = (f32)NumericLimits<i64>::Maximum;
        HELIUM_ASSERT( dest == test );
    }

    // f64->i32
    {
        f64 source = NumericLimits<f64>::Maximum;
        i32 dest = 0;
        RangeCast( source, dest, clamp );
        HELIUM_ASSERT( clamp ? dest == NumericLimits<i32>::Maximum : dest == 0 );
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