//----------------------------------------------------------------------------------------------------------------------
// ObjectLoaderInitializationWin.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_FRAMEWORK_WIN_OBJECT_LOADER_INITIALIZATION_WIN_H
#define LUNAR_FRAMEWORK_WIN_OBJECT_LOADER_INITIALIZATION_WIN_H

#include "FrameworkWin/FrameworkWin.h"
#include "Framework/ObjectLoaderInitialization.h"

namespace Lunar
{
    /// ObjectLoader initialization implementation for Windows.
    class LUNAR_FRAMEWORK_WIN_API ObjectLoaderInitializationWin : public ObjectLoaderInitialization
    {
    public:
        /// @name ObjectLoader Initialization
        //@{
        virtual ObjectLoader* Initialize();
        virtual void Shutdown();
        //@}
    };
}

#endif  // LUNAR_FRAMEWORK_WIN_OBJECT_LOADER_INITIALIZATION_WIN_H
