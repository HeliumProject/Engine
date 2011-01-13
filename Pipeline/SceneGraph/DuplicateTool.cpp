/*#include "Precompile.h"*/
#include "DuplicateTool.h"

#include "Pipeline/SceneGraph/Mesh.h"
#include "Pipeline/SceneGraph/Scene.h"
#include "Volume.h"
#include "VolumeType.h"

#include "Pipeline/SceneGraph/Pick.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_ABSTRACT(SceneGraph::DuplicateTool);

void DuplicateTool::InitializeType()
{
    Reflect::RegisterClassType< SceneGraph::DuplicateTool >( TXT( "SceneGraph::DuplicateTool" ) );
}

void DuplicateTool::CleanupType()
{
    Reflect::UnregisterClassType< SceneGraph::DuplicateTool >();
}

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