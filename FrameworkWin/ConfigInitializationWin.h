//----------------------------------------------------------------------------------------------------------------------
// ConfigInitializationWin.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_WIN_CONFIG_INITIALIZATION_WIN_H
#define HELIUM_FRAMEWORK_WIN_CONFIG_INITIALIZATION_WIN_H

#include "FrameworkWin/FrameworkWin.h"
#include "Framework/ConfigInitialization.h"

namespace Helium
{
    /// Config initialization for Windows.
    class HELIUM_FRAMEWORK_WIN_API ConfigInitializationWin : public ConfigInitialization
    {
    public:
        /// @name Config Initialization
        //@{
        virtual bool Initialize();
        //@}
    };
}

#endif  // HELIUM_FRAMEWORK_WIN_CONFIG_INITIALIZATION_WIN_H
