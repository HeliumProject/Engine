#pragma once

#include <cmath>
#include <iostream>

#include <vector>
#include <map>

#include "Utils.h"
#include "Vector2.h"

namespace Helium
{
    namespace Math
    {
        class Scale;

        class FOUNDATION_API Vector3
        {
        public:
            f32 x, y, z;

            const static Vector3 Zero;
            const static Vector3 Unit;
            const static Vector3 BasisX;
            const static Vector3 BasisY;
            const static Vector3 BasisZ;

            Vector3           () : x(0), y(0), z(0) {}
            explicit Vector3  ( const Vector2& v ) : x(v.x), y(v.y), z(0) {}
            explicit Vector3  ( f32 vx, f32 vy, f32 vz ) : x(vx), y(vy), z(vz) {}
            explicit Vector3  ( f32 val ) : x( val ), y( val ), z( val ) {}

            // FIXME lame, replacement for old SetXYZ
            inline   void     Set( f32 vx, f32 vy, f32 vz ) { x = vx; y = vy; z = vz; }

            Vector3&          operator= (const Vector3& v) { x = v.x; y = v.y; z = v.z; return *this; }
            Vector3&          operator= (const Vector2& v) { x = v.x; y = v.y; z = 0; return *this; }
            Vector3&          operator+= (const Vector3& v) { x += v.x; y += v.y; z += v.z; return *this; }
            Vector3&          operator-= (const Vector3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
            Vector3&          operator*= (const Vector3& v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
            Vector3&          operator*= (const f32 v) { x *= v; y *= v; z *= v; return *this; }
            Vector3&          operator/= (const Vector3& v) { x /= v.x; y /= v.y; z /= v.z; return *this; }
            Vector3&          operator/= (const f32 v) { x /= v; y /= v; z /= v; return *this; }

            // unary negation
            Vector3           operator- () const { return Vector3( -x, -y, -z ); }

            Vector3           operator+ (const Vector3& v) const { return Vector3 (x + v.x, y + v.y, z + v.z); }
            Vector3           operator- (const Vector3& v) const { return Vector3 (x - v.x, y - v.y, z - v.z); }
            Vector3           operator* (const Vector3& v) const { return Vector3 (x * v.x, y * v.y, z * v.z); }
            Vector3           operator* (const f32 v) const { return Vector3 (x * v, y * v, z * v); }
            Vector3           operator* (const Scale& v) const;
            Vector3           operator/ (const Vector3& v) const { return Vector3 (x / v.x, y / v.y, z / v.z); }
            Vector3           operator/ (const f32 v) const { return Vector3 (x / v, y / v, z / v); }

            f32&              operator[] (const u32 i) {  HELIUM_ASSERT(i < 3); return (&x)[i]; }
            const f32&        operator[] (const u32 i) const {  HELIUM_ASSERT(i < 3); return (&x)[i]; }

            bool              operator== (const Vector3& v) const { return (x == v.x && y == v.y && z == v.z); }
            bool              operator!= (const Vector3& v) const { return !(x == v.x && y == v.y && z == v.z); }
            bool              Equal (const Vector3& v, f32 error = 0) const;
            bool              Valid() { return IsValid(x) && IsValid(y) && IsValid(z); }

            f32               LengthSquared () const { return x * x + y * y + z * z; }
            f32               Length () const;

            Vector3&          Normalize ();
            Vector3           Normalized () const;

            f32               Dot (const Vector3& v) const; 
            Vector3           Cross (const Vector3& v) const;
            void              Clamp ( const Vector3 &min, const Vector3 &max );

            friend FOUNDATION_API tostream& operator<<(tostream& outStream, const Vector3& vector);
            friend FOUNDATION_API tistream& operator>>(tistream& inStream, Vector3& vector);
        };

        typedef std::vector< Vector3 > V_Vector3;
        typedef std::vector< V_Vector3 > VV_Vector3;

        inline bool Vector3::Equal(const Vector3& v, f32 error) const
        {
            return (fabs(x-v.x) <= error && fabs(y-v.y) <= error && fabs(z-v.z) <= error);
        }

        inline f32 Vector3::Length() const
        {
            f32 lenSqr = this->LengthSquared();

            if (lenSqr <= 0)
            {
                return 0;
            }

            return sqrt(lenSqr);
        }

        inline Vector3& Vector3::Normalize()
        { 
            f32 len = Length();

            if (len > DivisorNearZero)
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

        inline f32 Vector3::Dot(const Vector3& v) const
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

        typedef std::multimap<i32, i32> MM_i32;
        i32 FOUNDATION_API LookupPosInArray( const Vector3& pos, i32 min_key, i32 max_key, V_Vector3& pos_array, MM_i32& pos_lookup , f32 threshold);
    }
}