//----------------------------------------------------------------------------------------------------------------------
// TgaImageLoader.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EditorSupportPch.h"

#if HELIUM_TOOLS

#include "EditorSupport/TgaImageLoader.h"

#include "Foundation/Stream.h"
#include "Rendering/Color.h"
#include "EditorSupport/Image.h"

using namespace Helium;

// Main loop for reading TGA image pixel data.
template< typename ImageReader >
bool LoadImageData(
                   void* pPixelData,
                   ImageReader& rReader,
                   uint32_t width,
                   uint32_t height,
                   uint8_t bytesPerPixel,
                   bool bUpperLeftOrigin )
{
    uint8_t* pRow = static_cast< uint8_t* >( pPixelData );
    ptrdiff_t rowStep = static_cast< ptrdiff_t >( width ) * static_cast< ptrdiff_t >( bytesPerPixel );
    if( !bUpperLeftOrigin )
    {
        pRow += rowStep * ( height - 1 );
        rowStep = -rowStep;
    }

    for( uint32_t y = 0; y < height; ++y )
    {
        uint8_t* pDestPixel = pRow;
        for( uint32_t x = 0; x < width; ++x )
        {
            if( !rReader( pDestPixel ) )
            {
                return false;
            }

            pDestPixel += bytesPerPixel;
        }

        pRow += rowStep;
    }

    return true;
}

// Uncompressed image data reader.
class UncompressedReader
{
public:
    explicit UncompressedReader( Stream* pStream, uint8_t bytesPerPixel )
        : m_pStream( pStream )
        , m_bytesPerPixel( bytesPerPixel )
    {
        HELIUM_ASSERT( pStream );
        HELIUM_ASSERT( bytesPerPixel != 0 );
    }

    bool operator()( void* pDestPixel )
    {
        size_t readCount = m_pStream->Read( pDestPixel, m_bytesPerPixel, 1 );

        return ( readCount == 1 );
    }

private:
    Stream* m_pStream;
    uint8_t m_bytesPerPixel;
};

// RLE image data reader.
class RleReader
{
public:
    explicit RleReader( Stream* pStream, uint8_t bytesPerPixel )
        : m_pStream( pStream )
        , m_bytesPerPixel( bytesPerPixel )
    {
        HELIUM_ASSERT( pStream );
        HELIUM_ASSERT( bytesPerPixel != 0 );
        HELIUM_ASSERT( bytesPerPixel < HELIUM_ARRAY_COUNT( m_repeatColor ) );

        MemoryZero( &m_packetHeader, sizeof( m_packetHeader ) );
        MemoryZero( m_repeatColor, sizeof( m_repeatColor ) );
    }

    bool operator()( void* pDestPixel )
    {
        uint8_t count = m_packetHeader.count;
        if( count == 0 )
        {
            if( m_pStream->Read( &m_packetHeader, sizeof( m_packetHeader ), 1 ) != 1 )
            {
                return false;
            }

            if( m_packetHeader.bRun )
            {
                if( m_pStream->Read( m_repeatColor, m_bytesPerPixel, 1 ) != 1 )
                {
                    return false;
                }
            }

            // Count values in RLE packet headers are one less than the actual count, so add one to it here.  Since
            // we always decrement the count at the end of each iteration, we don't need to worry about the count
            // wrapping to zero.
            count = m_packetHeader.count + 1;
        }

        if( m_packetHeader.bRun )
        {
            MemoryCopy( pDestPixel, m_repeatColor, m_bytesPerPixel );
        }
        else
        {
            if( m_pStream->Read( pDestPixel, m_bytesPerPixel, 1 ) != 1 )
            {
                return false;
            }
        }

        m_packetHeader.count = count - 1;

        return true;
    }

private:
    Stream* m_pStream;
    uint8_t m_bytesPerPixel;

    struct
    {
#if HELIUM_ENDIAN_LITTLE
        uint8_t count : 7;
        uint8_t bRun  : 1;
#else
        uint8_t bRun  : 1;
        uint8_t count : 7;
#endif
    } m_packetHeader;

    uint8_t m_repeatColor[ 4 ];
};

/// Load a TGA image from a file.
///
/// @param[out] rImage         Loaded image data if loading was successful.
/// @param[in]  pSourceStream  Stream through which to load the image.  Note that this function does not buffer
///                            reads, so it is recommended to use a BufferedStream when loading from a file.
///
/// @return  True if loading was successful, false if not.
bool TgaImageLoader::Load( Image& rImage, Stream* pSourceStream )
{
    HELIUM_ASSERT( pSourceStream );

    // TGA images are stored in little-endian mode, so automatically perform byte-swapping on big-endian platforms.
#if HELIUM_ENDIAN_LITTLE
    Stream* pStream = pSourceStream;
#else
    ByteSwappingStream byteSwapStream( pSourceStream );
    Stream* pStream = &byteSwapStream;
#endif

    // Read the image ID length.
    uint8_t imageIdLength;
    if( pStream->Read( &imageIdLength, sizeof( imageIdLength ), 1 ) != 1 )
    {
        HELIUM_TRACE( TraceLevels::Error, TXT( "TgaImageLoader::Load(): Failed to read image ID length field from stream.\n" ) );

        return false;
    }

    // Read and validate the color map type.
    uint8_t colorMapType;
    if( pStream->Read( &colorMapType, sizeof( colorMapType ), 1 ) != 1 )
    {
        HELIUM_TRACE( TraceLevels::Error, TXT( "TgaImageLoader::Load(): Failed to read color map type from stream.\n" ) );

        return false;
    }

    if( colorMapType > 1 )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            TXT( "TgaImageLoader::Load(): Unsupported color map type (%" ) TPRIu8 TXT( ") specified.\n" ),
            colorMapType );

        return false;
    }

    // Read and validate the image type.
    uint8_t imageType;
    if( pStream->Read( &imageType, sizeof( imageType ), 1 ) != 1 )
    {
        HELIUM_TRACE( TraceLevels::Error, TXT( "TgaImageLoader::Load(): Failed to read image type from stream.\n" ) );

        return false;
    }

    if( imageType == 0 || ( imageType > 3 && imageType < 9 ) || imageType > 11 )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            TXT( "TgaImageLoader::Load(): Unsupported image type (%" ) TPRIu8 TXT( ") specified.\n" ),
            imageType );

        return false;
    }

    bool bRleImage = ( ( imageType & 0x8 ) != 0 );
    uint8_t imageColorType = ( imageType & 0x3 );

    bool bColorMapImageType = ( imageColorType == 1 );

    if( bColorMapImageType && colorMapType == 0 )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            ( TXT( "TgaImageLoader::Load(): Color-mapped image type (%" ) TPRIu8 TXT( ") specified, but no color " )
            TXT( "map data is provided in the image.\n" ) ),
            imageType );

        return false;
    }

    // Read and validate the color map specifications.
    uint16_t colorMapOrigin;
    if( pStream->Read( &colorMapOrigin, sizeof( colorMapOrigin ), 1 ) != 1 )
    {
        HELIUM_TRACE( TraceLevels::Error, TXT( "TgaImageLoader::Load(): Failed to read color map origin index from stream.\n" ) );

        return false;
    }

    uint16_t colorMapSize;
    if( pStream->Read( &colorMapSize, sizeof( colorMapSize ), 1 ) != 1 )
    {
        HELIUM_TRACE( TraceLevels::Error, TXT( "TgaImageLoader::Load(): Failed to read color map size from stream.\n" ) );

        return false;
    }

    if( colorMapSize != 0 && colorMapOrigin >= colorMapSize )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            ( TXT( "TgaImageLoader::Load(): Color map origin (%" ) TPRIu16 TXT( ") exceeds the range specified " )
            TXT( "by the color map size (%" ) TPRIu16 TXT( ").\n" ) ),
            colorMapOrigin,
            colorMapSize );

        return false;
    }

    uint8_t colorMapEntrySize;
    if( pStream->Read( &colorMapEntrySize, sizeof( colorMapEntrySize ), 1 ) != 1 )
    {
        HELIUM_TRACE( TraceLevels::Error, TXT( "TgaImageLoader::Load(): Failed to read color map entry size from stream.\n" ) );

        return false;
    }

    if( colorMapSize != 0 && colorMapEntrySize != 16 && colorMapEntrySize != 24 && colorMapEntrySize != 32 )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            ( TXT( "TgaImageLoader::Load(): Unsupported color map entry size (%" ) TPRIu8 TXT( ") specified.  " )
            TXT( "Color map entries must be 16, 24, or 32 bits in size.\n" ) ),
            colorMapEntrySize );

        return false;
    }

    // Read and validate the image specifications.
    uint16_t imageOriginX;
    if( pStream->Read( &imageOriginX, sizeof( imageOriginX ), 1 ) != 1 )
    {
        HELIUM_TRACE( TraceLevels::Error, TXT( "TgaImageLoader::Load(): Failed to read image x-origin from stream.\n" ) );

        return false;
    }

    uint16_t imageOriginY;
    if( pStream->Read( &imageOriginY, sizeof( imageOriginY ), 1 ) != 1 )
    {
        HELIUM_TRACE( TraceLevels::Error, TXT( "TgaImageLoader::Load(): Failed to read image y-origin from stream.\n" ) );

        return false;
    }

    uint16_t imageWidth;
    if( pStream->Read( &imageWidth, sizeof( imageWidth ), 1 ) != 1 )
    {
        HELIUM_TRACE( TraceLevels::Error, TXT( "TgaImageLoader::Load(): Failed to read image width from stream.\n" ) );

        return false;
    }

    if( imageWidth == 0 )
    {
        HELIUM_TRACE( TraceLevels::Error, TXT( "TgaImageLoader::Load(): Image has width of zero.\n" ) );

        return false;
    }

    uint16_t imageHeight;
    if( pStream->Read( &imageHeight, sizeof( imageHeight ), 1 ) != 1 )
    {
        HELIUM_TRACE( TraceLevels::Error, TXT( "TgaImageLoader::Load(): Failed to read image height from stream.\n" ) );

        return false;
    }

    if( imageHeight == 0 )
    {
        HELIUM_TRACE( TraceLevels::Error, TXT( "TgaImageLoader::Load(): Image has height of zero.\n" ) );

        return false;
    }

    uint8_t imagePixelSize;
    if( pStream->Read( &imagePixelSize, sizeof( imagePixelSize ), 1 ) != 1 )
    {
        HELIUM_TRACE( TraceLevels::Error, TXT( "TgaImageLoader::Load(): Failed to read image pixel size from stream.\n" ) );

        return false;
    }

    if( imagePixelSize != 8 && imagePixelSize != 16 && imagePixelSize != 24 && imagePixelSize != 32 )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            ( TXT( "TgaImageLoader::Load(): Unsupported image pixel size (%" ) TPRIu8 TXT( ") specified.  Only " )
            TXT( "8, 16, 24, and 32 bit pixel sizes are supported.\n" ) ),
            imagePixelSize );

        return false;
    }

    if( bColorMapImageType )
    {
        if( imagePixelSize > 16 )
        {
            HELIUM_TRACE(
                TraceLevels::Error,
                ( TXT( "TgaImageLoader::Load(): Color-mapped image types using more than 16 bits per pixel are " )
                TXT( "not supported.\n" ) ) );

            return false;
        }
    }
    else
    {
        if( imagePixelSize < 16 && imageColorType == 2 )
        {
            HELIUM_TRACE(
                TraceLevels::Error,
                ( TXT( "TgaImageLoader::Load(): True-color images with less than 16 bits per pixel are not " )
                TXT( "supported.\n" ) ) );

            return false;
        }
    }

    uint8_t imageDescriptor;
    if( pStream->Read( &imageDescriptor, sizeof( imageDescriptor ), 1 ) != 1 )
    {
        HELIUM_TRACE( TraceLevels::Error, TXT( "TgaImageLoader::Load(): Failed to read image descriptor byte from stream.\n" ) );

        return false;
    }

    uint8_t alphaBitCount = ( imageDescriptor & 0x0f );
    uint8_t reservedBit = ( imageDescriptor & 0x10 );
    uint8_t alternateScreenOrigin = ( imageDescriptor & 0x20 );
    uint8_t interleavingFlags = ( imageDescriptor & 0xc0 );

    if( alphaBitCount != 0 )
    {
        if( bColorMapImageType )
        {
            if( colorMapEntrySize == 16 )
            {
                if( alphaBitCount != 1 )
                {
                    HELIUM_TRACE(
                        TraceLevels::Error,
                        ( TXT( "TgaImageLoader::Load(): Only 1-bit alpha channels are supported for color-mapped " )
                        TXT( "images with 16-bit color map entries (%" ) TPRIu8 TXT( " bits specified).\n" ) ),
                        alphaBitCount );

                    return false;
                }
            }
            else if( colorMapEntrySize == 24 )
            {
                HELIUM_TRACE(
                    TraceLevels::Error,
                    ( TXT( "TgaImageLoader::Load(): Alpha channel data is not supported for color-mapped images " )
                    TXT( "with 24-bit color map entries (%" ) TPRIu8 TXT( " bits specified).\n" ) ),
                    alphaBitCount );

                return false;
            }
            else
            {
                HELIUM_ASSERT( colorMapEntrySize == 32 );

                if( alphaBitCount != 8 )
                {
                    HELIUM_TRACE(
                        TraceLevels::Error,
                        ( TXT( "TgaImageLoader::Load(): Only 8-bit alpha channels are supported for color-mapped " )
                        TXT( "images with 32-bit color map entries (%" ) TPRIu8 TXT( " bits specified).\n" ) ),
                        alphaBitCount );

                    return false;
                }
            }
        }
        else if( imageColorType == 3 )
        {
            HELIUM_TRACE(
                TraceLevels::Error,
                TXT( "TgaImageLoader::Load(): Alpha channel data is not supported for grayscale images.\n" ) );

            return false;
        }
        else
        {
            HELIUM_ASSERT( imagePixelSize != 8 );

            if( imagePixelSize == 16 )
            {
                if( alphaBitCount != 1 )
                {
                    HELIUM_TRACE(
                        TraceLevels::Error,
                        ( TXT( "TgaImageLoader::Load(): Only 1-bit alpha channels are supported for 16-bit " )
                        TXT( "true-color images (%" ) TPRIu8 TXT( " bits specified).\n" ) ),
                        alphaBitCount );

                    return false;
                }
            }
            else if( imagePixelSize == 24 )
            {
                HELIUM_TRACE(
                    TraceLevels::Error,
                    ( TXT( "TgaImageLoader::Load(): Alpha channel data is not supported for 24-bit true-color " )
                    TXT( "images (%" ) TPRIu8 TXT( " bits specified).\n" ) ),
                    alphaBitCount );

                return false;
            }
            else
            {
                HELIUM_ASSERT( imagePixelSize == 32 );

                if( alphaBitCount != 8 )
                {
                    HELIUM_TRACE(
                        TraceLevels::Error,
                        ( TXT( "TgaImageLoader::Load(): Only 8-bit alpha channels are supported for 32-bit " )
                        TXT( "true-color images (%" ) TPRIu8 TXT( " bits specified).\n" ) ),
                        alphaBitCount );

                    return false;
                }
            }
        }
    }

    if( reservedBit != 0 )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            ( TXT( "TgaImageLoader::Load(): Reserved bit (offset 4) set in image descriptor byte is not " )
            TXT( "supported.\n" ) ) );

        return false;
    }

    if( interleavingFlags != 0 )
    {
        HELIUM_TRACE( TraceLevels::Error, TXT( "TgaImageLoader::Load(): Only non-interleaved image formats are supported.\n" ) );

        return false;
    }

    // Skip over the image ID.
    uint8_t imageIdBuffer[ UINT8_MAX ];
    if( pStream->Read( imageIdBuffer, 1, imageIdLength ) != imageIdLength )
    {
        HELIUM_TRACE( TraceLevels::Error, TXT( "TgaImageLoader::Load(): Failed to read image ID from stream.\n" ) );

        return false;
    }

    // Read the color map data.
    DynamicArray< Color > colorMap;
    if( colorMapSize != 0 )
    {
        colorMap.Reserve( colorMapSize );

        uint_fast16_t colorMapSizeFast = colorMapSize;
        switch( colorMapEntrySize )
        {
        case 16:
            {
                for( uint_fast16_t colorMapIndex = 0; colorMapIndex < colorMapSizeFast; ++colorMapIndex )
                {
                    uint16_t colorValue;
                    if( pStream->Read( &colorValue, sizeof( colorValue ), 1 ) != 1 )
                    {
                        HELIUM_TRACE(
                            TraceLevels::Error,
                            ( TXT( "TgaImageLoader::Load(): Failed to read color map entry %" ) TPRIuFAST16
                            TXT( " from stream.\n" ) ),
                            colorMapIndex );

                        return false;
                    }

                    uint8_t red = static_cast< uint8_t >(
                        ( static_cast< uint_fast16_t >( ( colorValue >> 10 ) & 0x1f ) * 0xff ) / 0x1f );
                    uint8_t green = static_cast< uint8_t >(
                        ( static_cast< uint_fast16_t >( ( colorValue >> 5 ) & 0x1f ) * 0xff ) / 0x1f );
                    uint8_t blue = static_cast< uint8_t >(
                        ( static_cast< uint_fast16_t >( colorValue & 0x1f ) * 0xff ) / 0x1f );
                    uint8_t alpha = ( alphaBitCount != 0 && ( colorValue & 0x8000 ) == 0 ? 0 : 0xff );

                    HELIUM_VERIFY( colorMap.New( red, green, blue, alpha ) );
                }

                break;
            }

        default:
            {
                uint8_t colorValues[ 4 ];
                colorValues[ 3 ] = 0xff;

                size_t readCount = colorMapEntrySize / 8;

                for( uint_fast16_t colorMapIndex = 0; colorMapIndex < colorMapSizeFast; ++colorMapIndex )
                {
                    if( pStream->Read( colorValues, 1, readCount ) != readCount )
                    {
                        HELIUM_TRACE(
                            TraceLevels::Error,
                            ( TXT( "TgaImageLoader::Load(): Failed to read color map entry %" ) TPRIuFAST16
                            TXT( " from stream.\n" ) ),
                            colorMapIndex );

                        return false;
                    }

                    if( alphaBitCount == 0 )
                    {
                        colorValues[ 3 ] = 0xff;
                    }

                    HELIUM_VERIFY( colorMap.New(
                        colorValues[ 2 ],
                        colorValues[ 1 ],
                        colorValues[ 0 ],
                        colorValues[ 3 ] ) );
                }

                break;
            }
        }

        if( !bColorMapImageType )
        {
            // Dump the color map data we just loaded.
            HELIUM_TRACE(
                TraceLevels::Warning,
                TXT( "TgaImageLoader::Load(): Dumping loaded color map data for non-color mapped image.\n" ) );

            colorMap.Clear();
        }
    }

    // Initialize the image, as we'll read the image data directly into it.
    Image::InitParameters imageParameters;
    imageParameters.width = imageWidth;
    imageParameters.height = imageHeight;
    imageParameters.format.SetBytesPerPixel( imagePixelSize / 8 );
    imageParameters.format.SetPalette( colorMap.GetData(), static_cast< uint32_t >( colorMap.GetSize() ) );
    if( !bColorMapImageType )
    {
        switch( imagePixelSize )
        {
        case 8:
            {
                // We should have already verified that we are not trying to load an 8-bit true-color image by this
                // point.
                HELIUM_ASSERT( imageColorType == 3 );

                imageParameters.format.SetChannelBitOffset( Image::CHANNEL_RED, 0 );
                imageParameters.format.SetChannelBitCount( Image::CHANNEL_RED, 8 );

                break;
            }

        case 16:
            {
                if( imageColorType == 3 )
                {
                    imageParameters.format.SetChannelBitOffset( Image::CHANNEL_RED, 0 );
                    imageParameters.format.SetChannelBitCount( Image::CHANNEL_RED, 16 );
                }
                else
                {
                    imageParameters.format.SetChannelBitOffset( Image::CHANNEL_RED, 10 );
                    imageParameters.format.SetChannelBitCount( Image::CHANNEL_RED, 5 );
                    imageParameters.format.SetChannelBitOffset( Image::CHANNEL_GREEN, 5 );
                    imageParameters.format.SetChannelBitCount( Image::CHANNEL_GREEN, 5 );
                    imageParameters.format.SetChannelBitOffset( Image::CHANNEL_BLUE, 0 );
                    imageParameters.format.SetChannelBitCount( Image::CHANNEL_BLUE, 5 );
                    if( alphaBitCount != 0 )
                    {
                        imageParameters.format.SetChannelBitOffset( Image::CHANNEL_ALPHA, 15 );
                        imageParameters.format.SetChannelBitCount( Image::CHANNEL_ALPHA, 1 );
                    }
                }

                break;
            }

        case 24:
            {
                if( imageColorType == 3 )
                {
                    imageParameters.format.SetChannelBitOffset( Image::CHANNEL_RED, 0 );
                    imageParameters.format.SetChannelBitCount( Image::CHANNEL_RED, 24 );
                }
                else
                {
                    imageParameters.format.SetChannelBitOffset( Image::CHANNEL_RED, 16 );
                    imageParameters.format.SetChannelBitCount( Image::CHANNEL_RED, 8 );
                    imageParameters.format.SetChannelBitOffset( Image::CHANNEL_GREEN, 8 );
                    imageParameters.format.SetChannelBitCount( Image::CHANNEL_GREEN, 8 );
                    imageParameters.format.SetChannelBitOffset( Image::CHANNEL_BLUE, 0 );
                    imageParameters.format.SetChannelBitCount( Image::CHANNEL_BLUE, 8 );
                }

                break;
            }

        default:
            {
                HELIUM_ASSERT( imagePixelSize == 32 );

                if( imageColorType == 3 )
                {
                    imageParameters.format.SetChannelBitOffset( Image::CHANNEL_RED, 0 );
                    imageParameters.format.SetChannelBitCount( Image::CHANNEL_RED, 32 );
                }
                else
                {
                    imageParameters.format.SetChannelBitOffset( Image::CHANNEL_RED, 16 );
                    imageParameters.format.SetChannelBitCount( Image::CHANNEL_RED, 8 );
                    imageParameters.format.SetChannelBitOffset( Image::CHANNEL_GREEN, 8 );
                    imageParameters.format.SetChannelBitCount( Image::CHANNEL_GREEN, 8 );
                    imageParameters.format.SetChannelBitOffset( Image::CHANNEL_BLUE, 0 );
                    imageParameters.format.SetChannelBitCount( Image::CHANNEL_BLUE, 8 );
                    if( alphaBitCount != 0 )
                    {
                        imageParameters.format.SetChannelBitOffset( Image::CHANNEL_ALPHA, 24 );
                        imageParameters.format.SetChannelBitCount( Image::CHANNEL_ALPHA, 8 );
                    }
                }
            }
        }
    }

    Image stagingImage;
    if( !stagingImage.Initialize( imageParameters ) )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            TXT( "TgaImageLoader::Load(): Failed to initialize target image with image file parameters.\n" ) );

        return false;
    }

    // Read the image data.
    void* pPixelData = stagingImage.GetPixelData();
    HELIUM_ASSERT( pPixelData );

    uint8_t bytesPerPixel = imagePixelSize / 8;

    bool bImageLoadResult;
    if( bRleImage )
    {
        RleReader reader( pStream, bytesPerPixel );
        bImageLoadResult = LoadImageData(
            pPixelData,
            reader,
            imageWidth,
            imageHeight,
            bytesPerPixel,
            ( alternateScreenOrigin != 0 ) );
    }
    else
    {
        UncompressedReader reader( pStream, bytesPerPixel );
        bImageLoadResult = LoadImageData(
            pPixelData,
            reader,
            imageWidth,
            imageHeight,
            bytesPerPixel,
            ( alternateScreenOrigin != 0 ) );
    }

    if( !bImageLoadResult )
    {
        HELIUM_TRACE( TraceLevels::Error, TXT( "TgaImageLoader::Load(): Failed to load image pixel data from stream.\n" ) );

        return false;
    }

    // Image is now loaded.
    rImage.Swap( stagingImage );

    return true;
}

#endif  // HELIUM_TOOLS
