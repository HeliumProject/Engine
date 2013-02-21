#include "TestAppPch.h"

#include "TestAsset.h"
#include "Reflect/Data/DataDeduction.h"

HELIUM_IMPLEMENT_OBJECT( Helium::TestAsset1, TestApp, 0 );
HELIUM_IMPLEMENT_OBJECT( Helium::TestAsset2, TestApp, 0 );
HELIUM_IMPLEMENT_OBJECT( Helium::TestAsset3, TestApp, 0 );

using namespace Helium;

void TestAsset1::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField(            &TestAsset1::m_TestValue1,               TXT( "m_TestValue1" ) );
    comp.AddField(            &TestAsset1::m_TestValue2,               TXT( "m_TestValue2" ) );
}

void TestAsset2::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField(            &TestAsset2::m_TestValue1,               TXT( "m_TestValue1" ) );
    comp.AddField(            &TestAsset2::m_TestReference,            TXT( "m_TestReference" ), Reflect::FieldFlags::Share );
    comp.AddField(            &TestAsset2::m_TestDeepCopy,            TXT( "m_TestDeepCopy" ) );
}

void TestAsset3::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField(            &TestAsset3::m_TestValue1,               TXT( "m_TestValue1" ) );
    comp.AddField(            &TestAsset3::m_TestValue2,               TXT( "m_TestValue2" ) );
}
