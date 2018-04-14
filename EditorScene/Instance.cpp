#include "EditorScenePch.h"
#include "Instance.h"

#include "Foundation/Log.h"

#include "EditorScene/Scene.h"

HELIUM_DEFINE_ABSTRACT( Helium::Editor::Instance );

using namespace Helium;
using namespace Helium::Editor;

void Instance::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &Instance::m_Solid,                  "m_Solid" );
	comp.AddField( &Instance::m_SolidOverride,          "m_SolidOverride" );
	comp.AddField( &Instance::m_Transparent,            "m_Transparent" );
	comp.AddField( &Instance::m_TransparentOverride,    "m_TransparentOverride" );
}

Instance::Instance()
	: m_Solid (false)
	, m_SolidOverride (false)
	, m_Transparent (false)
	, m_TransparentOverride (false)
{
}

Instance::~Instance()
{
}
