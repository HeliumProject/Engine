//----------------------------------------------------------------------------------------------------------------------
// EditorObjectLoader.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_EDITOR_SUPPORT_EDITOR_OBJECT_LOADER_H
#define LUNAR_EDITOR_SUPPORT_EDITOR_OBJECT_LOADER_H

#include "EditorSupport/EditorSupport.h"

#if L_EDITOR

#include "Engine/GameObjectLoader.h"

#include "PcSupport/XmlPackageLoaderMap.h"

namespace Helium
{
    class XmlObjectLoader;

    /// Editor-based object loader.
    class EditorObjectLoader : public GameObjectLoader
    {
    public:
        /// @name Construction/Destruction
        //@{
        EditorObjectLoader();
        ~EditorObjectLoader();
        //@}

        /// @name Loading Interface
        //@{
        virtual bool CacheObject( GameObject* pObject, bool bEvictPlatformPreprocessedResourceData );
        //@}

        /// @name Static Initialization
        //@{
        LUNAR_EDITOR_SUPPORT_API static bool InitializeStaticInstance();
        //@}

    private:
        /// XML package loader map.
        XmlPackageLoaderMap m_packageLoaderMap;

        /// @name Loading Implementation
        //@{
        virtual PackageLoader* GetPackageLoader( GameObjectPath path );
        virtual void TickPackageLoaders();

        virtual void OnPrecacheReady( GameObject* pObject, PackageLoader* pPackageLoader );
        virtual void OnLoadComplete( GameObjectPath path, GameObject* pObject, PackageLoader* pPackageLoader );
        //@}
    };
}

#endif  // L_EDITOR

#endif  // LUNAR_EDITOR_SUPPORT_EDITOR_OBJECT_LOADER_H
