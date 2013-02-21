//----------------------------------------------------------------------------------------------------------------------
// GameSystem.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_GAME_SYSTEM_H
#define HELIUM_FRAMEWORK_GAME_SYSTEM_H

#include "Framework/System.h"

#include "Windowing/Window.h"

namespace Helium
{
    class AssetType;

    class CommandLineInitialization;
    class ObjectTypeRegistration;
    class MemoryHeapPreInitialization;
    class ObjectLoaderInitialization;
    class ConfigInitialization;
    class WindowManagerInitialization;
    class RendererInitialization;

    /// Base interface for game application systems.
    class HELIUM_FRAMEWORK_API GameSystem : public System
    {
    public:
        /// @name Construction/Destruction
        //@{
        GameSystem();
        virtual ~GameSystem();
        //@}

        /// @name Initialization
        //@{
        virtual bool Initialize(
            CommandLineInitialization& rCommandLineInitialization, 
            ObjectTypeRegistration& rObjectTypeRegistration,
            MemoryHeapPreInitialization& rMemoryHeapPreInitialization,
            ObjectLoaderInitialization& rObjectLoaderInitialization, 
            ConfigInitialization& rConfigInitialization,
            WindowManagerInitialization& rWindowManagerInitialization,
            RendererInitialization& rRendererInitialization);
        virtual void Shutdown();
        //@}

        /// @name Application Loop
        //@{
        virtual int32_t Run();
        //@}

        /// @name Static Initialization
        //@{
        static GameSystem* CreateStaticInstance();
        //@}

    protected:
        /// Asset type registration interface.
        ObjectTypeRegistration* m_pObjectTypeRegistration;
        /// AssetLoader initialization interface.
        ObjectLoaderInitialization* m_pObjectLoaderInitialization;

        /// Main application window.
        Window* m_pMainWindow;

        /// @name Window Callbacks
        //@{
        virtual void OnMainWindowDestroyed( Window* pWindow );
        //@}
    };
}

#endif  // HELIUM_FRAMEWORK_GAME_SYSTEM_H
