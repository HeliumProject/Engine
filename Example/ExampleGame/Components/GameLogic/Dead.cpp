#include "ExampleGamePch.h"

#include "Dead.h"
#include "Framework/WorldManager.h"
#include "ExampleGame/Components/GameLogic/Health.h"


using namespace Helium;
using namespace ExampleGame;

//////////////////////////////////////////////////////////////////////////
// DeadComponent

HELIUM_DEFINE_COMPONENT(ExampleGame::DeadComponent, 128);

void DeadComponent::PopulateMetaType( Reflect::MetaStruct& comp )
{

}

void DeadComponent::Initialize( float damageAmount )
{
	m_DamageAmount = damageAmount;
}

//////////////////////////////////////////////////////////////////////////
// DespawnOnDeathComponent

HELIUM_DEFINE_COMPONENT(ExampleGame::DespawnOnDeathComponent, 128);

void DespawnOnDeathComponent::PopulateMetaType( Reflect::MetaStruct& comp )
{

}

void DespawnOnDeathComponent::Initialize( const DespawnOnDeathComponentDefinition &definition )
{

}

//////////////////////////////////////////////////////////////////////////
// DespawnOnDeathComponentDefinition

HELIUM_DEFINE_CLASS(ExampleGame::DespawnOnDeathComponentDefinition);

void DespawnOnDeathComponentDefinition::PopulateMetaType( Reflect::MetaStruct& comp )
{

}

//////////////////////////////////////////////////////////////////////////
// TaskDestroyAllDead

void DoDestroyAllDead( DespawnOnDeathComponent *pDespawnOnDeathComponent, DeadComponent *pHealthComponent )
{
	pDespawnOnDeathComponent->GetEntity()->DeferredDestroy();
}

HELIUM_DEFINE_TASK( TaskDestroyAllDead, ( ForEachWorld< QueryComponents< DespawnOnDeathComponent, DeadComponent, DoDestroyAllDead > > ), TickTypes::Gameplay )

void TaskDestroyAllDead::DefineContract( Helium::TaskContract &rContract )
{
	rContract.ExecuteAfter<ExampleGame::KillAllWithZeroHealth>();
	rContract.ExecutesWithin<Helium::StandardDependencies::PostPhysicsGameplay>();
}
