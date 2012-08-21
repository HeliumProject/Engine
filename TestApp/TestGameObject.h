
#include "Engine/GameObject.h"

namespace Helium
{
    class TestGameObject1 : public Helium::GameObject
    {
        HELIUM_DECLARE_OBJECT( TestGameObject1, GameObject );
    public:
        float m_TestValue1;
        float m_TestValue2;

        static void PopulateComposite( Reflect::Composite& comp);
    };
        
    class TestGameObject3 : public Helium::GameObject
    {
        HELIUM_DECLARE_OBJECT( TestGameObject3, GameObject );
    public:
        float m_TestValue1;
        float m_TestValue2;

        static void PopulateComposite( Reflect::Composite& comp);
    };
    
    class TestGameObject2 : public Helium::GameObject
    {
        HELIUM_DECLARE_OBJECT( TestGameObject2, GameObject );
    public:
        float m_TestValue1;
        Helium::StrongPtr<TestGameObject3> m_TestReference;
        Helium::StrongPtr<TestGameObject3> m_TestDeepCopy;
        
        static void PopulateComposite( Reflect::Composite& comp);
    };
}
