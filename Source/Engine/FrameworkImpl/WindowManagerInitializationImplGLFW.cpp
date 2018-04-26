#include "Precompile.h"
#include "WindowManagerInitializationImpl.h"

#include "Windowing/WindowManager.h"

using namespace Helium;

WindowManagerInitializationImpl::WindowManagerInitializationImpl()
{
}

void WindowManagerInitializationImpl::Startup()
{
	WindowManager::Startup();

	WindowManager* pWindowManager = WindowManager::GetInstance();
	HELIUM_ASSERT( pWindowManager );

	HELIUM_VERIFY( pWindowManager->Initialize() );
}
