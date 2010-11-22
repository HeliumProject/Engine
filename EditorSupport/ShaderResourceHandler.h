//----------------------------------------------------------------------------------------------------------------------
// ShaderResourceHandler.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_EDITOR_SUPPORT_SHADER_RESOURCE_HANDLER_H
#define LUNAR_EDITOR_SUPPORT_SHADER_RESOURCE_HANDLER_H

#include "EditorSupport/EditorSupport.h"
#include "PcSupport/ResourceHandler.h"

#include "Graphics/Shader.h"

namespace Lunar
{
    /// Resource handler for Shader resource types.
    class LUNAR_EDITOR_SUPPORT_API ShaderResourceHandler : public ResourceHandler
    {
        L_DECLARE_OBJECT( ShaderResourceHandler, ResourceHandler );

    public:
        /// @name Construction/Destruction
        //@{
        ShaderResourceHandler();
        virtual ~ShaderResourceHandler();
        //@}

        /// @name Resource Handling Support
        //@{
        virtual Type* GetResourceType() const;
        virtual void GetSourceExtensions( const tchar_t* const*& rppExtensions, size_t& rExtensionCount ) const;

        virtual bool CacheResource(
            ObjectPreprocessor* pObjectPreprocessor, Resource* pResource, const String& rSourceFilePath );
        //@}

    private:
        /// @name Private Utility Functions
        //@{
        void ParseLine(
            GameObjectPath shaderPath, Shader::PersistentResourceData& rResourceData, const char* pLineStart,
            const char* pLineEnd );
        template< typename OptionType > bool ParseLineDuplicateOptionCheck(
            Name optionName, const DynArray< OptionType >& rOptions );
        //@}
    };
}

#endif  // LUNAR_EDITOR_SUPPORT_SHADER_RESOURCE_HANDLER_H
