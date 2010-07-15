#pragma once

#include "Platform/Types.h"
#include "Platform/Assert.h"

#include "API.h"

namespace Nocturnal
{
    template< class T >
    struct NumericLimits
    {
        const static T Minimum;
        const static T Maximum;
    };

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
        NOC_COMPILE_ASSERT( false );
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