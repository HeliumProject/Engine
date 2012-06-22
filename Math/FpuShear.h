#pragma once

#include "Math/API.h"
#include "Foundation/Math.h"
#include "Math/FpuVector3.h"

namespace Helium
{
    class HELIUM_MATH_API Shear
    {
    public:
        float32_t yz, xz, xy;

        const static Shear Identity;

        Shear				    ()
            : yz(0)
            , xz(0)
            , xy(0)
        {

        }

        explicit Shear	(float32_t vyz, float32_t vxz, float32_t vxy)
            : yz(vyz)
            , xz(vxz)
            , xy(vxy)
        {

        }

        explicit Shear	(const Vector3& v)
            : yz(v.x)
            , xz(v.y)
            , xy(v.z)
        {

        }

        Shear&			    operator= (const Shear& v)
        {
            yz = v.yz;
            xz = v.xz;
            xy = v.xy;
            return *this;
        }

        Shear&			    operator= (const Vector3& v)
        {
            yz = v.y;
            xz = v.z;
            xy = v.x;
            return *this;
        }

        float32_t&				    operator[] (const uint32_t i)
        {
            HELIUM_ASSERT( i >= 0 );
            HELIUM_ASSERT( i <= 2 );
            return (&yz)[i];
        }

        const float32_t&	    operator[] (const uint32_t i) const
        {
            HELIUM_ASSERT( i >= 0 );
            HELIUM_ASSERT( i <= 2 );
            return (&yz)[i];
        }

        bool				    operator== (const Shear& v) const
        {
            return (yz == v.yz && xz == v.xz && xy == v.xy);
        }

        bool				    operator!= (const Shear& v) const
        {
            return !(yz == v.yz && xz == v.xz && xy == v.xy);
        }

        bool				    Equal (const Shear& v, float32_t error = 0) const
        {
            return fabs(yz - v.yz) < error && fabs(xz - v.xz) < error && fabs(xy - v.xy) < error;
        }

        bool            Finite()
        {
            return IsFinite(yz) && IsFinite(xz) && IsFinite(xy);
        }
    };
}