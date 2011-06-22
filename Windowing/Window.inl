//----------------------------------------------------------------------------------------------------------------------
// Window.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
    /// Get the platform-specific handle associated with this window.
    ///
    /// @return  Platform-specific handle.
    ///
    /// @see GetTitle(), GetWidth(), GetHeight(), GetFullscreen()
    void* Window::GetHandle() const
    {
        return m_pHandle;
    }

    /// Get the window title.
    ///
    /// @return  String containing the window title.
    ///
    /// @see GetHandle(), GetWidth(), GetHeight(), GetFullscreen()
    const String& Window::GetTitle() const
    {
        return m_title;
    }

    /// Get the width of this window.
    ///
    /// @return  Window width, in pixels.
    ///
    /// @see GetHeight(), GetHandle(), GetTitle(), GetFullscreen()
    uint32_t Window::GetWidth() const
    {
        return m_width;
    }

    /// Get the height of this window.
    ///
    /// @return  Window height, in pixels.
    ///
    /// @see GetWidth(), GetHandle(), GetTitle(), GetFullscreen()
    uint32_t Window::GetHeight() const
    {
        return m_height;
    }

    /// Get whether this window is configured for display as a full-screen window.
    ///
    /// @return  True if the window is configured for display as a full-screen window, false if it is set up for
    ///          windowed display.
    ///
    /// @see GetHandle(), GetTitle(), GetWidth(), GetHeight()
    bool Window::GetFullscreen() const
    {
        return m_bFullscreen;
    }

    /// Get the callback that will be executed when this window is actually destroyed.
    ///
    /// If a callback is provided, it will be executed immediately upon platform window destruction, but immediately
    /// before the Window object itself is destroyed.
    ///
    /// @return  Callback to execute when this window is actually destroyed.
    ///
    /// @see SetOnDestroyed()
    const Window::OnDestroyed& Window::GetOnDestroyed() const
    {
        return m_onDestroyed;
    }

    /// Constructor.
    Window::Parameters::Parameters()
        : pTitle( NULL )
        , width( 0 )
        , height( 0 )
        , bFullscreen( false )
    {
    }
}
