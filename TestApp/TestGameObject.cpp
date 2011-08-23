
#include "TestAppPch.h"

#include "TestGameObject.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

using namespace Helium;

HELIUM_IMPLEMENT_OBJECT( TestGameObject, TestApp, 0 );

void TestGameObject::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField(            &TestGameObject::m_TestValue1,               TXT( "m_TestValue1" ) );
    comp.AddField(            &TestGameObject::m_TestValue2,               TXT( "m_TestValue2" ) );
}
