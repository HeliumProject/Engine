#include "SceneGraphPch.h"
#include "SceneGraph/Tool.h"
#include "SceneGraph/Scene.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_ABSTRACT(SceneGraph::Tool);

void Tool::InitializeType()
{

}

void Tool::CleanupType()
{

}

Tool::Tool( SceneGraph::Scene* scene, PropertiesGenerator* generator )
    : m_Generator ( generator )
    , m_View ( scene->GetViewport() )
    , m_Scene ( scene )
    , m_AllowSelection( true )
{

}