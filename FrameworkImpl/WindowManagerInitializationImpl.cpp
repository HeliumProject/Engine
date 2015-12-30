#include "FrameworkImplPch.h"
#include "FrameworkImpl/WindowManagerInitializationImpl.h"

#include "Windowing/WindowManager.h"

using namespace Helium;

void WindowManagerInitializationImpl::Shutdown()
{
	WindowManager::Shutdown();
}
