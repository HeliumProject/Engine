//----------------------------------------------------------------------------------------------------------------------
// Texture2dResourceHandler.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_EDITOR_SUPPORT_TEXTURE_2D_RESOURCE_HANDLER_H
#define HELIUM_EDITOR_SUPPORT_TEXTURE_2D_RESOURCE_HANDLER_H

#include "EditorSupport/EditorSupport.h"

#if HELIUM_EDITOR

#include "PcSupport/ResourceHandler.h"
#include "Foundation/File/Path.h"

namespace Helium
{
    /// Resource handler for Texture2d resource types.
    class HELIUM_EDITOR_SUPPORT_API Texture2dResourceHandler : public ResourceHandler
    {
        HELIUM_DECLARE_OBJECT( Texture2dResourceHandler, ResourceHandler );

    public:
        /// @name Construction/Destruction
        //@{
        Texture2dResourceHandler();
        virtual ~Texture2dResourceHandler();
        //@}

        /// @name Resource Handling Support
        //@{
        virtual const GameObjectType* GetResourceType() const;
        virtual void GetSourceExtensions( const tchar_t* const*& rppExtensions, size_t& rExtensionCount ) const;

        virtual bool CacheResource(
            ObjectPreprocessor* pObjectPreprocessor, Resource* pResource, const String& rSourceFilePath );
        //@}
    };
}

#endif  // HELIUM_EDITOR

#endif  // HELIUM_EDITOR_SUPPORT_TEXTURE_2D_RESOURCE_HANDLER_H
