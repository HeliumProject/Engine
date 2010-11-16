//----------------------------------------------------------------------------------------------------------------------
// CacheObjectLoader.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_ENGINE_CACHE_OBJECT_LOADER_H
#define LUNAR_ENGINE_CACHE_OBJECT_LOADER_H

#include "Engine/ObjectLoader.h"

namespace Lunar
{
    class CachePackageLoader;

    /// Object loader for loading objects from binary cached data.
    class LUNAR_ENGINE_API CacheObjectLoader : public ObjectLoader
    {
    public:
        /// @name Construction/Destruction
        //@{
        CacheObjectLoader();
        virtual ~CacheObjectLoader();
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
        virtual PackageLoader* GetPackageLoader( ObjectPath path );
        virtual void TickPackageLoaders();
        //@}
    };
}

#endif  // LUNAR_ENGINE_CACHE_OBJECT_LOADER_H
