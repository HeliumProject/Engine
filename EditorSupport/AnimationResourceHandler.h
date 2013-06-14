#pragma once

#include "EditorSupport/EditorSupport.h"

#if HELIUM_TOOLS

#include "PcSupport/ResourceHandler.h"

#include "Graphics/Animation.h"

namespace Helium
{
    class FbxSupport;

    /// Resource handler for Animation resource types.
    class HELIUM_EDITOR_SUPPORT_API AnimationResourceHandler : public ResourceHandler
    {
        HELIUM_DECLARE_ASSET( AnimationResourceHandler, ResourceHandler );

    public:
        /// @name Construction/Destruction
        //@{
        AnimationResourceHandler();
        virtual ~AnimationResourceHandler();
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
