
#include "TestAppPch.h"

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

class Components : public testing::Test
{
public:
    void SetUp()
    {
        Helium::Components::Initialize();

        m_ComponentOneTypeId = TestComponentOne::RegisterComponentType(10);
        m_ComponentTwoTypeId = TestComponentTwo::RegisterComponentType(10);
        m_ComponentThreeTypeId = TestComponentThree::RegisterComponentType(10);
    }

    void TearDown()
    {
		Helium::Components::Cleanup();
    }

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

    {
        DynamicArray<TestComponentOne *> component_list_1;
        DynamicArray<TestComponentTwo *> component_list_2;
        DynamicArray<TestComponentThree *> component_list_3;

        test_host.FindAllComponents<TestComponentOne>(component_list_1);
        test_host.FindAllComponents<TestComponentTwo>(component_list_2);
        test_host.FindAllComponents<TestComponentThree>(component_list_3);

        EXPECT_TRUE(component_list_1.GetSize() == 1);
        EXPECT_TRUE(component_list_2.GetSize() == 2);
        EXPECT_TRUE(component_list_3.GetSize() == 2);
    }

    c1->MarkForDeletion();
    c2_1->MarkForDeletion();
    c3_2->MarkForDeletion();

    {
        DynamicArray<TestComponentOne *> component_list_1;
        DynamicArray<TestComponentTwo *> component_list_2;
        DynamicArray<TestComponentThree *> component_list_3;

        test_host.FindAllComponents<TestComponentOne>(component_list_1);
        test_host.FindAllComponents<TestComponentTwo>(component_list_2);
        test_host.FindAllComponents<TestComponentThree>(component_list_3);

        EXPECT_TRUE(component_list_1.GetSize() == 1);
        EXPECT_TRUE(component_list_2.GetSize() == 2);
        EXPECT_TRUE(component_list_3.GetSize() == 2);
    }

    Helium::Components::ProcessPendingDeletes();

    // Now asking directly for c1 should fail, but an implementor of c1 should pass (returns c2 or c3)
    EXPECT_TRUE(test_host.FindOneComponent<TestComponentOne>() == 0);
    EXPECT_TRUE(test_host.FindOneComponentThatImplements<TestComponentOne>() != 0);

    {
        DynamicArray<TestComponentOne *> component_list_1;
        DynamicArray<TestComponentTwo *> component_list_2;
        DynamicArray<TestComponentThree *> component_list_3;

        test_host.FindAllComponents<TestComponentOne>(component_list_1);
        test_host.FindAllComponents<TestComponentTwo>(component_list_2);
        test_host.FindAllComponents<TestComponentThree>(component_list_3);

        EXPECT_TRUE(component_list_1.GetSize() == 0);
        EXPECT_TRUE(component_list_2.GetSize() == 1);
        EXPECT_TRUE(component_list_3.GetSize() == 1);
    }

    {
        DynamicArray<TestComponentOne *> component_list;
        test_host.FindAllComponents<TestComponentOne>(component_list);
        EXPECT_TRUE(component_list.GetSize() == 0);
    }

    {
        DynamicArray<TestComponentOne *> component_list;
        test_host.FindAllComponentsThatImplement<TestComponentOne>(component_list);
        EXPECT_TRUE(component_list.GetSize() == 2);
    }

    {
        DynamicArray<TestComponentTwo *> component_list;
        test_host.FindAllComponentsThatImplement<TestComponentTwo>(component_list);
        EXPECT_TRUE(component_list.GetSize() == 1);
    }

    //test_host.Free(*c2_2);
    //test_host.Free(*c3_1);

    c2_2->MarkForDeletion();
    c3_1->MarkForDeletion();

    Helium::Components::ProcessPendingDeletes();
}

TEST_F(Components, HostAttachDetachIterate)
{
    TestHost test_host;
    DynamicArray<Component *> all_components;

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
        all_components[i]->MarkForDeletion();
    }
    all_components.Clear();
    Helium::Components::ProcessPendingDeletes();
}

TEST_F(Components, SmartPtr)
{
    TestHost test_host;

    {
        // Make the ptr
        ComponentPtr<TestComponentTwo> component = test_host.Allocate<TestComponentTwo>();
        EXPECT_TRUE(component.IsGood());

        // Handle pending deletes and check our component is still around
        Helium::Components::ProcessPendingDeletes();
        EXPECT_TRUE(component.IsGood());

        // Delete the component and check that it is not around AFTER the pending delete is processed
        component->MarkForDeletion();
        EXPECT_TRUE(component.IsGood());
        Helium::Components::ProcessPendingDeletes();

        // Is good should fail here because the generation doesn't match
        EXPECT_FALSE(component.IsGood());
    }

    {
        // Make the ptr
        ComponentPtr<TestComponentTwo> component = test_host.Allocate<TestComponentTwo>();
        EXPECT_TRUE(component.IsGood());

        // Handle pending deletes and check our component is still around
        Helium::Components::ProcessPendingDeletes();
        EXPECT_TRUE(component.IsGood());

        // Delete the component and check that it is not around AFTER the pending delete is processed
        component->MarkForDeletion();
        EXPECT_TRUE(component.IsGood());

        for (int i = 0; i < 256; ++i)
        {
            Helium::Components::ProcessPendingDeletes();
        }

        // The component pointer should be wiped out BEFORE this call
        EXPECT_FALSE(component.IsGood());
    }
}

//class Object
//{
//
//};
//
//struct Stimulus
//{
//
//};
//
//struct Response
//{
//
//};
//
//struct NamedProperty
//{
//    Helium::Name m_Name;
//    const Reflect::Field *m_Field;
//};
//
//struct NamedPropertyValue
//{
//    Helium::Name m_Name;
//    // Don't know about this.. and I would want it to be capable of holding references
//    DynamicArray<uint8_t> m_SerializedValue;
//};
//
//struct NamedStimulus
//{
//    Helium::Name m_Name;
//    Reflect::Field *m_Field;
//    const Stimulus *m_Stimulus;
//};
//
//struct NamedResponse
//{
//    Helium::Name m_Name;
//    Reflect::Field *m_Field;
//    const Response *m_Response;
//};
//
//struct Linkage
//{
//    Helium::Name m_StimulusName;
//    Helium::Name m_ResponseName;
//};
//
//class ComponentDefinition
//{
//    DynamicArray<NamedProperty>      m_Properties;       // Declared externally provided properties
//    DynamicArray<NamedStimulus>      m_Stimuli;          // Stimuli are exposed by name
//    DynamicArray<NamedResponse>      m_Responses;        // Responses are exposed by name
//
//    // Fields that can be filled by other 
//    DynamicArray<FieldValue>    m_FieldValues;      // Overridden fields
//
//    ComponentDefinition*    m_BaseDefinition;   // Parent definition (we inherit its fields/params)
//    Component*              m_Model;            // Preallocated component we can clone (not in the component pool)
//};
//
//class ComponentCluster
//{
//    DynamicArray<NamedPropertyValue>    m_PropertyValues;  // Cluster can provide property values
//    DynamicArray<Linkage>               m_Linkages;
//    DynamicArray<ComponentDefinition *> m_Components;
//};
//
//class ObjectDefinition
//{
//    DynamicArray<NamedPropertyValue>    m_PropertyValues;  // Cluster can provide property values
//    DynamicArray<Linkage>               m_Linkages;
//
//    DynamicArray<ComponentCluster *>    m_ComponentClusters;
//    DynamicArray<ComponentDefinition *> m_Components;
//
//    ObjectDefinition *m_BaseDefinition;
//    Object *m_Model;
//};
//
//// 
//class InstanceFactory
//{
//    // 1. Gather up all the things we want to fill out
//    void AddProperties(DynamicArray<NamedProperty> &_properties);
//    void AddStimuli(DynamicArray<NamedStimulus> &_stimuli);
//    void AddResponses(DynamicArray<NamedResponse> &_responses);
//
//    // 2. Apply values to those things we can fill out
//    void ApplyPropertyValues(DynamicArray<NamedPropertyValue> &_property_values);
//    void ApplyLinkages(DynamicArray<Linkage> &_linkages);
//
//    // 3. Verify all mandatory parameters defined
//    bool ReadyToMaterialize();
//    void GetUnsatisfiedParameters();
//
//    // 4. Generate objects/components (can be called multiple times)
//    void MaterializeInstances();
//
//    // In addition, the editor can cook into a more compressed structure
//    CookedInstanceFactoryPtr Cook();
//};