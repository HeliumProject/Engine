#pragma once

#include "Platform/Math/MathCommon.h"
#include <vector>
#include <iostream>

#include "Utils.h"
#include "Vector3.h"

//
// Row Major Rotation Matrix
//
namespace Helium
{
    class Matrix4;
    class EulerAngles;
    class AngleAxis;

    class FOUNDATION_API Matrix3
    {
    public:
        union
        {
            float32_t array1d[9];
            float32_t array2d[3][3];
            struct
            { 
                Vector3 x;
                Vector3 y;
                Vector3 z;
            };
            struct
            {
                float32_t xx, xy, xz;
                float32_t yx, yy, yz;
                float32_t zx, zy, zz;
            };
        };

        const static Matrix3 Identity;
        const static Matrix3 Zero;

        Matrix3               ()
            : x (1, 0, 0)
            , y (0, 1, 0)
            , z (0, 0, 1)
        {

        }

        explicit Matrix3      (const Vector3& vx, const Vector3& vy, const Vector3& vz)
            : x(vx)
            , y(vy)
            , z(vz)
        {

        }

        explicit Matrix3      (const Matrix4& v);
        Matrix3&              operator= (const Matrix4& v);

        explicit Matrix3      (const EulerAngles& v);
        Matrix3&              operator= (const EulerAngles& v);

        explicit Matrix3      (const AngleAxis& v);
        Matrix3&              operator= (const AngleAxis& v);

        Matrix3&              operator= (const Matrix3& v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
            return *this;
        }

        Matrix3&              operator+= (const Matrix3& v);
        Matrix3               operator+ (const Matrix3& v) const;
        Matrix3&              operator*= (const Matrix3& v);
        Matrix3               operator* (const Matrix3& v) const;
        Vector3               operator* (const Vector3& v) const;

        Matrix3&              operator*= (const float32_t v)
        {
            x *= v;
            y *= v;
            z *= v;
            return *this;
        }

        Matrix3               operator* (const float32_t v) const
        {
            return Matrix3 (x * v, y * v, z * v);
        }

        Matrix3&              operator/= (const float32_t v)
        {
            x /= v;
            y /= v;
            z /= v;
            return *this;
        }

        Matrix3               operator/ (const float32_t v) const
        {
            return Matrix3 (x / v, y / v, z / v);
        }

        Vector3&              operator[] (const uint32_t i)
        {
            HELIUM_ASSERT(i < 3);
            return (&x)[i];
        }

        const Vector3&        operator[] (const uint32_t i) const
        {
            HELIUM_ASSERT(i < 3);
            return (&x)[i];
        }

        float32_t&                  operator()(const uint32_t i, const uint32_t j)
        {
            return (array2d[j][i]);
        }

        const float32_t&            operator()(const uint32_t i, const uint32_t j) const
        {
            return (array2d[j][i]);
        }

        bool                  operator== (const Matrix3& v) const
        {
            return (x == v.x && y == v.y && z == v.z);
        }

        bool                  operator!= (const Matrix3& v) const
        {
            return !(x == v.x && y == v.y && z == v.z);
        }

        bool                  Valid()
        {
            return x.Valid() && y.Valid() && z.Valid();
        }

        float32_t                   Determinant () const;

        Matrix3&              Invert ();
        Matrix3               Inverted () const;

        Matrix3&              Transpose ();
        Matrix3               Transposed () const;

        Matrix3&              Normalize ();
        Matrix3               Normalized () const;

        const Vector3&        GetBasis(Axis axis) const;
        Matrix3&              SetBasis(Axis axis, const Vector3& v);
        Matrix3&              Orthogonalize (Axis first = SingleAxes::Y, Axis second = SingleAxes::X, Axis third = SingleAxes::Z);

        static Matrix3        RotateX (float32_t theta);
        static Matrix3        RotateY (float32_t theta);
        static Matrix3        RotateZ (float32_t theta);

        friend FOUNDATION_API tostream& operator<<(tostream& outStream, const Matrix3& vector);
        friend FOUNDATION_API tistream& operator>>(tistream& inStream, Matrix3& vector);
        friend class FOUNDATION_API Matrix4;
    };

    typedef std::vector<Matrix3> V_Matrix3;

    inline Matrix3& Matrix3::operator+=(const Matrix3& b)
    {
        Matrix3 a (*this);
        Matrix3 &result = *this;
        result = Matrix3::Zero;

        for (unsigned row=0; row<3; row++)
            for (unsigned col=0; col<3; col++)
                result[row][col] = a[row][col]+b[row][col];

        return result;
    }

    inline Matrix3 Matrix3::operator+(const Matrix3& b) const
    {
        Matrix3 a (*this);
        Matrix3 result = Matrix3::Zero;

        for (unsigned row=0; row<3; row++)
            for (unsigned col=0; col<3; col++)
                result[row][col] = a[row][col]+b[row][col];

        return result;
    }

    inline Matrix3& Matrix3::operator*=(const Matrix3& b)
    {
        Matrix3 a (*this);
        Matrix3 &result = *this;
        result = Matrix3::Zero;

        for (unsigned row=0; row<3; row++)
            for (unsigned col=0; col<3; col++)
                for (unsigned mid=0; mid<3; mid++)
                    result[row][col] += a[row][mid]*b[mid][col];

        return result;
    }

    inline Matrix3 Matrix3::operator*(const Matrix3& b) const
    {
        Matrix3 a (*this);
        Matrix3 result = Matrix3::Zero;

        for (unsigned row=0; row<3; row++)
            for (unsigned col=0; col<3; col++)
                for (unsigned mid=0; mid<3; mid++)
                    result[row][col] += a[row][mid]*b[mid][col];

        return result;
    }

    inline Vector3 Matrix3::operator* (const Vector3& v) const
    {
        const Matrix3& m (*this);

        return Vector3(
            (m[0][0]*v[0]) + (m[1][0]*v[1]) + (m[2][0]*v[2]),
            (m[0][1]*v[0]) + (m[1][1]*v[1]) + (m[2][1]*v[2]),
            (m[0][2]*v[0]) + (m[1][2]*v[1]) + (m[2][2]*v[2]));
    }

    inline float32_t Matrix3::Determinant() const
    {
        const Matrix3 &m = *this;

        auto float32_t det1 = ((m[1][1] * m[2][2]) - (m[1][2] * m[2][1]));
        auto float32_t det2 = ((m[0][2] * m[2][1]) - (m[0][1] * m[2][2]));
        auto float32_t det3 = ((m[0][1] * m[1][2]) - (m[0][2] * m[1][1]));

        return (m[0][0] * det1) + (m[1][0] * det2) + (m[2][0] * det3);
    }

    inline Matrix3& Matrix3::Invert()
    {
        Matrix3 m = *this;
        Matrix3 &result = *this;

        float32_t det1 = ((m[1][1] * m[2][2]) - (m[1][2] * m[2][1]));
        float32_t det2 = ((m[0][2] * m[2][1]) - (m[0][1] * m[2][2]));
        float32_t det3 = ((m[0][1] * m[1][2]) - (m[0][2] * m[1][1]));

        float32_t det = (m[0][0] * det1) + (m[1][0] * det2) + (m[2][0] * det3);

        if (det != 0)
        {
            if (fabs(1.0/det) < HELIUM_ANGLE_NEAR_ZERO)
            {
                return (*this) = Matrix3 ();
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

        return *this;
    }

    inline Matrix3 Matrix3::Inverted() const
    {
        Matrix3 result = *this;

        result.Invert();

        return result;
    }

    inline Matrix3& Matrix3::Transpose()
    {
        float32_t temp;

        temp = x[1];  x[1] = y[0];  y[0] = temp;
        temp = x[2];  x[2] = z[0];  z[0] = temp;
        temp = y[2];  y[2] = z[1];  z[1] = temp;

        return *this;
    }

    inline Matrix3 Matrix3::Transposed() const
    {
        Matrix3 result = *this;
        return result.Transpose();
    }

    inline Matrix3& Matrix3::Normalize()
    {
        x.Normalize();
        y.Normalize();
        z.Normalize();

        return *this;
    }

    inline Matrix3 Matrix3::Normalized() const
    {
        Matrix3 result = *this;
        return result.Normalize();
    }

    inline const Vector3& Matrix3::GetBasis(Axis axis) const
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

        return Vector3::Zero;
    }

    inline Matrix3& Matrix3::SetBasis(Axis axis, const Vector3& v)
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

    inline Matrix3& Matrix3::Orthogonalize(Axis first, Axis second, Axis third)
    {
        HELIUM_ASSERT(first != second && second != third);

#ifdef HELIUM_ASSERT_ENABLED
        Vector3 oldFirst = GetBasis(first);
#endif

        Vector3 oldThird = GetBasis(third);
        SetBasis(third, GetBasis( NextAxis(third) ).Cross( GetBasis(PrevAxis(third)) ));
        Vector3 newThird = GetBasis(third);

        newThird.Normalize();
        oldThird.Normalize();
        if (newThird.Dot(oldThird) < 0.0f)
        {
            SetBasis(third, GetBasis(third) * -1.0f);
        }

        Vector3 oldSecond = GetBasis(second);
        SetBasis(second, GetBasis( NextAxis(second) ).Cross( GetBasis(PrevAxis(second)) ));
        Vector3 newSecond = GetBasis(second);

        newSecond.Normalize();
        oldSecond.Normalize();
        if (newSecond.Dot(oldSecond) < 0.0f)
        {
            SetBasis(second, GetBasis(second) * -1.0f);
        }

        HELIUM_ASSERT( oldFirst == GetBasis(first) );

        return *this;
    }

    inline tostream& operator<<(tostream& outStream, const Matrix3& matrix)
    {
        outStream << matrix.x.x << ", " << matrix.x.y << ", " << matrix.x.z << ", ";
        outStream << matrix.y.x << ", " << matrix.y.y << ", " << matrix.y.z << ", ";
        outStream << matrix.z.x << ", " << matrix.z.y << ", " << matrix.z.z;

        return outStream;
    }

    inline tistream& operator>>(tistream& inStream, Matrix3& matrix)
    {
        inStream >> matrix.x.x;
        inStream.ignore();

        inStream >> matrix.x.y;
        inStream.ignore();

        inStream >> matrix.x.z;
        inStream.ignore();

        inStream >> matrix.y.x;
        inStream.ignore();

        inStream >> matrix.y.y;
        inStream.ignore();

        inStream >> matrix.y.z;
        inStream.ignore();

        inStream >> matrix.z.x;
        inStream.ignore();

        inStream >> matrix.z.y;
        inStream.ignore();

        inStream >> matrix.z.z;

        return inStream;
    }
}