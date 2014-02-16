#include "EnginePch.h"
#include "Engine/ConfigAsset.h"

#include "Reflect/TranslatorDeduction.h"

HELIUM_IMPLEMENT_ASSET( Helium::ConfigAsset, Engine, 0 );

using namespace Helium;

/// Constructor.
ConfigAsset::ConfigAsset()
{
}

/// Destructor.
ConfigAsset::~ConfigAsset()
{
}

void ConfigAsset::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &ConfigAsset::m_ConfigObject, "m_ConfigObject" );
}
