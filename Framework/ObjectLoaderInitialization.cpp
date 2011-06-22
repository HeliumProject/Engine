//----------------------------------------------------------------------------------------------------------------------
// ObjectLoaderInitialization.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkPch.h"
#include "Framework/ObjectLoaderInitialization.h"

#include "Engine/CacheManager.h"
#include "Engine/GameObjectLoader.h"

using namespace Helium;

/// Destructor.
ObjectLoaderInitialization::~ObjectLoaderInitialization()
{
}

/// @fn GameObjectLoader* ObjectLoaderInitialization::Initialize()
/// Create and initialize a new GameObjectLoader instance.
///
/// @return  Pointer to the GameObjectLoader instance if initialized successfully, null if creation and initialization
///          failed.
///
/// @see Shutdown()

/// Shut down and destroy the GameObjectLoader and any related types.
///
/// @see Initialize()
void ObjectLoaderInitialization::Shutdown()
{
    GameObjectLoader::DestroyStaticInstance();
    CacheManager::DestroyStaticInstance();
}
