#include "BulletPch.h"
#include "Bullet/BulletEngine.h"
#include "Bullet/BulletBodyComponent.h"
#include "Reflect/TranslatorDeduction.h"
#include "Components/TransformComponent.h"

#include "Bullet/BulletWorldComponent.h"
#include "Framework/ComponentQuery.h"
#include "Framework/World.h"

#include "BulletCollision/CollisionDispatch/btGhostObject.h"

using namespace Helium;

HELIUM_DEFINE_CLASS(Helium::BulletBodyComponentDefinition);

void BulletBodyComponentDefinition::PopulateMetaType( Reflect::MetaStruct& comp )
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
	, m_FlagsCached(false)
	, m_InitialVelocity(Simd::Vector3::Zero)
{

}

void Helium::BulletBodyComponentDefinition::FinalizeLoad()
{
	m_AssignedGroups = 0;
	m_TrackPhysicalContactGroupMask = 0;
	m_FlagsCached = false;
}

void Helium::BulletBodyComponentDefinition::CacheFlags() const
{
	if (!m_FlagsCached && (!m_AssignedGroupFlags.IsEmpty() || !m_TrackPhysicalContactGroupFlags.IsEmpty()))
	{
		HELIUM_ASSERT( BulletSystemComponent::GetInstance() );
		if ( !BulletSystemComponent::GetInstance()->m_BodyFlags )
		{
			//HELIUM_TRACE(
			//	TraceLevels::Warning,
			//	"BulletBodyComponentDefinition::FinalizeLoad - Body '%s' uses body flags in m_AssignedGroupFlags or m_TrackPhysicalContactGroupFlags, but no flags are defined in the bullet system component\n",
			//	*GetPath().ToString());
		}
		else
		{
			if (!BulletSystemComponent::GetInstance()->m_BodyFlags->GetBitset(m_AssignedGroupFlags, m_AssignedGroups))
			{
				//HELIUM_TRACE(
				//	TraceLevels::Warning,
				//	"BulletBodyComponentDefinition::FinalizeLoad - Body '%s' refers to a body flag in field m_AssignedGroupFlags that does not exist. Is the flag defined in '%s'?\n",
				//	*GetPath().ToString(),
				//	*BulletSystemComponent::GetInstance()->m_BodyFlags->GetPath().ToString());
			}

			if (!BulletSystemComponent::GetInstance()->m_BodyFlags->GetBitset(m_TrackPhysicalContactGroupFlags, m_TrackPhysicalContactGroupMask))
			{
				//HELIUM_TRACE(
				//	TraceLevels::Warning,
				//	"BulletBodyComponentDefinition::FinalizeLoad - Body '%s' refers to a body flag in field m_TrackPhysicalContactGroupFlags that does not exist. Is the flag defined in '%s'?\n",
				//	*GetPath().ToString(),
				//	*BulletSystemComponent::GetInstance()->m_BodyFlags->GetPath().ToString());
			}
		}
	}

	m_FlagsCached = true;
}

HELIUM_DEFINE_COMPONENT(Helium::BulletBodyComponent, 128);

void BulletBodyComponent::PopulateMetaType( Reflect::MetaStruct& comp )
{

}

void BulletBodyComponent::Finalize( const BulletBodyComponentDefinition &definition )
{
	definition.CacheFlags();
	BulletWorldComponent *pBulletWorldComponent = GetWorld()->GetComponents().GetFirst<BulletWorldComponent>();
	HELIUM_ASSERT( pBulletWorldComponent );

	TransformComponent *pTransform = GetComponentCollection()->GetFirst<TransformComponent>();
	HELIUM_ASSERT( pTransform );

	m_Body.Initialize(
		*pBulletWorldComponent->GetBulletWorld(), 
		definition.m_BodyDefinition, 
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

HELIUM_DEFINE_TASK( PreProcessPhysics, (ForEachWorld< QueryComponents< BulletBodyComponent, TransformComponent, DoPreProcessPhysics > >), TickTypes::Gameplay )

void PreProcessPhysics::DefineContract( Helium::TaskContract &rContract )
{
	rContract.ExecutesWithin<Helium::StandardDependencies::ProcessPhysics>();
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

HELIUM_DEFINE_TASK( PostProcessPhysics, (ForEachWorld< QueryComponents< BulletBodyComponent, TransformComponent, DoPostProcessPhysics > >), TickTypes::Gameplay )

void PostProcessPhysics::DefineContract( Helium::TaskContract &rContract )
{
	rContract.ExecutesWithin<Helium::StandardDependencies::ProcessPhysics>();
	rContract.ExecuteAfter<Helium::ProcessPhysics>();
}
