//----------------------------------------------------------------------------------------------------------------------
// WindowManager.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "WindowingPch.h"
#include "Windowing/WindowManager.h"

namespace Lunar
{
    WindowManager* WindowManager::sm_pInstance = NULL;

    /// Constructor.
    WindowManager::WindowManager()
    {
    }

    /// Destructor.
    WindowManager::~WindowManager()
    {
    }

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
    /// @see DestroyStaticInstance()
    WindowManager* WindowManager::GetStaticInstance()
    {
        return sm_pInstance;
    }

    /// Destroy the global window manager instance if one exists.
    ///
    /// @see GetStaticInstance()
    void WindowManager::DestroyStaticInstance()
    {
        if( sm_pInstance )
        {
            sm_pInstance->Shutdown();
            delete sm_pInstance;
            sm_pInstance = NULL;
        }
    }
}
