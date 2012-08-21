//----------------------------------------------------------------------------------------------------------------------
// ArchiveObjectLoader.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_PC_SUPPORT_ARCHIVE_OBJECT_LOADER_H
#define HELIUM_PC_SUPPORT_ARCHIVE_OBJECT_LOADER_H

#include "PcSupport/PcSupport.h"

//#if HELIUM_EDITOR

#include "Engine/GameObjectLoader.h"

#include "PcSupport/ArchivePackageLoaderMap.h"

namespace Helium
{
    class ArchiveObjectLoader;

    /// Archive-based object loader.
    class HELIUM_PC_SUPPORT_API ArchiveObjectLoader : public GameObjectLoader
    {
    public:
        /// @name Construction/Destruction
        //@{
        ArchiveObjectLoader();
        ~ArchiveObjectLoader();
        //@}

        /// @name Loading Interface
        //@{
        virtual bool CacheObject( GameObject* pObject, bool bEvictPlatformPreprocessedResourceData );
        //@}

        /// @name Static Initialization
        //@{
        static bool InitializeStaticInstance();
        //@}

    private:
        /// XML package loader map.
        ArchivePackageLoaderMap m_packageLoaderMap;

        /// @name Loading Implementation
        //@{
        virtual PackageLoader* GetPackageLoader( GameObjectPath path );
        virtual void TickPackageLoaders();

        virtual void OnPrecacheReady( GameObject* pObject, PackageLoader* pPackageLoader );
        virtual void OnLoadComplete( GameObjectPath path, GameObject* pObject, PackageLoader* pPackageLoader );
        //@}
    };
}

//#endif  // HELIUM_EDITOR

#endif  // HELIUM_ENGINE_ARCHIVE_OBJECT_LOADER_H
