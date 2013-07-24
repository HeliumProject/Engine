#include "BulletPch.h"
#include "Bullet/BulletWorldDefinition.h"
#include "Reflect/TranslatorDeduction.h"

using namespace Helium;

HELIUM_IMPLEMENT_ASSET(Helium::BulletWorldDefinition, Bullet, 0);

void BulletWorldDefinition::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField(&BulletWorldDefinition::m_Gravity, TXT( "m_Gravity" ) );
}
