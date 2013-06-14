#pragma once

#include "EditorSupport/EditorSupport.h"

#if HELIUM_TOOLS

#include "PcSupport/ResourceHandler.h"

namespace Helium
{
    /// Resource handler for Material resource types.
    class HELIUM_EDITOR_SUPPORT_API MaterialResourceHandler : public ResourceHandler
    {
        HELIUM_DECLARE_ASSET( MaterialResourceHandler, ResourceHandler );

    public:
        /// @name Construction/Destruction
        //@{
        MaterialResourceHandler();
        virtual ~MaterialResourceHandler();
        //@}

        /// @name Resource Handling Support
        //@{
        virtual const AssetType* GetResourceType() const;

        virtual bool CacheResource(
            AssetPreprocessor* pAssetPreprocessor, Resource* pResource, const String& rSourceFilePath );
        //@}
    };
}

#endif  // HELIUM_TOOLS
