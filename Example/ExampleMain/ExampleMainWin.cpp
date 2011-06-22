//----------------------------------------------------------------------------------------------------------------------
// ExampleMainWin.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "ExampleMainPch.h"

#include "ExampleMain/ObjectTypeRegistration.h"

using namespace Helium;

/// Windows application entry point.
///
/// @param[in] hInstance      Handle to the current instance of the application.
/// @param[in] hPrevInstance  Handle to the previous instance of the application (always null; ignored).
/// @param[in] lpCmdLine      Command line for the application, excluding the program name.
/// @param[in] nCmdShow       Flags specifying how the application window should be shown.
///
/// @return  Result code of the application.
int APIENTRY _tWinMain( HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int nCmdShow )
{
    HELIUM_TRACE_SET_LEVEL( TRACE_DEBUG );

    int32_t result = 0;

    {
        // Initialize a GameSystem instance.
        CommandLineInitializationWin commandLineInitialization;
        Example::ObjectTypeRegistration< ObjectTypeRegistrationWin > objectTypeRegistration;
        MemoryHeapPreInitialization memoryHeapPreInitialization;
        ObjectLoaderInitializationWin objectLoaderInitialization;
        ConfigInitializationWin configInitialization;
        WindowManagerInitializationWin windowManagerInitialization( hInstance, nCmdShow );
        RendererInitializationWin rendererInitialization;
        //NullRendererInitialization rendererInitialization;

        GameSystem* pGameSystem = GameSystem::CreateStaticInstance();
        HELIUM_ASSERT( pGameSystem );
        bool bSystemInitSuccess = pGameSystem->Initialize(
            commandLineInitialization,
            objectTypeRegistration,
            memoryHeapPreInitialization,
            objectLoaderInitialization,
            configInitialization,
            windowManagerInitialization,
            rendererInitialization,
            NULL );
        if( bSystemInitSuccess )
        {
            // Run the application.
            result = pGameSystem->Run();
        }

        // Shut down and destroy the system.
        pGameSystem->Shutdown();
        System::DestroyStaticInstance();
    }

    // Perform final cleanup.
    ThreadLocalStackAllocator::ReleaseMemoryHeap();

#if HELIUM_ENABLE_MEMORY_TRACKING
    DynamicMemoryHeap::LogMemoryStats();
    ThreadLocalStackAllocator::ReleaseMemoryHeap();
#endif

    return result;
}
