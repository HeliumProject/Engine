#include "WindowingPch.h"
#include "Windowing/WindowManager.h"

#include "Windowing/Window.h"

using namespace Helium;

/// Constructor.
WindowManager::WindowManager()
{
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
bool WindowManager::Initialize()
{
	Shutdown();

	return true;
}

/// @copydoc WindowManager::Shutdown()
void WindowManager::Shutdown()
{
}

/// @copydoc WindowManager::Update()
bool WindowManager::Update()
{
	bool bQuit = false;
	return !bQuit;
}

/// @copydoc WindowManager::RequestQuit()
void WindowManager::RequestQuit()
{
}

/// @copydoc WindowManager::Create()
Window* WindowManager::Create( Window::Parameters& rParameters )
{
	return NULL;
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

