//----------------------------------------------------------------------------------------------------------------------
// ObjectLoaderInitializationWin.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_WIN_OBJECT_LOADER_INITIALIZATION_WIN_H
#define HELIUM_FRAMEWORK_WIN_OBJECT_LOADER_INITIALIZATION_WIN_H

#include "FrameworkWin/FrameworkWin.h"
#include "Framework/ObjectLoaderInitialization.h"

namespace Helium
{
    /// GameObjectLoader initialization implementation for Windows.
    class HELIUM_FRAMEWORK_WIN_API ObjectLoaderInitializationWin : public ObjectLoaderInitialization
    {
    public:
        /// @name GameObjectLoader Initialization
        //@{
        virtual GameObjectLoader* Initialize();
        virtual void Shutdown();
        //@}
    };
}

#endif  // HELIUM_FRAMEWORK_WIN_OBJECT_LOADER_INITIALIZATION_WIN_H
