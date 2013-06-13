
#include "BulletPch.h"
#include "Bullet/BulletWorld.h"
#include "Bullet/BulletWorldDefinition.h"

using namespace Helium;

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
