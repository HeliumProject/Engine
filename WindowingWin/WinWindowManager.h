//----------------------------------------------------------------------------------------------------------------------
// WinWindowManager.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_WINDOWING_WIN_WIN_WINDOW_MANAGER_H
#define HELIUM_WINDOWING_WIN_WIN_WINDOW_MANAGER_H

#include "WindowingWin/WindowingWin.h"
#include "Windowing/WindowManager.h"

namespace Helium
{
    /// Windows(R) WindowManager implementation.
    class WinWindowManager : public WindowManager
    {
    public:
        /// @name Initialization
        //@{
        HELIUM_WINDOWING_WIN_API bool Initialize( HINSTANCE hInstance, int nCmdShow );
        void Shutdown();
        //@}

        /// @name Updating
        //@{
        bool Update();
        void RequestQuit();
        //@}

        /// @name Window Creation
        //@{
        Window* Create( Window::Parameters& rParameters );
        void Destroy( Window* pWindow );
        //@}

        /// @name Static Initialization
        //@{
        HELIUM_WINDOWING_WIN_API static WinWindowManager* CreateStaticInstance();
        //@}

    protected:
        /// Handle to the application instance.
        HINSTANCE m_hInstance;
        /// Flags specifying how the application window should be shown.
        int m_nCmdShow;

        /// Default window class atom.
        ATOM m_windowClassAtom;

        /// @name Construction/Destruction
        //@{
        WinWindowManager();
        ~WinWindowManager();
        //@}

    private:
        /// @name Window Procedure Callback
        //@{
        static LRESULT CALLBACK WindowProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
        //@}
    };
}

#endif  // HELIUM_WINDOWING_WIN_WIN_WINDOW_MANAGER_H
