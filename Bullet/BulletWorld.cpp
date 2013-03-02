
#include "BulletPch.h"
#include "Bullet/BulletWorld.h"
#include "Bullet/BulletWorldDefinition.h"


#include "Framework/FrameworkDataDeduction.h"

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

    m_CollisionShapes = new btAlignedObjectArray<btCollisionShape*>();
}

BulletWorld::~BulletWorld()
{
    delete m_CollisionShapes;
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

//void Helium::BulletWorld::BuildTestObjects()
//{
//    
//	///-----initialization_end-----
//
//	///create a few basic rigid bodies
//	//btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.),btScalar(50.),btScalar(50.)));
//    btCollisionShape* groundShape = new btSphereShape(btScalar(50.));
//
//	//keep track of the shapes, we release memory at exit.
//	//make sure to re-use collision shapes among rigid bodies whenever possible!
//
//	m_CollisionShapes->push_back(groundShape);
//
//	btTransform groundTransform;
//	groundTransform.setIdentity();
//	groundTransform.setOrigin(btVector3(0,-50,0));
//
//	{
//		btScalar mass(0.);
//
//		//rigidbody is dynamic if and only if mass is non zero, otherwise static
//		bool isDynamic = (mass != 0.f);
//
//		btVector3 localInertia(0,0,0);
//		if (isDynamic)
//			groundShape->calculateLocalInertia(mass,localInertia);
//
//		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
//		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
//		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,groundShape,localInertia);
//		btRigidBody* body = new btRigidBody(rbInfo);
//
//		//add the body to the dynamics world
//		m_DynamicsWorld->addRigidBody(body);
//	}
//
//
//	{
//		//create a dynamic rigidbody
//
//		//btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
//		btCollisionShape* colShape = new btSphereShape(btScalar(1.));
//		m_CollisionShapes->push_back(colShape);
//
//		/// Create Dynamic Objects
//		btTransform startTransform;
//		startTransform.setIdentity();
//
//		btScalar	mass(1.f);
//
//		//rigidbody is dynamic if and only if mass is non zero, otherwise static
//		bool isDynamic = (mass != 0.f);
//
//		btVector3 localInertia(0,0,0);
//		if (isDynamic)
//			colShape->calculateLocalInertia(mass,localInertia);
//
//			startTransform.setOrigin(btVector3(5,10,3));
//		
//			//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
//			btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
//			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,colShape,localInertia);
//			btRigidBody* body = new btRigidBody(rbInfo);
//
//            //body->setLinearFactor(btVector3(1,1,0));
//			//body->setAngularFactor(btVector3(0,0,1));
//
//			m_DynamicsWorld->addRigidBody(body);
//	}
//
//}
