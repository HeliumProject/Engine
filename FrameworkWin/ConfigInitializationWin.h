//----------------------------------------------------------------------------------------------------------------------
// ConfigInitializationWin.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_FRAMEWORK_WIN_CONFIG_INITIALIZATION_WIN_H
#define LUNAR_FRAMEWORK_WIN_CONFIG_INITIALIZATION_WIN_H

#include "FrameworkWin/FrameworkWin.h"
#include "Framework/ConfigInitialization.h"

namespace Lunar
{
    /// Config initialization for Windows.
    class LUNAR_FRAMEWORK_WIN_API ConfigInitializationWin : public ConfigInitialization
    {
    public:
        /// @name Config Initialization
        //@{
        virtual bool Initialize();
        //@}
    };
}

#endif  // LUNAR_FRAMEWORK_WIN_CONFIG_INITIALIZATION_WIN_H
