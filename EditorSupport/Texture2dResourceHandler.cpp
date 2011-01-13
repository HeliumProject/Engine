//----------------------------------------------------------------------------------------------------------------------
// Texture2dResourceHandler.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EditorSupportPch.h"
#include "EditorSupport/Texture2dResourceHandler.h"

#include "Foundation/Stream/BufferedStream.h"
#include "Foundation/File/File.h"
#include "Foundation/File/Path.h"
#include "Foundation/Stream/FileStream.h"
#include "Engine/BinarySerializer.h"
#include "Graphics/Texture2d.h"
#include "PcSupport/ObjectPreprocessor.h"
#include "PcSupport/PlatformPreprocessor.h"
#include "EditorSupport/Image.h"
#include "EditorSupport/MemoryTextureOutputHandler.h"
#include "EditorSupport/PngImageLoader.h"
#include "EditorSupport/TgaImageLoader.h"
#include "Rendering/RendererTypes.h"

#include <nvtt/nvtt.h>

using namespace Lunar;

L_IMPLEMENT_OBJECT( Texture2dResourceHandler, EditorSupport, 0 );

/// Constructor.
Texture2dResourceHandler::Texture2dResourceHandler()
{
}

/// Destructor.
Texture2dResourceHandler::~Texture2dResourceHandler()
{
}

/// @copydoc ResourceHandler::GetResourceType()
const GameObjectType* Texture2dResourceHandler::GetResourceType() const
{
    return Texture2d::GetStaticType();
}

/// @copydoc ResourceHandler::GetSourceExtensions()
void Texture2dResourceHandler::GetSourceExtensions(
    const tchar_t* const*& rppExtensions,
    size_t& rExtensionCount ) const
{
    static const tchar_t* extensions[] = { TXT( ".png" ), TXT( ".tga" ) };

    rppExtensions = extensions;
    rExtensionCount = HELIUM_ARRAY_COUNT( extensions );
}

/// @copydoc ResourceHandler::CacheResource()
bool Texture2dResourceHandler::CacheResource(
    ObjectPreprocessor* pObjectPreprocessor,
    Resource* pResource,
    const String& rSourceFilePath )
{
    HELIUM_ASSERT( pObjectPreprocessor );
    HELIUM_ASSERT( pResource );

    Texture2d* pTexture = Reflect::AssertCast< Texture2d >( pResource );

    // Load the source texture data.
    FileStream* pSourceFileStream = File::Open( rSourceFilePath, FileStream::MODE_READ );
    if( !pSourceFileStream )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "Texture2dResourceHandler::CacheResource(): Failed to open source texture file \"%s\" for " )
            TXT( "reading.\n" ) ),
            *rSourceFilePath );

        return false;
    }

    Image sourceImage;
    bool bLoadSuccess;

    {
        BufferedStream sourceStream( pSourceFileStream );

        // Determine the proper image loader to used based on the image extension.
        Path sourceFilePath = *rSourceFilePath;
        String extension( sourceFilePath.Extension().c_str() );
        if( extension == TXT( "png" ) )
        {
            bLoadSuccess = PngImageLoader::Load( sourceImage, &sourceStream );
        }
        else
        {
            bLoadSuccess = TgaImageLoader::Load( sourceImage, &sourceStream );
        }
    }

    delete pSourceFileStream;

    if( !bLoadSuccess )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "Texture2dResourceHandler::CacheResource(): Failed to load source texture image \"%s\".\n" ),
            *rSourceFilePath );
    }

    // Convert the source image to a 32-bit BGRA image for the NVIDIA texture tools library to process.
    Image::Format bgraFormat;
    bgraFormat.SetBytesPerPixel( 4 );
    bgraFormat.SetChannelBitCount( Image::CHANNEL_RED, 8 );
    bgraFormat.SetChannelBitCount( Image::CHANNEL_GREEN, 8 );
    bgraFormat.SetChannelBitCount( Image::CHANNEL_BLUE, 8 );
    bgraFormat.SetChannelBitCount( Image::CHANNEL_ALPHA, 8 );
#if HELIUM_ENDIAN_LITTLE
    bgraFormat.SetChannelBitOffset( Image::CHANNEL_RED, 16 );
    bgraFormat.SetChannelBitOffset( Image::CHANNEL_GREEN, 8 );
    bgraFormat.SetChannelBitOffset( Image::CHANNEL_BLUE, 0 );
    bgraFormat.SetChannelBitOffset( Image::CHANNEL_ALPHA, 24 );
#else
    bgraFormat.SetChannelBitOffset( Image::CHANNEL_RED, 8 );
    bgraFormat.SetChannelBitOffset( Image::CHANNEL_GREEN, 16 );
    bgraFormat.SetChannelBitOffset( Image::CHANNEL_BLUE, 24 );
    bgraFormat.SetChannelBitOffset( Image::CHANNEL_ALPHA, 0 );
#endif

    Image bgraImage;

    HELIUM_VERIFY( sourceImage.Convert( bgraImage, bgraFormat ) );
    sourceImage.Unload();

    // If the texture is flagged to ignore alpha data, set the alpha channel to fully opaque for each pixel in the
    // image.  Otherwise, check if the image is fully opaque (in which case alpha data can be ignored during
    // compression, and we can potentially use cheaper compressed formats).
    uint32_t imageWidth = bgraImage.GetWidth();
    uint32_t imageHeight = bgraImage.GetHeight();
    uint32_t pixelCount = imageWidth * imageHeight;

    void* pImagePixelData = bgraImage.GetPixelData();
    HELIUM_ASSERT( pImagePixelData );

    uint8_t* pPixelAlpha = static_cast< uint8_t* >( pImagePixelData ) + 3;

    bool bIgnoreAlpha = pTexture->GetIgnoreAlpha();
    if( bIgnoreAlpha )
    {
        for( uint32_t pixelIndex = 0; pixelIndex < pixelCount; ++pixelIndex, pPixelAlpha += 4 )
        {
            *pPixelAlpha = 0xff;
        }
    }
    else
    {
        uint32_t pixelIndex;
        for( pixelIndex = 0; pixelIndex < pixelCount; ++pixelIndex, pPixelAlpha += 4 )
        {
            if( *pPixelAlpha != 0xff )
            {
                break;
            }
        }

        if( pixelIndex >= pixelCount )
        {
            bIgnoreAlpha = true;
        }
    }

    // Set up the input options for the texture compressor.
    Texture::ECompression compression = pTexture->GetCompression();
    HELIUM_ASSERT( static_cast< size_t >( compression ) < static_cast< size_t >( Texture::COMPRESSION_MAX ) );

    bool bIsNormalMap = Texture::IsNormalMapCompression( compression );

    bool bSrgb = pTexture->GetSrgb();
    bool bCreateMipmaps = pTexture->GetCreateMipmaps();

    nvtt::InputOptions inputOptions;
    inputOptions.setTextureLayout( nvtt::TextureType_2D, imageWidth, imageHeight );
    inputOptions.setMipmapData( pImagePixelData, imageWidth, imageHeight );
    inputOptions.setMipmapGeneration( bCreateMipmaps );
    inputOptions.setMipmapFilter( nvtt::MipmapFilter_Box );
    inputOptions.setWrapMode( nvtt::WrapMode_Repeat );

    float gamma = ( bSrgb ? 2.2f : 1.0f );
    inputOptions.setGamma( gamma, gamma );

    inputOptions.setNormalMap( bIsNormalMap );
    inputOptions.setNormalizeMipmaps( bIsNormalMap );

    // Set up the output options for the texture compressor.
    MemoryTextureOutputHandler outputHandler( imageWidth, imageHeight, false, bCreateMipmaps );

    nvtt::OutputOptions outputOptions;
    outputOptions.setOutputHandler( &outputHandler );
    outputOptions.setOutputHeader( false );

    // Set up the compression options for the texture compressor.
    nvtt::CompressionOptions compressionOptions;

    nvtt::Format outputFormat = nvtt::Format_BC1;
    ERendererPixelFormat pixelFormat = RENDERER_PIXEL_FORMAT_BC1;

    switch( compression )
    {
    case Texture::COMPRESSION_NONE:
        {
            outputFormat = nvtt::Format_RGBA;
#if HELIUM_ENDIAN_LITTLE
            compressionOptions.setPixelFormat( 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff );
#else
            compressionOptions.setPixelFormat( 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 );
#endif
            pixelFormat = ( bSrgb ? RENDERER_PIXEL_FORMAT_R8G8B8A8_SRGB : RENDERER_PIXEL_FORMAT_R8G8B8A8 );

            break;
        }

    case Texture::COMPRESSION_COLOR:
        {
            outputFormat = ( bIgnoreAlpha ? nvtt::Format_BC1 : nvtt::Format_BC1a );
            pixelFormat = ( bSrgb ? RENDERER_PIXEL_FORMAT_BC1_SRGB : RENDERER_PIXEL_FORMAT_BC1 );

            break;
        }

    case Texture::COMPRESSION_COLOR_SHARP_ALPHA:
        {
            if( bIgnoreAlpha )
            {
                outputFormat = nvtt::Format_BC1;
                pixelFormat = ( bSrgb ? RENDERER_PIXEL_FORMAT_BC1_SRGB : RENDERER_PIXEL_FORMAT_BC1 );
            }
            else
            {
                outputFormat = nvtt::Format_BC2;
                pixelFormat = ( bSrgb ? RENDERER_PIXEL_FORMAT_BC2_SRGB : RENDERER_PIXEL_FORMAT_BC2 );
            }

            break;
        }

    case Texture::COMPRESSION_COLOR_SMOOTH_ALPHA:
        {
            if( bIgnoreAlpha )
            {
                outputFormat = nvtt::Format_BC1;
                pixelFormat = ( bSrgb ? RENDERER_PIXEL_FORMAT_BC1_SRGB : RENDERER_PIXEL_FORMAT_BC1 );
            }
            else
            {
                outputFormat = nvtt::Format_BC3;
                pixelFormat = ( bSrgb ? RENDERER_PIXEL_FORMAT_BC3_SRGB : RENDERER_PIXEL_FORMAT_BC3 );
            }

            break;
        }

    case Texture::COMPRESSION_NORMAL_MAP:
        {
            outputFormat = nvtt::Format_BC3n;
            pixelFormat = RENDERER_PIXEL_FORMAT_BC3;

            break;
        }

    case Texture::COMPRESSION_NORMAL_MAP_COMPACT:
        {
            outputFormat = nvtt::Format_BC1;
            pixelFormat = RENDERER_PIXEL_FORMAT_BC1;

            break;
        }
    }

    compressionOptions.setFormat( outputFormat );
    compressionOptions.setQuality( nvtt::Quality_Normal );

    // Compress the texture.
    nvtt::Compressor compressor;
    bool bCompressSuccess = compressor.process( inputOptions, compressionOptions, outputOptions );
    HELIUM_ASSERT( bCompressSuccess );
    if( !bCompressSuccess )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "Texture2dResourceHandler::CacheResource(): Texture compression failed for texture image " )
            TXT( "\"%s\".\n" ) ),
            *rSourceFilePath );

        return false;
    }

    // Cache the data for each supported platform.
    const MemoryTextureOutputHandler::MipLevelArray& rMipLevels = outputHandler.GetFace( 0 );
    uint32_t mipLevelCount = static_cast< uint32_t >( rMipLevels.GetSize() );
    HELIUM_ASSERT( mipLevelCount != 0 );

    int32_t pixelFormatIndex = static_cast< int32_t >( pixelFormat );

    BinarySerializer serializer;
    for( size_t platformIndex = 0; platformIndex < static_cast< size_t >( Cache::PLATFORM_MAX ); ++platformIndex )
    {
        PlatformPreprocessor* pPreprocessor = pObjectPreprocessor->GetPlatformPreprocessor(
            static_cast< Cache::EPlatform >( platformIndex ) );
        if( !pPreprocessor )
        {
            continue;
        }

        Resource::PreprocessedData& rPreprocessedData = pTexture->GetPreprocessedData(
            static_cast< Cache::EPlatform >( platformIndex ) );

        // Serialize the persistent data about the texture first.
        serializer.SetByteSwapping( pPreprocessor->SwapBytes() );
        serializer.BeginSerialize();
        serializer << imageWidth;
        serializer << imageHeight;
        serializer << mipLevelCount;
        serializer << pixelFormatIndex;
        serializer.EndSerialize();

        rPreprocessedData.persistentDataBuffer = serializer.GetPropertyStreamBuffer();

        // Serialize each mip level.
        DynArray< DynArray< uint8_t > >& rSubDataBuffers = rPreprocessedData.subDataBuffers;
        rSubDataBuffers.Reserve( mipLevelCount );
        rSubDataBuffers.Resize( mipLevelCount );
        rSubDataBuffers.Trim();

        for( uint32_t mipLevelIndex = 0; mipLevelIndex < mipLevelCount; ++mipLevelIndex )
        {
            rSubDataBuffers[ mipLevelIndex ] = rMipLevels[ mipLevelIndex ];
        }

        // Platform data is now loaded.
        rPreprocessedData.bLoaded = true;
    }

    return true;
}
