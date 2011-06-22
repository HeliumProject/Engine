//----------------------------------------------------------------------------------------------------------------------
// WindowManager.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_WINDOWING_WINDOW_MANAGER_H
#define LUNAR_WINDOWING_WINDOW_MANAGER_H

#include "Windowing/Windowing.h"

#include "Windowing/Window.h"

namespace Helium
{
    /// Interface for managing window creation.
    class LUNAR_WINDOWING_API WindowManager : NonCopyable
    {
    public:
        /// @name Initialization
        //@{
        virtual void Shutdown() = 0;
        //@}

        /// @name Updating
        //@{
        virtual bool Update() = 0;
        virtual void RequestQuit() = 0;
        //@}

        /// @name Window Creation
        //@{
        virtual Window* Create( Window::Parameters& rParameters ) = 0;
        //@}

        /// @name Static Access
        //@{
        static WindowManager* GetStaticInstance();
        static void DestroyStaticInstance();
        //@}

    protected:
        /// Singleton instance.
        static WindowManager* sm_pInstance;

        /// @name Construction/Destruction
        //@{
        WindowManager();
        virtual ~WindowManager() = 0;
        //@}
    };
}

#endif  // LUNAR_WINDOWING_WINDOW_MANAGER_H
