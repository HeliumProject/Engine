//----------------------------------------------------------------------------------------------------------------------
// PcSupportTypeRegistration.cpp
//
// Copyright (C) 2012 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "PcSupportPch.h"
#include "Platform/Assert.h"
#include "Engine/Package.h"

#include "PcSupport/ResourceHandler.h"

static Helium::StrongPtr< Helium::Package > spPcSupportTypePackage;

HELIUM_PC_SUPPORT_API Helium::Package* GetPcSupportTypePackage()
{
    Helium::Package* pPackage = spPcSupportTypePackage;
    if( !pPackage )
    {
        Helium::GameObject* pTypesPackageObject = Helium::GameObject::FindChildOf( NULL, Helium::Name( TXT( "Types" ) ) );
        HELIUM_ASSERT( pTypesPackageObject );
        HELIUM_ASSERT( pTypesPackageObject->IsPackage() );

        HELIUM_VERIFY( Helium::GameObject::Create< Helium::Package >(
            spPcSupportTypePackage,
            Helium::Name( TXT( "PcSupport" ) ),
            pTypesPackageObject ) );
        pPackage = spPcSupportTypePackage;
        HELIUM_ASSERT( pPackage );
    }

    return pPackage;
}

HELIUM_PC_SUPPORT_API void ReleasePcSupportTypePackage()
{
    spPcSupportTypePackage = NULL;
}

HELIUM_PC_SUPPORT_API void RegisterPcSupportTypes()
{
    HELIUM_VERIFY( GetPcSupportTypePackage() );

    HELIUM_VERIFY( Helium::ResourceHandler::InitStaticType() );
}

HELIUM_PC_SUPPORT_API void UnregisterPcSupportTypes()
{
    Helium::ResourceHandler::ReleaseStaticType();

    ReleasePcSupportTypePackage();
}
