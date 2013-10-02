#pragma once

#include "PcSupport/PcSupport.h"
#include "Engine/Asset.h"

#include "Engine/Resource.h"

namespace Helium
{
    class Stream;
}

namespace Helium
{
    class AssetPreprocessor;

    /// Interface for parsing resources and creating and caching objects based on them.
    class HELIUM_PC_SUPPORT_API ResourceHandler : public Asset
    {
        HELIUM_DECLARE_ASSET( ResourceHandler, Asset );

    public:
        /// @name Construction/Destruction
        //@{
        ResourceHandler();
        virtual ~ResourceHandler();
        //@}

        /// @name Resource Handling Support
        //@{
        virtual const AssetType* GetResourceType() const;
        virtual void GetSourceExtensions( const char* const*& rppExtensions, size_t& rExtensionCount ) const;

#if HELIUM_TOOLS
        virtual bool CacheResource(
            AssetPreprocessor* pAssetPreprocessor, Resource* pResource, const String& rSourceFilePath );
        
        void SaveObjectToPersistentDataBuffer(Reflect::Object *_object, DynamicArray< uint8_t > &_buffer);
#endif
        //@}

        /// @name Static Resource Handling Support
        //@{
        static void GetAllResourceHandlers( DynamicArray< ResourceHandler* >& rResourceHandlers );
        static ResourceHandler* FindResourceHandlerForType( const AssetType* pType );
        //@}

		static ResourceHandler *GetBestResourceHandlerForFile( const String &fileName );
    };
}
