#pragma once

#include <iostream>
#include <vector>
#include <map>

#include "Math/API.h"
#include "Math/Common.h"
#include "Math/FpuVector2.h"

namespace Helium
{
    class Scale;

    class MATH_API Vector3
    {
    public:
        float32_t x, y, z;

        const static Vector3 Zero;
        const static Vector3 Unit;
        const static Vector3 BasisX;
        const static Vector3 BasisY;
        const static Vector3 BasisZ;

        Vector3           () : x(0), y(0), z(0) {}
        explicit Vector3  ( const Vector2& v ) : x(v.x), y(v.y), z(0) {}
        explicit Vector3  ( float32_t vx, float32_t vy, float32_t vz ) : x(vx), y(vy), z(vz) {}
        explicit Vector3  ( float32_t val ) : x( val ), y( val ), z( val ) {}

        // FIXME lame, replacement for old SetXYZ
        inline   void     Set( float32_t vx, float32_t vy, float32_t vz ) { x = vx; y = vy; z = vz; }

        Vector3&          operator= (const Vector3& v) { x = v.x; y = v.y; z = v.z; return *this; }
        Vector3&          operator= (const Vector2& v) { x = v.x; y = v.y; z = 0; return *this; }
        Vector3&          operator+= (const Vector3& v) { x += v.x; y += v.y; z += v.z; return *this; }
        Vector3&          operator-= (const Vector3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
        Vector3&          operator*= (const Vector3& v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
        Vector3&          operator*= (const float32_t v) { x *= v; y *= v; z *= v; return *this; }
        Vector3&          operator/= (const Vector3& v) { x /= v.x; y /= v.y; z /= v.z; return *this; }
        Vector3&          operator/= (const float32_t v) { x /= v; y /= v; z /= v; return *this; }

        // unary negation
        Vector3           operator- () const { return Vector3( -x, -y, -z ); }

        Vector3           operator+ (const Vector3& v) const { return Vector3 (x + v.x, y + v.y, z + v.z); }
        Vector3           operator- (const Vector3& v) const { return Vector3 (x - v.x, y - v.y, z - v.z); }
        Vector3           operator* (const Vector3& v) const { return Vector3 (x * v.x, y * v.y, z * v.z); }
        Vector3           operator* (const float32_t v) const { return Vector3 (x * v, y * v, z * v); }
        Vector3           operator* (const Scale& v) const;
        Vector3           operator/ (const Vector3& v) const { return Vector3 (x / v.x, y / v.y, z / v.z); }
        Vector3           operator/ (const float32_t v) const { return Vector3 (x / v, y / v, z / v); }

        float32_t&              operator[] (const uint32_t i) {  HELIUM_ASSERT(i < 3); return (&x)[i]; }
        const float32_t&        operator[] (const uint32_t i) const {  HELIUM_ASSERT(i < 3); return (&x)[i]; }

        bool              operator== (const Vector3& v) const { return (x == v.x && y == v.y && z == v.z); }
        bool              operator!= (const Vector3& v) const { return !(x == v.x && y == v.y && z == v.z); }
        bool              Equal (const Vector3& v, float32_t error = 0) const;
        bool              Finite() { return IsFinite(x) && IsFinite(y) && IsFinite(z); }

        float32_t               LengthSquared () const { return x * x + y * y + z * z; }
        float32_t               Length () const;

        Vector3&          Normalize ();
        Vector3           Normalized () const;

        float32_t               Dot (const Vector3& v) const; 
        Vector3           Cross (const Vector3& v) const;
        void              Clamp ( const Vector3 &min, const Vector3 &max );

        friend MATH_API tostream& operator<<(tostream& outStream, const Vector3& vector);
        friend MATH_API tistream& operator>>(tistream& inStream, Vector3& vector);
    };

    typedef std::vector< Vector3 > V_Vector3;
    typedef std::vector< V_Vector3 > VV_Vector3;

    inline bool Vector3::Equal(const Vector3& v, float32_t error) const
    {
        return (fabs(x-v.x) <= error && fabs(y-v.y) <= error && fabs(z-v.z) <= error);
    }

    inline float32_t Vector3::Length() const
    {
        float32_t lenSqr = this->LengthSquared();

        if (lenSqr <= 0)
        {
            return 0;
        }

        return sqrt(lenSqr);
    }

    inline Vector3& Vector3::Normalize()
    { 
        float32_t len = Length();

        if (len > HELIUM_DIVISOR_NEAR_ZERO)
        {
            return *this /= len; 
        }
        else
        {
            return *this = Vector3 (0, 0, 0);
        }
    }

    inline Vector3 Vector3::Normalized() const
    {
        Vector3 result = *this;
        return result.Normalize();
    }

    inline float32_t Vector3::Dot(const Vector3& v) const
    {
        return (x * v.x + y * v.y + z * v.z);
    }

    inline Vector3 Vector3::Cross(const Vector3& v) const
    {
        return Vector3 (y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
    }

    inline void Vector3::Clamp( const Vector3 &min, const Vector3 &max )
    {
        x = x < min.x ? min.x : ( x > max.x ) ? max.x : x; 
        y = y < min.y ? min.y : ( y > max.y ) ? max.y : y; 
        z = z < min.z ? min.z : ( z > max.z ) ? max.z : z; 
    }

    inline tostream& operator<<(tostream& outStream, const Vector3& vector)
    {
        outStream << vector.x << ", " << vector.y << ", " << vector.z;

        return outStream;
    }

    inline tistream& operator>>(tistream& inStream, Vector3& vector)
    {
        inStream >> vector.x;
        inStream.ignore();

        inStream >> vector.y;
        inStream.ignore();

        inStream >> vector.z;

        return inStream;
    }

    inline Vector3 ComponentMax( const Vector3 &v1, const Vector3 &v2 )
    {
        Vector3 maxVec;

        maxVec.x = v1.x >= v2.x ? v1.x : v2.x;
        maxVec.y = v1.y >= v2.y ? v1.y : v2.y;
        maxVec.z = v1.z >= v2.z ? v1.z : v2.z;

        return maxVec;
    }

    inline Vector3 ComponentMin( const Vector3 &v1, const Vector3 &v2 )
    {
        Vector3 minVec;

        minVec.x = v1.x <= v2.x ? v1.x : v2.x;
        minVec.y = v1.y <= v2.y ? v1.y : v2.y;
        minVec.z = v1.z <= v2.z ? v1.z : v2.z;

        return minVec;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  - performs a fast lookup (using a multi-map) of the specified position vector in the position array.
    //  - the purpose of the min and max keys (versus just a single key) is to avoid missing possible
    //     matches at the edge conditions because the keys are converted to ints from floats.
    //  - returns the index of the position vector or -1 if it was not found
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////

    typedef std::multimap<int32_t, int32_t> MM_i32;
    int32_t MATH_API LookupPosInArray( const Vector3& pos, int32_t min_key, int32_t max_key, V_Vector3& pos_array, MM_i32& pos_lookup , float32_t threshold);
}