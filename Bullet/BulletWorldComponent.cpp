#include "BulletPch.h"
#include "Bullet/BulletWorldComponent.h"
#include "Reflect/TranslatorDeduction.h"
#include "Framework/WorldManager.h"
#include "Framework/ComponentQuery.h"
#include "Bullet/HasPhysicalContacts.h"
#include "Framework/Entity.h"

using namespace Helium;

HELIUM_DEFINE_CLASS(Helium::BulletWorldComponentDefinition);

void Helium::BulletWorldComponentDefinition::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField(&BulletWorldComponentDefinition::m_WorldDefinition, "m_WorldDefinition");
}

HELIUM_DEFINE_COMPONENT(Helium::BulletWorldComponent, 32);

void Helium::BulletWorldComponent::PopulateMetaType( Reflect::MetaStruct& comp )
{

}

Helium::BulletWorldComponent::BulletWorldComponent()
	: m_World(0)
{
	
}

Helium::BulletWorldComponent::~BulletWorldComponent()
{
	delete m_World;
	m_World = 0;
}

void Helium::BulletWorldComponent::Initialize( const BulletWorldComponentDefinition &definition )
{
	HELIUM_ASSERT(!m_World);
	m_World = new BulletWorld();
	m_World->Initialize(definition.m_WorldDefinition);
	m_World->GetBulletWorld()->setWorldUserInfo(this);
}

void Helium::BulletWorldComponent::Simulate( float dt )
{
	m_World->Simulate(dt);
}

//////////////////////////////////////////////////////////////////////////

void DoProcessPhysics( BulletWorldComponent *pComponent )
{
	ComponentManager *pComponentManager = pComponent->GetComponentManager();
	HELIUM_ASSERT( pComponentManager );

	// For each HasPhysicalContactsComponent
	for (ComponentIteratorT<HasPhysicalContactsComponent> iter( *pComponentManager ); iter.GetBaseComponent(); iter.Advance())
	{
		iter->m_BeginFrameTouching.Clear();
		for (Set<EntityWPtr>::Iterator wptr_iter = iter->m_EndFrameTouching.Begin(); wptr_iter != iter->m_EndFrameTouching.End(); ++wptr_iter)
		{
			Entity *pEntity = *wptr_iter;
			if (pEntity)
			{
				iter->m_BeginFrameTouching.Insert(pEntity);
			}
		}
		iter->m_EverTouchedThisFrame = iter->m_BeginFrameTouching;
		iter->m_EndFrameTouching.Clear();
	}

	WorldManager* pWorldManager = WorldManager::GetInstance();
	HELIUM_ASSERT( pWorldManager );

	pComponent->Simulate( pWorldManager->GetFrameDeltaSeconds() );

	for (ComponentIteratorT<HasPhysicalContactsComponent> iter( *pComponentManager ); iter.GetBaseComponent(); iter.Advance())
	{
		// We care about
		// - BeginTouch = m_EverTouchedThisFrame - m_BeginFrameTouching
		// - EndTouch = m_EverTouchedThisFrame - m_SubtickTouching
		// - IsTouching = m_SubtickTouching
		// - Were we still touching it at end of physics tick? We have what is touching in last subtick in m_SubtickTouching, combine with m_EverTouchedThisFrame to populate m_EndTouch
		//   RATIONALE: Bouncing is important and must not get lost. Untouching a retouching during a frame is generally
		//   something we don't care about since it would never get rendered. We want BeginTouch, EndTouch, and Touching
		//   queries.
		HasPhysicalContactsComponent *pHasPhysicalContacts = *iter;

		pHasPhysicalContacts->m_BeginTouch.Clear();
		pHasPhysicalContacts->m_EndTouch.Clear();

		if (pHasPhysicalContacts->m_EverTouchedThisFrame.IsEmpty())
		{
			// These have to be cleared since we're using deferred delete
			HELIUM_ASSERT( pHasPhysicalContacts->m_BeginFrameTouching.IsEmpty() );
			HELIUM_ASSERT( pHasPhysicalContacts->m_EndFrameTouching.IsEmpty() );
			HELIUM_ASSERT( pHasPhysicalContacts->m_BeginTouch.IsEmpty() );
			HELIUM_ASSERT( pHasPhysicalContacts->m_EndTouch.IsEmpty() );
			HELIUM_ASSERT( pHasPhysicalContacts->m_EverTouchedThisFrame.IsEmpty() );
			pHasPhysicalContacts->FreeComponentDeferred();
			continue;
		}

		// TODO: I think there may be an O(n) way to do this if both sets are sorted
		for (Set<EntityWPtr>::Iterator ever_touched = pHasPhysicalContacts->m_EverTouchedThisFrame.Begin(); ever_touched != pHasPhysicalContacts->m_EverTouchedThisFrame.End(); ++ever_touched)
		{
			// For everything that touches in any subtick
			bool found = false;

			for (Set<EntityWPtr>::Iterator touching = pHasPhysicalContacts->m_BeginFrameTouching.Begin(); touching != pHasPhysicalContacts->m_BeginFrameTouching.End(); ++touching)
			{
				// See if it was touching when we started
				if (*touching == *ever_touched)
				{
					found = true;
					break;
				}
			}

			if (!found)
			{
				//HELIUM_TRACE(TraceLevels::Debug, "BeginTouch %x -> %x\n", pHasPhysicalContacts->GetOwner(), *ever_touched);
				// It's in ever touched, and not in touching, so we must have them untouching
				pHasPhysicalContacts->m_BeginTouch.Push(*ever_touched);
			}
		}

		// TODO: I think there may be an O(n) way to do this if both sets are sorted
		for (Set<EntityWPtr>::Iterator ever_touched = pHasPhysicalContacts->m_EverTouchedThisFrame.Begin(); ever_touched != pHasPhysicalContacts->m_EverTouchedThisFrame.End(); ++ever_touched)
		{
			// For everything that touches in any subtick
			bool found = false;

			for (Set<EntityWPtr>::Iterator touching = pHasPhysicalContacts->m_EndFrameTouching.Begin(); touching != pHasPhysicalContacts->m_EndFrameTouching.End(); ++touching)
			{
				// See if it was still touching at end of last subtick
				if (*touching == *ever_touched)
				{
					found = true;
					break;
				}
			}

			if (!found)
			{
				//HELIUM_TRACE(TraceLevels::Debug, "EndTouch %x -> %x\n", pHasPhysicalContacts->GetOwner(), *ever_touched);
				// It's in ever touched, and not in touching, so we must have them untouching
				pHasPhysicalContacts->m_EndTouch.Push(*ever_touched);
			}
		}
	}
};

HELIUM_DEFINE_TASK( ProcessPhysics, (ForEachWorld< QueryComponents< BulletWorldComponent, DoProcessPhysics > >), TickTypes::Gameplay )

void ProcessPhysics::DefineContract( Helium::TaskContract &rContract )
{
	rContract.ExecutesWithin<Helium::StandardDependencies::ProcessPhysics>();
}
