//----------------------------------------------------------------------------------------------------------------------
// WindowManagerInitializationWin.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_WIN_WINDOW_MANAGER_INITIALIZATION_WIN_H
#define HELIUM_FRAMEWORK_WIN_WINDOW_MANAGER_INITIALIZATION_WIN_H

#include "FrameworkWin/FrameworkWin.h"
#include "Framework/WindowManagerInitialization.h"

#include "Platform/Windows/Windows.h"

namespace Helium
{
    /// Window manager factory implementation for Windows.
    class HELIUM_FRAMEWORK_WIN_API WindowManagerInitializationWin : public WindowManagerInitialization
    {
    public:
        /// @name Construction/Destruction
        //@{
        WindowManagerInitializationWin( HINSTANCE hInstance, int nCmdShow );
        //@}

        /// @name Window Manager Initialization
        //@{
        virtual bool Initialize();
        //@}

    protected:
        /// Handle to the application instance.
        HINSTANCE m_hInstance;
        /// Flags specifying how the application window should be shown.
        int m_nCmdShow;
    };
}

#endif  // HELIUM_FRAMEWORK_WIN_WINDOW_MANAGER_INITIALIZATION_WIN_H
