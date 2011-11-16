#include "SceneGraphPch.h"
#include "Instance.h"

#include "Foundation/Log.h"

#include "SceneGraph/Scene.h"

REFLECT_DEFINE_ABSTRACT( Helium::SceneGraph::Instance );

using namespace Helium;
using namespace Helium::SceneGraph;

void Instance::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &Instance::m_Solid,                  TXT( "m_Solid" ) );
    comp.AddField( &Instance::m_SolidOverride,          TXT( "m_SolidOverride" ) );
    comp.AddField( &Instance::m_Transparent,            TXT( "m_Transparent" ) );
    comp.AddField( &Instance::m_TransparentOverride,    TXT( "m_TransparentOverride" ) );
}

void Instance::InitializeType()
{

}

void Instance::CleanupType()
{

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
