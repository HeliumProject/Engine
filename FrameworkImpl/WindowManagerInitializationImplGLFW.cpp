#include "FrameworkImplPch.h"
#include "FrameworkImpl/WindowManagerInitializationImpl.h"

#include "Windowing/WindowManager.h"

using namespace Helium;

/// Constructor.
WindowManagerInitializationImpl::WindowManagerInitializationImpl()
{
}

/// @copydoc WindowManager::Initialize()
bool WindowManagerInitializationImpl::Initialize()
{
    WindowManager* pWindowManager = WindowManager::CreateStaticInstance();
    HELIUM_ASSERT( pWindowManager );
    if( !pWindowManager )
    {
        return false;
    }

    if( !pWindowManager->Initialize() )
    {
        WindowManager::DestroyStaticInstance();
        return false;
    }

    return true;
}
