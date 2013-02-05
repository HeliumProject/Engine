#include "EditorSupportPch.h"

#if HELIUM_TOOLS

#include "EditorSupport/FontResourceHandler.h"

#include "Engine/FileLocations.h"
#include "Foundation/FileStream.h"
#include "Engine/BinarySerializer.h"
#include "PcSupport/ObjectPreprocessor.h"
#include "PcSupport/PlatformPreprocessor.h"
#include "EditorSupport/Image.h"
#include "EditorSupport/MemoryTextureOutputHandler.h"

#include FT_MODULE_H

#include <nvtt/nvtt.h>

HELIUM_IMPLEMENT_OBJECT( Helium::FontResourceHandler, EditorSupport, 0 );

using namespace Helium;

/// Maximum Unicode code point value.
static const uint_fast32_t UNICODE_CODE_POINT_MAX = 0x10ffff;

/// Allocate a block of memory for FreeType.
///
/// @param[in] pMemory  Handle to the source memory manager.
/// @param[in] size     Number of bytes to allocate.
///
/// @return  Address of the newly allocated block of memory, or null if allocation failed.
///
/// @see FreeTypeFree(), FreeTypeReallocate()
static void* FreeTypeAllocate( FT_Memory /*pMemory*/, long size )
{
    DefaultAllocator allocator;

    // FreeType uses setjmp()/longjmp(), and because our SSE settings bleed into dependency projects, jmp_buf needs to
    // be 16-byte aligned for backing up SSE register states.  To avoid misaligned jmp_buf instances, we align all
    // allocations of 16 bytes or greater to 16-byte boundaries.
    void* pBlock;
    if( size < HELIUM_SIMD_SIZE )
    {
        pBlock = allocator.Allocate( size );
    }
    else
    {
        pBlock = allocator.AllocateAligned( HELIUM_SIMD_ALIGNMENT, size );
    }

    return pBlock;
}

/// Free a block of memory previously allocated for FreeType.
///
/// @param[in] pMemory  Handle to the source memory manager.
/// @param[in] pBlock   Block of memory to free.
///
/// @see FreeTypeAllocate(), FreeTypeReallocate()
static void FreeTypeFree( FT_Memory /*pMemory*/, void* pBlock )
{
    DefaultAllocator allocator;

    // Our allocators allow for null pointers to be passed to their free functions, so we do not need to validate the
    // block parameter.
    allocator.Free( pBlock );
}

/// Reallocate a block of memory for FreeType.
///
/// @param[in] pMemory      Handle to the source memory manager.
/// @param[in] currentSize  Current block size, in bytes.
/// @param[in] newSize      New block size, in bytes.
/// @param[in] pBlock       Block to reallocate.
///
/// @return  Pointer to the reallocated block of memory, or null if reallocation failed.
///
/// @see FreeTypeAllocate(), FreeTypeFree()
static void* FreeTypeReallocate( FT_Memory /*pMemory*/, long currentSize, long newSize, void* pBlock )
{
    DefaultAllocator allocator;

    // We cannot call Reallocate() if either the old or new size requires SIMD alignment (see FreeTypeAllocate() for
    // more information about why we need to align some allocations), so call Free()/Allocate()/AllocateAligned()
    // instead as appropriate.
    if( newSize < HELIUM_SIMD_SIZE )
    {
        if( currentSize < HELIUM_SIMD_SIZE )
        {
            // Our allocators allow for null pointers to be passed to their reallocate functions, so we do not need to
            // validate the block parameter.
            pBlock = allocator.Reallocate( pBlock, newSize );
        }
        else
        {
            void* pOldBlock = pBlock;

            // Our allocators treat a realloc() with a size of zero as a free, so simulate that functionality here.
            pBlock = NULL;
            if( newSize )
            {
                pBlock = allocator.Allocate( newSize );
                if( pBlock )
                {
                    HELIUM_ASSERT( newSize < currentSize );
                    MemoryCopy( pBlock, pOldBlock, newSize );
                }
            }

            allocator.Free( pOldBlock );
        }
    }
    else
    {
        void* pOldBlock = pBlock;

        // Note that newSize >= HELIUM_SIMD_SIZE, so we don't need to check for non-zero sizes here.
        pBlock = allocator.AllocateAligned( HELIUM_SIMD_ALIGNMENT, newSize );
        if( pBlock )
        {
            MemoryCopy( pBlock, pOldBlock, Min( currentSize, newSize ) );
        }

        allocator.Free( pOldBlock );
    }

    return pBlock;
}

/// FreeType memory management routines.
static FT_MemoryRec_ s_freeTypeMemory = { NULL, FreeTypeAllocate, FreeTypeFree, FreeTypeReallocate };

FT_Library FontResourceHandler::sm_pLibrary = NULL;

/// Constructor.
FontResourceHandler::FontResourceHandler()
{
}

/// Destructor.
FontResourceHandler::~FontResourceHandler()
{
}

/// @copydoc ResourceHandler::GetResourceType()
const GameObjectType* FontResourceHandler::GetResourceType() const
{
    return Font::GetStaticType();
}

/// @copydoc ResourceHandler::GetSourceExtensions()
void FontResourceHandler::GetSourceExtensions( const tchar_t* const*& rppExtensions, size_t& rExtensionCount ) const
{
    static const tchar_t* extensions[] = { TXT( ".ttf" ) };

    rppExtensions = extensions;
    rExtensionCount = HELIUM_ARRAY_COUNT( extensions );
}

/// @copydoc ResourceHandler::CacheResource()
bool FontResourceHandler::CacheResource(
    ObjectPreprocessor* pObjectPreprocessor,
    Resource* pResource,
    const String& rSourceFilePath )
{
    HELIUM_ASSERT( pObjectPreprocessor );
    HELIUM_ASSERT( pResource );

    Font* pFont = Reflect::AssertCast< Font >( pResource );

    // Load the font into memory ourselves in order to make sure we properly support Unicode file names.
    FileStream* pFileStream = FileStream::OpenFileStream( rSourceFilePath, FileStream::MODE_READ );
    if( !pFileStream )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            TXT( "FontResourceHandler: Source file for font resource \"%s\" failed to open properly.\n" ),
            *rSourceFilePath );

        return false;
    }

    uint64_t fileSize64 = static_cast< uint64_t >( pFileStream->GetSize() );
    if( fileSize64 > SIZE_MAX )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            ( TXT( "FontResourceHandler: Font file \"%s\" exceeds the maximum addressable size of data in memory for " )
              TXT( "this platform and will not be cached.\n" ) ),
            *rSourceFilePath );

        delete pFileStream;

        return false;
    }

    size_t fileSize = static_cast< size_t >( fileSize64 );

    uint8_t* pFileData = new uint8_t [ fileSize ];
    if( !pFileData )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            ( TXT( "FontResourceHandler: Failed to allocate %" ) TPRIuSZ TXT( " bytes for resource data for font " )
              TXT( "\"%s\".\n" ) ),
            fileSize,
            *rSourceFilePath );

        delete pFileStream;

        return false;
    }

    size_t bytesRead = pFileStream->Read( pFileData, 1, fileSize );
    delete pFileStream;

    if( bytesRead != fileSize )
    {
        HELIUM_TRACE(
            TraceLevels::Warning,
            ( TXT( "FontResourceHandler: Attempted to read %" ) TPRIuSZ TXT( " bytes from font resource file \"%s\", " )
              TXT( "but only %" ) TPRIuSZ TXT( " bytes were read successfully.\n" ) ),
            fileSize,
            *rSourceFilePath,
            bytesRead );
    }

    // Create the font face.
    FT_Library pLibrary = GetStaticLibrary();
    HELIUM_ASSERT( pLibrary );

    FT_Face pFace = NULL;
    FT_Error error = FT_New_Memory_Face( pLibrary, pFileData, static_cast< FT_Long >( bytesRead ), 0, &pFace );
    if( error != 0 )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            TXT( "FontResourceHandler: Failed to create font face from resource file \"%s\".\n" ),
            *rSourceFilePath );

        delete [] pFileData;

        return false;
    }

    // Set the appropriate font size.
    int32_t pointSize = Font::Float32ToFixed26x6( pFont->GetPointSize() );
    uint32_t dpi = pFont->GetDpi();

    error = FT_Set_Char_Size( pFace, pointSize, pointSize, dpi, dpi );
    if( error != 0 )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            TXT( "FontResourceHandler: Failed to set size of font resource \"%s\".\n" ),
            *rSourceFilePath );

        FT_Done_Face( pFace );
        delete [] pFileData;

        return false;
    }

    // Get the general font size information.
    FT_Size pSize = pFace->size;
    HELIUM_ASSERT( pSize );

    Font::PersistentResourceData resource_data;
    int32_t ascender = pSize->metrics.ascender;
    int32_t descender = pSize->metrics.descender;
    int32_t height = pSize->metrics.height;
    int32_t maxAdvance = pSize->metrics.max_advance;

    // Make sure that all characters in the font will fit on a single texture sheet (note that we also need at least a
    // pixel on each side in order to pad each glyph).
    uint16_t textureSheetWidth = Max< uint16_t >( pFont->GetTextureSheetWidth(), 1 );
    uint16_t textureSheetHeight = Max< uint16_t >( pFont->GetTextureSheetHeight(), 1 );

    int32_t integerHeight = ( height + ( 1 << 6 ) - 1 ) >> 6;
    if( integerHeight + 2 > textureSheetHeight )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            ( TXT( "FontResourceHandler: Font height (%" ) TPRId32 TXT( ") exceeds the texture sheet height (%" )
              TPRIu16 TXT( ") for font resource \"%s\".\n" ) ),
            integerHeight,
            textureSheetHeight,
            *pResource->GetPath().ToString() );

        FT_Done_Face( pFace );
        delete [] pFileData;

        return false;
    }

    int32_t integerMaxAdvance = ( maxAdvance + ( 1 << 6 ) - 1 ) >> 6;
    if( integerMaxAdvance + 2 > textureSheetWidth )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            ( TXT( "FontResourceHandler: Maximum character advance (%" ) TPRId32 TXT( ") exceeds the texture sheet " )
              TXT( "width (%" ) TPRIu16 TXT( ") for font resource \"%s\".\n" ) ),
            integerMaxAdvance,
            textureSheetWidth,
            *pResource->GetPath().ToString() );

        FT_Done_Face( pFace );
        delete [] pFileData;

        return false;
    }

    // Allocate a buffer for building our texture sheets.
    uint_fast32_t texturePixelCount =
        static_cast< uint_fast32_t >( textureSheetWidth ) * static_cast< uint_fast32_t >( textureSheetHeight );
    uint8_t* pTextureBuffer = new uint8_t [ texturePixelCount ];
    HELIUM_ASSERT( pTextureBuffer );
    if( !pTextureBuffer )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            ( TXT( "FontResourceHandler: Failed to allocate %" ) TPRIuFAST32 TXT( " bytes for texture resource " )
              TXT( "buffer data while caching font resource \"%s\".\n" ) ),
            texturePixelCount,
            *pResource->GetPath().ToString() );

        FT_Done_Face( pFace );
        delete [] pFileData;

        return false;
    }

    MemoryZero( pTextureBuffer, texturePixelCount );

    // Build the texture sheets for our glyphs.
    Font::ECompression textureCompression = pFont->GetTextureCompression();
    bool bAntialiased = pFont->GetAntialiased();

    DynamicArray< DynamicArray< uint8_t > > textureSheets;

    uint16_t penX = 1;
    uint16_t penY = 1;
    uint16_t lineHeight = 0;

    FT_Int32 glyphLoadFlags = FT_LOAD_RENDER;
    if( !bAntialiased )
    {
        glyphLoadFlags |= FT_LOAD_TARGET_MONO;
    }

    for( uint_fast32_t codePoint = 0; codePoint <= UNICODE_CODE_POINT_MAX; ++codePoint )
    {
        // Check whether the current code point is contained within the font.
        FT_UInt characterIndex = FT_Get_Char_Index( pFace, static_cast< FT_ULong >( codePoint ) );
        if( characterIndex == 0 )
        {
            continue;
        }

        // Load and render the glyph for the current character.
        HELIUM_VERIFY( FT_Load_Glyph( pFace, characterIndex, glyphLoadFlags ) == 0 );

        FT_GlyphSlot pGlyph = pFace->glyph;
        HELIUM_ASSERT( pGlyph );

        // Proceed to the next line in the texture sheet or the next sheet itself if we don't have enough room in the
        // current line/sheet.
        HELIUM_ASSERT( pGlyph->bitmap.rows >= 0 );
        HELIUM_ASSERT( pGlyph->bitmap.width >= 0 );
        uint_fast32_t glyphRowCount = static_cast< uint32_t >( pGlyph->bitmap.rows );
        uint_fast32_t glyphWidth = static_cast< uint32_t >( pGlyph->bitmap.width );

        if( penX + glyphWidth + 1 >= textureSheetWidth )
        {
            penX = 1;

            if( penY + glyphRowCount + 1 >= textureSheetHeight )
            {
                CompressTexture(
                    pTextureBuffer,
                    textureSheetWidth,
                    textureSheetHeight,
                    textureCompression,
                    textureSheets );
                MemoryZero( pTextureBuffer, texturePixelCount );

                penY = 1;
            }
            else
            {
                penY += lineHeight + 1;
            }

            lineHeight = 0;
        }

        // Copy the character data from the glyph bitmap to the texture sheet.
        int_fast32_t glyphPitch = pGlyph->bitmap.pitch;

        const uint8_t* pGlyphBuffer = pGlyph->bitmap.buffer;
        HELIUM_ASSERT( pGlyphBuffer || glyphRowCount == 0 );

        uint8_t* pTexturePixel =
            pTextureBuffer + static_cast< size_t >( penY ) * static_cast< size_t >( textureSheetWidth ) + penX;

        if( bAntialiased )
        {
            // Anti-aliased fonts are rendered as 8-bit grayscale images, so just copy the data as-is.
            for( uint_fast32_t rowIndex = 0; rowIndex < glyphRowCount; ++rowIndex )
            {
                MemoryCopy( pTexturePixel, pGlyphBuffer, glyphWidth );
                pGlyphBuffer += glyphPitch;
                pTexturePixel += textureSheetWidth;
            }
        }
        else
        {
            // Fonts without anti-aliasing are rendered as 1-bit monochrome images, so we need to manually convert each
            // row to 8-bit grayscale.
            for( uint_fast32_t rowIndex = 0; rowIndex < glyphRowCount; ++rowIndex )
            {
                const uint8_t* pGlyphPixelBlock = pGlyphBuffer;
                pGlyphBuffer += glyphPitch;

                uint8_t* pCurrentTexturePixel = pTexturePixel;
                pTexturePixel += textureSheetWidth;

                uint_fast32_t remainingPixelCount = glyphWidth;
                while( remainingPixelCount >= 8 )
                {
                    remainingPixelCount -= 8;

                    uint8_t pixelBlock = *pGlyphPixelBlock;
                    ++pGlyphPixelBlock;

                    *( pCurrentTexturePixel++ ) = ( ( pixelBlock & ( 1 << 7 ) ) ? 255 : 0 );
                    *( pCurrentTexturePixel++ ) = ( ( pixelBlock & ( 1 << 6 ) ) ? 255 : 0 );
                    *( pCurrentTexturePixel++ ) = ( ( pixelBlock & ( 1 << 5 ) ) ? 255 : 0 );
                    *( pCurrentTexturePixel++ ) = ( ( pixelBlock & ( 1 << 4 ) ) ? 255 : 0 );
                    *( pCurrentTexturePixel++ ) = ( ( pixelBlock & ( 1 << 3 ) ) ? 255 : 0 );
                    *( pCurrentTexturePixel++ ) = ( ( pixelBlock & ( 1 << 2 ) ) ? 255 : 0 );
                    *( pCurrentTexturePixel++ ) = ( ( pixelBlock & ( 1 << 1 ) ) ? 255 : 0 );
                    *( pCurrentTexturePixel++ ) = ( ( pixelBlock & ( 1 << 0 ) ) ? 255 : 0 );
                }

                uint8_t pixelBlock = *pGlyphPixelBlock;
                uint8_t mask = ( 1 << 7 );
                while( remainingPixelCount != 0 )
                {
                    *( pCurrentTexturePixel++ ) = ( ( pixelBlock & mask ) ? 255 : 0 );
                    mask >>= 1;
                    --remainingPixelCount;
                }
            }
        }

        // Store the character information in our character array.
        Font::Character* pCharacter = resource_data.m_characters.New();
        HELIUM_ASSERT( pCharacter );
    
        pCharacter->codePoint = static_cast< uint32_t >( codePoint );
         
        pCharacter->imageX = penX;
        pCharacter->imageY = penY;
        pCharacter->imageWidth = static_cast< uint16_t >( glyphWidth );
        pCharacter->imageHeight = static_cast< uint16_t >( glyphRowCount );
    
        pCharacter->width = pGlyph->metrics.width;
        pCharacter->height = pGlyph->metrics.height;
        pCharacter->bearingX = pGlyph->metrics.horiBearingX;
        pCharacter->bearingY = pGlyph->metrics.horiBearingY;
        pCharacter->advance = pGlyph->metrics.horiAdvance;
    
        HELIUM_ASSERT( textureSheets.GetSize() < UINT8_MAX );
        pCharacter->texture = static_cast< uint8_t >( static_cast< uint8_t >( textureSheets.GetSize() ) );
    
        // Update the pen location as well as the maximum line height as appropriate based on the current line height.
        penX += static_cast< uint16_t >( glyphWidth ) + 1;
    
        HELIUM_ASSERT( glyphRowCount <= UINT16_MAX );
        lineHeight = Max< uint16_t >( lineHeight, static_cast< uint16_t >( glyphRowCount ) );
    }

    // Compress and store the last texture in the sheet.
    if( !resource_data.m_characters.IsEmpty() )
    {
        CompressTexture( pTextureBuffer, textureSheetWidth, textureSheetHeight, textureCompression, textureSheets );
    }

    // Done processing the font itself, so free some resources.
    delete [] pTextureBuffer;

    FT_Done_Face( pFace );
    delete [] pFileData;

    // Cache the font data.
    size_t characterCountActual = resource_data.m_characters.GetSize();
    HELIUM_ASSERT( characterCountActual <= UINT32_MAX );
    uint32_t characterCount = static_cast< uint32_t >( characterCountActual );

    size_t textureCountActual = textureSheets.GetSize();
    HELIUM_ASSERT( textureCountActual < UINT8_MAX );
    uint8_t textureCount = static_cast< uint8_t >( textureCountActual );

    resource_data.m_ascender = ascender;
    resource_data.m_descender = descender;
    resource_data.m_height = height;
    resource_data.m_maxAdvance = maxAdvance;
    resource_data.m_textureCount = textureCount;
    // m_characters is populated above

    for( size_t platformIndex = 0; platformIndex < static_cast< size_t >( Cache::PLATFORM_MAX ); ++platformIndex )
    {
        PlatformPreprocessor* pPreprocessor = pObjectPreprocessor->GetPlatformPreprocessor(
            static_cast< Cache::EPlatform >( platformIndex ) );

        if( !pPreprocessor )
        {
            continue;
        }

        Resource::PreprocessedData& rPreprocessedData = pResource->GetPreprocessedData(
            static_cast< Cache::EPlatform >( platformIndex ) );
        //rPreprocessedData.persistentDataBuffer = ;
        SaveObjectToPersistentDataBuffer(&resource_data, rPreprocessedData.persistentDataBuffer);
        rPreprocessedData.subDataBuffers = textureSheets;
        rPreprocessedData.bLoaded = true;

    }

    return true;
}

/// Initialize the static FreeType library instance.
///
/// @return  Handle for the initialized instance.
///
/// @see DestroyStaticInstance(), GetStaticInstance()
FT_Library FontResourceHandler::InitializeStaticLibrary()
{
    HELIUM_ASSERT_MSG( !sm_pLibrary, TXT( "FreeType 2 library has already been initialized." ) );

    HELIUM_VERIFY( FT_New_Library( &s_freeTypeMemory, &sm_pLibrary ) == 0 );
    HELIUM_ASSERT( sm_pLibrary );

    FT_Add_Default_Modules( sm_pLibrary );

    return sm_pLibrary;
}

/// Shut down FreeType and destroy the static FreeType library instance.
///
/// @see InitializeStaticInstance(), GetStaticInstance()
void FontResourceHandler::DestroyStaticLibrary()
{
    if( sm_pLibrary )
    {
        FT_Done_Library( sm_pLibrary );
        sm_pLibrary = NULL;
    }
}

/// Get the static FreeType library instance.
///
/// @return  Handle for the static FreeType library instance.
///
/// @see InitializeStaticLibrary(), DestroyStaticLibrary()
FT_Library FontResourceHandler::GetStaticLibrary()
{
    HELIUM_ASSERT_MSG(
        sm_pLibrary,
        ( TXT( "FreeType library instance has not been initialized.  Initialize by calling " )
          TXT( "FontResourceHandler::InitializeStaticLibrary() during startup (also make sure to call " )
          TXT( "Font::DestroyStaticLibrary() during shutdown as well)." ) ) );

    return sm_pLibrary;
}

/// Compress a font texture sheet and add the texture data to the given texture sheet array.
///
/// @param[in] pGrayscaleData  Texture sheet data, stored as a contiguous array of 8-bit grayscale values.
/// @param[in] textureWidth    Width of the texture sheet.
/// @param[in] textureHeight   Height of the texture sheet.
/// @param[in] compression     Font texture sheet compression method to use.
/// @param[in] rTextureSheets  Array of texture sheets to which the texture data should be appended.
void FontResourceHandler::CompressTexture(
    const uint8_t* pGrayscaleData,
    uint16_t textureWidth,
    uint16_t textureHeight,
    Font::ECompression compression,
    DynamicArray< DynamicArray< uint8_t > >& rTextureSheets )
{
    HELIUM_ASSERT( pGrayscaleData );

    DynamicArray< uint8_t >* pOutputSheet = rTextureSheets.New();
    HELIUM_ASSERT( pOutputSheet );

    // If the output is to be uncompressed grayscale data, simply copy the data to the output texture, as it's already
    // uncompressed grayscale data.
    if( compression == Font::ECompression::GRAYSCALE_UNCOMPRESSED )
    {
        size_t pixelCount = static_cast< size_t >( textureWidth ) * static_cast< size_t >( textureHeight );
        pOutputSheet->AddArray( pGrayscaleData, pixelCount );

        return;
    }

    // Convert the source image to a 32-bit BGRA image for the NVIDIA texture tools library to process.
    Image::InitParameters imageParameters;
    imageParameters.format.SetBytesPerPixel( 4 );
    imageParameters.format.SetChannelBitCount( Image::CHANNEL_RED, 8 );
    imageParameters.format.SetChannelBitCount( Image::CHANNEL_GREEN, 8 );
    imageParameters.format.SetChannelBitCount( Image::CHANNEL_BLUE, 8 );
    imageParameters.format.SetChannelBitCount( Image::CHANNEL_ALPHA, 8 );
#if HELIUM_ENDIAN_LITTLE
    imageParameters.format.SetChannelBitOffset( Image::CHANNEL_RED, 16 );
    imageParameters.format.SetChannelBitOffset( Image::CHANNEL_GREEN, 8 );
    imageParameters.format.SetChannelBitOffset( Image::CHANNEL_BLUE, 0 );
    imageParameters.format.SetChannelBitOffset( Image::CHANNEL_ALPHA, 24 );
#else
    imageParameters.format.SetChannelBitOffset( Image::CHANNEL_RED, 8 );
    imageParameters.format.SetChannelBitOffset( Image::CHANNEL_GREEN, 16 );
    imageParameters.format.SetChannelBitOffset( Image::CHANNEL_BLUE, 24 );
    imageParameters.format.SetChannelBitOffset( Image::CHANNEL_ALPHA, 0 );
#endif
    imageParameters.width = textureWidth;
    imageParameters.height = textureHeight;

    Image bgraImage;
    HELIUM_VERIFY( bgraImage.Initialize( imageParameters ) );

    uint_fast32_t imageWidth = textureWidth;
    uint_fast32_t imageHeight = textureHeight;
    uint_fast32_t imagePitch = bgraImage.GetPitch();

    uint8_t* pImagePixelData = static_cast< uint8_t* >( bgraImage.GetPixelData() );
    HELIUM_ASSERT( pImagePixelData );
    uint8_t* pImageRow = pImagePixelData;

    for( uint_fast32_t imageY = 0; imageY < imageHeight; ++imageY )
    {
        uint8_t* pOutputPixel = pImageRow;

        for( uint_fast32_t imageX = 0; imageX < imageWidth; ++imageX )
        {
            uint8_t pixel = *( pGrayscaleData++ );

            *( pOutputPixel++ ) = pixel;
            *( pOutputPixel++ ) = pixel;
            *( pOutputPixel++ ) = pixel;
            *( pOutputPixel++ ) = 0xff;
        }

        pImageRow += imagePitch;
    }

    // Set up the input options for the texture compressor.
    nvtt::InputOptions inputOptions;
    inputOptions.setTextureLayout( nvtt::TextureType_2D, textureWidth, textureHeight );
    inputOptions.setMipmapData( pImagePixelData, textureWidth, textureHeight );
    inputOptions.setMipmapGeneration( false );
    inputOptions.setWrapMode( nvtt::WrapMode_Repeat );
    inputOptions.setGamma( 1.0f, 1.0f );
    inputOptions.setNormalMap( false );

    // Set up the output options for the texture compressor.
    MemoryTextureOutputHandler outputHandler( textureWidth, textureHeight, false, false );

    nvtt::OutputOptions outputOptions;
    outputOptions.setOutputHandler( &outputHandler );
    outputOptions.setOutputHeader( false );

    // Set up the compression options for the texture compressor (note that the only compression option we currently
    // support other than uncompressed grayscale is BC1/DXT1).
    nvtt::CompressionOptions compressionOptions;
    compressionOptions.setFormat( nvtt::Format_BC1 );
    compressionOptions.setQuality( nvtt::Quality_Normal );

    // Compress the texture.
    nvtt::Compressor compressor;
    HELIUM_VERIFY( compressor.process( inputOptions, compressionOptions, outputOptions ) );

    // Store the compressed data in the output texture sheet.
    const MemoryTextureOutputHandler::MipLevelArray& rMipLevels = outputHandler.GetFace( 0 );
    HELIUM_ASSERT( rMipLevels.GetSize() == 1 );
    *pOutputSheet = rMipLevels[ 0 ];
}

#endif  // HELIUM_TOOLS
