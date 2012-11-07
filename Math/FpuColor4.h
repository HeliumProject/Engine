#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "Math/API.h"
#include "Foundation/Math.h"
#include "Math/FpuColor3.h"

namespace Helium
{
	class HELIUM_MATH_API Color4
	{
	public:
		uint8_t r, g, b, a;

		Color4            () : r(0), g(0), b(0), a(255) {}
		explicit Color4   ( const Color3& c) : r( c.r ), g( c.g ), b( c.b ), a( 255 ) {}
		explicit Color4   ( uint8_t r, uint8_t g, uint8_t b, uint8_t a ) : r( r ), g( g ), b( b ), a( a ) {}
		explicit Color4   ( uint8_t val ) : r( val ), g( val ), b( val ), a( val ) {}

		Color4&           operator= (const Color4& v) { r = v.r; g = v.g; b = v.b; a = v.a; return *this; }
		Color4&           operator= (const Color3& v) { r = v.r; g = v.g; b = v.b; a = 255; return *this; }
		Color4&           operator+= (const Color4& v) { r += v.r; g += v.g; b += v.b; a += v.a; return *this; }
		Color4&           operator-= (const Color4& v) { r -= v.r; g -= v.g; b -= v.b; a -= v.a; return *this; }
		Color4&           operator*= (const Color4& v) { r *= v.r; g *= v.g; b *= v.b; a *= v.a; return *this; }
		Color4&           operator*= (const uint8_t v) { r *= v; g *= v; b *= v; a *= v; return *this; }
		Color4&           operator/= (const Color4& v) { r /= v.r; g /= v.g; b /= v.b; a /= v.a; return *this; }
		Color4&           operator/= (const uint8_t v) { r /= v; g /= v; b /= v; a /= v; return *this; }

		Color4            operator+ (const Color4& v) const { return Color4 (r + v.r, g + v.g, b + v.b, a + v.a); }
		Color4            operator- (const Color4& v) const { return Color4 (r - v.r, g - v.g, b - v.b, a - v.a); }
		Color4            operator* (const Color4& v) const { return Color4 (r * v.r, g * v.g, b * v.b, a * v.a); }
		Color4            operator* (const uint8_t v) const { return Color4 (r * v, g * v, b * v, a * v); }
		Color4            operator/ (const Color4& v) const { return Color4 (r / v.r, g / v.g, b / v.b, a / v.a); }
		Color4            operator/ (const uint8_t v) const { return Color4 (r / v, g / v, b / v, a / v); }

		Color4            operator- () const { return Color4( -r, -g, -b, -a ); }

		uint8_t&               operator[] (const uint32_t i) {  HELIUM_ASSERT(i < 4); return (&r)[i]; }
		const uint8_t&         operator[] (const uint32_t i) const {  HELIUM_ASSERT(i < 4); return (&r)[i]; }

		bool              operator== (const Color4& v) const { return (r == v.r && g == v.g && b == v.b && a == v.a); }
		bool              operator!= (const Color4& v) const { return !(r == v.r && g == v.g && b == v.b && a == v.a); }

		void Set( uint8_t vx, uint8_t vy, uint8_t vz, uint8_t vw )
		{
			r = vx;
			g = vy;
			b = vz;
			a = vw;
		}

		void Set( float32_t vr, float32_t vg, float32_t vb, float32_t va )
		{
			r = (uint8_t)( vr * 255.0f );
			g = (uint8_t)( vg * 255.0f );
			b = (uint8_t)( vb * 255.0f );
			a = (uint8_t)( va * 255.0f );
		}

		void Get( float32_t& vr, float32_t& vg, float32_t& vb, float32_t& va ) const
		{
			vr = r / 255.0f;
			vg = g / 255.0f;
			vb = b / 255.0f;
			va = a / 255.0f;
		}

		friend HELIUM_MATH_API tostream& operator<<(tostream& outStream, const Color4& color);
		friend HELIUM_MATH_API tistream& operator>>(tistream& inStream, Color4& color);
	};

	typedef std::vector< Color4 > V_Color4;
	typedef std::vector< V_Color4 > VV_Color4;

	inline tostream& operator<<(tostream& outStream, const Color4& color)
	{
		outStream << (uint16_t)color.r << ", " << (uint16_t)color.g << ", " << (uint16_t)color.b << ", " << (uint16_t)color.a;

		return outStream;
	}

	inline tistream& operator>>(tistream& inStream, Color4& color)
	{
		uint16_t r = 0;
		uint16_t g = 0;
		uint16_t b = 0;
		uint16_t a = 0;
		tchar_t delim;
		inStream >> r >> delim >> g >> delim >> b >> delim >> a;

		color.r = (uint8_t)r;
		color.g = (uint8_t)g;
		color.b = (uint8_t)b;
		color.a = (uint8_t)a;

		return inStream;
	}
}