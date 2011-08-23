//----------------------------------------------------------------------------------------------------------------------
// ArchiveObjectLoader.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_ENGINE_ARCHIVE_OBJECT_LOADER_H
#define HELIUM_ENGINE_ARCHIVE_OBJECT_LOADER_H

#include "Engine/Engine.h"

#if HELIUM_EDITOR

#include "Engine/GameObjectLoader.h"

#include "Engine/ArchivePackageLoaderMap.h"

namespace Helium
{
    class ArchiveObjectLoader;

    /// Archive-based object loader.
    class ArchiveObjectLoader : public GameObjectLoader
    {
    public:
        /// @name Construction/Destruction
        //@{
        ArchiveObjectLoader();
        ~ArchiveObjectLoader();
        //@}

        /// @name Loading Interface
        //@{
        //virtual bool CacheObject( GameObject* pObject, bool bEvictPlatformPreprocessedResourceData );
        //@}

        /// @name Static Initialization
        //@{
        HELIUM_ENGINE_API static bool InitializeStaticInstance();
        //@}

    private:
        /// XML package loader map.
        ArchivePackageLoaderMap m_packageLoaderMap;

        /// @name Loading Implementation
        //@{
        virtual PackageLoader* GetPackageLoader( GameObjectPath path );
        virtual void TickPackageLoaders();

        //virtual void OnPrecacheReady( GameObject* pObject, PackageLoader* pPackageLoader );
        //virtual void OnLoadComplete( GameObjectPath path, GameObject* pObject, PackageLoader* pPackageLoader );
        //@}
    };
}

#endif  // HELIUM_EDITOR

#endif  // HELIUM_ENGINE_ARCHIVE_OBJECT_LOADER_H
