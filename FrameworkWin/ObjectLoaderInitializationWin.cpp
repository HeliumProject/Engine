//----------------------------------------------------------------------------------------------------------------------
// ObjectLoaderInitializationWin.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkWinPch.h"
#include "FrameworkWin/ObjectLoaderInitializationWin.h"

#include "PcSupport/ObjectPreprocessor.h"
#include "PcSupport/PcCacheObjectLoader.h"
#include "PreprocessingPc/PcPreprocessor.h"
#include "EditorSupport/EditorObjectLoader.h"

using namespace Helium;

/// @copydoc ObjectLoaderInitialization::Initialize()
GameObjectLoader* ObjectLoaderInitializationWin::Initialize()
{
#if HELIUM_TOOLS
    if( !EditorObjectLoader::InitializeStaticInstance() )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            TXT( "ObjectLoaderFactoryWin::Create(): Failed to initialize EditorObjectLoader instance.\n" ) );

        return NULL;
    }

    ObjectPreprocessor* pObjectPreprocessor = ObjectPreprocessor::CreateStaticInstance();
    HELIUM_ASSERT( pObjectPreprocessor );
    PlatformPreprocessor* pPlatformPreprocessor = new PcPreprocessor;
    HELIUM_ASSERT( pPlatformPreprocessor );
    pObjectPreprocessor->SetPlatformPreprocessor( Cache::PLATFORM_PC, pPlatformPreprocessor );
#else
    if( !PcCacheObjectLoader::InitializeStaticInstance() )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            TXT( "ObjectLoaderFactoryWin::Create() Failed to initialize PcCacheObjectLoader instance.\n" ) );

        return NULL;
    }
#endif

    GameObjectLoader* pObjectLoader = GameObjectLoader::GetStaticInstance();
    HELIUM_ASSERT( pObjectLoader );

    return pObjectLoader;
}

/// @copydoc ObjectLoaderInitialization::Shutdown()
void ObjectLoaderInitializationWin::Shutdown()
{
#if HELIUM_TOOLS
    ObjectPreprocessor::DestroyStaticInstance();
#endif

    ObjectLoaderInitialization::Shutdown();
}
