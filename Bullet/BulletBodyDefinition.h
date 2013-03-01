
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
    struct BulletShape;

    class HELIUM_BULLET_API BulletBodyDefinition : public Asset
    {
        HELIUM_DECLARE_ASSET(Helium::BulletBodyDefinition, Helium::Asset);
        static void PopulateComposite( Reflect::Composite& comp );

        Helium::DynamicArray<BulletShape *> m_Shapes;
        float m_Mass;
    };
}
