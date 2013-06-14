#include "WindowingPch.h"
#include "Windowing/Window.h"

using namespace Helium;

/// @copydoc Window::Destroy()
void Window::Destroy()
{
    HELIUM_ASSERT( m_pHandle );
    HELIUM_VERIFY( DestroyWindow( m_pHandle ) );
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
