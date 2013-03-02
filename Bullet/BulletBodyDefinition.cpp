
#include "BulletPch.h"
#include "Bullet/BulletBodyDefinition.h"
#include "Bullet/BulletShapes.h"

#include "Reflect/Data/DataDeduction.h"
#include "Framework/FrameworkDataDeduction.h"

using namespace Helium;

HELIUM_IMPLEMENT_ASSET(Helium::BulletBodyDefinition, Bullet, 0);

void Helium::BulletBodyDefinition::PopulateComposite( Reflect::Composite& comp )
{
    //TODO: m_Shapes won't properly register because BulletShape isn't recognized
    comp.AddField(&BulletBodyDefinition::m_Shapes, TXT( "m_Shapes" ), 0, Reflect::GetClass<Reflect::ObjectDynamicArrayData>());
    comp.AddField(&BulletBodyDefinition::m_Restitution, TXT( "m_Restitution" ));
    comp.AddField(&BulletBodyDefinition::m_Constrain2d, "m_Constrain2D");
}

Helium::BulletBodyDefinition::BulletBodyDefinition()
    : m_Constrain2d(false)
{

}
