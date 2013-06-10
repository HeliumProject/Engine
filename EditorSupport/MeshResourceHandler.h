//----------------------------------------------------------------------------------------------------------------------
// MeshResourceHandler.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_EDITOR_SUPPORT_MESH_RESOURCE_HANDLER_H
#define HELIUM_EDITOR_SUPPORT_MESH_RESOURCE_HANDLER_H

#include "EditorSupport/EditorSupport.h"

#if HELIUM_TOOLS

#include "PcSupport/ResourceHandler.h"

#include "Graphics/Mesh.h"

namespace Helium
{
    class FbxSupport;

    /// Resource handler for Mesh resource types.
    class HELIUM_EDITOR_SUPPORT_API MeshResourceHandler : public ResourceHandler
    {
        HELIUM_DECLARE_ASSET( MeshResourceHandler, ResourceHandler );

    public:
        /// @name Construction/Destruction
        //@{
        MeshResourceHandler();
        virtual ~MeshResourceHandler();
        //@}

        /// @name Resource Handling Support
        //@{
        virtual const AssetType* GetResourceType() const;
        virtual void GetSourceExtensions( const char* const*& rppExtensions, size_t& rExtensionCount ) const;

        virtual bool CacheResource(
            AssetPreprocessor* pAssetPreprocessor, Resource* pResource, const String& rSourceFilePath );
        //@}

    private:
        /// FBX support instance.
        FbxSupport& m_rFbxSupport;
    };
}

#endif  // HELIUM_TOOLS

#endif  // HELIUM_EDITOR_SUPPORT_MESH_RESOURCE_HANDLER_H
