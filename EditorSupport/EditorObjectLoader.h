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
#include "Engine/ObjectLoader.h"

#include "PcSupport/XmlPackageLoaderMap.h"

namespace Lunar
{
    class XmlObjectLoader;

    /// Editor-based object loader.
    class EditorObjectLoader : public ObjectLoader
    {
    public:
        /// @name Construction/Destruction
        //@{
        EditorObjectLoader();
        ~EditorObjectLoader();
        //@}

        /// @name Loading Interface
        //@{
        virtual bool CacheObject( Object* pObject, bool bEvictPlatformPreprocessedResourceData );
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
        virtual PackageLoader* GetPackageLoader( ObjectPath path );
        virtual void TickPackageLoaders();

        virtual void OnPrecacheReady( Object* pObject, PackageLoader* pPackageLoader );
        virtual void OnLoadComplete( ObjectPath path, Object* pObject, PackageLoader* pPackageLoader );
        //@}
    };
}

#endif  // LUNAR_EDITOR_SUPPORT_EDITOR_OBJECT_LOADER_H
