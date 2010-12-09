//----------------------------------------------------------------------------------------------------------------------
// Texture2dResourceHandler.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_EDITOR_SUPPORT_TEXTURE_2D_RESOURCE_HANDLER_H
#define LUNAR_EDITOR_SUPPORT_TEXTURE_2D_RESOURCE_HANDLER_H

#include "EditorSupport/EditorSupport.h"
#include "PcSupport/ResourceHandler.h"
#include "Foundation/File/Path.h"

namespace Lunar
{
    /// Resource handler for Texture2d resource types.
    class LUNAR_EDITOR_SUPPORT_API Texture2dResourceHandler : public ResourceHandler
    {
        L_DECLARE_OBJECT( Texture2dResourceHandler, ResourceHandler );

    public:
        /// @name Construction/Destruction
        //@{
        Texture2dResourceHandler();
        virtual ~Texture2dResourceHandler();
        //@}

        /// @name Resource Handling Support
        //@{
        virtual GameObjectType* GetResourceType() const;
        virtual void GetSourceExtensions( const tchar_t* const*& rppExtensions, size_t& rExtensionCount ) const;

        virtual bool CacheResource(
            ObjectPreprocessor* pObjectPreprocessor, Resource* pResource, const String& rSourceFilePath );
        //@}
    };
}

#endif  // LUNAR_EDITOR_SUPPORT_TEXTURE_2D_RESOURCE_HANDLER_H
