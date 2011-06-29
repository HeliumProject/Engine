//----------------------------------------------------------------------------------------------------------------------
// WindowManagerInitialization.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_WINDOW_MANAGER_INITIALIZATION_H
#define HELIUM_FRAMEWORK_WINDOW_MANAGER_INITIALIZATION_H

#include "Framework/Framework.h"

namespace Helium
{
    class WindowManager;

    /// Interface for window manager initialization.
    class HELIUM_FRAMEWORK_API WindowManagerInitialization
    {
    public:
        /// @name Construction/Destruction
        //@{
        virtual ~WindowManagerInitialization();
        //@}

        /// @name Window Manager Initialization
        //@{
        virtual bool Initialize() = 0;
        //@}
    };
}

#endif  // HELIUM_FRAMEWORK_WINDOW_MANAGER_INITIALIZATION_H
