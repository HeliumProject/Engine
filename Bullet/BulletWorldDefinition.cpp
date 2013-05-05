#include "BulletPch.h"
#include "Bullet/BulletWorldDefinition.h"
#include "Reflect/DataDeduction.h"

using namespace Helium;

HELIUM_IMPLEMENT_ASSET(Helium::BulletWorldDefinition, Bullet, 0);

void BulletWorldDefinition::PopulateStructure( Reflect::Structure& comp )
{
    comp.AddField(&BulletWorldDefinition::m_Gravity, TXT( "m_Gravity" ) );
}
