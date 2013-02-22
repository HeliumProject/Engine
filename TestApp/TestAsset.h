
#include "Engine/Asset.h"

namespace Helium
{
    class TestAsset1 : public Helium::Asset
    {
        HELIUM_DECLARE_ASSET( TestAsset1, Asset );
    public:
        float m_TestValue1;
        float m_TestValue2;

        static void PopulateComposite( Reflect::Composite& comp);
    };
        
    class TestAsset3 : public Helium::Asset
    {
        HELIUM_DECLARE_ASSET( TestAsset3, Asset );
    public:
        float m_TestValue1;
        float m_TestValue2;

        static void PopulateComposite( Reflect::Composite& comp);
    };
    
    class TestAsset2 : public Helium::Asset
    {
        HELIUM_DECLARE_ASSET( TestAsset2, Asset );
    public:
        float m_TestValue1;
        Helium::StrongPtr<TestAsset3> m_TestReference;
        Helium::StrongPtr<TestAsset3> m_TestDeepCopy;
        
        static void PopulateComposite( Reflect::Composite& comp);
    };
}
