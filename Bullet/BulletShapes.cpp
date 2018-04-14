#include "BulletPch.h"
#include "Bullet/BulletShapes.h"

#include "Reflect/TranslatorDeduction.h"

using namespace Helium;

//REFLECT_DEFINE_BASE_STRUCT(Helium::BulletShape);
HELIUM_DEFINE_CLASS(Helium::BulletShape);

void Helium::BulletShape::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField(&BulletShape::m_Mass, "m_Mass" );
	comp.AddField(&BulletShape::m_Position, "m_Position" );
	comp.AddField(&BulletShape::m_Rotation, "m_Rotation" );
}

Helium::BulletShape::BulletShape()
	: m_Mass(0.0f)
	, m_Position(Simd::Vector3::Zero)
	, m_Rotation(Simd::Quat::IDENTITY)
{
	
}

//REFLECT_DEFINE_DERIVED_STRUCT(Helium::BulletShapeSphere);
HELIUM_DEFINE_CLASS(Helium::BulletShapeSphere);

void Helium::BulletShapeSphere::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField(&BulletShapeSphere::m_Radius, "m_Radius" );
}

btCollisionShape * Helium::BulletShapeSphere::CreateShape() const
{
	return new btSphereShape(m_Radius);
}

Helium::BulletShapeSphere::BulletShapeSphere()
	: m_Radius(1.0f)
{

}
//REFLECT_DEFINE_DERIVED_STRUCT(Helium::BulletShapeBox);
HELIUM_DEFINE_CLASS(Helium::BulletShapeBox);

void Helium::BulletShapeBox::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField(&BulletShapeBox::m_Extents, "m_Extents" );
}

btCollisionShape * Helium::BulletShapeBox::CreateShape() const
{
	static const Simd::Vector3 half(0.5f, 0.5f, 0.5f);
	btVector3 extents;
	ConvertToBullet( half.Multiply(m_Extents), extents );
	return new btBoxShape(extents);
}

Helium::BulletShapeBox::BulletShapeBox()
	: m_Extents(1.0f, 1.0f, 1.0f)
{

}
