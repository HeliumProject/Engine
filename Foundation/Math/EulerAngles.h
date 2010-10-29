#pragma once

#include <cmath>
#include <float.h>
#include "Utils.h"

#include "EulerOrder.h"
#include "Vector3.h"

namespace Helium
{
    namespace Math
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
}