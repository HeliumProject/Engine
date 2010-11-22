//----------------------------------------------------------------------------------------------------------------------
// MaterialResourceHandler.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_EDITOR_SUPPORT_MATERIAL_RESOURCE_HANDLER_H
#define LUNAR_EDITOR_SUPPORT_MATERIAL_RESOURCE_HANDLER_H

#include "EditorSupport/EditorSupport.h"
#include "PcSupport/ResourceHandler.h"

namespace Lunar
{
    /// Resource handler for Material resource types.
    class LUNAR_EDITOR_SUPPORT_API MaterialResourceHandler : public ResourceHandler
    {
        L_DECLARE_OBJECT( MaterialResourceHandler, ResourceHandler );

    public:
        /// @name Construction/Destruction
        //@{
        MaterialResourceHandler();
        virtual ~MaterialResourceHandler();
        //@}

        /// @name Resource Handling Support
        //@{
        virtual Type* GetResourceType() const;

        virtual bool CacheResource(
            ObjectPreprocessor* pObjectPreprocessor, Resource* pResource, const String& rSourceFilePath );
        //@}
    };
}

#endif  // LUNAR_EDITOR_SUPPORT_MATERIAL_RESOURCE_HANDLER_H
