#include "TestAppPch.h"

#include "TestGameObject.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

HELIUM_IMPLEMENT_OBJECT( Helium::TestGameObject1, TestApp, 0 );
HELIUM_IMPLEMENT_OBJECT( Helium::TestGameObject2, TestApp, 0 );
HELIUM_IMPLEMENT_OBJECT( Helium::TestGameObject3, TestApp, 0 );

using namespace Helium;

void TestGameObject1::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField(            &TestGameObject1::m_TestValue1,               TXT( "m_TestValue1" ) );
    comp.AddField(            &TestGameObject1::m_TestValue2,               TXT( "m_TestValue2" ) );
}

void TestGameObject2::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField(            &TestGameObject2::m_TestValue1,               TXT( "m_TestValue1" ) );
    comp.AddField(            &TestGameObject2::m_TestReference,            TXT( "m_TestReference" ), Reflect::FieldFlags::Share );
    comp.AddField(            &TestGameObject2::m_TestDeepCopy,            TXT( "m_TestDeepCopy" ) );
}

void TestGameObject3::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField(            &TestGameObject3::m_TestValue1,               TXT( "m_TestValue1" ) );
    comp.AddField(            &TestGameObject3::m_TestValue2,               TXT( "m_TestValue2" ) );
}
