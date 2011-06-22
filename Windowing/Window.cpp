//----------------------------------------------------------------------------------------------------------------------
// Window.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "WindowingPch.h"
#include "Windowing/Window.h"

using namespace Helium;

/// Constructor.
///
/// Initializes all members to zero-default values.
Window::Window()
: m_onDestroyed( NULL )
, m_pHandle( NULL )
, m_width( 0 )
, m_height( 0 )
, m_bFullscreen( false )
{
}

/// Constructor.
///
/// @param[in] pHandle      Platform-specific window handle value.
/// @param[in] pTitle       Window title string (can be null).
/// @param[in] width        Window width, in pixels.
/// @param[in] height       Window height, in pixels.
/// @param[in] bFullscreen  True if the window is configured for display as a full-screen window, false if it is set
///                         up for windowed display.
Window::Window( void* pHandle, const tchar_t* pTitle, uint32_t width, uint32_t height, bool bFullscreen )
: m_pHandle( pHandle )
, m_width( width )
, m_height( height )
, m_bFullscreen( bFullscreen )
{
    if( pTitle )
    {
        m_title = pTitle;
    }
}

/// Destructor.
Window::~Window()
{
}

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

/// Set a callback to execute when this window is actually destroyed.
///
/// If a callback is provided, it will be executed immediately upon platform window destruction, but immediately
/// before the Window object itself is destroyed.
///
/// @param[in] rOnDestroyed  Callback to execute when this window is actually destroyed.
///
/// @see GetOnDestroyed()
void Window::SetOnDestroyed( const OnDestroyed& rOnDestroyed )
{
    m_onDestroyed = rOnDestroyed;
}
