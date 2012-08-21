//----------------------------------------------------------------------------------------------------------------------
// TestAppTypeRegistration.cpp
//
// Copyright (C) 2012 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "TestAppPch.h"
#include "Platform/Assert.h"
#include "Engine/Package.h"

#include "TestApp/TestGameObject.h"

static Helium::StrongPtr< Helium::Package > spTestAppTypePackage;

HELIUM_TEST_APP_API Helium::Package* GetTestAppTypePackage()
{
    Helium::Package* pPackage = spTestAppTypePackage;
    if( !pPackage )
    {
        Helium::GameObject* pTypesPackageObject = Helium::GameObject::FindChildOf( NULL, Helium::Name( TXT( "Types" ) ) );
        HELIUM_ASSERT( pTypesPackageObject );
        HELIUM_ASSERT( pTypesPackageObject->IsPackage() );

        HELIUM_VERIFY( Helium::GameObject::Create< Helium::Package >(
            spTestAppTypePackage,
            Helium::Name( TXT( "TestApp" ) ),
            pTypesPackageObject ) );
        pPackage = spTestAppTypePackage;
        HELIUM_ASSERT( pPackage );
    }

    return pPackage;
}

HELIUM_TEST_APP_API void ReleaseTestAppTypePackage()
{
    spTestAppTypePackage = NULL;
}

HELIUM_TEST_APP_API void RegisterTestAppTypes()
{
    HELIUM_VERIFY( GetTestAppTypePackage() );

    HELIUM_VERIFY( Helium::TestGameObject1::InitStaticType() );
    HELIUM_VERIFY( Helium::TestGameObject3::InitStaticType() );
    HELIUM_VERIFY( Helium::TestGameObject2::InitStaticType() );
}

HELIUM_TEST_APP_API void UnregisterTestAppTypes()
{
    Helium::TestGameObject1::ReleaseStaticType();
    Helium::TestGameObject3::ReleaseStaticType();
    Helium::TestGameObject2::ReleaseStaticType();

    ReleaseTestAppTypePackage();
}
