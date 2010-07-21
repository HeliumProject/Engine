#include "Numeric.h"

#define I8_MIN     (-127i8 - 1)
#define I8_MAX       127i8
#define U8_MAX      0xffui8

#define I16_MIN    (-32767i16 - 1)
#define I16_MAX      32767i16
#define U16_MAX     0xffffui16

#define I32_MIN    (-2147483647i32 - 1)
#define I32_MAX      2147483647i32
#define U32_MAX     0xffffffffui32

#define I64_MIN    (-9223372036854775807i64 - 1)
#define I64_MAX      9223372036854775807i64
#define U64_MAX     0xffffffffffffffffui64

#define F32_MIN     1.175494351e-38F
#define F32_MAX     3.402823466e+38F

#define F64_MIN     2.2250738585072014e-308
#define F64_MAX     1.7976931348623158e+308

using namespace Nocturnal;

FOUNDATION_API const i8 NumericLimits<i8>::Minimum = I8_MIN;
FOUNDATION_API const i8 NumericLimits<i8>::Maximum = I8_MAX;

FOUNDATION_API const u8 NumericLimits<u8>::Minimum = 0;
FOUNDATION_API const u8 NumericLimits<u8>::Maximum = U8_MAX;

FOUNDATION_API const i16 NumericLimits<i16>::Minimum = I16_MIN;
FOUNDATION_API const i16 NumericLimits<i16>::Maximum = I16_MAX;

FOUNDATION_API const u16 NumericLimits<u16>::Minimum = 0;
FOUNDATION_API const u16 NumericLimits<u16>::Maximum = U16_MAX;

FOUNDATION_API const i32 NumericLimits<i32>::Minimum = I32_MIN;
FOUNDATION_API const i32 NumericLimits<i32>::Maximum = I32_MAX;

FOUNDATION_API const u32 NumericLimits<u32>::Minimum = 0;
FOUNDATION_API const u32 NumericLimits<u32>::Maximum = U32_MAX;

FOUNDATION_API const i64 NumericLimits<i64>::Minimum = I64_MIN;
FOUNDATION_API const i64 NumericLimits<i64>::Maximum = I64_MAX;

FOUNDATION_API const u64 NumericLimits<u64>::Minimum = 0;
FOUNDATION_API const u64 NumericLimits<u64>::Maximum = U64_MAX;

FOUNDATION_API const f32 NumericLimits<f32>::Minimum = -F32_MAX;
FOUNDATION_API const f32 NumericLimits<f32>::Maximum = F32_MAX;

FOUNDATION_API const f64 NumericLimits<f64>::Minimum = -F64_MAX;
FOUNDATION_API const f64 NumericLimits<f64>::Maximum = F64_MAX;

static void Test( bool clamp )
{
    // 32->32 int
    {
        i32 source = NumericLimits<i32>::Maximum;
        u32 dest = 0;
        RangeCast( source, dest, clamp );
        NOC_ASSERT( dest == NumericLimits<i32>::Maximum );
    }

    // 32->32 int
    {
        u32 source = NumericLimits<u32>::Maximum;
        i32 dest = 0;
        RangeCast( source, dest, clamp );
        NOC_ASSERT( clamp ? dest == NumericLimits<i32>::Maximum : dest == 0 );
    }

    // 32->64 int
    {
        i32 source = NumericLimits<i32>::Maximum;
        i64 dest = 0;
        RangeCast( source, dest, clamp );
        NOC_ASSERT( dest == NumericLimits<i32>::Maximum );
    }

    // 32->64 float
    {
        f32 source = NumericLimits<f32>::Maximum;
        f64 dest = 0;
        RangeCast( source, dest, clamp );
        NOC_ASSERT( dest == NumericLimits<f32>::Maximum );
    }

    // 64->32 int
    {
        i64 source = NumericLimits<i64>::Maximum;
        i32 dest = 0;
        RangeCast( source, dest, clamp );
        NOC_ASSERT( clamp ? dest == NumericLimits<i32>::Maximum : dest == 0 );
    }

    // 64->32 float
    {
        f64 source = NumericLimits<f64>::Maximum;
        f32 dest = 0;
        RangeCast( source, dest, clamp );
        NOC_ASSERT( clamp ? dest == NumericLimits<f32>::Maximum : dest == 0 );
    }

    // i64->f32
    {
        i64 source = NumericLimits<i64>::Maximum;
        f32 dest = 0;
        RangeCast( source, dest, clamp );
        f32 test = (f32)NumericLimits<i64>::Maximum;
        NOC_ASSERT( dest == test );
    }

    // f64->i32
    {
        f64 source = NumericLimits<f64>::Maximum;
        i32 dest = 0;
        RangeCast( source, dest, clamp );
        NOC_ASSERT( clamp ? dest == NumericLimits<i32>::Maximum : dest == 0 );
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