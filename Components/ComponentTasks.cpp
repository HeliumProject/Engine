
#include "ComponentsPch.h"
#include "Framework/Components.h"
#include "Components/ComponentTasks.h"
#include "Framework/ComponentQuery.h"
#include <limits>
#include <vector>

#include "Foundation/DynamicArray.h"
#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/RotateComponent.h"
#include "Graphics/GraphicsManagerComponent.h"

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
	rContract.ExecuteBefore<StandardDependencies::ProcessPhysics>();
	rContract.ExecuteAfter<StandardDependencies::ReceiveInput>();
}

HELIUM_DEFINE_TASK( UpdateRotatorComponentsTask, (ForEachWorld< QueryComponents< RotateComponent, TransformComponent, UpdateRotatorComponents > >) )

//////////////////////////////////////////////////////////////////////////

static GraphicsScene *pGraphicsScene = NULL;

void UpdateMeshComponent(TransformComponent *pTransform, MeshComponent *pMeshComponent)
{
	pMeshComponent->Update( pGraphicsScene, pTransform );
}

void UpdateMeshComponents( World *pWorld )
{
	GraphicsManagerComponent *pGraphicsManager = pWorld->GetComponents().GetFirst<GraphicsManagerComponent>();
	HELIUM_ASSERT( pGraphicsManager );

	pGraphicsScene = pGraphicsManager->GetGraphicsScene();
	HELIUM_ASSERT( pGraphicsScene );

	QueryComponents< TransformComponent, MeshComponent, UpdateMeshComponent >( pWorld );
}

void Helium::UpdateMeshComponentsTask::DefineContract( TaskContract &rContract )
{
	rContract.ExecuteBefore<StandardDependencies::Render>();
	rContract.ExecuteAfter<StandardDependencies::ProcessPhysics>();
}

HELIUM_DEFINE_TASK( UpdateMeshComponentsTask, (ForEachWorld< UpdateMeshComponents >) );
