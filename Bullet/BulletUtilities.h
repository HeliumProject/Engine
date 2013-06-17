
#pragma once

#include "Math/Vector3.h"
#include "MathSimd/Vector3.h"
#include "MathSimd/Quat.h"
#include "MathSimd/Matrix44.h"
#include "LinearMath/btVector3.h"
#include "LinearMath/btQuaternion.h"
#include "LinearMath/btMatrix3x3.h"

template <class HeliumType, class BulletType>
inline void ConvertToBullet(const HeliumType &rHelium, BulletType &rBullet);

template <class BulletType, class HeliumType>
inline void ConvertFromBullet(const BulletType &rBullet, HeliumType &rHelium);

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
#ifdef BT_USE_SSE
	rBullet.set128(rHelium.GetSimdVector());
#else
	rBullet.setX(rHelium.m_x);
	rBullet.setY(rHelium.m_y);
	rBullet.setZ(rHelium.m_z);
	rBullet.setW(rHelium.m_w);
#endif
}

template <>
inline void ConvertToBullet< Helium::Simd::Matrix44, btMatrix3x3 >( const Helium::Simd::Matrix44 &rHelium, btMatrix3x3 &rBullet )
{
#ifdef BT_USE_SSE
	rBullet[0].set128(rHelium.GetSimdVector(0));
	rBullet[1].set128(rHelium.GetSimdVector(1));
	rBullet[2].set128(rHelium.GetSimdVector(2));
#else
	rBullet[0][0] = rHelium.GetRow(0).GetElement(0);
	rBullet[0][1] = rHelium.GetRow(0).GetElement(1);
	rBullet[0][2] = rHelium.GetRow(0).GetElement(2);
	rBullet[1][0] = rHelium.GetRow(1).GetElement(0);
	rBullet[1][1] = rHelium.GetRow(1).GetElement(1);
	rBullet[1][2] = rHelium.GetRow(1).GetElement(2);
	rBullet[2][0] = rHelium.GetRow(2).GetElement(0);
	rBullet[2][1] = rHelium.GetRow(2).GetElement(1);
	rBullet[2][2] = rHelium.GetRow(2).GetElement(2);
#endif
}

template <>
inline void ConvertFromBullet< btVector3, Helium::Vector3 >( const btVector3 &rBullet, Helium::Vector3 &rHelium )
{
	rHelium.Set(rBullet.x(), rBullet.y(), rBullet.z());
}

template <>
inline void ConvertFromBullet< btVector3, Helium::Simd::Vector3 >( const btVector3 &rBullet, Helium::Simd::Vector3 &rHelium )
{
#ifdef BT_USE_SSE
	rHelium.SetSimdVector(rBullet.get128());
#else
	rHelium.m_x = rBullet.getX();
	rHelium.m_y = rBullet.getY();
	rHelium.m_z = rBullet.getZ();
#endif
}

template <>
inline void ConvertFromBullet< btVector3, Helium::Simd::Vector4 >( const btVector3 &rBullet, Helium::Simd::Vector4 &rHelium )
{
#ifdef BT_USE_SSE
	rHelium.SetSimdVector(rBullet.get128());
#else
	rHelium.m_x = rBullet.getX();
	rHelium.m_y = rBullet.getY();
	rHelium.m_z = rBullet.getZ();
	rHelium.m_w = 1.0f;
#endif
}

template <>
inline void ConvertFromBullet< btQuaternion, Helium::Simd::Quat >( const btQuaternion &rBullet, Helium::Simd::Quat &rHelium )
{
#ifdef BT_USE_SSE
	rHelium.SetSimdVector(rBullet.get128());
#else
	rHelium.m_x = rBullet.getX();
	rHelium.m_y = rBullet.getY();
	rHelium.m_z = rBullet.getZ();
	rHelium.m_w = rBullet.getW();
#endif
}

template <>
inline void ConvertFromBullet< btMatrix3x3, Helium::Simd::Matrix44 >( const btMatrix3x3 &rBullet, Helium::Simd::Matrix44 &rHelium )
{
#ifdef BT_USE_SSE
	rHelium.SetSimdVector(0, rBullet[0].get128());
	rHelium.SetSimdVector(1, rBullet[1].get128());
	rHelium.SetSimdVector(2, rBullet[2].get128());
	rHelium.SetSimdVector(3, Helium::Simd::Vector4::BasisW.GetSimdVector());
	rHelium.Transpose();
#else
	//rHelium.SetRow(0, Helium::Simd::Vector4(rBullet[0][0], rBullet[0][1], rBullet[0][2], 0.0f));
	//rHelium.SetRow(1, Helium::Simd::Vector4(rBullet[1][0], rBullet[1][1], rBullet[1][2], 0.0f));
	//rHelium.SetRow(2, Helium::Simd::Vector4(rBullet[2][0], rBullet[2][1], rBullet[2][2], 0.0f));

	rHelium.SetRow(0, Helium::Simd::Vector4(rBullet[0][0], rBullet[1][0], rBullet[2][0], 0.0f));
	rHelium.SetRow(1, Helium::Simd::Vector4(rBullet[0][1], rBullet[1][1], rBullet[2][1], 0.0f));
	rHelium.SetRow(2, Helium::Simd::Vector4(rBullet[0][2], rBullet[1][2], rBullet[2][2], 0.0f));
	rHelium.SetRow(3, Helium::Simd::Vector4::BasisW);
#endif

	//rHelium.SetElement(0, 1.0f);
	//rHelium.SetElement(1, 0.0f);
	//rHelium.SetElement(2, 0.0f);
	//rHelium.SetElement(3, 0.0f);
	//rHelium.SetElement(4, 0.0f);
	//rHelium.SetElement(5, 1.0f);
	//rHelium.SetElement(6, 0.0f);
	//rHelium.SetElement(7, 0.0f);
	//rHelium.SetElement(8, 0.0f);
	//rHelium.SetElement(9, 0.0f);
	//rHelium.SetElement(10, 1.0f);
	//rHelium.SetElement(11, 0.0f);
}

template <>
inline void ConvertFromBullet< btTransform, Helium::Simd::Matrix44 >( const btTransform &rBullet, Helium::Simd::Matrix44 &rHelium )
{
//#if 0
//	ConvertFromBullet( rBullet.getBasis(), rHelium );
//
//#if 1
//#ifdef BT_USE_SSE
//	rHelium.SetSimdVector(3, rBullet.getOrigin().get128());
//#else
//	rHelium.SetRow(3, Helium::Simd::Vector4(rBullet.getOrigin().getX(), rBullet.getOrigin().getY(), rBullet.getOrigin().getZ(), 1.0f) );
//#endif
//#endif
//
//	//rHelium.SetElement(12, 0.0f);
//	//rHelium.SetElement(13, 0.0f);
//	//rHelium.SetElement(14, 0.0f);
//
//	rHelium.SetElement(15, 1.0f);
//#endif

	ConvertFromBullet( rBullet.getBasis(), rHelium );
#ifdef BT_USE_SSE

	Helium::Simd::Vector4 originRow( rBullet.getOrigin().get128() );
	originRow += Helium::Simd::Vector4::BasisW;
	rHelium.SetSimdVector( 3, originRow.GetSimdVector() );
#else
	rHelium.SetRow(3, Helium::Simd::Vector4(rBullet.getOrigin().getX(), rBullet.getOrigin().getY(), rBullet.getOrigin().getZ(), 1.0f) );
#endif
}
