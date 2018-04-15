#include "Precompile.h"
#include "Windowing/WindowManager.h"
#include "Framework/TaskScheduler.h"
#include "Framework/GameSystem.h"

using namespace Helium;

static uint32_t g_InitCount = 0;
WindowManager* WindowManager::sm_pInstance = NULL;

/// @fn void WindowManager::Shutdown()
/// Shut down this manager.

/// @fn bool WindowManager::Update()
/// Update this window manager and process system messages.
///
/// @return  True if the application should continue running, false if it should shut down.

/// @fn void WindowManager::RequestQuit()
/// Request application exit.
///
/// This will post a request to quit on the system message queue.  Update() will return false once it has processed
/// this message.

/// @fn Window* WindowManager::Create( Window::Parameters& rParameters )
/// Create a window.
///
/// @param[in] rParameters  Window creation parameters.
///
/// @return  Pointer to a window if created successfully, null if creation failed.
///
/// @see Window::Destroy()

/// Get the global window manager instance.
///
/// A window manager instance must be initialized first through the interface of one of the WindowManager
/// subclasses.
///
/// @return  WindowManager instance.  If an instance has not yet been initialized, this will return null.
///
/// @see Startup(), Shutdown()
WindowManager* WindowManager::GetInstance()
{
	HELIUM_ASSERT( sm_pInstance );
	return sm_pInstance;
}

/// Create the static window manager instance as a WindowManager.
///
/// @see Shudown(), GetInstance()
void WindowManager::Startup()
{
	if ( ++g_InitCount == 1 )
	{
		HELIUM_ASSERT( !sm_pInstance );
		sm_pInstance = new WindowManager;
		HELIUM_ASSERT( sm_pInstance );
	}
}

/// Destroy the global window manager instance if one exists.
///
/// @see Startup(), GetInstance()
void WindowManager::Shutdown()
{
	if ( --g_InitCount == 0 )
	{
		HELIUM_ASSERT( sm_pInstance );
		sm_pInstance->Cleanup();
		delete sm_pInstance;
		sm_pInstance = NULL;
	}
}

struct HELIUM_WINDOWING_API WindowManagerUpdateTask : public TaskDefinition
{
	HELIUM_DECLARE_TASK(WindowManagerUpdateTask)
	virtual void DefineContract(TaskContract &rContract)
	{
		rContract.ExecuteAfter< Helium::StandardDependencies::Render >();
	}
};

void UpdateWindows( DynamicArray< WorldPtr > & )
{
	WindowManager *pWindowManager = WindowManager::GetInstance();
	HELIUM_ASSERT( pWindowManager );

	if ( !pWindowManager->Update() )
	{
		GameSystem::GetInstance()->StopRunning();
	}
}

HELIUM_DEFINE_TASK( WindowManagerUpdateTask, UpdateWindows, TickTypes::Client )