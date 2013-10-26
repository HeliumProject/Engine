#include "SceneGraphPch.h"
#include "Instance.h"

#include "Foundation/Log.h"

#include "SceneGraph/Scene.h"

HELIUM_DEFINE_ABSTRACT( Helium::SceneGraph::Instance );

using namespace Helium;
using namespace Helium::SceneGraph;

void Instance::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField( &Instance::m_Solid,                  TXT( "m_Solid" ) );
    comp.AddField( &Instance::m_SolidOverride,          TXT( "m_SolidOverride" ) );
    comp.AddField( &Instance::m_Transparent,            TXT( "m_Transparent" ) );
    comp.AddField( &Instance::m_TransparentOverride,    TXT( "m_TransparentOverride" ) );
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
