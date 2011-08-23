
#include "Engine/GameObject.h"

namespace Helium
{
    class TestGameObject : public Helium::GameObject
    {
        HELIUM_DECLARE_OBJECT( TestGameObject, GameObject );
    public:
        float m_TestValue1;
        float m_TestValue2;

        static void PopulateComposite( Reflect::Composite& comp);
    };
}
