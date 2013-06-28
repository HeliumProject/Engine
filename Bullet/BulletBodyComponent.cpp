#include "BulletPch.h"
#include "Bullet/BulletBodyComponent.h"
#include "Reflect/TranslatorDeduction.h"
#include "Components/TransformComponent.h"

#include "Bullet/BulletWorldComponent.h"
#include "Framework/ComponentQuery.h"

#include "BulletCollision/CollisionDispatch/btGhostObject.h"

using namespace Helium;

HELIUM_IMPLEMENT_ASSET(Helium::BulletBodyComponentDefinition, Bullet, 0);

void BulletBodyComponentDefinition::PopulateStructure( Reflect::Structure& comp )
{
	comp.AddField(&BulletBodyComponentDefinition::m_BodyDefinition, "m_BodyDefinition");
	comp.AddField(&BulletBodyComponentDefinition::m_InitialVelocity, "m_InitialVelocity");
}

BulletBodyComponentDefinition::BulletBodyComponentDefinition()
	: m_TrackPhysicalContactsGroup(0)
	, m_TrackPhysicalContactsMask(0)
	, m_InitialVelocity(Simd::Vector3::Zero)
{

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

	m_TrackPhysicalContactsGroup = definition.m_TrackPhysicalContactsGroup;
	m_TrackPhysicalContactsMask = definition.m_TrackPhysicalContactsMask;
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
