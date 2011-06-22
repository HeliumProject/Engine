//----------------------------------------------------------------------------------------------------------------------
// WinWindow.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_WINDOWING_WIN_WIN_WINDOW_H
#define LUNAR_WINDOWING_WIN_WIN_WINDOW_H

#include "WindowingWin/WindowingWin.h"
#include "Windowing/Window.h"

namespace Helium
{
    /// Windows(R) Window implementation.
    class WinWindow : public Window
    {
    public:
        /// @name Construction/Destruction
        //@{
        WinWindow();
        ~WinWindow();
        //@}

        /// @name Window Management
        //@{
        void Destroy();
        //@}

        /// @name Data Access
        //@{
        void Set( HWND hWnd, const tchar_t* pTitle, uint32_t width, uint32_t height, bool bFullscreen );
        //@}
    };
}

#endif  // LUNAR_WINDOWING_WIN_WIN_WINDOW_H
