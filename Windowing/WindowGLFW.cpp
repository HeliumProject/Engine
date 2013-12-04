#include "WindowingPch.h"
#include "Windowing/Window.h"

#include "GLFW/glfw3.h"

#if HELIUM_OS_WIN
# define GLFW_EXPOSE_NATIVE_WIN32
# define GLFW_EXPOSE_NATIVE_WGL
#elif HELIUM_OS_LINUX
# define GLFW_EXPOSE_NATIVE_X11
# define GLFW_EXPOSE_NATIVE_GLX
#elif HELIUM_OS_MAC
# define GLFW_EXPOSE_NATIVE_COCOA
# define GLFW_EXPOSE_NATIVE_NSGL
#endif
#include "GLFW/glfw3native.h"

using namespace Helium;

#if HELIUM_OPENGL
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
#if HELIUM_OS_WIN
    return glfwGetWin32Window( m_pHandle );
#elif HELIUM_OS_LINUX
    return glfwGetX11Window( m_pHandle );
#elif HELIUM_OS_MAC
    return glfwGetCocoaWindow( m_pHandle );
#endif
}
#endif

/// @fn void Window::Destroy()
/// Destroy this window.
///
/// Since a window may not be immediately destroyed by the platform windowing system (i.e. platform message
/// processing may need to be performed to fulfill the destroy request), windows cannot be safely deleted using the
/// default "delete" operator.
///
/// To avoid dangling references to Window objects, references should be cleared immediately after calling this
/// function or in response to the execution of an "on destroyed" callback registered using SetOnDestroyed().
///
/// @see WindowManager::Create(), SetOnDestroyed(), GetOnDestroyed()
void Window::Destroy()
{
	if( m_onDestroyed.Valid() )
	{
		m_onDestroyed.Invoke( this );
	}

	glfwDestroyWindow( m_pHandle );
}
