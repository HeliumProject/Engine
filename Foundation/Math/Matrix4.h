#pragma once

#include "Platform/Math/MathCommon.h"
#include <vector>
#include <iostream>

#include "Utils.h"
#include "Vector4.h"
#include "Scale.h"
#include "Shear.h"

//
// Row Major Transformation Matrix
//
namespace Helium
{
    class Matrix3;
    class EulerAngles;
    class AngleAxis;

    class FOUNDATION_API Matrix4
    {
    public: 
        union
        {
            float32_t array1d[16];
            float32_t array2d[4][4];
            struct
            {
                Vector4 x;
                Vector4 y;
                Vector4 z;
                Vector4 t;
            };
            struct
            {
                float32_t xx, xy, xz, xw;
                float32_t yx, yy, yz, yw;
                float32_t zx, zy, zz, zw;
                float32_t wx, wy, wz, ww;
            };
        };

        const static Matrix4 Identity;
        const static Matrix4 Zero;

        Matrix4               ()
            : x(1, 0, 0, 0)
            , y(0, 1, 0, 0)
            , z(0, 0, 1, 0)
            , t(0, 0, 0, 1)
        {

        }

        explicit Matrix4      (const Vector4& vx, const Vector4& vy, const Vector4& vz, const Vector4& vt)
            : x(vx)
            , y(vy)
            , z(vz)
            , t(vt)
        {

        }

        explicit Matrix4      (float32_t vxx, float32_t vxy, float32_t vxz, float32_t vxw, float32_t vyx, float32_t vyy, float32_t vyz, float32_t vyw, float32_t vzx, float32_t vzy, float32_t vzz, float32_t vzw, float32_t vwx, float32_t vwy, float32_t vwz, float32_t vww)
            : xx(vxx), xy(vxy), xz(vxz), xw(vxw)
            , yx(vyx), yy(vyy), yz(vyz), yw(vyw)
            , zx(vzx), zy(vzy), zz(vzz), zw(vzw)
            , wx(vwx), wy(vwy), wz(vwz), ww(vww)
        {

        }

        explicit Matrix4      (const Matrix3& v);
        Matrix4&              operator= (const Matrix3& v);

        explicit Matrix4      (const EulerAngles& v);
        Matrix4&              operator= (const EulerAngles& v);

        explicit Matrix4      (const AngleAxis& v);
        Matrix4&              operator= (const AngleAxis& v);

        explicit Matrix4      (const Scale& v);
        Matrix4&              operator= (const Scale& v);

        explicit Matrix4      (const Shear& v);
        Matrix4&              operator= (const Shear& v);

        explicit Matrix4      (const Vector3& v)
            : x(1, 0, 0, 0)
            , y(0, 1, 0, 0)
            , z(0, 0, 1, 0)
        {
            t.x = v.x;
            t.y = v.y;
            t.z = v.z;
            t.w = 1;
        }

        explicit Matrix4      (const Vector4& v)
            : x(1, 0, 0, 0)
            , y(0, 1, 0, 0)
            , z(0, 0, 1, 0)
        {
            t.x = v.x;
            t.y = v.y;
            t.z = v.z;
            t.w = v.w;
        }

        Matrix4&              operator= (const Matrix4& v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
            t = v.t;
            return *this;
        }

        Matrix4&              operator+= (const Matrix4& v);
        Matrix4               operator+ (const Matrix4& v) const;
        Matrix4&              operator*= (const Matrix4& v);
        Matrix4               operator* (const Matrix4& v) const;
        Vector4               operator* (const Vector4& v) const;

        Matrix4&              operator*= (const float32_t v)
        {
            x *= v;
            y *= v;
            z *= v;
            t *= v;
            return *this;
        }

        Matrix4               operator* (const float32_t v) const
        {
            return Matrix4 (x * v, y * v, z * v, t * v);
        }

        Vector4&              operator[] (const uint32_t i)
        {
            HELIUM_ASSERT(i < 4);
            return (&x)[i];
        }

        const Vector4&        operator[] (const uint32_t i) const
        {
            HELIUM_ASSERT(i < 4); return (&x)[i];
        }

        float32_t&                  operator()(const uint32_t i, const uint32_t j)
        {
            return (array2d[j][i]);
        }

        const float32_t&            operator()(const uint32_t i, const uint32_t j) const
        {
            return (array2d[j][i]);
        }

        bool                  operator== (const Matrix4& v) const
        {
            return (x == v.x && y == v.y && z == v.z && t == v.t);
        }

        bool                  operator!= (const Matrix4& v) const
        {
            return !(x == v.x && y == v.y && z == v.z && t == v.t);
        }

        bool                  Valid()
        {
            return x.Valid() && y.Valid() && z.Valid() && t.Valid();
        }

        float32_t                   Determinant() const;

        Matrix4&              Invert ();
        Matrix4               Inverted () const;

        Matrix4&              AffineInvert();
        Matrix4               AffineInverted() const;

        Matrix4&              Transpose ();
        Matrix4               Transposed () const;

        Matrix4&              Normalize ();
        Matrix4               Normalized () const;

        const Vector4&        GetBasis(Axis axis) const;
        Matrix4&              SetBasis(Axis axis, const Vector4& v);
        Matrix4&              Orthogonalize (Axis first = SingleAxes::Y, Axis second = SingleAxes::X, Axis third = SingleAxes::Z);

        void                  Transform(Vector4& v) const;
        void                  Transform(Vector3& v, float32_t w) const;
        void                  TransformVertex (Vector3& v) const;
        void                  TransformNormal (Vector3& n) const;

        void                  Decompose (Scale& scale, Matrix3& rotate, Vector3& translate) const;
        void                  Decompose (Scale& scale, Shear& shear, Matrix3& rotate, Vector3& translate) const;

        void                  Decompose (Scale& scale, EulerAngles& rotate, Vector3& translate) const;
        void                  Decompose (Scale& scale, Shear& shear, EulerAngles& rotate, Vector3& translate) const;

        static Matrix4        RotateX (float32_t theta);
        static Matrix4        RotateY (float32_t theta);
        static Matrix4        RotateZ (float32_t theta);

        Matrix4&              RowScale( const Vector4& scaleVector );

        friend FOUNDATION_API tostream& operator<<(tostream& outStream, const Matrix4& vector);
        friend FOUNDATION_API tistream& operator>>(tistream& inStream, Matrix4& vector);
        friend class FOUNDATION_API Matrix3;
    };

    typedef std::vector< Matrix4 >   V_Matrix4;
    typedef std::vector< V_Matrix4 > VV_Matrix4;

    inline Matrix4& Matrix4::operator+=(const Matrix4& b)
    {
        Matrix4 a (*this);
        Matrix4 &result = *this;
        result = Matrix4::Zero;

        for (unsigned row=0; row<4; row++)
            for (unsigned col=0; col<4; col++)
                result[row][col] = a[row][col]+b[row][col];

        return result;
    }

    inline Matrix4 Matrix4::operator+(const Matrix4& b) const
    {
        Matrix4 a (*this);
        Matrix4 result = Matrix4::Zero;

        for (unsigned row=0; row<4; row++)
            for (unsigned col=0; col<4; col++)
                result[row][col] = a[row][col]+b[row][col];

        return result;
    }

    inline Matrix4& Matrix4::operator*=(const Matrix4& b)
    {
        Matrix4 a (*this);
        Matrix4 &result = *this;
        result = Matrix4::Zero;

        for (unsigned row=0; row<4; row++)
            for (unsigned col=0; col<4; col++)
                for (unsigned mid=0; mid<4; mid++)
                    result[row][col] += a[row][mid]*b[mid][col];

        return result;
    }

    inline Matrix4 Matrix4::operator*(const Matrix4& b) const
    {
        Matrix4 a (*this);
        Matrix4 result = Matrix4::Zero;

        for (unsigned row=0; row<4; row++)
            for (unsigned col=0; col<4; col++)
                for (unsigned mid=0; mid<4; mid++)
                    result[row][col] += a[row][mid]*b[mid][col];

        return result;
    }

    inline Vector4 Matrix4::operator*(const Vector4& v) const
    {
        const Matrix4& m (*this);

        return Vector4(
            (m[0][0]*v[0]) + (m[1][0]*v[1]) + (m[2][0]*v[2]) + (m[3][0]*v[3]),
            (m[0][1]*v[0]) + (m[1][1]*v[1]) + (m[2][1]*v[2]) + (m[3][1]*v[3]),
            (m[0][2]*v[0]) + (m[1][2]*v[1]) + (m[2][2]*v[2]) + (m[3][2]*v[3]),
            (m[0][3]*v[0]) + (m[1][3]*v[1]) + (m[2][3]*v[2]) + (m[3][3]*v[3]));
    }

    inline float32_t Matrix4::Determinant() const
    {
        const Matrix4 &m = *this;

        auto float32_t det1 = ((m[1][1] * m[2][2]) - (m[1][2] * m[2][1]));
        auto float32_t det2 = ((m[0][2] * m[2][1]) - (m[0][1] * m[2][2]));
        auto float32_t det3 = ((m[0][1] * m[1][2]) - (m[0][2] * m[1][1]));

        return (m[0][0] * det1) + (m[1][0] * det2) + (m[2][0] * det3);
    }

    inline Matrix4& Matrix4::Invert()
    {
        Matrix4 &a = *this;
        Matrix4 result;

        float32_t a0 = a.array1d[ 0]*a.array1d[ 5] - a.array1d[ 1]*a.array1d[ 4];
        float32_t a1 = a.array1d[ 0]*a.array1d[ 6] - a.array1d[ 2]*a.array1d[ 4];
        float32_t a2 = a.array1d[ 0]*a.array1d[ 7] - a.array1d[ 3]*a.array1d[ 4];
        float32_t a3 = a.array1d[ 1]*a.array1d[ 6] - a.array1d[ 2]*a.array1d[ 5];
        float32_t a4 = a.array1d[ 1]*a.array1d[ 7] - a.array1d[ 3]*a.array1d[ 5];
        float32_t a5 = a.array1d[ 2]*a.array1d[ 7] - a.array1d[ 3]*a.array1d[ 6];
        float32_t b0 = a.array1d[ 8]*a.array1d[13] - a.array1d[ 9]*a.array1d[12];
        float32_t b1 = a.array1d[ 8]*a.array1d[14] - a.array1d[10]*a.array1d[12];
        float32_t b2 = a.array1d[ 8]*a.array1d[15] - a.array1d[11]*a.array1d[12];
        float32_t b3 = a.array1d[ 9]*a.array1d[14] - a.array1d[10]*a.array1d[13];
        float32_t b4 = a.array1d[ 9]*a.array1d[15] - a.array1d[11]*a.array1d[13];
        float32_t b5 = a.array1d[10]*a.array1d[15] - a.array1d[11]*a.array1d[14];

        float32_t d = a0*b5-a1*b4+a2*b3+a3*b2-a4*b1+a5*b0;
        if (fabs(d) <= 0.f)
        {
            return *this = Zero;
        }

        result[0][0] = + a.array1d[ 5]*b5 - a.array1d[ 6]*b4 + a.array1d[ 7]*b3;
        result[1][0] = - a.array1d[ 4]*b5 + a.array1d[ 6]*b2 - a.array1d[ 7]*b1;
        result[2][0] = + a.array1d[ 4]*b4 - a.array1d[ 5]*b2 + a.array1d[ 7]*b0;
        result[3][0] = - a.array1d[ 4]*b3 + a.array1d[ 5]*b1 - a.array1d[ 6]*b0;
        result[0][1] = - a.array1d[ 1]*b5 + a.array1d[ 2]*b4 - a.array1d[ 3]*b3;
        result[1][1] = + a.array1d[ 0]*b5 - a.array1d[ 2]*b2 + a.array1d[ 3]*b1;
        result[2][1] = - a.array1d[ 0]*b4 + a.array1d[ 1]*b2 - a.array1d[ 3]*b0;
        result[3][1] = + a.array1d[ 0]*b3 - a.array1d[ 1]*b1 + a.array1d[ 2]*b0;
        result[0][2] = + a.array1d[13]*a5 - a.array1d[14]*a4 + a.array1d[15]*a3;
        result[1][2] = - a.array1d[12]*a5 + a.array1d[14]*a2 - a.array1d[15]*a1;
        result[2][2] = + a.array1d[12]*a4 - a.array1d[13]*a2 + a.array1d[15]*a0;
        result[3][2] = - a.array1d[12]*a3 + a.array1d[13]*a1 - a.array1d[14]*a0;
        result[0][3] = - a.array1d[ 9]*a5 + a.array1d[10]*a4 - a.array1d[11]*a3;
        result[1][3] = + a.array1d[ 8]*a5 - a.array1d[10]*a2 + a.array1d[11]*a1;
        result[2][3] = - a.array1d[ 8]*a4 + a.array1d[ 9]*a2 - a.array1d[11]*a0;
        result[3][3] = + a.array1d[ 8]*a3 - a.array1d[ 9]*a1 + a.array1d[10]*a0;

        float32_t d_inverse = ((float32_t)1.0)/d;

        for (int32_t row = 0; row < 4; row++)
        {
            for (int32_t col = 0; col < 4; col++)
            {
                result[row][col] *= d_inverse;
            }
        }

        return *this = result;
    }

    inline Matrix4 Matrix4::Inverted() const
    {
        Matrix4 result = *this;
        return result.Invert();
    }

    inline Matrix4& Matrix4::AffineInvert()
    {
        Matrix4 m = *this;
        Matrix4 &result = *this;

        auto float32_t det1 = ((m[1][1] * m[2][2]) - (m[1][2] * m[2][1]));
        auto float32_t det2 = ((m[0][2] * m[2][1]) - (m[0][1] * m[2][2]));
        auto float32_t det3 = ((m[0][1] * m[1][2]) - (m[0][2] * m[1][1]));
        auto float32_t det = (m[0][0] * det1) + (m[1][0] * det2) + (m[2][0] * det3);

        if (det != 0)
        {
            if (fabs(1.0/det) < HELIUM_ANGLE_NEAR_ZERO)
            {
                return *this = Matrix4 ();
            }
            else
            {
                result[0][0] = det1 / det;
                result[1][0] = ((m[2][0] * m[1][2]) - (m[1][0] * m[2][2])) / det;
                result[2][0] = ((m[1][0] * m[2][1]) - (m[2][0] * m[1][1])) / det;
                result[0][1] = det2 / det;
                result[1][1] = ((m[0][0] * m[2][2]) - (m[2][0] * m[0][2])) / det;
                result[2][1] = ((m[2][0] * m[0][1]) - (m[0][0] * m[2][1])) / det;
                result[0][2] = det3 / det;
                result[1][2] = ((m[1][0] * m[0][2]) - (m[0][0] * m[1][2])) / det;
                result[2][2] = ((m[0][0] * m[1][1]) - (m[1][0] * m[0][1])) / det;
            }
        }

        result[3][0] = -((m[3][0] * result[0][0]) + (m[3][1] * result[1][0]) + (m[3][2] * result[2][0]));
        result[3][1] = -((m[3][0] * result[0][1]) + (m[3][1] * result[1][1]) + (m[3][2] * result[2][1]));
        result[3][2] = -((m[3][0] * result[0][2]) + (m[3][1] * result[1][2]) + (m[3][2] * result[2][2]));

        result.x[3] = 0;
        result.y[3] = 0;
        result.z[3] = 0;
        result.t[3] = 1;

        return *this;
    }

    inline Matrix4 Matrix4::AffineInverted() const
    {
        Matrix4 result = *this;
        return result.AffineInvert();
    }

    inline Matrix4& Matrix4::Transpose()
    {
        auto float32_t temp;

        temp = x[1]; x[1] = y[0]; y[0] = temp;
        temp = x[2]; x[2] = z[0]; z[0] = temp;
        temp = x[3]; x[3] = t[0]; t[0] = temp;
        temp = y[2]; y[2] = z[1]; z[1] = temp;
        temp = y[3]; y[3] = t[1]; t[1] = temp;
        temp = z[3]; z[3] = t[2]; t[2] = temp;

        return *this;
    }

    inline Matrix4 Matrix4::Transposed() const
    {
        Matrix4 result = *this;
        return result.Transpose();
    }

    inline Matrix4& Matrix4::Normalize()
    {
        x.Normalize();
        y.Normalize();
        z.Normalize();

        return *this;
    }

    inline Matrix4 Matrix4::Normalized() const
    {
        Matrix4 result = *this;
        return result.Normalize();
    }

    inline const Vector4& Matrix4::GetBasis(Axis axis) const
    {
        switch (axis)
        {
        case SingleAxes::X:
            return x;

        case SingleAxes::Y:
            return y;

        case SingleAxes::Z:
            return z;
        }

        return Vector4::Zero;
    }

    inline Matrix4& Matrix4::SetBasis(Axis axis, const Vector4& v)
    {
        switch (axis)
        {
        case SingleAxes::X:
            x = v;
            break;

        case SingleAxes::Y:
            y = v;
            break;

        case SingleAxes::Z:
            z = v;
            break;
        }

        return *this;
    }

    inline Matrix4& Matrix4::Orthogonalize(Axis first, Axis second, Axis third)
    {
        HELIUM_ASSERT(first != second && second != third);

#ifdef HELIUM_ASSERT_ENABLED
        Vector4 oldFirst = GetBasis(first);
#endif

        Vector4 oldThird = GetBasis(third);
        SetBasis(third, GetBasis( NextAxis(third) ).Cross( GetBasis(PrevAxis(third)) ));
        Vector4 newThird = GetBasis(third);

        newThird.Normalize();
        oldThird.Normalize();
        if (newThird.Dot(oldThird) < 0.0f)
        {
            SetBasis(third, GetBasis(third) * -1.0f);
        }

        Vector4 oldSecond = GetBasis(second);
        SetBasis(second, GetBasis( NextAxis(second) ).Cross( GetBasis(PrevAxis(second)) ));
        Vector4 newSecond = GetBasis(second);

        newSecond.Normalize();
        oldSecond.Normalize();
        if (newSecond.Dot(oldSecond) < 0.0f)
        {
            SetBasis(second, GetBasis(second) * -1.0f);
        }

        HELIUM_ASSERT( oldFirst == GetBasis(first) );

        return *this;
    }

    inline void Vector4::Transform(const Matrix4& m)
    {
        *this = (m * (*this));
    }

    inline void Matrix4::Transform(Vector4& v) const
    {
        v = *this * v;
    }

    inline void Matrix4::Transform(Vector3& v, float32_t w) const
    {
        const Matrix4& m (*this);

        v = Vector3(
            (m[0][0]*v[0]) + (m[1][0]*v[1]) + (m[2][0]*v[2]) + (m[3][0]*w),
            (m[0][1]*v[0]) + (m[1][1]*v[1]) + (m[2][1]*v[2]) + (m[3][1]*w),
            (m[0][2]*v[0]) + (m[1][2]*v[1]) + (m[2][2]*v[2]) + (m[3][2]*w));
    }

    inline void Matrix4::TransformVertex(Vector3& v) const
    {
        const Matrix4& m = *this;

        v = Vector3(
            (m[0][0]*v[0]) + (m[1][0]*v[1]) + (m[2][0]*v[2]) + (m[3][0]),
            (m[0][1]*v[0]) + (m[1][1]*v[1]) + (m[2][1]*v[2]) + (m[3][1]),
            (m[0][2]*v[0]) + (m[1][2]*v[1]) + (m[2][2]*v[2]) + (m[3][2]));
    }

    inline void Matrix4::TransformNormal(Vector3& n) const
    {
        Matrix4 m (*this);

        m.Invert();

        m.Transpose();

        n = Vector3(
            (m[0][0]*n[0]) + (m[1][0]*n[1]) + (m[2][0]*n[2]),
            (m[0][1]*n[0]) + (m[1][1]*n[1]) + (m[2][1]*n[2]),
            (m[0][2]*n[0]) + (m[1][2]*n[1]) + (m[2][2]*n[2]));
    }

    inline tostream& operator<<(tostream& outStream, const Matrix4& matrix)
    {
        outStream << matrix.x.x << ", " << matrix.x.y << ", " << matrix.x.z << ", " << matrix.x.w << ", ";
        outStream << matrix.y.x << ", " << matrix.y.y << ", " << matrix.y.z << ", " << matrix.y.w << ", ";
        outStream << matrix.z.x << ", " << matrix.z.y << ", " << matrix.z.z << ", " << matrix.z.w << ", ";
        outStream << matrix.t.x << ", " << matrix.t.y << ", " << matrix.t.z << ", " << matrix.t.w;

        return outStream;
    }

    inline tistream& operator>>(tistream& inStream, Matrix4& matrix)
    {
        inStream >> matrix.x.x;
        inStream.ignore();

        inStream >> matrix.x.y;
        inStream.ignore();

        inStream >> matrix.x.z;
        inStream.ignore();

        inStream >> matrix.x.w;
        inStream.ignore();

        inStream >> matrix.y.x;
        inStream.ignore();

        inStream >> matrix.y.y;
        inStream.ignore();

        inStream >> matrix.y.z;
        inStream.ignore();

        inStream >> matrix.y.w;
        inStream.ignore();

        inStream >> matrix.z.x;
        inStream.ignore();

        inStream >> matrix.z.y;
        inStream.ignore();

        inStream >> matrix.z.z;
        inStream.ignore();

        inStream >> matrix.z.w;
        inStream.ignore();

        inStream >> matrix.t.x;
        inStream.ignore();

        inStream >> matrix.t.y;
        inStream.ignore();

        inStream >> matrix.t.z;
        inStream.ignore();

        inStream >> matrix.t.w;

        return inStream;
    }
}