//----------------------------------------------------------------------------------------------------------------------
// WinWindow.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "WinWindowingPch.h"
#include "WinWindowing/WinWindow.h"

namespace Lunar
{
    /// Constructor.
    WinWindow::WinWindow()
    {
    }

    /// Destructor.
    WinWindow::~WinWindow()
    {
    }

    /// @copydoc Window::Destroy()
    void WinWindow::Destroy()
    {
        HWND hWnd = static_cast< HWND >( m_pHandle );
        HELIUM_ASSERT( hWnd );

        HELIUM_VERIFY( DestroyWindow( hWnd ) );
    }

    /// Set the window parameters after construction.
    ///
    /// @param[in] hWnd         Window handle.
    /// @param[in] pTitle       Window title string (can be null).
    /// @param[in] width        Window width, in pixels.
    /// @param[in] height       Window height, in pixels.
    /// @param[in] bFullscreen  True if the window is configured for display as a full-screen window, false if it is set
    ///                         up for windowed display.
    void WinWindow::Set( HWND hWnd, const tchar_t* pTitle, uint32_t width, uint32_t height, bool bFullscreen )
    {
        HELIUM_ASSERT( hWnd );

        m_pHandle = hWnd;

        if( pTitle )
        {
            m_title = pTitle;
        }

        m_width = width;
        m_height = height;
        m_bFullscreen = bFullscreen;
    }
}
