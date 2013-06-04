
#include "ComponentsPch.h"
#include "Framework/Components.h"
#include "Components/ComponentJobs.h"
#include "Framework/ComponentQuery.h"
#include <limits>
#include <vector>

#include "Foundation/DynamicArray.h"
#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/RotateComponent.h"

using namespace Helium;

//////////////////////////////////////////////////////////////////////////
//
//void ClearTransformComponentDirtyFlags( World *pWorld )
//{
//    Components::ComponentListT<TransformComponent> list = pWorld->GetComponentManager()->GetAllocatedComponents<TransformComponent>();
// 
//    for (int i = 0; i < list.m_Count; ++i)
//    {
//        list.m_pComponents[i]->ClearDirtyFlag();
//    }
//}

void ClearTransformComponentDirtyFlags( TransformComponent *pWorld )
{
    pWorld->ClearDirtyFlag();
}

void Helium::ClearTransformComponentDirtyFlagsTask::DefineContract( TaskContract &rContract )
{
    rContract.ExecuteAfter<StandardDependencies::Render>();
}

//HELIUM_DEFINE_TASK(ClearTransformComponentDirtyFlagsTask, ForEachWorld<ClearTransformComponentDirtyFlags> )
HELIUM_DEFINE_TASK( ClearTransformComponentDirtyFlagsTask, (ForEachWorld< QueryComponents< TransformComponent, ClearTransformComponentDirtyFlags > >) )

//////////////////////////////////////////////////////////////////////////

void UpdateRotatorComponents(RotateComponent *pRotate, TransformComponent *pTransform)
{
    pRotate->ApplyRotation(pTransform);
}

void Helium::UpdateRotatorComponentsTask::DefineContract( TaskContract &rContract )
{
    rContract.ExecuteBefore<StandardDependencies::Render>();
    rContract.ExecuteAfter<StandardDependencies::ReceiveInput>();
}

HELIUM_DEFINE_TASK( UpdateRotatorComponentsTask, (ForEachWorld< QueryComponents< RotateComponent, TransformComponent, UpdateRotatorComponents > >) )

//////////////////////////////////////////////////////////////////////////

void UpdateMeshComponents(TransformComponent *pTransform, MeshComponent *pMeshComponent)
{
    pMeshComponent->Update(pTransform);
}

void Helium::UpdateMeshComponentsTask::DefineContract( TaskContract &rContract )
{
    rContract.ExecuteBefore<StandardDependencies::Render>();
    rContract.ExecuteAfter<UpdateRotatorComponentsTask>();
}

HELIUM_DEFINE_TASK( UpdateMeshComponentsTask, (ForEachWorld< QueryComponents< TransformComponent, MeshComponent, UpdateMeshComponents > >) );
