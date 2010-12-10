//----------------------------------------------------------------------------------------------------------------------
// Package.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EnginePch.h"
#include "Engine/Package.h"

using namespace Lunar;

L_IMPLEMENT_OBJECT_NOINITTYPE( Package, Engine );

/// Constructor.
Package::Package()
: m_pLoader( NULL )
{
    // Set the package flag by default.
    SetFlags( FLAG_PACKAGE );
}

/// Destructor.
Package::~Package()
{
}

/// Initialize the static type information for the "Package" class.
///
/// @return  Static "Package" type.
GameObjectType* Package::InitStaticType()
{
    if( !sm_pStaticType )
    {
        // Package type is registered manually during GameObject type initialization, so retrieve the type info from the
        // existing registered data.
        HELIUM_VERIFY( GameObject::InitStaticType() );

        sm_pStaticType = GameObjectType::Find( Name( TXT( "Package" ) ) );
        HELIUM_ASSERT( sm_pStaticType );

        sm_spStaticTypeTemplate = static_cast< Package* >( sm_pStaticType->GetTemplate() );
        HELIUM_ASSERT( sm_spStaticTypeTemplate );
    }

    return sm_pStaticType;
}

/// Set the loader for this package.
///
/// @param[in] pLoader  Package loader.
///
/// @see GetLoader()
void Package::SetLoader( PackageLoader* pLoader )
{
    m_pLoader = pLoader;
}
