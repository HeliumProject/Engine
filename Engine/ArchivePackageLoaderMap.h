//----------------------------------------------------------------------------------------------------------------------
// ArchivePackageLoaderMap.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_ENGINE_ARCHIVE_PACKAGE_LOADER_MAP_H
#define HELIUM_ENGINE_ARCHIVE_PACKAGE_LOADER_MAP_H

#include "Engine/Engine.h"

namespace Helium
{
    class ArchivePackageLoader;

    /// Archive package loader management for object loaders.
    class HELIUM_ENGINE_API ArchivePackageLoaderMap
    {
    public:
        /// @name Construction/Destruction
        //@{
        ArchivePackageLoaderMap();
        ~ArchivePackageLoaderMap();
        //@}

        /// @name Package Loader Access
        //@{
        ArchivePackageLoader* GetPackageLoader( GameObjectPath path );
        void TickPackageLoaders();
        //@}

    private:
        /// Package loader hash map (package path used as loader key).
        ConcurrentHashMap< GameObjectPath, ArchivePackageLoader* > m_packageLoaderMap;

    };
}

#endif  // HELIUM_ENGINE_ARCHIVE_PACKAGE_LOADER_MAP_H
