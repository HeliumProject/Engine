
#include "Bullet/Bullet.h"
#include "Bullet/BulletShapes.h"
#include "Engine/Asset.h"
#include "Math/Vector3.h"

template <class T>
class btAlignedObjectArray;

namespace Helium
{
    struct BulletShape;
    typedef Helium::StrongPtr<BulletShape> BulletShapePtr;

    struct HELIUM_BULLET_API BulletBodyDefinition : public Asset
    {
    public:
        HELIUM_DECLARE_ASSET(Helium::BulletBodyDefinition, Helium::Asset);
        static void PopulateStructure( Reflect::Structure& comp );

        BulletBodyDefinition();

        Helium::DynamicArray<BulletShapePtr> m_Shapes;
        float m_Restitution;
        bool m_Constrain2d;
    };
    typedef Helium::StrongPtr<BulletBodyDefinition> BulletBodyDefinitionPtr;
}
