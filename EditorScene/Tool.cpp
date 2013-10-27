#include "SceneGraphPch.h"
#include "SceneGraph/Tool.h"
#include "SceneGraph/Scene.h"

HELIUM_DEFINE_ABSTRACT( Helium::SceneGraph::Tool );

using namespace Helium;
using namespace Helium::SceneGraph;

Tool::Tool( SceneGraph::Scene* scene, PropertiesGenerator* generator )
	: m_Generator ( generator )
	, m_View ( scene->GetViewport() )
	, m_Scene ( scene )
	, m_AllowSelection( true )
{

}