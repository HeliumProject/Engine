
#include "BulletPch.h"
#include "BulletShapes.h"

#include "btBulletDynamicsCommon.h"

#include "Reflect/Data/DataDeduction.h"
#include "Framework/FrameworkDataDeduction.h"

using namespace Helium;

REFLECT_DEFINE_BASE_STRUCTURE(Helium::BulletShape);

void Helium::BulletShape::PopulateComposite( Reflect::Composite& comp )
{
    
}

REFLECT_DEFINE_DERIVED_STRUCTURE(Helium::BulletShapeBox);

void Helium::BulletShapeBox::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField(&BulletShapeBox::m_Extents, TXT( "m_Extents" ) );
}


REFLECT_DEFINE_DERIVED_STRUCTURE(Helium::BulletShapeSphere);

void Helium::BulletShapeSphere::PopulateComposite( Reflect::Composite& comp )
{

}