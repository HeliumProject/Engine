//----------------------------------------------------------------------------------------------------------------------
// ObjectLoaderInitialization.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_FRAMEWORK_OBJECT_LOADER_INITIALIZATION_H
#define LUNAR_FRAMEWORK_OBJECT_LOADER_INITIALIZATION_H

#include "Framework/Framework.h"

namespace Lunar
{
    class ObjectLoader;

    /// Interface for initializing and uninitializing the ObjectLoader and related objects.
    class LUNAR_FRAMEWORK_API ObjectLoaderInitialization
    {
    public:
        /// @name Construction/Destruction
        //@{
        virtual ~ObjectLoaderInitialization();
        //@}

        /// @name ObjectLoader Initialization
        //@{
        virtual ObjectLoader* Initialize() = 0;
        virtual void Shutdown();
        //@}
    };
}

#endif  // LUNAR_FRAMEWORK_OBJECT_LOADER_INITIALIZATION_H
