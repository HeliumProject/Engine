#include "SceneGraphPch.h"
#include "Instance.h"

#include "Foundation/Log.h"

#include "SceneGraph/Scene.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_ABSTRACT(Instance);

void Instance::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &Instance::m_Solid,                  TXT( "m_Solid" ) );
    comp.AddField( &Instance::m_SolidOverride,          TXT( "m_SolidOverride" ) );
    comp.AddField( &Instance::m_Transparent,            TXT( "m_Transparent" ) );
    comp.AddField( &Instance::m_TransparentOverride,    TXT( "m_TransparentOverride" ) );
}

void Instance::InitializeType()
{
    Reflect::RegisterClassType< Instance >( TXT( "SceneGraph::Instance" ) );
}

void Instance::CleanupType()
{
    Reflect::UnregisterClassType< Instance >();
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
