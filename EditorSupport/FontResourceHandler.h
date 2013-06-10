#pragma once

#include "EditorSupport/EditorSupport.h"

#if HELIUM_TOOLS

#include "PcSupport/ResourceHandler.h"

#include "Graphics/Font.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Helium
{
    /// Resource handler for Font resource types.
    class HELIUM_EDITOR_SUPPORT_API FontResourceHandler : public ResourceHandler
    {
        HELIUM_DECLARE_ASSET( FontResourceHandler, ResourceHandler );

    public:
        /// @name Construction/Destruction
        //@{
        FontResourceHandler();
        virtual ~FontResourceHandler();
        //@}

        /// @name Resource Handling Support
        //@{
        virtual const AssetType* GetResourceType() const;
        virtual void GetSourceExtensions( const char* const*& rppExtensions, size_t& rExtensionCount ) const;

        virtual bool CacheResource(
            AssetPreprocessor* pAssetPreprocessor, Resource* pResource, const String& rSourceFilePath );
        //@}

        /// @name Static Data Access
        //@{
        static FT_Library InitializeStaticLibrary();
        static void DestroyStaticLibrary();
        static FT_Library GetStaticLibrary();
        //@}

    private:
        /// FreeType library instance.
        static FT_Library sm_pLibrary;
		static int32_t sm_InitCount;

        /// @name Texture Sheet Compression
        //@{
        static void CompressTexture(
            const uint8_t* pGrayscaleData, uint16_t textureWidth, uint16_t textureHeight,
            Font::ECompression compression, DynamicArray< DynamicArray< uint8_t > >& rTextureSheets );
        //@}
    };
}

#endif  // HELIUM_TOOLS
