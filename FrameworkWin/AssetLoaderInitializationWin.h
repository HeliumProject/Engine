//----------------------------------------------------------------------------------------------------------------------
// AssetLoaderInitializationWin.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_WIN_OBJECT_LOADER_INITIALIZATION_WIN_H
#define HELIUM_FRAMEWORK_WIN_OBJECT_LOADER_INITIALIZATION_WIN_H

#include "FrameworkWin/FrameworkWin.h"
#include "Framework/AssetLoaderInitialization.h"

namespace Helium
{
    /// AssetLoader initialization implementation for Windows.
    class HELIUM_FRAMEWORK_WIN_API AssetLoaderInitializationWin : public AssetLoaderInitialization
    {
    public:
        /// @name AssetLoader Initialization
        //@{
        virtual AssetLoader* Initialize();
        virtual void Shutdown();
        //@}
    };
}

#endif  // HELIUM_FRAMEWORK_WIN_OBJECT_LOADER_INITIALIZATION_WIN_H
