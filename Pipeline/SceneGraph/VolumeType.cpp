/*#include "Precompile.h"*/
#include "VolumeType.h"

#include "Pipeline/SceneGraph/Volume.h"
#include "Pipeline/SceneGraph/Scene.h"
#include "Pipeline/SceneGraph/Viewport.h"
#include "Pipeline/SceneGraph/Color.h"
#include "Pipeline/SceneGraph/PrimitiveCube.h"
#include "Pipeline/SceneGraph/PrimitiveSphere.h"
#include "Pipeline/SceneGraph/PrimitiveCapsule.h"
#include "Pipeline/SceneGraph/PrimitiveCylinder.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_ABSTRACT(VolumeType);

void VolumeType::InitializeType()
{
    Reflect::RegisterClassType< VolumeType >( TXT( "SceneGraph::VolumeType" ) );
}

void VolumeType::CleanupType()
{
    Reflect::UnregisterClassType< VolumeType >();
}

VolumeType::VolumeType( Scene* scene, int32_t instanceType )
: InstanceType( scene, instanceType )
{
    m_Cube = new PrimitiveCube( scene->GetViewport()->GetResources() );
    m_Cube->Update();

    m_Cylinder = new PrimitiveCylinder( scene->GetViewport()->GetResources() );
    m_Cylinder->Update();

    m_Sphere = new PrimitiveSphere( scene->GetViewport()->GetResources() );
    m_Sphere->Update();

    m_Capsule = new PrimitiveCapsule( scene->GetViewport()->GetResources() );
    m_Capsule->Update();
}

VolumeType::~VolumeType()
{
    delete m_Cube;
    delete m_Cylinder;
    delete m_Sphere;
    delete m_Capsule;
}

void VolumeType::Create()
{
    __super::Create();

    m_Cube->Create();
    m_Cylinder->Create();
    m_Sphere->Create();
    m_Capsule->Create();
}

void VolumeType::Delete()
{
    __super::Delete();

    m_Cube->Delete();
    m_Cylinder->Delete();
    m_Sphere->Delete();
    m_Capsule->Delete();
}

const Primitive* VolumeType::GetShape( VolumeShape shape ) const
{
    switch (shape)
    {
    case VolumeShapes::Cube:
        return m_Cube;

    case VolumeShapes::Cylinder:
        return m_Cylinder;

    case VolumeShapes::Sphere:
        return m_Sphere;

    case VolumeShapes::Capsule:
        return m_Capsule;
    }

    return NULL;
}