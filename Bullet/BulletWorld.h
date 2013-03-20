
#pragma once 

#include "Bullet/Bullet.h"
#include "Math/Vector3.h"

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btCollisionShape;
class btDynamicsWorld;

template <class T>
class btAlignedObjectArray;

namespace Helium
{
    class BulletWorldDefinition;

    class HELIUM_BULLET_API BulletWorld
    {
    public:
        ~BulletWorld();
        
        void Initialize(const BulletWorldDefinition &rWorldDefinition);

        btDynamicsWorld *GetBulletWorld() { return m_DynamicsWorld; }

        void Simulate(float dt);

    private:
        btDefaultCollisionConfiguration *m_CollisionConfiguration;
	    btCollisionDispatcher* m_Dispatcher;
	    btBroadphaseInterface* m_OverlappingPairCache;
	    btSequentialImpulseConstraintSolver* m_Solver;
        btDynamicsWorld * m_DynamicsWorld;
    };
    typedef Helium::StrongPtr< BulletWorld > BulletWorldPtr;
}
