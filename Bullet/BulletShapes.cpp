
#include "BulletPch.h"
#include "Bullet/BulletShapes.h"

#include "Reflect/Data/DataDeduction.h"
#include "Framework/FrameworkDataDeduction.h"

using namespace Helium;

//REFLECT_DEFINE_BASE_STRUCTURE(Helium::BulletShape);
REFLECT_DEFINE_ABSTRACT(Helium::BulletShape);

void Helium::BulletShape::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField(&BulletShape::m_Mass, TXT( "m_Mass" ) );
}

Helium::BulletShape::BulletShape()
    : m_Mass(0.0f)
{
    
}

//REFLECT_DEFINE_DERIVED_STRUCTURE(Helium::BulletShapeSphere);
REFLECT_DEFINE_OBJECT(Helium::BulletShapeSphere);

void Helium::BulletShapeSphere::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField(&BulletShapeSphere::m_Radius, TXT( "m_Radius" ) );
}

btCollisionShape * Helium::BulletShapeSphere::CreateShape() const
{
    return new btSphereShape(m_Radius);
}

Helium::BulletShapeSphere::BulletShapeSphere()
    : m_Radius(1.0f)
{

}
//REFLECT_DEFINE_DERIVED_STRUCTURE(Helium::BulletShapeBox);
REFLECT_DEFINE_OBJECT(Helium::BulletShapeBox);

void Helium::BulletShapeBox::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField(&BulletShapeBox::m_Extents, TXT( "m_Extents" ) );
}

btCollisionShape * Helium::BulletShapeBox::CreateShape() const
{
    btVector3 extents(m_Extents.x / 2.0f, m_Extents.y / 2.0f, m_Extents.z / 2.0f);
    return new btBoxShape(extents);
}

Helium::BulletShapeBox::BulletShapeBox()
    : m_Extents(1.0f, 1.0f, 1.0f)
{

}
