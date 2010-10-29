#pragma once

#include "Color.h"
#include "Color3.h"
#include "Vector3.h"
#include "Utils.h"

#include <cmath>
#include <string>
#include <vector>
#include <iostream>

namespace Helium
{
    class HDRColor4;

    class FOUNDATION_API HDRColor3 : public Color3
    {
    public:
        f32 s; // scale, for HDR

        HDRColor3           () : s(1.0) {}
        HDRColor3           ( const Color3& c ) : Color3( c ), s(1.0) {}
        HDRColor3           ( const Vector3& v ) { FromFloat( v.x, v.y, v.z ); }
        explicit HDRColor3  ( u8 vr, u8 vg, u8 vb, f32 vs ) : Color3( vr, vg, vb ), s(vs) {}
        explicit HDRColor3  ( u8 val ) : Color3( val ), s(1.0) {}
        explicit HDRColor3  ( f32 r, f32 g, f32 b ) { FromFloat( r, g, b ); }

        HDRColor3&          operator= (const Color3& v) { r = v.r; g = v.g; b = v.b; return *this; }
        HDRColor3&          operator= (const HDRColor3& v) { r = v.r; g = v.g; b = v.b; s = v.s; return *this; }
        HDRColor3&          operator= (const HDRColor4& v);
        HDRColor3&          operator= (const Vector3& v) { FromFloat( v.x, v.y, v.z ); return *this; }
        HDRColor3&          operator+= (const HDRColor3& v) { r += v.r; g += v.g; b += v.b; return *this; }
        HDRColor3&          operator-= (const HDRColor3& v) { r -= v.r; g -= v.g; b -= v.b; return *this; }
        HDRColor3&          operator*= (const HDRColor3& v) { r *= v.r; g *= v.g; b *= v.b; return *this; }
        HDRColor3&          operator*= (const u8 v) { r *= v; g *= v; b *= v; return *this; }
        HDRColor3&          operator/= (const HDRColor3& v) { r /= v.r; g /= v.g; b /= v.b; return *this; }
        HDRColor3&          operator/= (const u8 v) { r /= v; g /= v; b /= v; return *this; }

        HDRColor3           operator+ (const HDRColor3& v) const { return HDRColor3 (r + v.r, g + v.g, b + v.b, s); }
        HDRColor3           operator- (const HDRColor3& v) const { return HDRColor3 (r - v.r, g - v.g, b - v.b, s); }
        HDRColor3           operator* (const HDRColor3& v) const { return HDRColor3 (r * v.r, g * v.g, b * v.b, s); }
        HDRColor3           operator* (const u8 v) const { return HDRColor3 (r * v, g * v, b * v, s); }
        HDRColor3           operator/ (const HDRColor3& v) const { return HDRColor3 (r / v.r, g / v.g, b / v.b, s); }
        HDRColor3           operator/ (const u8 v) const { return HDRColor3 (r / v, g / v, b / v, s); }

        u8&                 operator[] (const u32 i) {  HELIUM_ASSERT(i < 3); return (&r)[i]; }
        const u8&           operator[] (const u32 i) const {  HELIUM_ASSERT(i < 3); return (&r)[i]; }

        bool                operator== (const HDRColor3& v) const { return (r == v.r && g == v.g && b == v.b && s == v.s); }
        bool                operator!= (const HDRColor3& v) const { return !(r == v.r && g == v.g && b == v.b && s == v.s); }

        operator HDRColor4();

        void Set( u8 vr, u8 vg, u8 vb )
        {
            r = vr; g = vg; b = vb;
        }

        void Set( u8 vr, u8 vg, u8 vb, f32 vs )
        {
            r = vr; g = vg; b = vb; s = vs;
        }

        void FromFloat( f32 r, f32 g, f32 b );
        void ToFloat( f32& r, f32& g, f32& b ) const;
        void ToLinearFloat( f32& r, f32& g, f32& b ) const;
        void Clamp ( const HDRColor3 &min, const HDRColor3 &mar );

        friend FOUNDATION_API tostream& operator<<(tostream& outStream, const HDRColor3& color);
        friend FOUNDATION_API tistream& operator>>(tistream& inStream, HDRColor3& color);
    };

    typedef std::vector< HDRColor3 > V_HDRColor3;
    typedef std::vector< V_Color3 > VV_HDRColor3;

    inline void HDRColor3::FromFloat( f32 r, f32 g, f32 b )
    {
        s = (r > g)? r:g;
        s = (s > b)? s:b;
        if (s < 1.0f)
        {
            s = 1.0f;
        }
        r = r / s;
        g = g / s;
        b = b / s;

        this->r = (u8)(r * 255.0f);
        this->g = (u8)(g * 255.0f);
        this->b = (u8)(b * 255.0f);
    }

    inline void HDRColor3::ToFloat( f32& red, f32& green, f32& blue ) const
    {
        red   = f32(r)/255.f * s;
        green = f32(g)/255.f * s;
        blue  = f32(b)/255.f * s;
    }

    inline void HDRColor3::ToLinearFloat( f32& red, f32& green, f32& blue ) const
    {
        red   = SRGBToLinear(f32(r)/255.f) * s;
        green = SRGBToLinear(f32(g)/255.f) * s;
        blue  = SRGBToLinear(f32(b)/255.f) * s;
    }

    inline void HDRColor3::Clamp( const HDRColor3 &min, const HDRColor3 &mar )
    {
        r = r < min.r ? min.r : ( r > mar.r ) ? mar.r : r;
        g = g < min.g ? min.g : ( g > mar.g ) ? mar.g : g;
        b = b < min.b ? min.b : ( b > mar.b ) ? mar.b : b;
    }

    inline tostream& operator<<(tostream& outStream, const HDRColor3& color)
    {
        outStream << (u16)color.r << ", " << (u16)color.g << ", " << (u16)color.b << ", " << color.s;

        return outStream;
    }

    inline tistream& operator>>(tistream& inStream, HDRColor3& color)
    {
        u32 r = 0;
        u32 g = 0;
        u32 b = 0;
        f32 s = 0.f;

        tstring line;
        std::getline( inStream, line );
        if (4 == _stscanf( line.c_str(), TXT("%u, %u, %u, %f"), &r, &g, &b, &s))
        {
            color.r = (u8)r;
            color.g = (u8)g;
            color.b = (u8)b;
            color.s = s;
        }

        return inStream;
    }

    inline HDRColor3 ComponentMin( const HDRColor3 &v1, const HDRColor3 &v2 )
    {
        HDRColor3 minVec;

        minVec.r = v1.r <= v2.r ? v1.r : v2.r;
        minVec.g = v1.g <= v2.g ? v1.g : v2.g;
        minVec.b = v1.b <= v2.b ? v1.b : v2.b;
        minVec.s = v1.s <= v2.s ? v1.s : v2.s;

        return minVec;
    }
}
