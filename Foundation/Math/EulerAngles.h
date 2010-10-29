#pragma once

#include <cmath>
#include <float.h>
#include "Utils.h"

#include "EulerOrder.h"
#include "Vector3.h"

namespace Helium
{
    class Matrix3;
    class Matrix4;
    class AngleAxis;
    class Quaternion;

    class FOUNDATION_API EulerAngles
    {
    public:
        Vector3 angles;
        EulerOrder order;

        const static EulerAngles Zero;

        EulerAngles           ()
            : order (EulerOrders::XYZs)
        {

        }

        explicit EulerAngles  (const Vector3& v)
            : order (EulerOrders::XYZs)
            , angles (v)
        {

        }

        explicit EulerAngles  (const Vector3& v, const EulerOrder& o)
            : order (o)
            , angles (v)
        {

        }

        explicit EulerAngles  (const f32& rx, const f32& ry, const f32& rz)
            : order (EulerOrders::XYZs)
            , angles (rx, ry, rz)
        {

        }

        explicit EulerAngles  (const f32& rx, const f32& ry, const f32& rz, const EulerOrder& o)
            : order (o)
            , angles (rx, ry, rz)
        {

        }

        explicit EulerAngles  (const Matrix3& v);
        explicit EulerAngles  (const Matrix3& v, const EulerOrder& order);
        EulerAngles&          operator= (const Matrix3& v);

        explicit EulerAngles  (const Matrix4& v);
        explicit EulerAngles  (const Matrix4& v, const EulerOrder& order);
        EulerAngles&          operator= (const Matrix4& v);

        explicit EulerAngles  (const AngleAxis& v);
        explicit EulerAngles  (const AngleAxis& v, const EulerOrder& order);
        EulerAngles&          operator= (const AngleAxis& v);

        explicit EulerAngles  (const Quaternion& v);
        explicit EulerAngles  (const Quaternion& v, const EulerOrder& order);
        EulerAngles&          operator= (const Quaternion& v);

        bool                  operator== (const EulerAngles& v) const
        {
            return (angles == v.angles && order == v.order); 
        }

        bool                  operator!= (const EulerAngles& v) const
        {
            return !(angles == v.angles && order == v.order);
        }

        bool                  Valid()
        {
            return angles.Valid();
        }

        f32                 	I ()
        {
            return angles[order.I()];
        }

        void                  I (f32 v)
        {
            angles[order.I()] = v;
        }

        f32                 	J ()
        {
            return angles[order.J()];
        }

        void                  J (f32 v)
        {
            angles[order.J()] = v;
        }

        f32                 	H ()
        {
            return angles[order.H()];
        }

        void                  H (f32 v)
        {
            angles[order.H()] = v;
        }

        EulerAngles&          Reorder (EulerOrder v);
    };
}