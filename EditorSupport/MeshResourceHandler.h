//----------------------------------------------------------------------------------------------------------------------
// MeshResourceHandler.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_EDITOR_SUPPORT_MESH_RESOURCE_HANDLER_H
#define LUNAR_EDITOR_SUPPORT_MESH_RESOURCE_HANDLER_H

#include "EditorSupport/EditorSupport.h"

#if L_EDITOR

#include "PcSupport/ResourceHandler.h"

#include "Framework/Mesh.h"

namespace Lunar
{
    class FbxSupport;

    /// Resource handler for Mesh resource types.
    class LUNAR_EDITOR_SUPPORT_API MeshResourceHandler : public ResourceHandler
    {
        L_DECLARE_OBJECT( MeshResourceHandler, ResourceHandler );

    public:
        /// @name Construction/Destruction
        //@{
        MeshResourceHandler();
        virtual ~MeshResourceHandler();
        //@}

        /// @name Resource Handling Support
        //@{
        virtual const GameObjectType* GetResourceType() const;
        virtual void GetSourceExtensions( const tchar_t* const*& rppExtensions, size_t& rExtensionCount ) const;

        virtual bool CacheResource(
            ObjectPreprocessor* pObjectPreprocessor, Resource* pResource, const String& rSourceFilePath );
        //@}

    private:
        /// FBX support instance.
        FbxSupport& m_rFbxSupport;
    };
}

#endif  // L_EDITOR

#endif  // LUNAR_EDITOR_SUPPORT_MESH_RESOURCE_HANDLER_H
