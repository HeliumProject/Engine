
#include "TestAppPch.h"

#include "Foundation/InitializerStack.h"
#include "Engine/Components.h"

using namespace Helium;
using namespace Helium::Components;

class TestHost : public Helium::Components::HasComponents
{

};

class TestComponentOne : public Component
{
public:
    TestComponentOne()
    {
        static int32_t next_id = 100;
        m_Id = next_id++;
    }

    int32_t m_Id;

    OBJECT_DECLARE_COMPONENT( TestComponentOne, Components::Component );
};

class TestComponentTwo : public TestComponentOne
{
public:
    OBJECT_DECLARE_COMPONENT( TestComponentTwo, TestComponentOne );
};

class TestComponentThree : public TestComponentOne
{
public:
    OBJECT_DECLARE_COMPONENT( TestComponentThree, TestComponentOne );
};

OBJECT_DEFINE_COMPONENT(TestComponentOne);
OBJECT_DEFINE_COMPONENT(TestComponentTwo);
OBJECT_DEFINE_COMPONENT(TestComponentThree);

//std::auto_ptr<Nocturnal::InitializerStack> InitStack;

class Components : public testing::Test
{
public:
    Components()
        : m_InitStack(true)
    {
    }

    void SetUp()
    {
        m_InitStack.Push(Helium::Components::Initialize, Helium::Components::Cleanup);
        m_InitStack.Push(Reflect::RegisterClassType<TestComponentOne>(TXT("TestComponentOne")));
        m_InitStack.Push(Reflect::RegisterClassType<TestComponentTwo>(TXT("TestComponentTwo")));
        m_InitStack.Push(Reflect::RegisterClassType<TestComponentThree>(TXT("TestComponentThree")));

        m_ComponentOneTypeId = TestComponentOne::RegisterComponentType(10);
        m_ComponentTwoTypeId = TestComponentTwo::RegisterComponentType(10);
        m_ComponentThreeTypeId = TestComponentThree::RegisterComponentType(10);
    }

    void TearDown()
    {

    }

    Helium::InitializerStack m_InitStack;

    Components::TypeId m_ComponentOneTypeId;
    Components::TypeId m_ComponentTwoTypeId;
    Components::TypeId m_ComponentThreeTypeId;
};

TEST_F(Components, ComponentTypeId)
{
    // Check GetType return values
    EXPECT_EQ(Helium::Components::GetType<TestComponentOne>(), m_ComponentOneTypeId);
    EXPECT_EQ(Helium::Components::GetType<TestComponentTwo>(), m_ComponentTwoTypeId);
    EXPECT_EQ(Helium::Components::GetType<TestComponentThree>(), m_ComponentThreeTypeId);
}

TEST_F(Components, HostFindOne)
{
    TestHost test_host;

    DynArray<TestComponentOne *> component_list_1;
    DynArray<TestComponentTwo *> component_list_2;
    DynArray<TestComponentThree *> component_list_3;

    // Nothing is attached
    EXPECT_TRUE(test_host.FindOneComponent<TestComponentOne>() == 0);
    EXPECT_TRUE(test_host.FindOneComponent<TestComponentTwo>() == 0);
    EXPECT_TRUE(test_host.FindOneComponent<TestComponentThree>() == 0);

    // Nothing is attached
    EXPECT_TRUE(test_host.FindOneComponentThatImplements<TestComponentOne>() == 0);
    EXPECT_TRUE(test_host.FindOneComponentThatImplements<TestComponentTwo>() == 0);
    EXPECT_TRUE(test_host.FindOneComponentThatImplements<TestComponentThree>() == 0);

    // Attach components
    TestComponentOne *c1 = test_host.Allocate<TestComponentOne>();
    TestComponentTwo *c2_1 = test_host.Allocate<TestComponentTwo>();
    TestComponentTwo *c2_2 = test_host.Allocate<TestComponentTwo>();
    TestComponentThree *c3_1 = test_host.Allocate<TestComponentThree>();
    TestComponentThree *c3_2 = test_host.Allocate<TestComponentThree>();

    // FindOne should return the exact component
    EXPECT_EQ(test_host.FindOneComponent<TestComponentOne>(), c1);
    //EXPECT_EQ(test_host.FindOneComponent<TestComponentTwo>(), c2);
    //EXPECT_EQ(test_host.FindOneComponent<TestComponentThree>(), c3);

    // FindOneThatImplements should now return *something* for each type
    EXPECT_TRUE(test_host.FindOneComponentThatImplements<TestComponentOne>() != 0);
    EXPECT_TRUE(test_host.FindOneComponentThatImplements<TestComponentTwo>() != 0);
    EXPECT_TRUE(test_host.FindOneComponentThatImplements<TestComponentThree>() != 0);

    test_host.FindAllComponents<TestComponentOne>(component_list_1);
    test_host.FindAllComponents<TestComponentTwo>(component_list_2);
    test_host.FindAllComponents<TestComponentThree>(component_list_3);

    EXPECT_TRUE(component_list_1.GetSize() == 1);
    EXPECT_TRUE(component_list_2.GetSize() == 2);
    EXPECT_TRUE(component_list_3.GetSize() == 2);

    // Take out the base component
    test_host.Free(*c1);
    test_host.Free(*c2_1);
    //test_host.Free(*c2_2);
    test_host.Free(*c3_2);
    //test_host.Free(*c3_1);

    // Now asking directly for c1 should fail, but an implementor of c1 should pass (returns c2 or c3)
    EXPECT_TRUE(test_host.FindOneComponent<TestComponentOne>() == 0);
    EXPECT_TRUE(test_host.FindOneComponentThatImplements<TestComponentOne>() != 0);
    
    component_list_1.Clear();
    test_host.FindAllComponents<TestComponentOne>(component_list_1);
    EXPECT_TRUE(component_list_1.GetSize() == 0);

    component_list_1.Clear();
    test_host.FindAllComponentsThatImplement<TestComponentOne>(component_list_1);
    EXPECT_TRUE(component_list_1.GetSize() == 2);

    component_list_2.Clear();
    test_host.FindAllComponentsThatImplement<TestComponentTwo>(component_list_2);
    EXPECT_TRUE(component_list_2.GetSize() == 1);

    test_host.Free(*c2_2);
    test_host.Free(*c3_1);
}

TEST_F(Components, HostAttachDetachIterate)
{
    TestHost test_host;
    DynArray<Component *> all_components;

    Helium::Components::Component *c1 = 0;
    Helium::Components::Component *c2 = 0; 
    Helium::Components::Component *c3 = 0;

    // Attach 10 components of each type and get direct pointer to 3 significant components
    for (int32_t i = 0; i < 10; ++i)
    {
        TestComponentOne *one = test_host.Allocate<TestComponentOne>();
        TestComponentOne *two = test_host.Allocate<TestComponentTwo>();
        TestComponentOne *three = test_host.Allocate<TestComponentThree>();

        // first
        if (i == 0)
        {
            c1 = one;
        }

        // middle
        if (i == 4)
        {
            c2 = two;
        }

        //last
        if (i == 9)
        {
            c3 = three;
        }

        all_components.Add(one);
        all_components.Add(two);
        all_components.Add(three);
    }

    ASSERT_TRUE(c1 && c2 && c3);

    // Test iterating over every component
    //     uint32_t count = 0;
    //     for (Helium::Components::Iterator iter = test_host.ComponentsBegin(); iter != test_host.ComponentsEnd(); ++iter)
    //     {
    //         ++count;
    //     }
    //     EXPECT_EQ(count, 30);

    //     // Test iterating over component two.. we should not see ones or threes (one is parent class, three is sibling class)
    //     count = 0;
    //     for (Components::OfTypeIterator<TestComponentTwo> iter = test_host.ComponentsOfTypeBegin<TestComponentTwo>(); 
    //         iter != test_host.ComponentsOfTypeEnd<TestComponentTwo>(); 
    //         ++iter)
    //     {
    //         EXPECT_NE(static_cast<TestComponentOne *>(c1)->m_Id, static_cast<TestComponentOne *>(&*iter)->m_Id);
    //         EXPECT_NE(static_cast<TestComponentOne *>(c3)->m_Id, static_cast<TestComponentOne *>(&*iter)->m_Id);
    //         ++count;
    //     }
    //     EXPECT_EQ(count, 10);
    // 
    //     // Test iterating over component one.. we should not see twos or threes (they are child classes)
    //     count = 0;
    //     for (Components::OfTypeIterator<TestComponentOne> iter = test_host.ComponentsOfTypeBegin<TestComponentOne>(); 
    //         iter != test_host.ComponentsOfTypeEnd<TestComponentOne>(); 
    //         ++iter)
    //     {
    //         EXPECT_NE(static_cast<TestComponentOne *>(c2)->m_Id, static_cast<TestComponentOne *>(&*iter)->m_Id);
    //         EXPECT_NE(static_cast<TestComponentOne *>(c3)->m_Id, static_cast<TestComponentOne *>(&*iter)->m_Id);
    //         ++count;
    //     }
    //     EXPECT_EQ(count, 10);
    // 
    //     // Test iterating over component two.. we should not see ones or threes (one is parent class, three is sibling class)
    //     count = 0;
    //     for (Components::ImplementsTypeIterator<TestComponentTwo> iter = test_host.ComponentsImplementingTypeBegin<TestComponentTwo>(); 
    //         iter != test_host.ComponentsImplementingTypeEnd<TestComponentTwo>(); 
    //         ++iter)
    //     {
    //         EXPECT_NE(static_cast<TestComponentOne *>(c1)->m_Id, static_cast<TestComponentOne *>(&*iter)->m_Id);
    //         EXPECT_NE(static_cast<TestComponentOne *>(c3)->m_Id, static_cast<TestComponentOne *>(&*iter)->m_Id);
    //         ++count;
    //     }
    //     EXPECT_EQ(count, 10);
    // 
    //     // Test iterating by implements over component one
    //     count = 0;
    //     for (Components::ImplementsTypeIterator<TestComponentOne> iter = test_host.ComponentsImplementingTypeBegin<TestComponentOne>(); 
    //         iter != test_host.ComponentsImplementingTypeEnd<TestComponentOne>(); 
    //         ++iter)
    //     {
    //         ++count;
    //     }
    //     EXPECT_EQ(count, 30);
    // 
    //     test_host.DetachComponent(*c1);
    //     test_host.DetachComponent(*c2);
    //     test_host.DetachComponent(*c3);
    // 
    //     // Test iterating over all components.. we should not see any of the components we just removed
    //     count = 0;
    //     for (Components::Iterator iter = test_host.ComponentsBegin(); iter != test_host.ComponentsEnd(); ++iter)
    //     {
    //         EXPECT_NE(&*iter, c1);
    //         EXPECT_NE(&*iter, c2);
    //         EXPECT_NE(&*iter, c3);
    //         ++count;
    //     }
    //     EXPECT_EQ(count, 27);
    // 
    //     // TODO: Test resolve handle

    for (size_t i = 0; i < all_components.GetSize(); ++i)
    {
        test_host.Free(*all_components[i]);
    }
    all_components.Clear();
}
