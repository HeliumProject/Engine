#pragma once

#include <cmath>
#include <string>
#include <vector>
#include <iostream>

#include "Utils.h"
#include "Vector3.h"

namespace Helium
{
    namespace Math
    {

        class Color4;

        class FOUNDATION_API Color3
        {
        public:
            u8 r, g, b;

            Color3            () : r(0), g(0), b(0) {}
            Color3            ( const Color3& c ) : r(c.r), g(c.g), b(c.b) {}
            Color3            ( const Vector3& v ) : r( (u8)(v.x*255.0f) ), g( (u8)(v.y*255.0f) ), b( (u8)(v.z*255.0f) ) {}
            explicit Color3   ( u8 vr, u8 vg, u8 vb ) : r(vr), g(vg), b(vb) {}
            explicit Color3   ( u8 val ) : r( val ), g( val ), b( val ) {}

            Color3&           operator= (const Color4& v);
            Color3&           operator= (const Color3& v) { r = v.r; g = v.g; b = v.b; return *this; }
            Color3&           operator= (const Vector3& v) { r = (u8)(v.x*255.0f); g = (u8)(v.y*255.0f); b = (u8)(v.z*255.0f); return *this; }
            Color3&           operator+= (const Color3& v) { r += v.r; g += v.g; b += v.b; return *this; }
            Color3&           operator-= (const Color3& v) { r -= v.r; g -= v.g; b -= v.b; return *this; }
            Color3&           operator*= (const Color3& v) { r *= v.r; g *= v.g; b *= v.b; return *this; }
            Color3&           operator*= (const u8 v) { r *= v; g *= v; b *= v; return *this; }
            Color3&           operator/= (const Color3& v) { r /= v.r; g /= v.g; b /= v.b; return *this; }
            Color3&           operator/= (const u8 v) { r /= v; g /= v; b /= v; return *this; }

            Color3            operator+ (const Color3& v) const { return Color3 (r + v.r, g + v.g, b + v.b); }
            Color3            operator- (const Color3& v) const { return Color3 (r - v.r, g - v.g, b - v.b); }
            Color3            operator* (const Color3& v) const { return Color3 (r * v.r, g * v.g, b * v.b); }
            Color3            operator* (const u8 v) const { return Color3 (r * v, g * v, b * v); }
            Color3            operator/ (const Color3& v) const { return Color3 (r / v.r, g / v.g, b / v.b); }
            Color3            operator/ (const u8 v) const { return Color3 (r / v, g / v, b / v); }

            u8&               operator[] (const u32 i) {  HELIUM_ASSERT(i < 3); return (&r)[i]; }
            const u8&         operator[] (const u32 i) const {  HELIUM_ASSERT(i < 3); return (&r)[i]; }

            bool              operator== (const Color3& v) const { return (r == v.r && g == v.g && b == v.b); }
            bool              operator!= (const Color3& v) const { return !(r == v.r && g == v.g && b == v.b); }

            operator Color4();

            void Set( u8 vr, u8 vg, u8 vb )
            {
                r = vr;
                g = vg;
                b = vb;
            }

            void Set( f32 vr, f32 vg, f32 vb )
            {
                r = (u8)( vr * 255.0f );
                g = (u8)( vg * 255.0f );
                b = (u8)( vb * 255.0f );
            }

            void Get( f32& vr, f32& vg, f32& vb ) const
            {
                vr = r / 255.0f;
                vg = g / 255.0f;
                vb = b / 255.0f;
            }

            void GetRGBA( u32& out, u8 a = 0xFF ) const
            {
                out = ( ( r << 24 ) | ( g << 16 ) | ( b << 8 ) | a );
            }

            void Clamp ( const Color3 &min, const Color3 &mar );

            friend FOUNDATION_API tostream& operator<<(tostream& outStream, const Color3& color);
            friend FOUNDATION_API tistream& operator>>(tistream& inStream, Color3& color);
        };

        typedef std::vector< Color3 > V_Color3;
        typedef std::vector< V_Color3 > VV_Color3;

        inline void Color3::Clamp( const Color3 &min, const Color3 &mar )
        {
            r = r < min.r ? min.r : ( r > mar.r ) ? mar.r : r; 
            g = g < min.g ? min.g : ( g > mar.g ) ? mar.g : g; 
            b = b < min.b ? min.b : ( b > mar.b ) ? mar.b : b; 
        }

        inline tostream& operator<<(tostream& outStream, const Color3& color)
        {
            outStream << (u16)color.r << ", " << (u16)color.g << ", " << (u16)color.b;

            return outStream;
        }

        inline tistream& operator>>(tistream& inStream, Color3& color)
        {
            u32 r = 0;
            u32 g = 0;
            u32 b = 0;
            tstring line;
            std::getline( inStream, line );

            if (3 == _stscanf( line.c_str(), TXT("%u, %u, %u"), &r, &g, &b))
            {
                color.r = (u8)r;
                color.g = (u8)g;
                color.b = (u8)b;
            }

            return inStream;
        }

        inline Color3 ComponentMin( const Color3 &v1, const Color3 &v2 )
        {
            Color3 minVec;

            minVec.r = v1.r <= v2.r ? v1.r : v2.r;
            minVec.g = v1.g <= v2.g ? v1.g : v2.g;
            minVec.b = v1.b <= v2.b ? v1.b : v2.b;

            return minVec;
        }
    }
}