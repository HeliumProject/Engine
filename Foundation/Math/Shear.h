#pragma once

#include <cmath>
#include "Utils.h"

#include "Vector3.h"

namespace Helium
{
    namespace Math
    {
        class FOUNDATION_API Shear
        {
        public:
            f32 yz, xz, xy;

            const static Shear Identity;

            Shear				    ()
                : yz(0)
                , xz(0)
                , xy(0)
            {

            }

            explicit Shear	(f32 vyz, f32 vxz, f32 vxy)
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

            f32&				    operator[] (const u32 i)
            {
                HELIUM_ASSERT( i >= 0 );
                HELIUM_ASSERT( i <= 2 );
                return (&yz)[i];
            }

            const f32&	    operator[] (const u32 i) const
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

            bool				    Equal (const Shear& v, f32 error = 0) const
            {
                return fabs(yz - v.yz) < error && fabs(xz - v.xz) < error && fabs(xy - v.xy) < error;
            }

            bool            Valid()
            {
                return IsValid(yz) && IsValid(xz) && IsValid(xy);
            }
        };
    }
}