#include "SceneGraphPch.h"
#include "DuplicateTool.h"

#include "SceneGraph/Mesh.h"
#include "SceneGraph/Scene.h"
#include "SceneGraph/Pick.h"

HELIUM_DEFINE_ABSTRACT( Helium::SceneGraph::DuplicateTool );

using namespace Helium;
using namespace Helium::SceneGraph;

DuplicateTool::DuplicateTool(SceneGraph::Scene* scene, PropertiesGenerator* generator)
	: SceneGraph::CreateTool (scene, generator)
	, m_Source (NULL)
{
	if (!m_Scene->GetSelection().GetItems().Empty())
	{
		m_Source = Reflect::SafeCast<SceneGraph::Transform>( *m_Scene->GetSelection().GetItems().Begin() );
	}
}

DuplicateTool::~DuplicateTool()
{

}

SceneGraph::TransformPtr DuplicateTool::CreateNode()
{
	if (m_Source)
	{
		return Reflect::SafeCast<SceneGraph::Transform>( m_Source->Duplicate() );
	}
	else
	{
		return NULL;
	}
}