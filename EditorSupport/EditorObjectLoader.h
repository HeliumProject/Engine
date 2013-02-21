//----------------------------------------------------------------------------------------------------------------------
// EditorObjectLoader.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_EDITOR_SUPPORT_EDITOR_OBJECT_LOADER_H
#define HELIUM_EDITOR_SUPPORT_EDITOR_OBJECT_LOADER_H

#include "EditorSupport/EditorSupport.h"

#if HELIUM_TOOLS

#include "Engine/AssetLoader.h"

#include "PcSupport/ArchivePackageLoaderMap.h"

namespace Helium
{
    class XmlObjectLoader;

    /// Editor-based object loader.
    class EditorObjectLoader : public AssetLoader
    {
    public:
        /// @name Construction/Destruction
        //@{
        EditorObjectLoader();
        ~EditorObjectLoader();
        //@}

        /// @name Loading Interface
        //@{
        virtual bool CacheObject( Asset* pObject, bool bEvictPlatformPreprocessedResourceData );
        //@}

        /// @name Static Initialization
        //@{
        HELIUM_EDITOR_SUPPORT_API static bool InitializeStaticInstance();
        //@}

    private:
        /// XML package loader map.
        ArchivePackageLoaderMap m_packageLoaderMap;

        /// @name Loading Implementation
        //@{
        virtual PackageLoader* GetPackageLoader( AssetPath path );
        virtual void TickPackageLoaders();

        virtual void OnPrecacheReady( Asset* pObject, PackageLoader* pPackageLoader );
        virtual void OnLoadComplete( AssetPath path, Asset* pObject, PackageLoader* pPackageLoader );
        //@}
    };
}

#endif  // HELIUM_TOOLS

#endif  // HELIUM_EDITOR_SUPPORT_EDITOR_OBJECT_LOADER_H
