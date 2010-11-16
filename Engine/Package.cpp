//----------------------------------------------------------------------------------------------------------------------
// Package.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EnginePch.h"
#include "Engine/Package.h"

namespace Lunar
{
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
    Type* Package::InitStaticType()
    {
        Type* pType = sm_spStaticType;
        if( !pType )
        {
            // Package type is registered manually during Object type initialization, so retrieve the type info from the
            // existing registered data.
            HELIUM_VERIFY( Object::InitStaticType() );

            pType = Type::Find( Name( TXT( "Package" ) ) );
            HELIUM_ASSERT( pType );

            sm_spStaticType = pType;
            sm_spStaticTypeTemplate = static_cast< Package* >( pType->GetTypeTemplate() );
            HELIUM_ASSERT( sm_spStaticTypeTemplate );
        }

        return pType;
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
}
