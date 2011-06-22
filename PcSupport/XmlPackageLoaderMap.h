//----------------------------------------------------------------------------------------------------------------------
// XmlPackageLoaderMap.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_PC_SUPPORT_XML_PACKAGE_LOADER_MAP_H
#define LUNAR_PC_SUPPORT_XML_PACKAGE_LOADER_MAP_H

#include "PcSupport/PcSupport.h"

namespace Helium
{
    class XmlPackageLoader;

    /// XML package loader management for object loaders.
    class LUNAR_PC_SUPPORT_API XmlPackageLoaderMap
    {
    public:
        /// @name Construction/Destruction
        //@{
        XmlPackageLoaderMap();
        ~XmlPackageLoaderMap();
        //@}

        /// @name Package Loader Access
        //@{
        XmlPackageLoader* GetPackageLoader( GameObjectPath path );
        void TickPackageLoaders();
        //@}

    private:
        /// Package loader hash map (package path used as loader key).
        ConcurrentHashMap< GameObjectPath, XmlPackageLoader* > m_packageLoaderMap;
        /// Cached list of package loaders iterated over in Tick() (separated to avoid deadlocks with concurrent hash
        /// map access).
        DynArray< XmlPackageLoader* > m_packageLoaderTickArray;
    };
}

#endif  // LUNAR_PC_SUPPORT_XML_PACKAGE_LOADER_MAP_H
