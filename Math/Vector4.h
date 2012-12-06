#pragma once

#include <iostream>
#include <vector>

#include "Math/API.h"
#include "Foundation/Math.h"
#include "Math/Vector3.h"

namespace Helium
{
    class HELIUM_MATH_API Matrix4;

    class HELIUM_MATH_API Vector4
    {
    public:
        float32_t x, y, z, w;

        const static Vector4 Zero;
        const static Vector4 Point;
        const static Vector4 BasisX;
        const static Vector4 BasisY;
        const static Vector4 BasisZ;
        const static Vector4 BasisW;

        Vector4           () : x(0), y(0), z(0), w(0) {}
        explicit Vector4  ( const Vector3& v ) : x(v.x), y(v.y), z(v.z), w(0) {}
        explicit Vector4  ( float32_t vx, float32_t vy, float32_t vz, float32_t vw ) : x(vx), y(vy), z(vz), w(vw) {}
        explicit Vector4  ( float32_t val ) : x( val ), y( val ), z( val ), w( val ) {}

        // lame, people just called SetXYZW too much in the old lib, this is a concession to that
        inline void       Set( float32_t vx, float32_t vy, float32_t vz, float32_t vw ) { x = vx; y = vy; z = vz; w = vw; }

        Vector4&          operator= (const Vector4& v) { x = v.x; y = v.y; z = v.z; w = v.w; return *this; }
        Vector4&          operator= (const Vector3& v) { x = v.x; y = v.y; z = v.z; return *this; }
        Vector4&          operator+= (const Vector4& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
        Vector4&          operator-= (const Vector4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
        Vector4&          operator*= (const Vector4& v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; return *this; }
        Vector4&          operator*= (const float32_t v) { x *= v; y *= v; z *= v; w *= v; return *this; }
        Vector4&          operator/= (const Vector4& v) { x /= v.x; y /= v.y; z /= v.z; w /= v.w; return *this; }
        Vector4&          operator/= (const float32_t v) { x /= v; y /= v; z /= v; w /= v; return *this; }

        Vector4           operator+ (const Vector4& v) const { return Vector4 (x + v.x, y + v.y, z + v.z, w + v.w); }
        Vector4           operator- (const Vector4& v) const { return Vector4 (x - v.x, y - v.y, z - v.z, w - v.w); }
        Vector4           operator* (const Vector4& v) const { return Vector4 (x * v.x, y * v.y, z * v.z, w * v.w); }
        Vector4           operator* (const float32_t v) const { return Vector4 (x * v, y * v, z * v, w * v); }
        Vector4           operator/ (const Vector4& v) const { return Vector4 (x / v.x, y / v.y, z / v.z, w / v.w); }
        Vector4           operator/ (const float32_t v) const { return Vector4 (x / v, y / v, z / v, w / v); }

        // unary negation
        Vector4           operator- () const { return Vector4( -x, -y, -z, -w ); }

        float32_t&              operator[] (const uint32_t i) {  HELIUM_ASSERT(i < 4); return (&x)[i]; }
        const float32_t&        operator[] (const uint32_t i) const {  HELIUM_ASSERT(i < 4); return (&x)[i]; }

        bool              operator== (const Vector4& v) const { return (x == v.x && y == v.y && z == v.z && w == v.w); }
        bool              operator!= (const Vector4& v) const { return !(x == v.x && y == v.y && z == v.z && w == v.w); }
        bool              Equal (const Vector4& v, float32_t error = 0) const;
        bool              Finite() { return IsFinite(x) && IsFinite(y) && IsFinite(z) && IsFinite(w); }

        float32_t               LengthSquared () const { return x * x + y * y + z * z + w * w; }
        float32_t               Length () const;

        Vector4&          Normalize ();
        Vector4           Normalized () const;

        float32_t               Dot (const Vector4& other) const; 
        Vector4           Cross (const Vector4& v) const;

        void              Transform (const Matrix4& m);

        friend HELIUM_MATH_API tostream& operator<<(tostream& outStream, const Vector4& vector);
        friend HELIUM_MATH_API tistream& operator>>(tistream& inStream, Vector4& vector);
    };

    typedef std::vector< Vector4 > V_Vector4;
    typedef std::vector< V_Vector4 > VV_Vector4;

    inline bool Vector4::Equal(const Vector4& v, float32_t error) const
    {
        return (fabs(x - v.x) <= error && fabs(y - v.y) <= error && fabs(z - v.z) <= error && fabs(w - v.w) <= error);
    }

    inline float32_t Vector4::Length() const
    {
        float32_t lenSqr = this->LengthSquared();

        if (lenSqr <= 0)
            return 0;

        return sqrt((float32_t)lenSqr);
    }

    inline Vector4& Vector4::Normalize() 
    { 
        float32_t len = this->Length();

        if (len > HELIUM_DIVISOR_NEAR_ZERO)
        {
            return *this /= len;
        }
        else
        {
            return *this = Vector4 (0, 0, 0, 0);
        }
    }

    inline Vector4 Vector4::Normalized() const 
    {
        Vector4 result = *this;
        return result.Normalize();
    }

    inline float32_t Vector4::Dot(const Vector4& v) const
    {
        return (x * v.x + y * v.y + z * v.z + w * v.w);
    }

    inline Vector4 Vector4::Cross(const Vector4& v) const
    {
        return Vector4 (y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x, 0);
    }

    inline tostream& operator<<(tostream& outStream, const Vector4& vector)
    {
        outStream << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w;

        return outStream;
    }

    inline tistream& operator>>(tistream& inStream, Vector4& vector)
    {
        inStream >> vector.x;
        inStream.ignore();

        inStream >> vector.y;
        inStream.ignore();

        inStream >> vector.z;
        inStream.ignore();

        inStream >> vector.w;

        return inStream;
    }
}