#include "Precompile.h"
#include "Windowing/Window.h"

#include "Platform/SystemWin.h"

using namespace Helium;

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
    HELIUM_ASSERT( m_pHandle );
    HELIUM_VERIFY( DestroyWindow( static_cast<HWND>( m_pHandle ) ) );
}
