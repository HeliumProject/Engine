
#pragma once

#include "Math/Vector3.h"
#include "MathSimd/Vector3.h"
#include "MathSimd/Quat.h"
#include "LinearMath/btVector3.h"
#include "LinearMath/btQuaternion.h"

template <class HeliumType, class BulletType>
inline void ConvertToBullet(const HeliumType &rHelium, BulletType &rBullet);

template <>
inline void ConvertToBullet< Helium::Vector3, btVector3 >( const Helium::Vector3 &rHelium, btVector3 &rBullet )
{
    rBullet.setValue(rHelium.x, rHelium.y, rHelium.z);
}

template <>
inline void ConvertToBullet< Helium::Simd::Vector3, btVector3 >( const Helium::Simd::Vector3 &rHelium, btVector3 &rBullet )
{
#ifdef BT_USE_SSE
    rBullet.set128(rHelium.GetSimdVector());
#else
    rBullet.m_floats[0] = rHelium.m_x;
    rBullet.m_floats[1] = rHelium.m_y;
    rBullet.m_floats[2] = rHelium.m_z;
    rBullet.m_floats[3] = rHelium.m_w;
#endif
}

template <>
inline void ConvertToBullet< Helium::Simd::Quat, btQuaternion >( const Helium::Simd::Quat &rHelium, btQuaternion &rBullet )
{
#if BT_USE_SSE
    rBullet.set128(rHelium.GetSimdVector());
#else
    rBullet.setX(rHelium.m_x);
    rBullet.setY(rHelium.m_y);
    rBullet.setZ(rHelium.m_z);
    rBullet.setW(rHelium.m_w);
#endif
}
