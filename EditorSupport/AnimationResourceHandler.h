//----------------------------------------------------------------------------------------------------------------------
// AnimationResourceHandler.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_EDITOR_SUPPORT_ANIMATION_RESOURCE_HANDLER_H
#define LUNAR_EDITOR_SUPPORT_ANIMATION_RESOURCE_HANDLER_H

#include "EditorSupport/EditorSupport.h"
#include "PcSupport/ResourceHandler.h"

#include "Graphics/Animation.h"

namespace Lunar
{
    class FbxSupport;

    /// Resource handler for Animation resource types.
    class LUNAR_EDITOR_SUPPORT_API AnimationResourceHandler : public ResourceHandler
    {
        L_DECLARE_OBJECT( AnimationResourceHandler, ResourceHandler );

    public:
        /// @name Construction/Destruction
        //@{
        AnimationResourceHandler();
        virtual ~AnimationResourceHandler();
        //@}

        /// @name Resource Handling Support
        //@{
        virtual Type* GetResourceType() const;
        virtual void GetSourceExtensions( const tchar_t* const*& rppExtensions, size_t& rExtensionCount ) const;

        virtual bool CacheResource(
            ObjectPreprocessor* pObjectPreprocessor, Resource* pResource, const String& rSourceFilePath );
        //@}

    private:
        /// FBX support instance.
        FbxSupport& m_rFbxSupport;
    };
}

#endif  // LUNAR_EDITOR_SUPPORT_ANIMATION_RESOURCE_HANDLER_H
