
#include "ExampleGamePch.h"
#include "ExampleGame/Components/GameLogic/EnemyWaveManager.h"

#include "Framework/WorldManager.h"
#include "Reflect/TranslatorDeduction.h"
#include "ExampleGame/Components/GameLogic/PlayerManager.h"
#include "Components/TransformComponent.h"

using namespace Helium;
using namespace ExampleGame;

//////////////////////////////////////////////////////////////////////////
// EnemyWaveFormation
HELIUM_DEFINE_ABSTRACT( ExampleGame::EnemyWaveFormation )

//////////////////////////////////////////////////////////////////////////
// EnemyWaveFormation_Circle
HELIUM_DEFINE_CLASS( ExampleGame::EnemyWaveFormation_Circle )

ExampleGame::EnemyWaveFormation_Circle::EnemyWaveFormation_Circle()
{

}

Helium::Simd::Vector3 ExampleGame::EnemyWaveFormation_Circle::GetSpawnLocation( ParameterSet_ActionSpawnEnemyWave *parameters, int index )
{
	float radians = ( static_cast<float>( index ) / static_cast<float>( parameters->m_Count ) ) * static_cast<float>(HELIUM_TWOPI);

	return parameters->m_Location + Helium::Simd::Vector3( parameters->m_Distance * Helium::Sin( radians ), parameters->m_Distance * Helium::Cos( radians ), 0.0f);
}


//////////////////////////////////////////////////////////////////////////
// EnemyWaveDefinition
HELIUM_IMPLEMENT_ASSET( ExampleGame::EnemyWaveDefinition, ExampleGame, 0 );

void EnemyWaveDefinition::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &EnemyWaveDefinition::m_Formation, "m_Formation" );
	comp.AddField( &EnemyWaveDefinition::m_Entity, "m_Entity" );
}

ExampleGame::EnemyWaveDefinition::EnemyWaveDefinition()
{

}

//////////////////////////////////////////////////////////////////////////
// ActionSpawnEnemyWave
HELIUM_IMPLEMENT_ASSET( ExampleGame::ActionSpawnEnemyWave, ExampleGame, 0 );

void ActionSpawnEnemyWave::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &ActionSpawnEnemyWave::m_WaveDefinition, "m_WaveDefinition" );
	comp.AddField( &ActionSpawnEnemyWave::m_DefaultParameters, "m_DefaultParameters" );
}

void ActionSpawnEnemyWave::PerformAction( World &pWorld, ParameterSet *pParamSet )
{
	EnemyWaveManagerComponent *pWaveManager = pWorld.GetComponents().GetFirst<EnemyWaveManagerComponent>();
	HELIUM_ASSERT( pWaveManager );

	ParameterSet_ActionSpawnEnemyWave *pParams = pParamSet->FindParameterSet<ParameterSet_ActionSpawnEnemyWave>();

	if (!pParams)
	{
		pParams = m_DefaultParameters.Get();
		ExampleGame::PlayerInfo &playerInfo = pWorld.GetComponents().GetFirst<PlayerManagerComponent>()->GetPlayerInfo(0);
		
		if (playerInfo.m_PlayerEntity)
		{
			PlayerComponent *pPlayerComponent = playerInfo.m_PlayerEntity->GetFirst<PlayerComponent>();
			if (pPlayerComponent && pPlayerComponent->m_Avatar)
			{
				pParams->m_Location = pPlayerComponent->m_Avatar->GetFirst<TransformComponent>()->GetPosition();
			}
		}
	}

	HELIUM_ASSERT(pParams);

	pWaveManager->GetWaveManager().SpawnWave( m_WaveDefinition, pParams );
}

//////////////////////////////////////////////////////////////////////////
// ParameterSet_ActionSpawnEnemyWave
HELIUM_DEFINE_CLASS( ExampleGame::ParameterSet_ActionSpawnEnemyWave );

ExampleGame::ParameterSet_ActionSpawnEnemyWave::ParameterSet_ActionSpawnEnemyWave()
: m_Count( 5 )
, m_Distance( 200.0f )
, m_Location( Simd::Vector3::Zero )
{

}

void ParameterSet_ActionSpawnEnemyWave::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &ParameterSet_ActionSpawnEnemyWave::m_Location, "m_Location" );
	comp.AddField( &ParameterSet_ActionSpawnEnemyWave::m_Count, "m_Count" );
	comp.AddField( &ParameterSet_ActionSpawnEnemyWave::m_Distance, "m_Distance" );
}

//////////////////////////////////////////////////////////////////////////
// PredicateEnemyWaveAlive

HELIUM_IMPLEMENT_ASSET( ExampleGame::PredicateEnemyWaveAlive, ExampleGame, 0 );

void PredicateEnemyWaveAlive::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &PredicateEnemyWaveAlive::m_WaveDefinition, "m_WaveDefinition" );
	comp.AddField( &PredicateEnemyWaveAlive::m_FractionAlive, "m_FractionAlive" );
}

bool PredicateEnemyWaveAlive::Evaluate( World &pWorld, ParameterSet *pParamSet )
{
	EnemyWaveManagerComponent *pEnemyWaveManager = pWorld.GetComponents().GetFirst<EnemyWaveManagerComponent>();

	float fractionAlive = pEnemyWaveManager->GetWaveManager().GetPercentAlive( m_WaveDefinition );

	return (fractionAlive + HELIUM_EPSILON) > m_FractionAlive;
}

//////////////////////////////////////////////////////////////////////////
// EnemyWaveManager
void EnemyWaveManager::Initialize(World *pWorld)
{
	m_pWorld = pWorld;
}

void EnemyWaveManager::Update( float dt )
{
	for (size_t i = m_ActiveWaves.GetSize() - 1; i < m_ActiveWaves.GetSize(); --i)
	{
		bool hasEntityRemaining = false;

		for (Helium::DynamicArray< WaveEntityState >::Iterator entity_iter = m_ActiveWaves[i].m_Entities.Begin(); 
			entity_iter != m_ActiveWaves[i].m_Entities.End(); ++entity_iter)
		{
			if (entity_iter->m_Entity)
			{
				hasEntityRemaining = true;
				break;
			}
		}

		m_ActiveWaves.RemoveSwap(i);
	}
}

void EnemyWaveManager::SpawnWave( EnemyWaveDefinition *pWave, ParameterSet_ActionSpawnEnemyWave *pParameters )
{
	HELIUM_ASSERT(pParameters);

	WaveState *pWaveState = m_ActiveWaves.New();
	pWaveState->m_Entities.Reserve(pParameters->m_Count);

	for (int i = 0; i < pParameters->m_Count; ++i)
	{
		HELIUM_ASSERT(pWave->m_Formation);
		Helium::Simd::Vector3 location = pWave->m_Formation->GetSpawnLocation( pParameters, i );
		HELIUM_TRACE(
			TraceLevels::Info,
			"Spawn wave %d: %f %f %f\n",
			i,
			location.GetElement(0), location.GetElement(1), location.GetElement(2));

		ParameterSetBuilder builder;
		ParameterSet_InitLocated *pInitLocated = builder.AddParameterSet<ParameterSet_InitLocated>();
		pInitLocated->m_Position = location;

		HELIUM_ASSERT( pWave->m_Entity );
		Entity *pEntity = m_pWorld->GetRootSlice()->CreateEntity(pWave->m_Entity, builder.GetSet());

		WaveEntityState *pEntityState = pWaveState->m_Entities.New();
		pEntityState->m_Entity = pEntity;
	}
}

float ExampleGame::EnemyWaveManager::GetPercentAlive( EnemyWaveDefinition *pDefinition )
{
	float returnValue = 0.0f;

	for (DynamicArray< WaveState >::Iterator wave_iter = m_ActiveWaves.Begin();
		wave_iter != m_ActiveWaves.End(); ++wave_iter)
	{
		if (wave_iter->m_WaveDefinition.Get() == pDefinition)
		{
			float fractionalValue = 1.0f / static_cast<float>(wave_iter->m_Entities.GetSize());

			for (Helium::DynamicArray< WaveEntityState >::Iterator entity_iter = wave_iter->m_Entities.Begin(); 
				entity_iter != wave_iter->m_Entities.End(); ++entity_iter)
			{
				if (entity_iter->m_Entity)
				{
					returnValue += fractionalValue;
				}
			}
		}
	}

	return returnValue;
}

//////////////////////////////////////////////////////////////////////////
// EnemyWaveManagerComponent

HELIUM_DEFINE_COMPONENT( ExampleGame::EnemyWaveManagerComponent, 1 );

void EnemyWaveManagerComponent::PopulateMetaType( Reflect::MetaStruct &structure )
{

}

void EnemyWaveManagerComponent::Initialize( const EnemyWaveManagerComponentDefinition &pDefinition )
{
	m_WaveManager.Initialize( GetWorld() );
}

//////////////////////////////////////////////////////////////////////////
// EnemyWaveManagerComponentDefinition

HELIUM_DEFINE_CLASS( ExampleGame::EnemyWaveManagerComponentDefinition );

void EnemyWaveManagerComponentDefinition::PopulateMetaType( Helium::Reflect::MetaStruct& comp )
{
}

//////////////////////////////////////////////////////////////////////////
// TaskUPdateEnemyWaveManager
void DoUpdateEnemyWaveManager( EnemyWaveManagerComponent *c )
{
	c->GetEnemyWaveManager().Update(WorldManager::GetStaticInstance().GetFrameDeltaSeconds());
}

void TaskUpdateEnemyWaveManager::DefineContract( TaskContract &rContract )
{
	rContract.ExecutesWithin<StandardDependencies::PostPhysicsGameplay>();
}

HELIUM_DEFINE_TASK( TaskUpdateEnemyWaveManager, (ForEachWorld< QueryComponents< EnemyWaveManagerComponent, DoUpdateEnemyWaveManager > >), TickTypes::Gameplay );