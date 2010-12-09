//----------------------------------------------------------------------------------------------------------------------
// GameSystem.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_FRAMEWORK_GAME_SYSTEM_H
#define LUNAR_FRAMEWORK_GAME_SYSTEM_H

#include "Framework/System.h"

#include "Windowing/Window.h"

namespace Lunar
{
    class GameObjectType;

    class CommandLineInitialization;
    class ObjectTypeRegistration;
    class MemoryHeapPreInitialization;
    class ObjectLoaderInitialization;
    class ConfigInitialization;
    class WindowManagerInitialization;
    class RendererInitialization;

    /// Base interface for game application systems.
    class LUNAR_FRAMEWORK_API GameSystem : public System
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
            CommandLineInitialization& rCommandLineInitialization, ObjectTypeRegistration& rObjectTypeRegistration,
            MemoryHeapPreInitialization& rMemoryHeapPreInitialization,
            ObjectLoaderInitialization& rObjectLoaderInitialization, ConfigInitialization& rConfigInitialization,
            WindowManagerInitialization& rWindowManagerInitialization,
            RendererInitialization& rRendererInitialization, GameObjectType* pWorldType );
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
        /// GameObject type registration interface.
        ObjectTypeRegistration* m_pObjectTypeRegistration;
        /// GameObjectLoader initialization interface.
        ObjectLoaderInitialization* m_pObjectLoaderInitialization;

        /// Main application window.
        Window* m_pMainWindow;

        /// @name Window Callbacks
        //@{
        virtual void OnMainWindowDestroyed( Window* pWindow );
        //@}
    };
}

#endif  // LUNAR_FRAMEWORK_GAME_SYSTEM_H
