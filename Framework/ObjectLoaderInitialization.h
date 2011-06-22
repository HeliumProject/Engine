//----------------------------------------------------------------------------------------------------------------------
// ObjectLoaderInitialization.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_OBJECT_LOADER_INITIALIZATION_H
#define HELIUM_FRAMEWORK_OBJECT_LOADER_INITIALIZATION_H

#include "Framework/Framework.h"

namespace Helium
{
    class GameObjectLoader;

    /// Interface for initializing and uninitializing the GameObjectLoader and related objects.
    class HELIUM_FRAMEWORK_API ObjectLoaderInitialization
    {
    public:
        /// @name Construction/Destruction
        //@{
        virtual ~ObjectLoaderInitialization();
        //@}

        /// @name GameObjectLoader Initialization
        //@{
        virtual GameObjectLoader* Initialize() = 0;
        virtual void Shutdown();
        //@}
    };
}

#endif  // HELIUM_FRAMEWORK_OBJECT_LOADER_INITIALIZATION_H
