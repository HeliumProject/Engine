#pragma once

#include <cmath>
#include "Utils.h"

#include "Vector3.h"

namespace Helium
{
    class Matrix3;
    class Matrix4;
    class EulerAngles;
    class Quaternion;

    class FOUNDATION_API AngleAxis
    {
    public:
        float32_t angle;
        Vector3 axis;

        const static AngleAxis Zero;
        const static AngleAxis Identity;

        AngleAxis               ()
            : angle(0)
            , axis(1, 0, 0)
        {

        }

        explicit AngleAxis      (const float32_t& angle, const Vector3& axis)
            : angle(angle)
            , axis(axis)
        {

        }

        explicit AngleAxis      (const float32_t& angle, const float32_t& x, const float32_t& y, const float32_t& z)
            : angle(angle)
            , axis(x,y,z)
        {

        }

        explicit AngleAxis      (const Matrix3& v);
        AngleAxis&              operator= (const Matrix3& v);

        explicit AngleAxis      (const Matrix4& v);
        AngleAxis&              operator= (const Matrix4& v);

        explicit AngleAxis      (const EulerAngles& v);
        AngleAxis&              operator= (const EulerAngles& v);

        explicit AngleAxis      (const Quaternion& v);
        AngleAxis&              operator= (const Quaternion& v);

        bool                    operator== (const AngleAxis& v) const
        {
            return (angle == v.angle && axis == v.axis);
        }

        bool                    operator!= (const AngleAxis& v) const
        {
            return !(angle == v.angle && axis == v.axis);
        }

        bool                    Valid()
        {
            return IsValid(angle) && axis.Valid();
        }

        AngleAxis&              Invert ()
        {
            angle = -angle;
            return *this;
        }

        AngleAxis               Inverted () const
        {
            return AngleAxis(-angle, axis);
        }

        AngleAxis&              Normalize ()
        {
            axis.Normalize();
            return *this;
        }

        static AngleAxis        Rotation (const Vector3& v1, const Vector3& v2)
        {
            Vector3 a(v1), b(v2);

            a.Normalize();
            b.Normalize();

            float32_t angle = acos((float32_t)(a.Dot(b)));
            Vector3 axis = a.Cross(b).Normalize();

            return AngleAxis (angle, axis);
        }
    };
}