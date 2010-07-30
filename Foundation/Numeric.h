#pragma once

#include "Platform/Types.h"
#include "Platform/Assert.h"

#include "API.h"

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

namespace Helium
{
    template< class T >
    struct NumericLimits
    {
        const static T Minimum;
        const static T Maximum;
    };

    const i8 NumericLimits<i8>::Minimum = I8_MIN;
    const i8 NumericLimits<i8>::Maximum = I8_MAX;

    const u8 NumericLimits<u8>::Minimum = 0;
    const u8 NumericLimits<u8>::Maximum = U8_MAX;

    const i16 NumericLimits<i16>::Minimum = I16_MIN;
    const i16 NumericLimits<i16>::Maximum = I16_MAX;

    const u16 NumericLimits<u16>::Minimum = 0;
    const u16 NumericLimits<u16>::Maximum = U16_MAX;

    const i32 NumericLimits<i32>::Minimum = I32_MIN;
    const i32 NumericLimits<i32>::Maximum = I32_MAX;

    const u32 NumericLimits<u32>::Minimum = 0;
    const u32 NumericLimits<u32>::Maximum = U32_MAX;

    const i64 NumericLimits<i64>::Minimum = I64_MIN;
    const i64 NumericLimits<i64>::Maximum = I64_MAX;

    const u64 NumericLimits<u64>::Minimum = 0;
    const u64 NumericLimits<u64>::Maximum = U64_MAX;

    const f32 NumericLimits<f32>::Minimum = -F32_MAX;
    const f32 NumericLimits<f32>::Maximum = F32_MAX;

    const f64 NumericLimits<f64>::Minimum = -F64_MAX;
    const f64 NumericLimits<f64>::Maximum = F64_MAX;

    template< class S, class D >
    inline bool RangeCastInteger( const S source, D& dest, bool clamp = false )
    {
        if ( source > 0 )
        {
            if ( (u64)source <= (u64)NumericLimits<D>::Maximum )
            {
                dest = static_cast<D>( source );
                return true;
            }
            else if ( clamp )
            {
                dest = NumericLimits<D>::Maximum;
                return true;
            }
            else
            {
                return false;
            }
        }
        else if ( source < 0 )
        {
            if ( (i64)source >= (i64)NumericLimits<D>::Minimum )
            {
                dest = static_cast<D>( source );
                return true;
            }
            else if ( clamp )
            {
                dest = NumericLimits<D>::Minimum;
                return true;
            }
            else
            {
                return false;
            }
        }

        dest = 0;
        return true;
    }

    template< class S, class D >
    inline bool RangeCastFloat( const S source, D& dest, bool clamp = false )
    {
        if ( source > 0 )
        {
            if ( source <= NumericLimits<D>::Maximum )
            {
                dest = static_cast<D>( source );
                return true;
            }
            else if ( clamp )
            {
                dest = NumericLimits<D>::Maximum;
                return true;
            }
            else
            {
                return false;
            }
        }
        else if ( source < 0 )
        {
            if ( source >= NumericLimits<D>::Minimum )
            {
                dest = static_cast<D>( source );
                return true;
            }
            else if ( clamp )
            {
                dest = NumericLimits<D>::Minimum;
                return true;
            }
            else
            {
                return false;
            }
        }

        dest = 0;
        return true;
    }

    template< class S, class D >
    inline bool RangeCast( const S source, D& dest, bool clamp = false )
    {
        HELIUM_COMPILE_ASSERT( false );
        return false;
    }

    template <> inline bool RangeCast( const i8 source, i8& dest, bool clamp )          { dest = source; return true; }
    template <> inline bool RangeCast( const i8 source, i16& dest, bool clamp )         { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i8 source, i32& dest, bool clamp )         { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i8 source, i64& dest, bool clamp )         { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i8 source, u8& dest, bool clamp )          { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i8 source, u16& dest, bool clamp )         { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i8 source, u32& dest, bool clamp )         { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i8 source, u64& dest, bool clamp )         { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i8 source, f32& dest, bool clamp )         { return RangeCastFloat( source, dest, clamp ); }
    template <> inline bool RangeCast( const i8 source, f64& dest, bool clamp )         { return RangeCastFloat( source, dest, clamp ); }

    template <> inline bool RangeCast( const u8 source, i8& dest, bool clamp )          { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u8 source, i16& dest, bool clamp )         { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u8 source, i32& dest, bool clamp )         { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u8 source, i64& dest, bool clamp )         { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u8 source, u8& dest, bool clamp )          { dest = source; return true; }
    template <> inline bool RangeCast( const u8 source, u16& dest, bool clamp )         { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u8 source, u32& dest, bool clamp )         { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u8 source, u64& dest, bool clamp )         { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u8 source, f32& dest, bool clamp )         { return RangeCastFloat( source, dest, clamp ); }
    template <> inline bool RangeCast( const u8 source, f64& dest, bool clamp )         { return RangeCastFloat( source, dest, clamp ); }

    template <> inline bool RangeCast( const i16 source, i8& dest, bool clamp )         { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i16 source, i16& dest, bool clamp )        { dest = source; return true; }
    template <> inline bool RangeCast( const i16 source, i32& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i16 source, i64& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i16 source, u8& dest, bool clamp )         { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i16 source, u16& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i16 source, u32& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i16 source, u64& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i16 source, f32& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }
    template <> inline bool RangeCast( const i16 source, f64& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }

    template <> inline bool RangeCast( const u16 source, i8& dest, bool clamp )         { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u16 source, i16& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u16 source, i32& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u16 source, i64& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u16 source, u8& dest, bool clamp )         { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u16 source, u16& dest, bool clamp )        { dest = source; return true; }
    template <> inline bool RangeCast( const u16 source, u32& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u16 source, u64& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u16 source, f32& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }
    template <> inline bool RangeCast( const u16 source, f64& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }

    template <> inline bool RangeCast( const i32 source, i8& dest, bool clamp )         { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i32 source, i16& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i32 source, i32& dest, bool clamp )        { dest = source; return true; }
    template <> inline bool RangeCast( const i32 source, i64& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i32 source, u8& dest, bool clamp )         { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i32 source, u16& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i32 source, u32& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i32 source, u64& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i32 source, f32& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }
    template <> inline bool RangeCast( const i32 source, f64& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }

    template <> inline bool RangeCast( const u32 source, i8& dest, bool clamp )         { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u32 source, i16& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u32 source, i32& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u32 source, i64& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u32 source, u8& dest, bool clamp )         { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u32 source, u16& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u32 source, u32& dest, bool clamp )        { dest = source; return true; }
    template <> inline bool RangeCast( const u32 source, u64& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u32 source, f32& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }
    template <> inline bool RangeCast( const u32 source, f64& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }

    template <> inline bool RangeCast( const i64 source, i8& dest, bool clamp )         { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i64 source, i16& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i64 source, i32& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i64 source, i64& dest, bool clamp )        { dest = source; return true; }
    template <> inline bool RangeCast( const i64 source, u8& dest, bool clamp )         { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i64 source, u16& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i64 source, u32& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i64 source, u64& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const i64 source, f32& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }
    template <> inline bool RangeCast( const i64 source, f64& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }

    template <> inline bool RangeCast( const u64 source, i8& dest, bool clamp )         { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u64 source, i16& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u64 source, i32& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u64 source, i64& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u64 source, u8& dest, bool clamp )         { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u64 source, u16& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u64 source, u32& dest, bool clamp )        { return RangeCastInteger( source, dest, clamp ); }
    template <> inline bool RangeCast( const u64 source, u64& dest, bool clamp )        { dest = source; return true; }
    template <> inline bool RangeCast( const u64 source, f32& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }
    template <> inline bool RangeCast( const u64 source, f64& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }

    template <> inline bool RangeCast( const f32 source, i8& dest, bool clamp )         { return RangeCastFloat( source, dest, clamp ); }
    template <> inline bool RangeCast( const f32 source, i16& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }
    template <> inline bool RangeCast( const f32 source, i32& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }
    template <> inline bool RangeCast( const f32 source, i64& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }
    template <> inline bool RangeCast( const f32 source, u8& dest, bool clamp )         { return RangeCastFloat( source, dest, clamp ); }
    template <> inline bool RangeCast( const f32 source, u16& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }
    template <> inline bool RangeCast( const f32 source, u32& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }
    template <> inline bool RangeCast( const f32 source, u64& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }
    template <> inline bool RangeCast( const f32 source, f32& dest, bool clamp )        { dest = source; return true; }
    template <> inline bool RangeCast( const f32 source, f64& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }

    template <> inline bool RangeCast( const f64 source, i8& dest, bool clamp )         { return RangeCastFloat( source, dest, clamp ); }
    template <> inline bool RangeCast( const f64 source, i16& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }
    template <> inline bool RangeCast( const f64 source, i32& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }
    template <> inline bool RangeCast( const f64 source, i64& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }
    template <> inline bool RangeCast( const f64 source, u8& dest, bool clamp )         { return RangeCastFloat( source, dest, clamp ); }
    template <> inline bool RangeCast( const f64 source, u16& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }
    template <> inline bool RangeCast( const f64 source, u32& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }
    template <> inline bool RangeCast( const f64 source, u64& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }
    template <> inline bool RangeCast( const f64 source, f32& dest, bool clamp )        { return RangeCastFloat( source, dest, clamp ); }
    template <> inline bool RangeCast( const f64 source, f64& dest, bool clamp )        { dest = source; return true; }

    extern void NumericTest();
}