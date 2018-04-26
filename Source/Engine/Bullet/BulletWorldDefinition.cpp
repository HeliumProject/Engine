#include "Precompile.h"
#include "Bullet/BulletWorldDefinition.h"
#include "Reflect/TranslatorDeduction.h"

using namespace Helium;

HELIUM_DEFINE_BASE_STRUCT(Helium::BulletWorldDefinition);

void BulletWorldDefinition::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField(&BulletWorldDefinition::m_Gravity, "m_Gravity" );
}
