#include "WindowingPch.h"
#include "Windowing/WindowManager.h"

#include "Windowing/Window.h"
#include "Dependencies/glfw/include/GLFW/glfw3.h"

using namespace Helium;

/// Constructor.
WindowManager::WindowManager()
: m_isInitialized(false)
, m_isQuitting(false)
{
	glfwSetErrorCallback(WindowManager::GLFWErrorCallback);
}

/// Destructor.
WindowManager::~WindowManager()
{
}

/// Initialize this manager.
///
/// @return  True if window manager initialization was successful, false if not.
///
/// @see Shutdown()
bool WindowManager::Initialize( )
{
	Shutdown();

	// Initialize GLFW
	m_isQuitting = false;
	m_isInitialized = (GL_TRUE == glfwInit());
	HELIUM_ASSERT( m_isInitialized );

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	return m_isInitialized;
}

/// @copydoc WindowManager::Shutdown()
void WindowManager::Shutdown()
{
    glfwDefaultWindowHints();
	glfwTerminate();
	m_isInitialized = false;
}

/// @copydoc WindowManager::Update()
bool WindowManager::Update()
{
	// Poll for events, process callbacks.
	glfwPollEvents();

	// Report whether or not the user has called RequestQuit().
	return !m_isQuitting;
}

/// @copydoc WindowManager::RequestQuit()
void WindowManager::RequestQuit()
{
	m_isQuitting = true;
}

/// @copydoc WindowManager::Create()
Window* WindowManager::Create( Window::Parameters& rParameters )
{
	HELIUM_ASSERT( m_isInitialized );

	// Validate window creation parameters.
	uint32_t width = rParameters.width;
	if( width == 0 )
	{
		HELIUM_TRACE(
			TraceLevels::Warning,
			"WindowManager::Create(): Zero width specified.  Actual window will have a width of 1.\n" );
		width = 1;
	}
	uint32_t height = rParameters.height;
	if( height == 0 )
	{
		HELIUM_TRACE(
			TraceLevels::Warning,
			"WindowManager::Create(): Zero height specified.  Actual window will have a height of 1.\n"  );
		height = 1;
	}

	// If the user specified full screen, get a handle to the primary monitor.
	GLFWmonitor *pMonitor = NULL;
	if (rParameters.bFullscreen)
	{
		pMonitor = glfwGetPrimaryMonitor();
		HELIUM_ASSERT( pMonitor );
	}

	// Create the window itself.
	Window::Handle pHandle = glfwCreateWindow( (int)width, (int)height, rParameters.pTitle, pMonitor, NULL );
	HELIUM_ASSERT( pHandle );

	// Create a Window object, return a pointer to the user.
	Window *pWindow = new Window( pHandle, rParameters.pTitle, width, height, rParameters.bFullscreen );
	HELIUM_ASSERT( pWindow );

	// Register window for close callbacks.  Also, associate our GLFW window handle
	// with a reference to our Helium Window object.
	glfwSetWindowCloseCallback( pHandle, WindowManager::GLFWCloseCallback );
	glfwSetWindowUserPointer( pHandle, static_cast<void*>(pWindow) );

	return pWindow;
}

/// Create the static window manager instance as a WindowManager.
///
/// @return  Pointer to the newly allocated window manager instance if one was created successfully, null if not or
///          another window manager instance already exists.
WindowManager* WindowManager::CreateStaticInstance()
{
	if( sm_pInstance )
	{
		return NULL;
	}

	WindowManager* pWindowManager = new WindowManager;
	HELIUM_ASSERT( pWindowManager );
	sm_pInstance = pWindowManager;

	return pWindowManager;
}

/// Command the specified window to close.
void WindowManager::GLFWCloseCallback( Window::Handle pHandle)
{
	// Don't immediately destroy the window here; tell the Window object to destroy itself.
	Window *pWindow = static_cast<Window*>(glfwGetWindowUserPointer( pHandle ));
	HELIUM_ASSERT( pWindow );
	pWindow->Destroy();
}

/// Handle GLFW error messages.  Print them to the Helium logger.
void WindowManager::GLFWErrorCallback( int error, const char* description )
{
	HELIUM_TRACE(
		TraceLevels::Error,
		"WindowManager: %s\n", description );
}
