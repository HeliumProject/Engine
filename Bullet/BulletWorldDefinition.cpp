
#include "BulletPch.h"
#include "Bullet/BulletWorldDefinition.h"

#include "Framework/FrameworkDataDeduction.h"

using namespace Helium;

HELIUM_IMPLEMENT_ASSET(Helium::BulletWorldDefinition, Bullet, 0);

void BulletWorldDefinition::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddStructureField(&BulletWorldDefinition::m_Gravity, TXT( "m_Gravity" ) );
}
