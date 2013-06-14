#pragma once

#include "EditorSupport/EditorSupport.h"

#if HELIUM_TOOLS

#include "PcSupport/ResourceHandler.h"

#include "Graphics/Shader.h"

namespace Helium
{
    /// Resource handler for Shader resource types.
    class HELIUM_EDITOR_SUPPORT_API ShaderResourceHandler : public ResourceHandler
    {
        HELIUM_DECLARE_ASSET( ShaderResourceHandler, ResourceHandler );

    public:
        /// @name Construction/Destruction
        //@{
        ShaderResourceHandler();
        virtual ~ShaderResourceHandler();
        //@}

        /// @name Resource Handling Support
        //@{
        virtual const AssetType* GetResourceType() const;
        virtual void GetSourceExtensions( const char* const*& rppExtensions, size_t& rExtensionCount ) const;

        virtual bool CacheResource(
            AssetPreprocessor* pAssetPreprocessor, Resource* pResource, const String& rSourceFilePath );
        //@}

    private:
        /// @name Private Utility Functions
        //@{
        void ParseLine(
            AssetPath shaderPath, Shader::PersistentResourceData& rResourceData, const char* pLineStart,
            const char* pLineEnd );
        template< typename OptionType > bool ParseLineDuplicateOptionCheck(
            Name optionName, const DynamicArray< OptionType >& rOptions );
        //@}
    };
}

#endif  // HELIUM_TOOLS
