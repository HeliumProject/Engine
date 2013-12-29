#include "WindowingPch.h"
#include "Windowing/Window.h"

using namespace Helium;

/// Constructor.
///
/// Initializes all members to zero-default values.
Window::Window()
: m_pHandle( NULL )
, m_width( 0 )
, m_height( 0 )
, m_bFullscreen( false )
{
}

/// Constructor.
///
/// @param[in] pHandle      Platform-specific window handle value.
/// @param[in] pTitle       Window title string (can be null).
/// @param[in] width        Window width, in pixels.
/// @param[in] height       Window height, in pixels.
/// @param[in] bFullscreen  True if the window is configured for display as a full-screen window, false if it is set
///                         up for windowed display.
Window::Window( Handle pHandle, const char* pTitle, uint32_t width, uint32_t height, bool bFullscreen )
: m_pHandle( pHandle )
, m_width( width )
, m_height( height )
, m_bFullscreen( bFullscreen )
{
    if( pTitle )
    {
        m_title = pTitle;
    }
}

/// Destructor.
Window::~Window()
{
}

/// Set the window parameters after construction.
///
/// @param[in] pHandle      Window handle.
/// @param[in] pTitle       Window title string (can be null).
/// @param[in] width        Window width, in pixels.
/// @param[in] height       Window height, in pixels.
/// @param[in] bFullscreen  True if the window is configured for display as a full-screen window, false if it is set
///                         up for windowed display.
void Window::Set( Handle pHandle, const char* pTitle, uint32_t width, uint32_t height, bool bFullscreen )
{
    HELIUM_ASSERT( pHandle );
    m_pHandle = pHandle;

    if( pTitle )
    {
        m_title = pTitle;
    }

    m_width = width;
    m_height = height;
    m_bFullscreen = bFullscreen;
}

#if !HELIUM_OPENGL
/// Get the platform-specific handle associated with this window.
///
/// @return  Window handle.
///
/// @see GetTitle(), GetWidth(), GetHeight(), GetFullscreen(), GetHandle()
void* Window::GetNativeHandle() const
{
    // This GetNativeHandle() method exists as a workaround to expose native window handles
    // even when we are using GLFW as a platform independent windowing API.  We need this
    // platform specific handle to pass to OIS for input handling.  Eventually, input handling
    // should also be routed through GLFW and this code should be removed.
    return GetHandle();
}
#endif

/// Set a callback to execute when this window is actually destroyed.
///
/// If a callback is provided, it will be executed immediately upon platform window destruction, but immediately
/// before the Window object itself is destroyed.
///
/// @param[in] rOnDestroyed  Callback to execute when this window is actually destroyed.
///
/// @see GetOnDestroyed()
void Window::SetOnDestroyed( const Delegate<Window*>& rOnDestroyed )
{
    m_onDestroyed = rOnDestroyed;
}
