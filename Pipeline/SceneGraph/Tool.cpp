/*#include "Precompile.h"*/
#include "Pipeline/SceneGraph/Tool.h"
#include "Pipeline/SceneGraph/Scene.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_ABSTRACT(SceneGraph::Tool);

void Tool::InitializeType()
{
  Reflect::RegisterClassType< SceneGraph::Tool >( TXT( "SceneGraph::Tool" ) );
}

void Tool::CleanupType()
{
  Reflect::UnregisterClassType< SceneGraph::Tool >();
}

Tool::Tool( SceneGraph::Scene* scene, PropertiesGenerator* generator )
: m_Generator ( generator )
, m_View ( scene->GetViewport() )
, m_Scene ( scene )
, m_AllowSelection( true )
{

}