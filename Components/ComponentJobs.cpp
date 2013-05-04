
#include "ComponentsPch.h"
#include "Engine/Components.h"
#include "Components/ComponentJobs.h"
#include "Engine/ComponentQuery.h"
#include <limits>
#include <vector>

#include "Foundation/DynamicArray.h"
#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/RotateComponent.h"

using namespace Helium;

//////////////////////////////////////////////////////////////////////////

void ClearTransformComponentDirtyFlags()
{
    DynamicArray<TransformComponent *> transformComponents;
    Helium::Components::GetAllComponents<TransformComponent>(transformComponents);
 
    for (DynamicArray<TransformComponent *>::Iterator iter = transformComponents.Begin();
        iter != transformComponents.End(); ++iter)
    {
        (*iter)->ClearDirtyFlag();
    }
}


void Helium::ClearTransformComponentDirtyFlagsTask::DefineContract( TaskContract &rContract )
{
    rContract.ExecuteAfter<StandardDependencies::Render>();
}

HELIUM_DEFINE_TASK(ClearTransformComponentDirtyFlagsTask, ClearTransformComponentDirtyFlags)

//////////////////////////////////////////////////////////////////////////

class UpdateRotateComponentsQuery : public ComponentQuery<RotateComponent, TransformComponent>
{
    virtual void HandleTuple(RotateComponent *pRotate, TransformComponent *pTransform)
    {
        pRotate->ApplyRotation(pTransform);
    }
};

void UpdateRotatorComponents()
{
    UpdateRotateComponentsQuery UpdateRotateComponents; 
    UpdateRotateComponents.Run();
}


void Helium::UpdateRotatorComponentsTask::DefineContract( TaskContract &rContract )
{
    rContract.ExecuteBefore<StandardDependencies::Render>();
    rContract.ExecuteAfter<StandardDependencies::ReceiveInput>();
}

HELIUM_DEFINE_TASK(UpdateRotatorComponentsTask, UpdateRotatorComponents)

//////////////////////////////////////////////////////////////////////////

class UpdateMeshComponentsQuery : public ComponentQuery<TransformComponent, MeshComponent>
{
    virtual void HandleTuple(TransformComponent *pTransform, MeshComponent *pMeshComponent)
    {
        pMeshComponent->Update(pMeshComponent->m_OwningSet->GetWorld(), pTransform);
    }
};

void UpdateMeshComponents()
{
    UpdateMeshComponentsQuery updateMeshComponents; 
    updateMeshComponents.Run();
}

void Helium::UpdateMeshComponentsTask::DefineContract( TaskContract &rContract )
{
    rContract.ExecuteBefore<StandardDependencies::Render>();
    rContract.ExecuteAfter<UpdateRotatorComponentsTask>();
}

HELIUM_DEFINE_TASK(UpdateMeshComponentsTask, UpdateMeshComponents);
