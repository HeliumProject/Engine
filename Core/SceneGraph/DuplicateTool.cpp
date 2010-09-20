/*#include "Precompile.h"*/
#include "DuplicateTool.h"

#include "Core/SceneGraph/Mesh.h"
#include "Core/SceneGraph/Scene.h"
#include "Volume.h"
#include "VolumeType.h"

#include "Core/SceneGraph/Pick.h"

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
        m_Source = Reflect::ObjectCast<SceneGraph::Transform>( *m_Scene->GetSelection().GetItems().Begin() );
    }
}

DuplicateTool::~DuplicateTool()
{

}

SceneGraph::TransformPtr DuplicateTool::CreateNode()
{
    if (m_Source)
    {
        return Reflect::ObjectCast<SceneGraph::Transform>( m_Source->Duplicate() );
    }
    else
    {
        return NULL;
    }
}