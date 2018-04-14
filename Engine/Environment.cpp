
#include "EnginePch.h"
#include "Engine/Environment.h"

HELIUM_DEFINE_CLASS(Helium::HeliumEnvironment);

void Helium::HeliumEnvironment::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField(&HeliumEnvironment::m_Plugins, "m_Plugins" );
}
