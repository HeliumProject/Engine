
#include "Bullet/Bullet.h"
#include "Engine/Asset.h"
#include "Engine/AssetType.h"
#include "Math/Vector3.h"

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btCollisionShape;

template <class T>
class btAlignedObjectArray;

namespace Helium
{
    class BulletBodyDefinition;

    class HELIUM_BULLET_API BulletWorldDefinition : public Asset
    {
        HELIUM_DECLARE_ASSET(Helium::BulletWorldDefinition, Helium::Asset);
        static void PopulateComposite( Reflect::Composite& comp );

        Helium::Vector3 m_Gravity;
    };

    class HELIUM_BULLET_API BulletWorld
    {
    public:
        void Initialize(/*BulletWorldDefinition *pWorldDefinition*/ Helium::Vector3 gravity);
        ~BulletWorld();

        void BuildTestObjects();
        void AddRigidBody(const BulletBodyDefinition *pBodyDefinition) { }

        void Simulate(float dt);

    private:
        btDefaultCollisionConfiguration *m_CollisionConfiguration;
	    btCollisionDispatcher* m_Dispatcher;
	    btBroadphaseInterface* m_OverlappingPairCache;
	    btSequentialImpulseConstraintSolver* m_Solver;
        btDiscreteDynamicsWorld * m_DynamicsWorld;
	    btAlignedObjectArray<btCollisionShape*> *m_CollisionShapes;
    };
}
