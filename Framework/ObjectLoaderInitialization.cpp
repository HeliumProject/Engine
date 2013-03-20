//----------------------------------------------------------------------------------------------------------------------
// ObjectLoaderInitialization.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkPch.h"
#include "Framework/ObjectLoaderInitialization.h"

#include "Engine/CacheManager.h"
#include "Engine/AssetLoader.h"

using namespace Helium;

/// Destructor.
ObjectLoaderInitialization::~ObjectLoaderInitialization()
{
}

/// @fn AssetLoader* ObjectLoaderInitialization::Initialize()
/// Create and initialize a new AssetLoader instance.
///
/// @return  Pointer to the AssetLoader instance if initialized successfully, null if creation and initialization
///          failed.
///
/// @see Shutdown()

/// Shut down and destroy the AssetLoader and any related types.
///
/// @see Initialize()
void ObjectLoaderInitialization::Shutdown()
{
    AssetLoader::DestroyStaticInstance();
    CacheManager::DestroyStaticInstance();
}
