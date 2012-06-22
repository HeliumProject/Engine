#pragma once

#include <float.h>

#include "Math/API.h"
#include "Math/Common.h"
#include "Math/FpuEulerOrder.h"
#include "Math/FpuVector3.h"

namespace Helium
{
    class Matrix3;
    class Matrix4;
    class AngleAxis;

    class HELIUM_MATH_API EulerAngles
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

        explicit EulerAngles  (const float32_t& rx, const float32_t& ry, const float32_t& rz)
            : order (EulerOrders::XYZs)
            , angles (rx, ry, rz)
        {

        }

        explicit EulerAngles  (const float32_t& rx, const float32_t& ry, const float32_t& rz, const EulerOrder& o)
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

        bool                  operator== (const EulerAngles& v) const
        {
            return (angles == v.angles && order == v.order); 
        }

        bool                  operator!= (const EulerAngles& v) const
        {
            return !(angles == v.angles && order == v.order);
        }

        bool                  Finite()
        {
            return angles.Finite();
        }

        float32_t                 	I ()
        {
            return angles[order.I()];
        }

        void                  I (float32_t v)
        {
            angles[order.I()] = v;
        }

        float32_t                 	J ()
        {
            return angles[order.J()];
        }

        void                  J (float32_t v)
        {
            angles[order.J()] = v;
        }

        float32_t                 	H ()
        {
            return angles[order.H()];
        }

        void                  H (float32_t v)
        {
            angles[order.H()] = v;
        }

        EulerAngles&          Reorder (EulerOrder v);
    };
}