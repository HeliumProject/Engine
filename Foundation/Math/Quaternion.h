#pragma once

#include <cmath>
#include "Utils.h"

#include "Vector4.h"

namespace Helium
{
    class Matrix3;
    class EulerAngles;
    class AngleAxis;

    class FOUNDATION_API Quaternion
    {
    private:
        enum
        {
            QX = 0,
            QY,
            QZ,
            QW,
        };

    public:
        Vector4 values;

        const static Quaternion Zero;
        const static Quaternion Identity;

        Quaternion            () : values(0, 0, 0, 1) {}

        explicit Quaternion   (Vector4 v) : values(v) {}
        explicit Quaternion   (f32 vx, f32 vy, f32 vz, f32 vw) : values(vx, vy, vz, vw) {}

        explicit Quaternion   (const Matrix3& v);
        Quaternion&           operator= (const Matrix3& v);

        explicit Quaternion   (const Matrix4& v);
        Quaternion&           operator= (const Matrix4& v);

        explicit Quaternion   (const EulerAngles& v);
        Quaternion&           operator= (const EulerAngles& v);

        explicit Quaternion   (const AngleAxis& v);
        Quaternion&           operator= (const AngleAxis& v);

        Quaternion            operator- () const;
        Quaternion&           operator= (const Quaternion& r) { values = r.values; return *this; }
        Quaternion&           operator*= (const Quaternion& v);
        Quaternion&           operator+= (const Quaternion& v);
        Quaternion            operator* (const Quaternion& v) const;
        Quaternion            operator* ( const Vector4& rhs ) const;
        Quaternion            operator+ ( const Quaternion &rhs ) const;

        f32&                  operator[] (const u32 i) { return values[i]; }
        const f32&            operator[] (const u32 i) const { return values[i]; }

        bool                  operator== (const Quaternion& v) const { return (values == v.values); }
        bool                  operator!= (const Quaternion& v) const { return !(values == v.values); }
        bool                  Valid() { return values.Valid(); }

        // Methods
        Quaternion&           Invert ()
        {
            values[QX] *= -1;
            values[QY] *= -1;
            values[QZ] *= -1;
            return *this;
        }

        Quaternion            Inverted () const
        {
            return Quaternion(values[QX] * -1, values[QY] * -1, values[QZ] * -1, values[QW]);
        }

        Quaternion&           Normalize ()
        {
            values.Normalize();
            return *this;
        }

        Quaternion            Normalized () const
        {
            return Quaternion (values.Normalized());
        }

        Quaternion            Slerp (Quaternion& q2, f32 param);

        f32                   Dot ( const Quaternion& q2 ) const;
        bool                  Equal( const Quaternion &q2, f32 error = 0.0f ) const;

        friend FOUNDATION_API tostream& operator<<(tostream& outStream, const Quaternion& quaternion);
        friend FOUNDATION_API tistream& operator>>(tistream& inStream, Quaternion& quaternion);
    };

    inline bool Quaternion::Equal( const Quaternion& q2, f32 error ) const
    {
        Quaternion q1 ( *this );
        return (fabs( q1[QX] - q2[QX] ) <= error && fabs( q1[QY] - q2[QY] ) <= error && fabs( q1[QZ] - q2[QZ] ) <= error && fabs( q1[QW] - q2[QW] ) <= error );
    }

    inline Quaternion Quaternion::operator+ ( const Quaternion& rhs ) const
    {
        Quaternion result;

        result.values.x = values.x + rhs.values.x;
        result.values.y = values.y + rhs.values.y;
        result.values.z = values.z + rhs.values.z;
        result.values.w = values.w + rhs.values.w;

        return result;
    }

    inline Quaternion& Quaternion::operator*=(const Quaternion& v)
    {
        Quaternion  v0 (*this);
        Quaternion& result = *this;

        result[QW] = v0[QW]*v[QW] - v0[QX]*v[QX] - v0[QY]*v[QY] - v0[QZ]*v[QZ];
        result[QX] = v0[QW]*v[QX] + v[QW]*v0[QX] + v0[QY]*v[QZ] - v0[QZ]*v[QY];
        result[QY] = v0[QW]*v[QY] + v[QW]*v0[QY] + v0[QZ]*v[QX] - v0[QX]*v[QZ];
        result[QZ] = v0[QW]*v[QZ] + v[QW]*v0[QZ] + v0[QX]*v[QY] - v0[QY]*v[QX];

        return result;
    }

    inline Quaternion& Quaternion::operator+=(const Quaternion& v)
    {
        Quaternion  v0 (*this);
        Quaternion& result = *this;

        result[QW] = v0[ QW ] + v[ QW ];
        result[QX] = v0[ QX ] + v[ QX ];
        result[QY] = v0[ QY ] + v[ QY ];
        result[QZ] = v0[ QZ ] + v[ QZ ];

        return result;
    }

    inline Quaternion Quaternion::operator*(const Quaternion& v) const
    {
        Quaternion result;

        result[QW] = (*this)[QW]*v[QW] - (*this)[QX]*v[QX] - (*this)[QY]*v[QY] - (*this)[QZ]*v[QZ];
        result[QX] = (*this)[QW]*v[QX] + v[QW]*(*this)[QX] + (*this)[QY]*v[QZ] - (*this)[QZ]*v[QY];
        result[QY] = (*this)[QW]*v[QY] + v[QW]*(*this)[QY] + (*this)[QZ]*v[QX] - (*this)[QX]*v[QZ];
        result[QZ] = (*this)[QW]*v[QZ] + v[QW]*(*this)[QZ] + (*this)[QX]*v[QY] - (*this)[QY]*v[QX];

        return result;
    }

    inline Quaternion Quaternion::Slerp(Quaternion& q2, f32 param)
    {
        //
        // Quaternion Interpolation With Extra Spins, pp. 96f, 461f
        // Jack Morrison, Graphics Gems III, AP Professional
        //

        Quaternion qt;

        f32 alpha, beta;
        f32 cosom = this->values.x*q2.values.x + this->values.y*q2.values.y + this->values.z*q2.values.z + this->values.w*q2.values.w; 
        f32 slerp_epsilon = 0.00001f;

        bool flip;

        if (flip = (cosom < 0))
            cosom = -cosom;

        if ((1.0 - cosom) > slerp_epsilon)
        {
            f32 omega = acos (cosom);
            f32 sinom = sin (omega);
            alpha = (f32)(sin((1.0 - param) * omega) / sinom);
            beta = (f32)(sin(param * omega) / sinom);
        }
        else
        {
            alpha = (f32)(1.0 - param);
            beta = (f32)param;
        }

        if (flip)
            beta = -beta;

        qt.values.x = (f32) (alpha*this->values.x + beta*q2.values.x);
        qt.values.y = (f32) (alpha*this->values.y + beta*q2.values.y);
        qt.values.z = (f32) (alpha*this->values.z + beta*q2.values.z);
        qt.values.w = (f32) (alpha*this->values.w + beta*q2.values.w);

        return qt;
    }

    inline f32 Quaternion::Dot( const Quaternion& q2 ) const
    {
        return ( ( values.x * q2.values.x ) + ( values.y * q2.values.y ) + ( values.z * q2.values.z ) + ( values.w * q2.values.w ) );
    }

    inline tostream& operator<<(tostream& outStream, const Quaternion& quaternion)
    {
        outStream << quaternion.values;

        return outStream;
    }

    inline tistream& operator>>(tistream& inStream, Quaternion& quaternion)
    {
        inStream >> quaternion.values;

        return inStream;
    }

    typedef std::vector< Quaternion > V_Quaternion;
}