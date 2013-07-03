
#include "BulletPch.h"
#include "Bullet/BulletWorld.h"
#include "Bullet/BulletWorldDefinition.h"
#include "Bullet/BulletBodyComponent.h"
#include "Bullet/BulletWorldComponent.h"

using namespace Helium;

void InternalTickCallback(btDynamicsWorld *world, btScalar timeStep)
{
	BulletWorld * pWorld = static_cast<BulletWorld *>( world->getWorldUserInfo() );

	ComponentManager &pComponentManager = *static_cast<BulletWorldComponent *>(world->getWorldUserInfo())->GetComponentManager();
	for (ComponentIteratorT<HasPhysicalContactsComponent> iter( pComponentManager ); iter.GetBaseComponent(); iter.Advance())
	{
		//HELIUM_TRACE( TraceLevels::Debug, "iter->m_EndFrameTouching.Clear %d\n", iter->m_EndFrameTouching.GetSize());
		iter->m_EndFrameTouching.Clear();
	}

	int numManifolds = world->getDispatcher()->getNumManifolds();
	for (int i=0;i<numManifolds;i++)
	{
		btPersistentManifold* contactManifold =  world->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = static_cast<const btCollisionObject*>(contactManifold->getBody0());
		const btCollisionObject* obB = static_cast<const btCollisionObject*>(contactManifold->getBody1());
		
		BulletBodyComponent *pBodyComponentA = static_cast<BulletBodyComponent *>( obA->getUserPointer() );
		BulletBodyComponent *pBodyComponentB = static_cast<BulletBodyComponent *>( obB->getUserPointer() );

		bool trackACollisions = pBodyComponentA && pBodyComponentA->ShouldTrackCollisions();
		bool trackBCollisions = pBodyComponentB && pBodyComponentB->ShouldTrackCollisions();

		trackACollisions = pBodyComponentA->GetShouldTrackPhysicalContact( pBodyComponentB );
		trackBCollisions = pBodyComponentB->GetShouldTrackPhysicalContact( pBodyComponentA );

		if ( trackACollisions || trackBCollisions )
		{
			int numContacts = contactManifold->getNumContacts();

			if (numContacts)
			{
				if ( trackACollisions )
				{
					HasPhysicalContactsComponent *pContacts = pBodyComponentA->GetOrCreateHasPhysicalContactsComponent();
					pContacts->m_EverTouchedThisFrame.Insert( pBodyComponentB->GetEntity() );

					// TODO: Only need to do this on last subtick really
					pContacts->m_EndFrameTouching.Insert( pBodyComponentB->GetEntity() );
				}

				if ( trackBCollisions )
				{
					HasPhysicalContactsComponent *pContacts = pBodyComponentB->GetOrCreateHasPhysicalContactsComponent();
					pContacts->m_EverTouchedThisFrame.Insert( pBodyComponentA->GetEntity() );

					// TODO: Only need to do this on last subtick really
					pContacts->m_EndFrameTouching.Insert( pBodyComponentA->GetEntity() );
				}
			}

			// If we want more complex collision tracking than just touch, we could maybe run the below code given the existence of a "complex touch tracking" flag
#if 0
			int numContacts = contactManifold->getNumContacts();
			for (int j=0;j<numContacts;j++)
			{
				HELIUM_TRACE(
					TraceLevels::Debug,
					"%d\n",
					numContacts);

				btManifoldPoint& pt = contactManifold->getContactPoint(j);
				if (pt.getDistance()<0.f)
				{
					Simd::Vector3 ptA;
					Simd::Vector3 ptB;
					Simd::Vector3 normalB;

					ConvertFromBullet( pt.getPositionWorldOnA(), ptA );
					ConvertFromBullet( pt.getPositionWorldOnB(), ptB );
					ConvertFromBullet( pt.m_normalWorldOnB, normalB );

					if ( trackACollisions )
					{
						HasPhysicalContactsComponent *pContacts = pBodyComponentA->GetOrCreateHasPhysicalContactsComponent();
						ContactInfo *pInfo = pContacts->CreateContact();
						pInfo->m_pEntity = pBodyComponentB->GetEntity();
						pInfo->m_OurPosition = ptA;
						pInfo->m_TheirPosition = ptB;
						pInfo->m_Normal = normalB;
					}

					if ( trackBCollisions )
					{
						Simd::Vector3 normalA;
						normalB.GetNegated( normalA );

						HasPhysicalContactsComponent *pContacts = pBodyComponentB->GetOrCreateHasPhysicalContactsComponent();
						ContactInfo *pInfo = pContacts->CreateContact();
						pInfo->m_pEntity = pBodyComponentA->GetEntity();
						pInfo->m_OurPosition = ptB;
						pInfo->m_TheirPosition = ptA;
						pInfo->m_Normal = normalA;
					}
				}
			}
#endif
		}
	}
}

void BulletWorld::Initialize(const BulletWorldDefinition &rWorldDefinition)
{	
	// collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	m_CollisionConfiguration = new btDefaultCollisionConfiguration();

	// use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	m_Dispatcher = new btCollisionDispatcher(m_CollisionConfiguration);

	// btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	m_OverlappingPairCache = new btDbvtBroadphase();

	// the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	m_Solver = new btSequentialImpulseConstraintSolver;
	
	m_DynamicsWorld = new btDiscreteDynamicsWorld(
		m_Dispatcher,
		m_OverlappingPairCache,
		m_Solver,
		m_CollisionConfiguration);

	btVector3 gravity;
	//ConvertToBullet(pWorldDefinition->m_Gravity, gravity);
	ConvertToBullet(rWorldDefinition.m_Gravity, gravity);

	m_DynamicsWorld->setGravity(gravity);
	m_DynamicsWorld->setInternalTickCallback(&InternalTickCallback, this, false);
}

BulletWorld::~BulletWorld()
{
	delete m_DynamicsWorld;
	delete m_Solver;
	delete m_OverlappingPairCache;
	delete m_Dispatcher;
	delete m_CollisionConfiguration;
}

void BulletWorld::Simulate( float dt )
{
	m_DynamicsWorld->stepSimulation(dt,10);
}
