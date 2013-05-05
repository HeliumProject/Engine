//----------------------------------------------------------------------------------------------------------------------
// CacheAssetLoader.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_ENGINE_CACHE_OBJECT_LOADER_H
#define HELIUM_ENGINE_CACHE_OBJECT_LOADER_H

#include "Engine/AssetLoader.h"

namespace Helium
{
    class CachePackageLoader;

    /// Asset loader for loading objects from binary cached data.
    class HELIUM_ENGINE_API CacheAssetLoader : public AssetLoader
    {
    public:
        /// @name Construction/Destruction
        //@{
        CacheAssetLoader();
        virtual ~CacheAssetLoader();
        //@}

        /// @name Static Initialization
        //@{
        static bool InitializeStaticInstance();
        //@}

    protected:
        /// Package loader (currently only one, may support multiple later).
        CachePackageLoader* m_pPackageLoader;

        /// @name Loading Implementation
        //@{
        virtual PackageLoader* GetPackageLoader( AssetPath path );
        virtual void TickPackageLoaders();
        //@}
    };
}

#endif  // HELIUM_ENGINE_CACHE_OBJECT_LOADER_H
