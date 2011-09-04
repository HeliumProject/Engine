
#include "TestAppPch.h"

#include "TestGameObject.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

using namespace Helium;

HELIUM_IMPLEMENT_OBJECT( TestGameObject1, TestApp, 0 );

void TestGameObject1::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField(            &TestGameObject1::m_TestValue1,               TXT( "m_TestValue1" ) );
    comp.AddField(            &TestGameObject1::m_TestValue2,               TXT( "m_TestValue2" ) );
}

HELIUM_IMPLEMENT_OBJECT( TestGameObject2, TestApp, 0 );

void TestGameObject2::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField(            &TestGameObject2::m_TestValue1,               TXT( "m_TestValue1" ) );
    comp.AddField(            &TestGameObject2::m_TestReference,            TXT( "m_TestReference" ), Reflect::FieldFlags::Share );
    comp.AddField(            &TestGameObject2::m_TestDeepCopy,            TXT( "m_TestDeepCopy" ) );
}

HELIUM_IMPLEMENT_OBJECT( TestGameObject3, TestApp, 0 );

void TestGameObject3::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField(            &TestGameObject3::m_TestValue1,               TXT( "m_TestValue1" ) );
    comp.AddField(            &TestGameObject3::m_TestValue2,               TXT( "m_TestValue2" ) );
}
