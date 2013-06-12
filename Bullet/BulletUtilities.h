
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
    rBullet.set128(rHelium.GetSimdVector());
}

template <>
inline void ConvertToBullet< Helium::Simd::Quat, btQuaternion >( const Helium::Simd::Quat &rHelium, btQuaternion &rBullet )
{
    rBullet.set128(rHelium.GetSimdVector());
}
