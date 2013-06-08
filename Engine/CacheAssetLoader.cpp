//----------------------------------------------------------------------------------------------------------------------
// CacheAssetLoader.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EnginePch.h"
#include "Engine/CacheAssetLoader.h"
#include "Engine/CachePackageLoader.h"
#include "Engine/Package.h"

using namespace Helium;

/// Constructor.
CacheAssetLoader::CacheAssetLoader()
{
    m_pPackageLoader = new CachePackageLoader;
    HELIUM_ASSERT( m_pPackageLoader );
    HELIUM_VERIFY( m_pPackageLoader->Initialize( Name( TXT("Asset") ) ) );

    HELIUM_VERIFY( m_pPackageLoader->BeginPreload() );
}

/// Destructor.
CacheAssetLoader::~CacheAssetLoader()
{
    delete m_pPackageLoader;
    m_pPackageLoader = NULL;
}

/// Initialize the static object loader instance as a CacheAssetLoader.
///
/// @return  True if the loader was initialized successfully, false if not or another object loader instance already
///          exists.
bool CacheAssetLoader::InitializeStaticInstance()
{
    if( sm_pInstance )
    {
        return false;
    }

    sm_pInstance = new CacheAssetLoader;
    HELIUM_ASSERT( sm_pInstance );

    return true;
}

/// @copydoc AssetLoader::GetPackageLoader()
PackageLoader* CacheAssetLoader::GetPackageLoader( AssetPath /*path*/ )
{
    return m_pPackageLoader;
}

/// @copydoc AssetLoader::TickPackageLoaders()
void CacheAssetLoader::TickPackageLoaders()
{
    m_pPackageLoader->Tick();
}
