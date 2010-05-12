#pragma once

#include "Preferences.h"
#include "PreferencesInit.h"

using namespace UIToolKit;

namespace PrefTestEnumValues
{
  enum PrefTestEnumValue
  {
    One,
    Two,
    Three
  };
  static void PrefTestEnumValueEnumerateEnumeration( Reflect::Enumeration* info )
  {
    info->AddElement(One, "One");
    info->AddElement(Two, "Two");
    info->AddElement(Three, "Three");
  }
}
typedef PrefTestEnumValues::PrefTestEnumValue PrefTestEnumValue;

// Helper class for testing preferences
class PrefTest : public UIToolKit::Preferences
{
private:
  i32 m_I32Var;
  PrefTestEnumValue m_EnumVar;

public:
  PrefTest()
    : m_I32Var( -1 )
    , m_EnumVar( PrefTestEnumValues::One )
  {
  }

  const Reflect::Field* I32Var() const 
  { 
    return GetClass()->FindField( &PrefTest::m_I32Var ); 
  }
  
  const Reflect::Field* EnumVar() const 
  { 
    return GetClass()->FindField( &PrefTest::m_EnumVar ); 
  }
  
  REFLECT_DECLARE_CLASS( PrefTest, UIToolKit::Preferences );
  static void EnumerateClass( Reflect::Compositor<PrefTest>& comp )
  {
    Reflect::Field* fieldI32Var = comp.AddField( &PrefTest::m_I32Var, "m_I32Var" );
    Reflect::EnumerationField* enumEnumVar = comp.AddEnumerationField( &PrefTest::m_EnumVar, "m_EnumVar" );
  }
};
typedef Nocturnal::SmartPtr< PrefTest > PrefTestPtr;
REFLECT_DEFINE_CLASS( PrefTest );

// Prepare tests in this file
void Setup()
{
  Reflect::Initialize();
  InitializePreferences();

  Reflect::RegisterEnumeration<PrefTestEnumValues::PrefTestEnumValue>( &PrefTestEnumValues::PrefTestEnumValueEnumerateEnumeration, "PrefTestEnumValue" );
  Reflect::RegisterClass<PrefTest>( "PrefTest" );
}

// Cleanup tests in this file
void TearDown()
{
  Reflect::UnregisterEnumeration<PrefTestEnumValues::PrefTestEnumValue>();
  Reflect::UnregisterClass<PrefTest>();
  
  CleanupPreferences();
  Reflect::Cleanup();
}

// Test case for getting and setting preferences
BOOST_AUTO_TEST_CASE( testGetSetPrefs )
{
  Setup();

  PrefTestPtr prefs = new PrefTest();

  // Get i32
  i32 temp = 0;
  BOOST_REQUIRE( prefs->Get( prefs->I32Var(), temp ) );
  BOOST_REQUIRE( temp == -1 );

  // Set i32
  BOOST_REQUIRE( prefs->Set( prefs->I32Var(), 10 ) );
  BOOST_REQUIRE( prefs->Get( prefs->I32Var(), temp ) );
  BOOST_REQUIRE( temp == 10 );

  // Get enum
  PrefTestEnumValue e = PrefTestEnumValues::Three;
  BOOST_REQUIRE( prefs->GetEnum( prefs->EnumVar(), e ) );
  BOOST_REQUIRE( e == PrefTestEnumValues::One );

  // Set i32
  BOOST_REQUIRE( prefs->SetEnum( prefs->EnumVar(), PrefTestEnumValues::Two ) );
  BOOST_REQUIRE( prefs->GetEnum( prefs->EnumVar(), e ) );
  BOOST_REQUIRE( e == PrefTestEnumValues::Two );

  TearDown();
}