//----------------------------------------------------------------------------------------------------------------------
// CacheObjectLoader.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EnginePch.h"
#include "Engine/CacheObjectLoader.h"

#include "Engine/CachePackageLoader.h"

using namespace Lunar;

/// Constructor.
CacheObjectLoader::CacheObjectLoader()
{
    m_pPackageLoader = new CachePackageLoader;
    HELIUM_ASSERT( m_pPackageLoader );
    HELIUM_VERIFY( m_pPackageLoader->Initialize( GetCacheName() ) );

    HELIUM_VERIFY( m_pPackageLoader->BeginPreload() );
}

/// Destructor.
CacheObjectLoader::~CacheObjectLoader()
{
    delete m_pPackageLoader;
    m_pPackageLoader = NULL;
}

/// Initialize the static object loader instance as a CacheObjectLoader.
///
/// @return  True if the loader was initialized successfully, false if not or another object loader instance already
///          exists.
bool CacheObjectLoader::InitializeStaticInstance()
{
    if( sm_pInstance )
    {
        return false;
    }

    sm_pInstance = new CacheObjectLoader;
    HELIUM_ASSERT( sm_pInstance );

    return true;
}

/// @copydoc GameObjectLoader::GetPackageLoader()
PackageLoader* CacheObjectLoader::GetPackageLoader( GameObjectPath /*path*/ )
{
    return m_pPackageLoader;
}

/// @copydoc GameObjectLoader::TickPackageLoaders()
void CacheObjectLoader::TickPackageLoaders()
{
    m_pPackageLoader->Tick();
}
