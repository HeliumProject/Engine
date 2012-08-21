//----------------------------------------------------------------------------------------------------------------------
// EditorSupportTypeRegistration.cpp
//
// Copyright (C) 2012 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EditorSupportPch.h"
#include "Platform/Assert.h"
#include "Engine/Package.h"

#include "EditorSupport/ShaderVariantResourceHandler.h"
#include "EditorSupport/FontResourceHandler.h"
#include "EditorSupport/Texture2dResourceHandler.h"
#include "EditorSupport/ShaderResourceHandler.h"
#include "EditorSupport/MaterialResourceHandler.h"
#include "EditorSupport/MeshResourceHandler.h"
#include "EditorSupport/AnimationResourceHandler.h"

static Helium::StrongPtr< Helium::Package > spEditorSupportTypePackage;

HELIUM_EDITOR_SUPPORT_API Helium::Package* GetEditorSupportTypePackage()
{
    Helium::Package* pPackage = spEditorSupportTypePackage;
    if( !pPackage )
    {
        Helium::GameObject* pTypesPackageObject = Helium::GameObject::FindChildOf( NULL, Helium::Name( TXT( "Types" ) ) );
        HELIUM_ASSERT( pTypesPackageObject );
        HELIUM_ASSERT( pTypesPackageObject->IsPackage() );

        HELIUM_VERIFY( Helium::GameObject::Create< Helium::Package >(
            spEditorSupportTypePackage,
            Helium::Name( TXT( "EditorSupport" ) ),
            pTypesPackageObject ) );
        pPackage = spEditorSupportTypePackage;
        HELIUM_ASSERT( pPackage );
    }

    return pPackage;
}

HELIUM_EDITOR_SUPPORT_API void ReleaseEditorSupportTypePackage()
{
    spEditorSupportTypePackage = NULL;
}

HELIUM_EDITOR_SUPPORT_API void RegisterEditorSupportTypes()
{
    HELIUM_VERIFY( GetEditorSupportTypePackage() );

    HELIUM_VERIFY( Helium::AnimationResourceHandler::InitStaticType() );
    HELIUM_VERIFY( Helium::FontResourceHandler::InitStaticType() );
    HELIUM_VERIFY( Helium::MaterialResourceHandler::InitStaticType() );
    HELIUM_VERIFY( Helium::MeshResourceHandler::InitStaticType() );
    HELIUM_VERIFY( Helium::ShaderResourceHandler::InitStaticType() );
    HELIUM_VERIFY( Helium::ShaderVariantResourceHandler::InitStaticType() );
    HELIUM_VERIFY( Helium::Texture2dResourceHandler::InitStaticType() );
}

HELIUM_EDITOR_SUPPORT_API void UnregisterEditorSupportTypes()
{
    Helium::AnimationResourceHandler::ReleaseStaticType();
    Helium::FontResourceHandler::ReleaseStaticType();
    Helium::MaterialResourceHandler::ReleaseStaticType();
    Helium::MeshResourceHandler::ReleaseStaticType();
    Helium::ShaderResourceHandler::ReleaseStaticType();
    Helium::ShaderVariantResourceHandler::ReleaseStaticType();
    Helium::Texture2dResourceHandler::ReleaseStaticType();

    ReleaseEditorSupportTypePackage();
}
