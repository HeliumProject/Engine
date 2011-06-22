//----------------------------------------------------------------------------------------------------------------------
// PcCacheObjectLoader.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_PC_SUPPORT_PC_CACHE_OBJECT_LOADER_H
#define LUNAR_PC_SUPPORT_PC_CACHE_OBJECT_LOADER_H

#include "PcSupport/PcSupport.h"
#include "Engine/CacheObjectLoader.h"

#include "PcSupport/XmlPackageLoaderMap.h"

namespace Helium
{
    class XmlObjectLoader;

    /// Binary cache object loader, with support for loading configuration data from XML packages.
    class PcCacheObjectLoader : public CacheObjectLoader
    {
    public:
        /// @name Construction/Destruction
        //@{
        PcCacheObjectLoader();
        ~PcCacheObjectLoader();
        //@}

        /// @name Static Initialization
        //@{
        LUNAR_PC_SUPPORT_API static bool InitializeStaticInstance();
        //@}

    private:
        /// XML package loader map.
        XmlPackageLoaderMap m_packageLoaderMap;

        /// @name Loading Implementation
        //@{
        virtual PackageLoader* GetPackageLoader( GameObjectPath path );
        virtual void TickPackageLoaders();
        //@}
    };
}

#endif  // LUNAR_PC_SUPPORT_PC_CACHE_OBJECT_LOADER_H
