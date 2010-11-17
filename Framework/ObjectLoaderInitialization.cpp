//----------------------------------------------------------------------------------------------------------------------
// ObjectLoaderInitialization.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkPch.h"
#include "Framework/ObjectLoaderInitialization.h"

#include "Engine/CacheManager.h"
#include "Engine/ObjectLoader.h"

namespace Lunar
{
    /// Destructor.
    ObjectLoaderInitialization::~ObjectLoaderInitialization()
    {
    }

    /// @fn ObjectLoader* ObjectLoaderInitialization::Initialize()
    /// Create and initialize a new ObjectLoader instance.
    ///
    /// @return  Pointer to the ObjectLoader instance if initialized successfully, null if creation and initialization
    ///          failed.
    ///
    /// @see Shutdown()

    /// Shut down and destroy the ObjectLoader and any related types.
    ///
    /// @see Initialize()
    void ObjectLoaderInitialization::Shutdown()
    {
        ObjectLoader::DestroyStaticInstance();
        CacheManager::DestroyStaticInstance();
    }
}
