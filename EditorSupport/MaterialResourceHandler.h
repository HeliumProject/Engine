//----------------------------------------------------------------------------------------------------------------------
// MaterialResourceHandler.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_EDITOR_SUPPORT_MATERIAL_RESOURCE_HANDLER_H
#define HELIUM_EDITOR_SUPPORT_MATERIAL_RESOURCE_HANDLER_H

#include "EditorSupport/EditorSupport.h"

#if L_EDITOR

#include "PcSupport/ResourceHandler.h"

namespace Helium
{
    /// Resource handler for Material resource types.
    class HELIUM_EDITOR_SUPPORT_API MaterialResourceHandler : public ResourceHandler
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
        virtual const GameObjectType* GetResourceType() const;

        virtual bool CacheResource(
            ObjectPreprocessor* pObjectPreprocessor, Resource* pResource, const String& rSourceFilePath );
        //@}
    };
}

#endif  // L_EDITOR

#endif  // HELIUM_EDITOR_SUPPORT_MATERIAL_RESOURCE_HANDLER_H
