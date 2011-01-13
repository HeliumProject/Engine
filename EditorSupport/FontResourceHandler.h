#pragma once

#include "EditorSupport/EditorSupport.h"

#if L_EDITOR

#include "PcSupport/ResourceHandler.h"

#include "Graphics/Font.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Lunar
{
    /// Resource handler for Font resource types.
    class LUNAR_EDITOR_SUPPORT_API FontResourceHandler : public ResourceHandler
    {
        L_DECLARE_OBJECT( FontResourceHandler, ResourceHandler );

    public:
        /// @name Construction/Destruction
        //@{
        FontResourceHandler();
        virtual ~FontResourceHandler();
        //@}

        /// @name Resource Handling Support
        //@{
        virtual const GameObjectType* GetResourceType() const;
        virtual void GetSourceExtensions( const tchar_t* const*& rppExtensions, size_t& rExtensionCount ) const;

        virtual bool CacheResource(
            ObjectPreprocessor* pObjectPreprocessor, Resource* pResource, const String& rSourceFilePath );
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

        /// @name Texture Sheet Compression
        //@{
        static void CompressTexture(
            const uint8_t* pGrayscaleData, uint16_t textureWidth, uint16_t textureHeight,
            Font::ECompression compression, DynArray< DynArray< uint8_t > >& rTextureSheets );
        //@}
    };
}

#endif  // L_EDITOR
