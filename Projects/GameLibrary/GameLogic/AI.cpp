
#include "GameLibraryPch.h"

#include "AI.h"
#include "GameLibrary/GameLogic/AvatarController.h"
#include "GameLibrary/GameLogic/PlayerManager.h"
#include "Foundation/Numeric.h"
#include "Framework/World.h"

using namespace Helium;
using namespace GameLibrary;

//////////////////////////////////////////////////////////////////////////
// AIComponentChasePlayer
HELIUM_DEFINE_COMPONENT( GameLibrary::AIComponentChasePlayer, 16 )

void AIComponentChasePlayer::Initialize( const AIComponentChasePlayerDefinition &definition )
{

}

//////////////////////////////////////////////////////////////////////////
// AIComponentChasePlayerDefinition

HELIUM_DEFINE_CLASS( GameLibrary::AIComponentChasePlayerDefinition )

void AIComponentChasePlayerDefinition::PopulateMetaType( Helium::Reflect::MetaStruct& comp )
{

}


//////////////////////////////////////////////////////////////////////////
// TaskProcessAI

typedef DynamicArray< Pair< PlayerComponent *, Simd::Vector3 > > PlayerList;
static PlayerList g_PlayerList;

void UpdateAI_ChasePlayer( AIComponentChasePlayer *pAiComponent, AvatarControllerComponent *pController )
{
	PlayerComponent *pTarget = NULL;
	float pTargetDistanceSquared = NumericLimits<float>::Maximum;
	Simd::Vector3 targetPosition = Simd::Vector3::Zero;
	Simd::Vector3 myPosition = Simd::Vector3::Zero;

	TransformComponent *pTransform = pAiComponent->GetComponentCollection()->GetFirst<TransformComponent>();
	
	if ( pTransform )
	{
		myPosition = pTransform->GetPosition();
		for (PlayerList::Iterator iter = g_PlayerList.Begin(); iter != g_PlayerList.End(); ++iter)
		{
			float d = (iter->Second() - myPosition).GetMagnitudeSquared();
			if ( d < pTargetDistanceSquared )
			{
				pTargetDistanceSquared = d;
				pTarget = iter->First();
				targetPosition = iter->Second();
			}
		}
	}
	
	if ( pTarget )
	{
		Simd::Vector3 moveDir = (targetPosition - myPosition).GetNormalized();

		pController->m_MoveDir.SetX( moveDir.GetElement(0));
		pController->m_MoveDir.SetY( moveDir.GetElement(1));
		pController->m_AimDir = Simd::Vector3::Zero;
		pController->m_bShoot = false;
	}
	else
	{
		pController->m_MoveDir = Simd::Vector2::Zero;
		pController->m_AimDir = Simd::Vector3::Zero;
		pController->m_bShoot = false;
	}
}

void ProcessAI( World *pWorld )
{
	g_PlayerList.Clear();

	for ( ImplementingComponentIterator<PlayerComponent> iterator( *pWorld->GetComponentManager() ); iterator.GetBaseComponent(); iterator.Advance() )
	{
		if ( iterator->m_Avatar )
		{
			TransformComponent *pTransform = iterator->m_Avatar->GetFirst<TransformComponent>();

			if ( pTransform )
			{
				g_PlayerList.New( *iterator, pTransform->GetPosition() );
			}
		}
	}

	QueryComponents< AIComponentChasePlayer, AvatarControllerComponent, UpdateAI_ChasePlayer >( pWorld );
}

HELIUM_DEFINE_TASK( TaskProcessAI, ( ForEachWorld< ProcessAI > ), TickTypes::Gameplay )

void TaskProcessAI::DefineContract( Helium::TaskContract &rContract )
{
	rContract.ExecuteAfter<Helium::StandardDependencies::ReceiveInput>();
	rContract.ExecuteBefore<Helium::StandardDependencies::ProcessPhysics>();
}
