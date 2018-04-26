#include "Precompile.h"
#include "ComponentSetDefinition.h"
#include "Framework/ParameterSet.h"
#include "Foundation/Log.h"
#include "Framework/ComponentDefinition.h"
#include "Reflect/TranslatorDeduction.h"

HELIUM_IMPLEMENT_ASSET(Helium::ComponentSetDefinition, Framework, 0);

void Helium::ComponentSetDefinition::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &ComponentSetDefinition::m_Set, "m_Set" );
}
