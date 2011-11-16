#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "Foundation/API.h"
#include "Foundation/Math/Common.h"
#include "Foundation/Math/FpuColor.h"
#include "Foundation/Math/FpuColor3.h"
#include "Foundation/Math/FpuVector3.h"

namespace Helium
{
    class HDRColor4;

    class HELIUM_FOUNDATION_API HDRColor3 : public Color3
    {
    public:
        float32_t s; // scale, for HDR

        HDRColor3           () : s(1.0) {}
        HDRColor3           ( const Color3& c ) : Color3( c ), s(1.0) {}
        HDRColor3           ( const Vector3& v ) { FromFloat( v.x, v.y, v.z ); }
        explicit HDRColor3  ( uint8_t vr, uint8_t vg, uint8_t vb, float32_t vs ) : Color3( vr, vg, vb ), s(vs) {}
        explicit HDRColor3  ( uint8_t val ) : Color3( val ), s(1.0) {}
        explicit HDRColor3  ( float32_t r, float32_t g, float32_t b ) { FromFloat( r, g, b ); }

        HDRColor3&          operator= (const Color3& v) { r = v.r; g = v.g; b = v.b; return *this; }
        HDRColor3&          operator= (const HDRColor3& v) { r = v.r; g = v.g; b = v.b; s = v.s; return *this; }
        HDRColor3&          operator= (const HDRColor4& v);
        HDRColor3&          operator= (const Vector3& v) { FromFloat( v.x, v.y, v.z ); return *this; }
        HDRColor3&          operator+= (const HDRColor3& v) { r += v.r; g += v.g; b += v.b; return *this; }
        HDRColor3&          operator-= (const HDRColor3& v) { r -= v.r; g -= v.g; b -= v.b; return *this; }
        HDRColor3&          operator*= (const HDRColor3& v) { r *= v.r; g *= v.g; b *= v.b; return *this; }
        HDRColor3&          operator*= (const uint8_t v) { r *= v; g *= v; b *= v; return *this; }
        HDRColor3&          operator/= (const HDRColor3& v) { r /= v.r; g /= v.g; b /= v.b; return *this; }
        HDRColor3&          operator/= (const uint8_t v) { r /= v; g /= v; b /= v; return *this; }

        HDRColor3           operator+ (const HDRColor3& v) const { return HDRColor3 (r + v.r, g + v.g, b + v.b, s); }
        HDRColor3           operator- (const HDRColor3& v) const { return HDRColor3 (r - v.r, g - v.g, b - v.b, s); }
        HDRColor3           operator* (const HDRColor3& v) const { return HDRColor3 (r * v.r, g * v.g, b * v.b, s); }
        HDRColor3           operator* (const uint8_t v) const { return HDRColor3 (r * v, g * v, b * v, s); }
        HDRColor3           operator/ (const HDRColor3& v) const { return HDRColor3 (r / v.r, g / v.g, b / v.b, s); }
        HDRColor3           operator/ (const uint8_t v) const { return HDRColor3 (r / v, g / v, b / v, s); }

        uint8_t&                 operator[] (const uint32_t i) {  HELIUM_ASSERT(i < 3); return (&r)[i]; }
        const uint8_t&           operator[] (const uint32_t i) const {  HELIUM_ASSERT(i < 3); return (&r)[i]; }

        bool                operator== (const HDRColor3& v) const { return (r == v.r && g == v.g && b == v.b && s == v.s); }
        bool                operator!= (const HDRColor3& v) const { return !(r == v.r && g == v.g && b == v.b && s == v.s); }

        operator HDRColor4();

        void Set( uint8_t vr, uint8_t vg, uint8_t vb )
        {
            r = vr; g = vg; b = vb;
        }

        void Set( uint8_t vr, uint8_t vg, uint8_t vb, float32_t vs )
        {
            r = vr; g = vg; b = vb; s = vs;
        }

        void FromFloat( float32_t r, float32_t g, float32_t b );
        void ToFloat( float32_t& r, float32_t& g, float32_t& b ) const;
        void ToLinearFloat( float32_t& r, float32_t& g, float32_t& b ) const;
        void Clamp ( const HDRColor3 &min, const HDRColor3 &mar );

        friend HELIUM_FOUNDATION_API tostream& operator<<(tostream& outStream, const HDRColor3& color);
        friend HELIUM_FOUNDATION_API tistream& operator>>(tistream& inStream, HDRColor3& color);
    };

    typedef std::vector< HDRColor3 > V_HDRColor3;
    typedef std::vector< V_Color3 > VV_HDRColor3;

    inline void HDRColor3::FromFloat( float32_t r, float32_t g, float32_t b )
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

        this->r = (uint8_t)(r * 255.0f);
        this->g = (uint8_t)(g * 255.0f);
        this->b = (uint8_t)(b * 255.0f);
    }

    inline void HDRColor3::ToFloat( float32_t& red, float32_t& green, float32_t& blue ) const
    {
        red   = float32_t(r)/255.f * s;
        green = float32_t(g)/255.f * s;
        blue  = float32_t(b)/255.f * s;
    }

    inline void HDRColor3::ToLinearFloat( float32_t& red, float32_t& green, float32_t& blue ) const
    {
        red   = SRGBToLinear(float32_t(r)/255.f) * s;
        green = SRGBToLinear(float32_t(g)/255.f) * s;
        blue  = SRGBToLinear(float32_t(b)/255.f) * s;
    }

    inline void HDRColor3::Clamp( const HDRColor3 &min, const HDRColor3 &mar )
    {
        r = r < min.r ? min.r : ( r > mar.r ) ? mar.r : r;
        g = g < min.g ? min.g : ( g > mar.g ) ? mar.g : g;
        b = b < min.b ? min.b : ( b > mar.b ) ? mar.b : b;
    }

    inline tostream& operator<<(tostream& outStream, const HDRColor3& color)
    {
        outStream << (uint16_t)color.r << ", " << (uint16_t)color.g << ", " << (uint16_t)color.b << ", " << color.s;

        return outStream;
    }

    inline tistream& operator>>(tistream& inStream, HDRColor3& color)
    {
        uint32_t r = 0;
        uint32_t g = 0;
        uint32_t b = 0;
        float32_t s = 0.f;

        tstring line;
        std::getline( inStream, line );
        if (4 == _stscanf( line.c_str(), TXT("%u, %u, %u, %f"), &r, &g, &b, &s))
        {
            color.r = (uint8_t)r;
            color.g = (uint8_t)g;
            color.b = (uint8_t)b;
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
