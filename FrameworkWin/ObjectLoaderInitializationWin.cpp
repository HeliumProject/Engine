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

namespace Lunar
{
    /// @copydoc ObjectLoaderInitialization::Initialize()
    ObjectLoader* ObjectLoaderInitializationWin::Initialize()
    {
#if L_EDITOR && 1
        if( !EditorObjectLoader::InitializeStaticInstance() )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
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
                TRACE_ERROR,
                TXT( "ObjectLoaderFactoryWin::Create() Failed to initialize PcCacheObjectLoader instance.\n" ) );

            return NULL;
        }
#endif

        ObjectLoader* pObjectLoader = ObjectLoader::GetStaticInstance();
        HELIUM_ASSERT( pObjectLoader );

        return pObjectLoader;
    }

    /// @copydoc ObjectLoaderInitialization::Shutdown()
    void ObjectLoaderInitializationWin::Shutdown()
    {
#if L_EDITOR
        ObjectPreprocessor::DestroyStaticInstance();
#endif

        ObjectLoaderInitialization::Shutdown();
    }
}
