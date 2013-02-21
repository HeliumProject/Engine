//----------------------------------------------------------------------------------------------------------------------
// EngineTypeRegistration.cpp
//
// Copyright (C) 2012 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EnginePch.h"
#include "Platform/Assert.h"
#include "Engine/Package.h"

#include "Engine/Package.h"
#include "Engine/Resource.h"

static Helium::StrongPtr< Helium::Package > spEngineTypePackage;

HELIUM_ENGINE_API Helium::Package* GetEngineTypePackage()
{
    Helium::Package* pPackage = spEngineTypePackage;
    if( !pPackage )
    {
        HELIUM_VERIFY( Helium::Asset::InitStaticType() );

        Helium::Asset* pTypesPackageObject = Helium::Asset::FindChildOf( NULL, Helium::Name( TXT( "Types" ) ) );
        HELIUM_ASSERT( pTypesPackageObject );
        HELIUM_ASSERT( pTypesPackageObject->IsPackage() );

        Helium::Asset* pPackageObject = Helium::Asset::FindChildOf(
            pTypesPackageObject,
            Helium::Name( TXT( "Engine" ) ) );
        HELIUM_ASSERT( pPackageObject );
        HELIUM_ASSERT( pPackageObject->IsPackage() );

        pPackage = Helium::Reflect::AssertCast< Helium::Package >( pPackageObject );
        spEngineTypePackage = pPackage;
    }

    return pPackage;
}

HELIUM_ENGINE_API void ReleaseEngineTypePackage()
{
    spEngineTypePackage = NULL;
}

HELIUM_ENGINE_API void RegisterEngineTypes()
{
    HELIUM_VERIFY( GetEngineTypePackage() );

    HELIUM_VERIFY( Helium::Package::InitStaticType() );
    HELIUM_VERIFY( Helium::Resource::InitStaticType() );
}

HELIUM_ENGINE_API void UnregisterEngineTypes()
{
    Helium::Package::ReleaseStaticType();
    Helium::Resource::ReleaseStaticType();

    ReleaseEngineTypePackage();
}
