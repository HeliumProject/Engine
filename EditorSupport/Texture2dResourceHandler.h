#pragma once

#include "EditorSupport/EditorSupport.h"

#if HELIUM_TOOLS

#include "PcSupport/ResourceHandler.h"
#include "Foundation/FilePath.h"

namespace Helium
{
    /// Resource handler for Texture2d resource types.
    class HELIUM_EDITOR_SUPPORT_API Texture2dResourceHandler : public ResourceHandler
    {
        HELIUM_DECLARE_ASSET( Texture2dResourceHandler, ResourceHandler );

    public:
        /// @name Construction/Destruction
        //@{
        Texture2dResourceHandler();
        virtual ~Texture2dResourceHandler();
        //@}

        /// @name Resource Handling Support
        //@{
        virtual const AssetType* GetResourceType() const override;
        virtual void GetSourceExtensions( const char* const*& rppExtensions, size_t& rExtensionCount ) const override;

        virtual bool CacheResource(
            AssetPreprocessor* pAssetPreprocessor, Resource* pResource, const String& rSourceFilePath ) override;
        //@}
    };
}

#endif  // HELIUM_TOOLS
