#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "Math/API.h"
#include "Foundation/Math.h"
#include "Math/FpuColor.h"
#include "Math/FpuColor4.h"
#include "Math/FpuVector4.h"

namespace Helium
{
	class HELIUM_MATH_API HDRColor4 : public Color4
	{
	public:
		float32_t s; // scale, for HDR

		HDRColor4           () : s(1.0) {}
		HDRColor4           ( const Color4& c ) : Color4( c ), s(1.0) {}
		HDRColor4           ( const Vector4& v ) { FromFloat( v.x, v.y, v.z, v.w ); }
		explicit HDRColor4  ( uint8_t vr, uint8_t vg, uint8_t vb, uint8_t va, float32_t vs ) : Color4( vr, vg, vb, va ), s(vs) {}
		explicit HDRColor4  ( uint8_t val ) : Color4( val ), s(1.0) {}
		explicit HDRColor4  ( float32_t r, float32_t g, float32_t b, float32_t a ) { FromFloat( r, g, b, a ); }

		HDRColor4&          operator= (const Color4& v) { r = v.r; g = v.g; b = v.b; a = v.a; return *this; }
		HDRColor4&          operator= (const HDRColor4& v) { r = v.r; g = v.g; b = v.b; a = v.a; s = v.s; return *this; }
		HDRColor4&          operator= (const Vector4& v) { FromFloat( v.x, v.y, v.z, v.w ); return *this; }
		HDRColor4&          operator+= (const HDRColor4& v) { r += v.r; g += v.g; b += v.b; a += v.a; return *this; }
		HDRColor4&          operator-= (const HDRColor4& v) { r -= v.r; g -= v.g; b -= v.b; a -= v.a; return *this; }
		HDRColor4&          operator*= (const HDRColor4& v) { r *= v.r; g *= v.g; b *= v.b; a *= v.a; return *this; }
		HDRColor4&          operator*= (const uint8_t v) { r *= v; g *= v; b *= v; a *= v; return *this; }
		HDRColor4&          operator/= (const HDRColor4& v) { r /= v.r; g /= v.g; b /= v.b; a /= v.a; return *this; }
		HDRColor4&          operator/= (const uint8_t v) { r /= v; g /= v; b /= v; a /= v; return *this; }

		HDRColor4           operator+ (const HDRColor4& v) const { return HDRColor4 (r + v.r, g + v.g, b + v.b, a + v.a, s); }
		HDRColor4           operator- (const HDRColor4& v) const { return HDRColor4 (r - v.r, g - v.g, b - v.b, a - v.a, s); }
		HDRColor4           operator* (const HDRColor4& v) const { return HDRColor4 (r * v.r, g * v.g, b * v.b, a * v.a, s); }
		HDRColor4           operator* (const uint8_t v) const { return HDRColor4 (r * v, g * v, b * v, a * v, s); }
		HDRColor4           operator/ (const HDRColor4& v) const { return HDRColor4 (r / v.r, g / v.g, b / v.b, a / v.a, s); }
		HDRColor4           operator/ (const uint8_t v) const { return HDRColor4 (r / v, g / v, b / v, a / v, s); }

		uint8_t&                 operator[] (const uint32_t i) {  HELIUM_ASSERT(i < 4); return (&r)[i]; }
		const uint8_t&           operator[] (const uint32_t i) const {  HELIUM_ASSERT(i < 4); return (&r)[i]; }

		bool                operator== (const HDRColor4& v) const { return (r == v.r && g == v.g && b == v.b && a == v.a && s == v.s); }
		bool                operator!= (const HDRColor4& v) const { return !(r == v.r && g == v.g && b == v.b && a == v.a && s == v.s); }

		void Set( uint8_t vr, uint8_t vg, uint8_t vb, uint8_t va )
		{
			r = vr; g = vg; b = vb; a = va;
		}

		void Set( uint8_t vr, uint8_t vg, uint8_t vb, uint8_t va, float32_t vs )
		{
			r = vr; g = vg; b = vb; a = va; s = vs;
		}

		void FromFloat( float32_t r, float32_t g, float32_t b, float32_t a );
		void ToFloat( float32_t& r, float32_t& g, float32_t& b, float32_t& a );
		void ToLinearFloat( float32_t& r, float32_t& g, float32_t& b, float32_t& a );
		void Clamp ( const HDRColor4 &min, const HDRColor4 &mar );

		friend HELIUM_MATH_API tostream& operator<<(tostream& outStream, const HDRColor4& color);
		friend HELIUM_MATH_API tistream& operator>>(tistream& inStream, HDRColor4& color);
	};

	typedef std::vector< HDRColor4 > V_HDRColor4;
	typedef std::vector< V_Color4 > VV_HDRColor4;

	inline void HDRColor4::FromFloat( float32_t r, float32_t g, float32_t b, float32_t a )
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

		this->r = (uint8_t)(r * 255.0f);
		this->g = (uint8_t)(g * 255.0f);
		this->b = (uint8_t)(b * 255.0f);
		this->a = (uint8_t)(a * 255.0f);
	}

	inline void HDRColor4::ToFloat( float32_t& red, float32_t& green, float32_t& blue, float32_t& alpha )
	{
		red   = float32_t(r)/255.f * s;
		green = float32_t(g)/255.f * s;
		blue  = float32_t(b)/255.f * s;
		alpha = float32_t(a)/255.f * s;
	}

	inline void HDRColor4::ToLinearFloat( float32_t& red, float32_t& green, float32_t& blue, float32_t& alpha )
	{
		red   = SRGBToLinear(float32_t(r)/255.f) * s;
		green = SRGBToLinear(float32_t(g)/255.f) * s;
		blue  = SRGBToLinear(float32_t(b)/255.f) * s;
		alpha = float32_t(a)/255.f * s;
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
		outStream << (uint16_t)color.r << ", " << (uint16_t)color.g << ", " << (uint16_t)color.b << ", " << (uint16_t)color.b << ", " << color.s;

		return outStream;
	}

	inline tistream& operator>>(tistream& inStream, HDRColor4& color)
	{
		uint16_t r = 0;
		uint16_t g = 0;
		uint16_t b = 0;
		uint16_t a = 0;
		float32_t s = 0.f;
		tchar_t delim;
		inStream >> r >> delim >> g >> delim >> b >> delim >> a >> delim >> s;

		color.r = (uint8_t)r;
		color.g = (uint8_t)g;
		color.b = (uint8_t)b;
		color.a = (uint8_t)a;
		color.s = s;

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
