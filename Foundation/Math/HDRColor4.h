#pragma once

#include "Color.h"
#include "Color4.h"
#include "Vector4.h"
#include "Utils.h"

#include <cmath>
#include <string>
#include <vector>
#include <iostream>

namespace Helium
{
    namespace Math
    {
        class FOUNDATION_API HDRColor4 : public Color4
        {
        public:
            f32 s; // scale, for HDR

            HDRColor4           () : s(1.0) {}
            HDRColor4           ( const Color4& c ) : Color4( c ), s(1.0) {}
            HDRColor4           ( const Vector4& v ) { FromFloat( v.x, v.y, v.z, v.w ); }
            explicit HDRColor4  ( u8 vr, u8 vg, u8 vb, u8 va, f32 vs ) : Color4( vr, vg, vb, va ), s(vs) {}
            explicit HDRColor4  ( u8 val ) : Color4( val ), s(1.0) {}
            explicit HDRColor4  ( f32 r, f32 g, f32 b, f32 a ) { FromFloat( r, g, b, a ); }

            HDRColor4&          operator= (const Color4& v) { r = v.r; g = v.g; b = v.b; a = v.a; return *this; }
            HDRColor4&          operator= (const HDRColor4& v) { r = v.r; g = v.g; b = v.b; a = v.a; s = v.s; return *this; }
            HDRColor4&          operator= (const Vector4& v) { FromFloat( v.x, v.y, v.z, v.w ); return *this; }
            HDRColor4&          operator+= (const HDRColor4& v) { r += v.r; g += v.g; b += v.b; a += v.a; return *this; }
            HDRColor4&          operator-= (const HDRColor4& v) { r -= v.r; g -= v.g; b -= v.b; a -= v.a; return *this; }
            HDRColor4&          operator*= (const HDRColor4& v) { r *= v.r; g *= v.g; b *= v.b; a *= v.a; return *this; }
            HDRColor4&          operator*= (const u8 v) { r *= v; g *= v; b *= v; a *= v; return *this; }
            HDRColor4&          operator/= (const HDRColor4& v) { r /= v.r; g /= v.g; b /= v.b; a /= v.a; return *this; }
            HDRColor4&          operator/= (const u8 v) { r /= v; g /= v; b /= v; a /= v; return *this; }

            HDRColor4           operator+ (const HDRColor4& v) const { return HDRColor4 (r + v.r, g + v.g, b + v.b, a + v.a, s); }
            HDRColor4           operator- (const HDRColor4& v) const { return HDRColor4 (r - v.r, g - v.g, b - v.b, a - v.a, s); }
            HDRColor4           operator* (const HDRColor4& v) const { return HDRColor4 (r * v.r, g * v.g, b * v.b, a * v.a, s); }
            HDRColor4           operator* (const u8 v) const { return HDRColor4 (r * v, g * v, b * v, a * v, s); }
            HDRColor4           operator/ (const HDRColor4& v) const { return HDRColor4 (r / v.r, g / v.g, b / v.b, a / v.a, s); }
            HDRColor4           operator/ (const u8 v) const { return HDRColor4 (r / v, g / v, b / v, a / v, s); }

            u8&                 operator[] (const u32 i) {  HELIUM_ASSERT(i < 4); return (&r)[i]; }
            const u8&           operator[] (const u32 i) const {  HELIUM_ASSERT(i < 4); return (&r)[i]; }

            bool                operator== (const HDRColor4& v) const { return (r == v.r && g == v.g && b == v.b && a == v.a && s == v.s); }
            bool                operator!= (const HDRColor4& v) const { return !(r == v.r && g == v.g && b == v.b && a == v.a && s == v.s); }

            void Set( u8 vr, u8 vg, u8 vb, u8 va )
            {
                r = vr; g = vg; b = vb; a = va;
            }

            void Set( u8 vr, u8 vg, u8 vb, u8 va, f32 vs )
            {
                r = vr; g = vg; b = vb; a = va; s = vs;
            }

            void FromFloat( f32 r, f32 g, f32 b, f32 a );
            void ToFloat( f32& r, f32& g, f32& b, f32& a );
            void ToLinearFloat( f32& r, f32& g, f32& b, f32& a );
            void Clamp ( const HDRColor4 &min, const HDRColor4 &mar );

            friend FOUNDATION_API tostream& operator<<(tostream& outStream, const HDRColor4& color);
            friend FOUNDATION_API tistream& operator>>(tistream& inStream, HDRColor4& color);
        };

        typedef std::vector< HDRColor4 > V_HDRColor4;
        typedef std::vector< V_Color4 > VV_HDRColor4;

        inline void HDRColor4::FromFloat( f32 r, f32 g, f32 b, f32 a )
        {
            s = (r > g)? r:g;
            s = (s > b)? s:b;
            s = (s > a)? s:a;
            if (s < 1.0f)
            {
                s = 1.0f;
            }
            r = r / s;
            g = g / s;
            b = b / s;
            a = a / s;

            this->r = (u8)(r * 255.0f);
            this->g = (u8)(g * 255.0f);
            this->b = (u8)(b * 255.0f);
            this->a = (u8)(a * 255.0f);
        }

        inline void HDRColor4::ToFloat( f32& red, f32& green, f32& blue, f32& alpha )
        {
            red   = f32(r)/255.f * s;
            green = f32(g)/255.f * s;
            blue  = f32(b)/255.f * s;
            alpha = f32(a)/255.f * s;
        }

        inline void HDRColor4::ToLinearFloat( f32& red, f32& green, f32& blue, f32& alpha )
        {
            red   = SRGBToLinear(f32(r)/255.f) * s;
            green = SRGBToLinear(f32(g)/255.f) * s;
            blue  = SRGBToLinear(f32(b)/255.f) * s;
            alpha = f32(a)/255.f * s;
        }

        inline void HDRColor4::Clamp( const HDRColor4 &min, const HDRColor4 &mar )
        {
            r = r < min.r ? min.r : ( r > mar.r ) ? mar.r : r;
            g = g < min.g ? min.g : ( g > mar.g ) ? mar.g : g;
            b = b < min.b ? min.b : ( b > mar.b ) ? mar.b : b;
            a = a < min.a ? min.a : ( a > mar.a ) ? mar.a : a;
        }

        inline tostream& operator<<(tostream& outStream, const HDRColor4& color)
        {
            outStream << (u16)color.r << ", " << (u16)color.g << ", " << (u16)color.b << ", " << (u16)color.b << ", " << color.s;

            return outStream;
        }

        inline tistream& operator>>(tistream& inStream, HDRColor4& color)
        {
            u32 r = 0;
            u32 g = 0;
            u32 b = 0;
            u32 a = 0;
            f32 s = 0.f;

            tstring line;
            std::getline( inStream, line );
            if (4 == _stscanf( line.c_str(), TXT("%u, %u, %u, &u, %f"), &r, &g, &b, &a, &s))
            {
                color.r = (u8)r;
                color.g = (u8)g;
                color.b = (u8)b;
                color.a = (u8)a;
                color.s = s;
            }

            return inStream;
        }

        inline HDRColor4 ComponentMin( const HDRColor4 &v1, const HDRColor4 &v2 )
        {
            HDRColor4 minVec;

            minVec.r = v1.r <= v2.r ? v1.r : v2.r;
            minVec.g = v1.g <= v2.g ? v1.g : v2.g;
            minVec.b = v1.b <= v2.b ? v1.b : v2.b;
            minVec.a = v1.a <= v2.a ? v1.a : v2.a;
            minVec.s = v1.s <= v2.s ? v1.s : v2.s;

            return minVec;
        }
    }
}