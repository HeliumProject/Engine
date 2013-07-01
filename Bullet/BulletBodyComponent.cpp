#include "BulletPch.h"
#include "Bullet/BulletBodyComponent.h"
#include "Reflect/TranslatorDeduction.h"
#include "Components/TransformComponent.h"

#include "Bullet/BulletWorldComponent.h"
#include "Framework/ComponentQuery.h"

#include "BulletCollision/CollisionDispatch/btGhostObject.h"

using namespace Helium;

HELIUM_IMPLEMENT_ASSET(Helium::BulletBodyComponentDefinition, Bullet, 0);

Map< Name, int > BulletBodyComponentDefinition::m_GroupNameRegistry;
int BulletBodyComponentDefinition::m_NextGroupNameFlag = 1;


void BulletBodyComponentDefinition::PopulateStructure( Reflect::Structure& comp )
{
	comp.AddField(&BulletBodyComponentDefinition::m_BodyDefinition, "m_BodyDefinition");
	comp.AddField(&BulletBodyComponentDefinition::m_InitialVelocity, "m_InitialVelocity");
	comp.AddField(&BulletBodyComponentDefinition::m_AssignedGroupFlags, "m_AssignedGroupFlags");
	comp.AddField(&BulletBodyComponentDefinition::m_TrackPhysicalContactGroupFlags, "m_TrackPhysicalContactGroupFlags");
	comp.AddField(&BulletBodyComponentDefinition::m_AssignedGroups, "m_AssignedGroups", Reflect::FieldFlags::Discard);
	comp.AddField(&BulletBodyComponentDefinition::m_TrackPhysicalContactGroupMask, "m_TrackPhysicalContactGroupMask", Reflect::FieldFlags::Discard);
}

BulletBodyComponentDefinition::BulletBodyComponentDefinition()
	: m_AssignedGroups(0)
	, m_TrackPhysicalContactGroupMask(0)
	, m_InitialVelocity(Simd::Vector3::Zero)
{

}

void Helium::BulletBodyComponentDefinition::FinalizeLoad()
{
	m_AssignedGroups = 0;
	m_TrackPhysicalContactGroupMask = 0;

	for (DynamicArray< Name >::Iterator flagIter = m_AssignedGroupFlags.Begin(); flagIter != m_AssignedGroupFlags.End(); ++flagIter)
	{
		int groupFlag = 0;

		Map< Name, int >::Iterator iter = m_GroupNameRegistry.Find( *flagIter );
		if (iter == m_GroupNameRegistry.End())
		{
			if (m_NextGroupNameFlag)
			{
				m_GroupNameRegistry.Insert(iter, Map< Name, int >::ValueType(*flagIter, m_NextGroupNameFlag));
				groupFlag = m_NextGroupNameFlag;

				m_NextGroupNameFlag = m_NextGroupNameFlag << 1;
			}
			else
			{
				// TODO: Warn we are out of groups
				HELIUM_ASSERT(0);
			}
		}
		else
		{
			groupFlag = iter->Second();
		}

		m_AssignedGroups |= groupFlag;
	}

	for (DynamicArray< Name >::Iterator flagIter = m_TrackPhysicalContactGroupFlags.Begin(); flagIter != m_TrackPhysicalContactGroupFlags.End(); ++flagIter)
	{
		int groupFlag = 0;

		Map< Name, int >::Iterator iter = m_GroupNameRegistry.Find( *flagIter );
		if (iter == m_GroupNameRegistry.End())
		{
			if (m_NextGroupNameFlag)
			{
				m_GroupNameRegistry.Insert(iter, Map< Name, int >::ValueType(*flagIter, m_NextGroupNameFlag));
				groupFlag = m_NextGroupNameFlag;

				m_NextGroupNameFlag = m_NextGroupNameFlag << 1;
			}
			else
			{
				// TODO: Warn we are out of groups
				HELIUM_ASSERT(0);
			}
		}
		else
		{
			groupFlag = iter->Second();
		}

		m_TrackPhysicalContactGroupMask |= groupFlag;
	}
}

HELIUM_DEFINE_COMPONENT(Helium::BulletBodyComponent, 128);

void BulletBodyComponent::PopulateStructure( Reflect::Structure& comp )
{

}

void BulletBodyComponent::Initialize( const BulletBodyComponentDefinition &definition )
{
}

void BulletBodyComponent::Finalize( const BulletBodyComponentDefinition &definition )
{
	BulletWorldComponent *pBulletWorldComponent = GetWorld()->GetComponents().GetFirst<BulletWorldComponent>();
	HELIUM_ASSERT( pBulletWorldComponent );
	HELIUM_ASSERT( definition.m_BodyDefinition );

	TransformComponent *pTransform = GetComponentCollection()->GetFirst<TransformComponent>();
	HELIUM_ASSERT( pTransform );

	m_Body.Initialize(
		*pBulletWorldComponent->GetBulletWorld(), 
		*definition.m_BodyDefinition, 
		pTransform ? pTransform->GetPosition() : Simd::Vector3::Zero, 
		pTransform ? pTransform->GetRotation() : Simd::Quat::IDENTITY);

	btVector3 velocity;
	ConvertToBullet(definition.m_InitialVelocity, velocity);
	m_Body.GetBody()->setLinearVelocity(velocity);
	m_Body.GetBody()->setUserPointer( this );

	m_AssignedGroups = definition.m_AssignedGroups;
	m_TrackPhysicalContactGroupMask = definition.m_TrackPhysicalContactGroupMask;
}

BulletBodyComponent::~BulletBodyComponent()
{
	if (m_Body.HasBody())
	{
		BulletWorldComponent *pBulletWorldComponent = GetWorld()->GetComponents().GetFirst<BulletWorldComponent>();

		m_Body.Destruct( *pBulletWorldComponent->GetBulletWorld() );
	}
}

void BulletBodyComponent::WakeUp()
{
	m_Body.GetBody()->activate();
}

void BulletBodyComponent::ApplyForce( const Simd::Vector3 &force )
{
	btVector3 bulletForce;
	ConvertToBullet(force, bulletForce);
	m_Body.GetBody()->activate();
	m_Body.GetBody()->applyCentralForce(bulletForce);
}

void BulletBodyComponent::SetVelocity( const Simd::Vector3 &velocity )
{
	btVector3 bulletVelocity;
	ConvertToBullet(velocity, bulletVelocity);
	m_Body.GetBody()->activate();
	m_Body.GetBody()->setLinearVelocity(bulletVelocity);
}

void BulletBodyComponent::SetAngularVelocity( const Simd::Vector3 &velocity )
{
	btVector3 bulletVelocity;
	ConvertToBullet(velocity, bulletVelocity);
	m_Body.GetBody()->activate();
	m_Body.GetBody()->setAngularVelocity(bulletVelocity);
}

//////////////////////////////////////////////////////////////////////////

void DoPreProcessPhysics( BulletBodyComponent *pBodyComponent, Helium::TransformComponent *pTransformComponent )
{
	if (pBodyComponent->GetBody().GetBody()->isKinematicObject())
	{
		pBodyComponent->GetBody().SetPosition(pTransformComponent->GetPosition());
		pBodyComponent->GetBody().SetRotation(pTransformComponent->GetRotation());
	}
};

HELIUM_DEFINE_TASK( PreProcessPhysics, (ForEachWorld< QueryComponents< BulletBodyComponent, TransformComponent, DoPreProcessPhysics > >) )

void PreProcessPhysics::DefineContract( Helium::TaskContract &rContract )
{
	rContract.Fulfills<Helium::StandardDependencies::ProcessPhysics>();
	rContract.ExecuteBefore<Helium::ProcessPhysics>();
}

//////////////////////////////////////////////////////////////////////////

void DoPostProcessPhysics( BulletBodyComponent *pBodyComponent, Helium::TransformComponent *pTransformComponent )
{
	Simd::Vector3 position;
	Simd::Quat rotation;

	// TODO: This extra copy is lame, either make a mutable Get on transform component or refactor motion state to talk straight to transform component
	pBodyComponent->GetBody().GetPosition(position);
	pTransformComponent->SetPosition(position);

	pBodyComponent->GetBody().GetRotation(rotation);
	pTransformComponent->SetRotation(rotation);
};

HELIUM_DEFINE_TASK( PostProcessPhysics, (ForEachWorld< QueryComponents< BulletBodyComponent, TransformComponent, DoPostProcessPhysics > >) )

void PostProcessPhysics::DefineContract( Helium::TaskContract &rContract )
{
	rContract.Fulfills<Helium::StandardDependencies::ProcessPhysics>();
	rContract.ExecuteAfter<Helium::ProcessPhysics>();
}
