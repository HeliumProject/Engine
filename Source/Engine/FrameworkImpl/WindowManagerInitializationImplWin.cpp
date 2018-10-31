#include "Precompile.h"
#include "FrameworkImpl/WindowManagerInitializationImpl.h"

#include "Windowing/WindowManager.h"

using namespace Helium;

/// Constructor.
///
/// @param[in] hInstance  Handle to the application instance.
/// @param[in] nCmdShow   Flags specifying how the application window should be shown (passed in from WinMain()).
WindowManagerInitializationImpl::WindowManagerInitializationImpl( void* hInstance, int nCmdShow )
: m_hInstance( hInstance )
, m_nCmdShow( nCmdShow )
{
	HELIUM_ASSERT( hInstance );
}

void WindowManagerInitializationImpl::Startup()
{
	WindowManager::Startup();

	WindowManager* pWindowManager = WindowManager::GetInstance();
	HELIUM_ASSERT( pWindowManager );

	HELIUM_VERIFY( pWindowManager->Initialize( m_hInstance, m_nCmdShow ) );
}
