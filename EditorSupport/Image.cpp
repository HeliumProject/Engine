//----------------------------------------------------------------------------------------------------------------------
// Image.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EditorSupportPch.h"

#if HELIUM_TOOLS

#include "EditorSupport/Image.h"

#include "Rendering/Color.h"

using namespace Helium;

// Pixel value reader for one-byte pixel sizes.
class PixelValueReader1
{
public:
    static const uint32_t BYTES_PER_PIXEL = 1;

    uint32_t operator()( const void* pPixel ) const
    {
        return *static_cast< const uint8_t* >( pPixel );
    }
};

// Pixel value reader for two-byte pixel sizes.
class PixelValueReader2
{
public:
    static const uint32_t BYTES_PER_PIXEL = 2;

    uint32_t operator()( const void* pPixel ) const
    {
        return *static_cast< const uint16_t* >( pPixel );
    }
};

// Pixel value reader for three-byte pixel sizes.
class PixelValueReader3
{
public:
    static const uint32_t BYTES_PER_PIXEL = 3;

    uint32_t operator()( const void* pPixel ) const
    {
        const uint8_t* pPixelBytes = static_cast< const uint8_t* >( pPixel );
#if HELIUM_ENDIAN_LITTLE
        uint32_t pixelValue =
            static_cast< uint32_t >( pPixelBytes[ 0 ] ) |
            ( static_cast< uint32_t >( pPixelBytes[ 1 ] ) << 8 ) |
            ( static_cast< uint32_t >( pPixelBytes[ 2 ] ) << 16 );
#else
        uint32_t pixelValue =
            ( static_cast< uint32_t >( pPixelBytes[ 0 ] ) << 16 ) |
            ( static_cast< uint32_t >( pPixelBytes[ 1 ] ) << 8 ) |
            static_cast< uint32_t >( pPixelBytes[ 2 ] );
#endif

        return pixelValue;
    }
};

// Pixel value reader for four-byte pixel sizes.
class PixelValueReader4
{
public:
    static const uint32_t BYTES_PER_PIXEL = 4;

    uint32_t operator()( const void* pPixel ) const
    {
        return *static_cast< const uint32_t* >( pPixel );
    }
};

// Palettized pixel color reader.
class PalettizedColorReader
{
public:
    PalettizedColorReader( const Color* pPalette, uint32_t paletteSize )
        : m_pPalette( pPalette )
        , m_paletteSize( paletteSize )
    {
        HELIUM_ASSERT( pPalette );
        HELIUM_ASSERT( paletteSize != 0 );
    }

    void operator()(
        uint32_t pixelValue,
        uint32_t& rRed,
        uint32_t& rGreen,
        uint32_t& rBlue,
        uint32_t& rAlpha ) const
    {
        if( pixelValue >= m_paletteSize )
        {
            pixelValue = 0;
        }

        const Color& rColor = m_pPalette[ pixelValue ];

        rRed = rColor.GetR();
        rGreen = rColor.GetG();
        rBlue = rColor.GetB();
        rAlpha = rColor.GetA();
    }

private:
    const Color* m_pPalette;
    uint32_t m_paletteSize;
};

// Non-palettized pixel color reader.
class DirectColorReader
{
public:
    DirectColorReader( const uint8_t* pChannelBitCounts, const uint8_t* pChannelBitOffsets )
        : m_pChannelBitOffsets( pChannelBitOffsets )
    {
        HELIUM_ASSERT( pChannelBitCounts );
        HELIUM_ASSERT( pChannelBitOffsets );

        for( size_t channelIndex = 0; channelIndex < Image::CHANNEL_MAX; ++channelIndex )
        {
            m_channelBitMasks[ channelIndex ] = ( 1U << pChannelBitCounts[ channelIndex ] ) - 1;
        }
    }

    void operator()(
        uint32_t pixelValue,
        uint32_t& rRed,
        uint32_t& rGreen,
        uint32_t& rBlue,
        uint32_t& rAlpha ) const
    {
        rRed = ( pixelValue >> m_pChannelBitOffsets[ Image::CHANNEL_RED ] ) &
            m_channelBitMasks[ Image::CHANNEL_RED ];
        rGreen = ( pixelValue >> m_pChannelBitOffsets[ Image::CHANNEL_GREEN ] ) &
            m_channelBitMasks[ Image::CHANNEL_GREEN ];
        rBlue = ( pixelValue >> m_pChannelBitOffsets[ Image::CHANNEL_BLUE ] ) &
            m_channelBitMasks[ Image::CHANNEL_BLUE ];
        rAlpha = ( pixelValue >> m_pChannelBitOffsets[ Image::CHANNEL_ALPHA ] ) &
            m_channelBitMasks[ Image::CHANNEL_ALPHA ];
    }

private:
    const uint8_t* m_pChannelBitOffsets;
    uint32_t m_channelBitMasks[ Image::CHANNEL_MAX ];
};

// Palettized pixel color writer.
class PalettizedColorWriter
{
public:
    PalettizedColorWriter( const Color* pPalette, uint32_t paletteSize )
        : m_pPalette( pPalette )
        , m_paletteSize( paletteSize )
    {
        HELIUM_ASSERT( pPalette );
        HELIUM_ASSERT( paletteSize != 0 );
    }

    uint32_t operator()( uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha ) const
    {
        Color color(
            static_cast< uint8_t >( red ),
            static_cast< uint8_t >( green ),
            static_cast< uint8_t >( blue ),
            static_cast< uint8_t >( alpha ) );

        uint32_t closestColorIndex = 0;
        uint32_t closestColorDifference = ComputeColorDifference( m_pPalette[ 0 ], color );

        for( uint32_t colorIndex = 1; colorIndex < m_paletteSize; ++colorIndex )
        {
            uint32_t colorDifference = ComputeColorDifference( m_pPalette[ colorIndex ], color );
            if( colorDifference < closestColorDifference )
            {
                closestColorIndex = colorIndex;
                closestColorDifference = colorDifference;
            }
        }

        return closestColorIndex;
    }

private:
    const Color* m_pPalette;
    uint32_t m_paletteSize;

    static uint32_t ComputeColorDifference( const Color& rColor0, const Color& rColor1 )
    {
        int32_t difference =
            Abs( static_cast< int32_t >( rColor0.GetR() ) - static_cast< int32_t >( rColor1.GetR() ) ) +
            Abs( static_cast< int32_t >( rColor0.GetG() ) - static_cast< int32_t >( rColor1.GetG() ) ) +
            Abs( static_cast< int32_t >( rColor0.GetB() ) - static_cast< int32_t >( rColor1.GetB() ) ) +
            Abs( static_cast< int32_t >( rColor0.GetA() ) - static_cast< int32_t >( rColor1.GetA() ) );

        return static_cast< uint32_t >( difference );
    }
};

// Non-palettized pixel color writer.
class DirectColorWriter
{
public:
    DirectColorWriter( const uint8_t* pChannelBitOffsets )
        : m_pChannelBitOffsets( pChannelBitOffsets )
    {
        HELIUM_ASSERT( pChannelBitOffsets );
    }

    uint32_t operator()( uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha ) const
    {
        uint32_t pixelValue =
            ( red << m_pChannelBitOffsets[ Image::CHANNEL_RED ] ) |
            ( green << m_pChannelBitOffsets[ Image::CHANNEL_GREEN ] ) |
            ( blue << m_pChannelBitOffsets[ Image::CHANNEL_BLUE ] ) |
            ( alpha << m_pChannelBitOffsets[ Image::CHANNEL_ALPHA ] );

        return pixelValue;
    }

private:
    const uint8_t* m_pChannelBitOffsets;
};

// Pixel value writer for one-byte pixel sizes.
class PixelValueWriter1
{
public:
    static const uint32_t BYTES_PER_PIXEL = 1;

    void operator()( void* pPixel, uint32_t pixelValue ) const
    {
        *static_cast< uint8_t* >( pPixel ) = static_cast< uint8_t >( pixelValue );
    }
};

// Pixel value writer for two-byte pixel sizes.
class PixelValueWriter2
{
public:
    static const uint32_t BYTES_PER_PIXEL = 2;

    void operator()( void* pPixel, uint32_t pixelValue ) const
    {
        *static_cast< uint16_t* >( pPixel ) = static_cast< uint16_t >( pixelValue );
    }
};

// Pixel value writer for three-byte pixel sizes.
class PixelValueWriter3
{
public:
    static const uint32_t BYTES_PER_PIXEL = 3;

    void operator()( void* pPixel, uint32_t pixelValue ) const
    {
        uint8_t* pPixelBytes = static_cast< uint8_t* >( pPixel );
#if HELIUM_ENDIAN_LITTLE
        pPixelBytes[ 0 ] = static_cast< uint8_t >( pixelValue );
        pPixelBytes[ 1 ] = static_cast< uint8_t >( pixelValue >> 8 );
        pPixelBytes[ 2 ] = static_cast< uint8_t >( pixelValue >> 16 );
#else
        pPixelBytes[ 0 ] = static_cast< uint8_t >( pixelValue >> 16 );
        pPixelBytes[ 1 ] = static_cast< uint8_t >( pixelValue >> 8 );
        pPixelBytes[ 2 ] = static_cast< uint8_t >( pixelValue );
#endif
    }
};

// Pixel value writer for four-byte pixel sizes.
class PixelValueWriter4
{
public:
    static const uint32_t BYTES_PER_PIXEL = 4;

    void operator()( void* pPixel, uint32_t pixelValue ) const
    {
        *static_cast< uint32_t* >( pPixel ) = pixelValue;
    }
};

// Image conversion loop.
template<
typename PixelValueReaderType,
typename ColorReaderType,
typename ColorWriterType,
typename PixelValueWriterType >
void ConvertImage(
                  PixelValueReaderType& rValueReader,
                  ColorReaderType& rColorReader,
                  ColorWriterType& rColorWriter,
                  PixelValueWriterType& rValueWriter,
                  const void* pSourceData,
                  uint32_t sourcePitch,
                  const uint32_t* pSourceChannelMaxValues,
                  void* pDestData,
                  uint32_t destPitch,
                  const uint32_t* pDestChannelMaxValues,
                  const uint32_t* pChannelAdjustments,
                  uint32_t width,
                  uint32_t height )
{
    HELIUM_ASSERT( pSourceData );
    HELIUM_ASSERT( pSourceChannelMaxValues );
    HELIUM_ASSERT( pDestData );
    HELIUM_ASSERT( pDestChannelMaxValues );
    HELIUM_ASSERT( pChannelAdjustments );

    const uint8_t* pSourceRow = static_cast< const uint8_t* >( pSourceData );
    uint8_t* pDestRow = static_cast< uint8_t* >( pDestData );

    uint32_t red, green, blue, alpha;

    for( uint32_t y = 0; y < height; ++y )
    {
        const uint8_t* pSourcePixel = pSourceRow;
        uint8_t* pDestPixel = pDestRow;

        for( uint32_t x = 0; x < width; ++x )
        {
            uint32_t pixelValue = rValueReader( pSourcePixel );
            rColorReader( pixelValue, red, green, blue, alpha );

            red = ( red * pDestChannelMaxValues[ Image::CHANNEL_RED ] ) /
                pSourceChannelMaxValues[ Image::CHANNEL_RED ] +
                pChannelAdjustments[ Image::CHANNEL_RED ];
            green = ( green * pDestChannelMaxValues[ Image::CHANNEL_GREEN ] ) /
                pSourceChannelMaxValues[ Image::CHANNEL_GREEN ] +
                pChannelAdjustments[ Image::CHANNEL_GREEN ];
            blue = ( blue * pDestChannelMaxValues[ Image::CHANNEL_BLUE ] ) /
                pSourceChannelMaxValues[ Image::CHANNEL_BLUE ] +
                pChannelAdjustments[ Image::CHANNEL_BLUE ];
            alpha = ( alpha * pDestChannelMaxValues[ Image::CHANNEL_ALPHA ] ) /
                pSourceChannelMaxValues[ Image::CHANNEL_ALPHA ] +
                pChannelAdjustments[ Image::CHANNEL_ALPHA ];

            pixelValue = rColorWriter( red, green, blue, alpha );
            rValueWriter( pDestPixel, pixelValue );

            pSourcePixel += PixelValueReaderType::BYTES_PER_PIXEL;
            pDestPixel += PixelValueWriterType::BYTES_PER_PIXEL;
        }

        pSourceRow += sourcePitch;
        pDestRow += destPitch;
    }
}

// Inner-most switch statement for running the image conversion loop.
template< typename PixelValueReaderType, typename ColorReaderType, typename ColorWriterType >
void ConvertImageDestPixelSizeSwitch(
                                     PixelValueReaderType& rValueReader,
                                     ColorReaderType& rColorReader,
                                     ColorWriterType& rColorWriter,
                                     const void* pSourceData,
                                     uint32_t sourcePitch,
                                     const uint32_t* pSourceChannelMaxValues,
                                     void* pDestData,
                                     uint32_t destBytesPerPixel,
                                     uint32_t destPitch,
                                     const uint32_t* pDestChannelMaxValues,
                                     const uint32_t* pChannelAdjustments,
                                     uint32_t width,
                                     uint32_t height )
{
    switch( destBytesPerPixel )
    {
    case 1:
        {
            PixelValueWriter1 valueWriter;
            ConvertImage(
                rValueReader,
                rColorReader,
                rColorWriter,
                valueWriter,
                pSourceData,
                sourcePitch,
                pSourceChannelMaxValues,
                pDestData,
                destPitch,
                pDestChannelMaxValues,
                pChannelAdjustments,
                width,
                height );

            break;
        }

    case 2:
        {
            PixelValueWriter2 valueWriter;
            ConvertImage(
                rValueReader,
                rColorReader,
                rColorWriter,
                valueWriter,
                pSourceData,
                sourcePitch,
                pSourceChannelMaxValues,
                pDestData,
                destPitch,
                pDestChannelMaxValues,
                pChannelAdjustments,
                width,
                height );

            break;
        }

    case 3:
        {
            PixelValueWriter3 valueWriter;
            ConvertImage(
                rValueReader,
                rColorReader,
                rColorWriter,
                valueWriter,
                pSourceData,
                sourcePitch,
                pSourceChannelMaxValues,
                pDestData,
                destPitch,
                pDestChannelMaxValues,
                pChannelAdjustments,
                width,
                height );

            break;
        }

    default:
        {
            HELIUM_ASSERT( destBytesPerPixel == 4 );

            PixelValueWriter4 valueWriter;
            ConvertImage(
                rValueReader,
                rColorReader,
                rColorWriter,
                valueWriter,
                pSourceData,
                sourcePitch,
                pSourceChannelMaxValues,
                pDestData,
                destPitch,
                pDestChannelMaxValues,
                pChannelAdjustments,
                width,
                height );

            break;
        }
    }
}

// Second inner-most switch statement for running the image conversion loop.
template< typename ColorReaderType, typename ColorWriterType >
void ConvertImageSourcePixelSizeSwitch(
                                       ColorReaderType& rColorReader,
                                       ColorWriterType& rColorWriter,
                                       const void* pSourceData,
                                       uint32_t sourceBytesPerPixel,
                                       uint32_t sourcePitch,
                                       const uint32_t* pSourceChannelMaxValues,
                                       void* pDestData,
                                       uint32_t destBytesPerPixel,
                                       uint32_t destPitch,
                                       const uint32_t* pDestChannelMaxValues,
                                       const uint32_t* pChannelAdjustments,
                                       uint32_t width,
                                       uint32_t height )
{
    switch( sourceBytesPerPixel )
    {
    case 1:
        {
            PixelValueReader1 valueReader;
            ConvertImageDestPixelSizeSwitch(
                valueReader,
                rColorReader,
                rColorWriter,
                pSourceData,
                sourcePitch,
                pSourceChannelMaxValues,
                pDestData,
                destBytesPerPixel,
                destPitch,
                pDestChannelMaxValues,
                pChannelAdjustments,
                width,
                height );

            break;
        }

    case 2:
        {
            PixelValueReader2 valueReader;
            ConvertImageDestPixelSizeSwitch(
                valueReader,
                rColorReader,
                rColorWriter,
                pSourceData,
                sourcePitch,
                pSourceChannelMaxValues,
                pDestData,
                destBytesPerPixel,
                destPitch,
                pDestChannelMaxValues,
                pChannelAdjustments,
                width,
                height );

            break;
        }

    case 3:
        {
            PixelValueReader3 valueReader;
            ConvertImageDestPixelSizeSwitch(
                valueReader,
                rColorReader,
                rColorWriter,
                pSourceData,
                sourcePitch,
                pSourceChannelMaxValues,
                pDestData,
                destBytesPerPixel,
                destPitch,
                pDestChannelMaxValues,
                pChannelAdjustments,
                width,
                height );

            break;
        }

    default:
        {
            HELIUM_ASSERT( destBytesPerPixel == 4 );

            PixelValueReader4 valueReader;
            ConvertImageDestPixelSizeSwitch(
                valueReader,
                rColorReader,
                rColorWriter,
                pSourceData,
                sourcePitch,
                pSourceChannelMaxValues,
                pDestData,
                destBytesPerPixel,
                destPitch,
                pDestChannelMaxValues,
                pChannelAdjustments,
                width,
                height );

            break;
        }
    }
}

/// Constructor.
Image::Image()
: m_pPixelData( NULL )
, m_width( 0 )
, m_height( 0 )
, m_pitch( 0 )
{
}

/// Copy constructor.
///
/// @param[in] rSource  Source image from which to copy.
Image::Image( const Image& rSource )
{
    PrivateCopy( rSource );
}

/// Destructor.
Image::~Image()
{
    PrivateFree();
}

/// Initialize this image based on the specified parameters.
///
/// @param[in] rParameters  Initialization parameters.
///
/// @return  True if initialization was successful, false if not.
///
/// @see Unload()
bool Image::Initialize( const InitParameters& rParameters )
{
    // Unload any existing image data first.
    Unload();

    // Validate the initialization parameters.
    if( rParameters.width == 0 )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "Image::Initialize(): Cannot initialize an image with a width of zero.\n" ) );

        return false;
    }

    if( rParameters.height == 0 )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "Image::Initialize(): Cannot initialize an image with a height of zero.\n" ) );

        return false;
    }

    if( !rParameters.format.Validate() )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "Image::Initialize(): Image format validation failed.\n" ) );

        return false;
    }

    uint32_t pitchMin = rParameters.width * rParameters.format.GetBytesPerPixel();
    uint32_t pitch = rParameters.pitch;
    if( pitch == 0 )
    {
        pitch = pitchMin;
    }
    else if( pitch < pitchMin )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "Image::Initialize(): Pitch specified (%" ) TPRIu32 TXT( ") is less than the minimum pitch " )
            TXT( "necessary (%" ) TPRIu32 TXT( ") for the target image width and bytes per pixel.\n" ) ),
            pitch,
            pitchMin );

        return false;
    }

    // Check for palette data.
    const Color* pPalette = rParameters.format.GetPalette();
    uint32_t paletteSize = rParameters.format.GetPaletteSize();
    if( pPalette )
    {
        if( paletteSize == 0 )
        {
            pPalette = NULL;
        }
    }
    else
    {
        if( paletteSize != 0 )
        {
            HELIUM_TRACE(
                TRACE_WARNING,
                ( TXT( "Image::Initialize(): Null palette specified with a non-zero palette size.  No palette " )
                TXT( "will be used.\n" ) ) );

            paletteSize = 0;
        }
    }

    // Copy the palette if one was provided.
    Color* pPaletteCopy = NULL;
    if( pPalette )
    {
        pPaletteCopy = new Color [ paletteSize ];
        HELIUM_ASSERT( pPaletteCopy );
        if( !pPaletteCopy )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "Image::Initialize(): Failed to allocate %" ) TPRIu32 TXT( " color entries for paletted " )
                TXT( "data.\n" ) ),
                paletteSize );

            return false;
        }

        ArrayCopy( pPaletteCopy, pPalette, paletteSize );
    }

    // Allocate new image data.
    size_t imageSize = pitch * rParameters.height;
    uint8_t* pPixelData = new uint8_t [ imageSize ];
    HELIUM_ASSERT( pPixelData );
    if( !pPixelData )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "Image::Initialize(): Failed to allocate %" ) TPRIuSZ TXT( " bytes for image pixel data.\n" ),
            imageSize );

        delete [] pPaletteCopy;

        return false;
    }

    // If existing image data was provided, copy it into the new image pixel buffer.
    if( rParameters.pPixelData )
    {
        MemoryCopy( pPixelData, rParameters.pPixelData, imageSize );
    }

    // Store the new image data.
    m_pPixelData = pPixelData;

    m_width = rParameters.width;
    m_height = rParameters.height;
    m_pitch = pitch;

    m_format.SetBytesPerPixel( rParameters.format.GetBytesPerPixel() );

    if( pPaletteCopy )
    {
        m_format.SetPalette( pPaletteCopy, paletteSize );
    }
    else
    {
        for( size_t channelIndex = 0; channelIndex < CHANNEL_MAX; ++channelIndex )
        {
            EChannel channel = static_cast< EChannel >( channelIndex );

            m_format.SetChannelBitCount( channel, rParameters.format.GetChannelBitCount( channel ) );
            m_format.SetChannelBitOffset( channel, rParameters.format.GetChannelBitOffset( channel ) );
        }
    }

    return true;
}

/// Unload all allocated image data and reset this object to its uninitialized state.
///
/// @see Initialize()
void Image::Unload()
{
    PrivateFree();

    m_pPixelData = NULL;

    m_width = 0;
    m_height = 0;
    m_pitch = 0;

    m_format = Format();
}

/// Swap the contents of this image with the given image.
///
/// @param[in] rImage  Image with which to swap.
void Image::Swap( Image& rImage )
{
    Helium::Swap( m_pPixelData, rImage.m_pPixelData );

    Helium::Swap( m_width, rImage.m_width );
    Helium::Swap( m_height, rImage.m_height );
    Helium::Swap( m_pitch, rImage.m_pitch );

    Helium::Swap( m_format, rImage.m_format );
}

/// Convert this image to the given format and store in the destination image object.
///
/// @param[out] rDestination  Converted image.
/// @param[in]  rFormat       Destination format.
///
/// @return  True if conversion was successful, false if not.
bool Image::Convert( Image& rDestination, const Format& rFormat ) const
{
    // Initialize a temporary image into which the converted image will be initially written.
    Image::InitParameters imageParameters;
    imageParameters.width = m_width;
    imageParameters.height = m_height;
    imageParameters.format = rFormat;

    Image stagingImage;
    if( !stagingImage.Initialize( imageParameters ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "Image::Convert(): Failed to initialize staging image with the destination format.\n" ) );

        return false;
    }

    // Convert the image based on key properties of the source and destination formats (specifically, the number of
    // bytes per pixel and whether a color palette is used.
    uint32_t sourceBytesPerPixel = m_format.GetBytesPerPixel();
    const Color* pSourcePalette = m_format.GetPalette();
    uint32_t sourcePaletteSize = m_format.GetPaletteSize();
    const uint8_t* pSourceChannelBitCounts = m_format.GetChannelBitCounts();
    const uint8_t* pSourceChannelBitOffsets = m_format.GetChannelBitOffsets();

    uint32_t destBytesPerPixel = stagingImage.m_format.GetBytesPerPixel();
    const Color* pDestPalette = stagingImage.m_format.GetPalette();
    uint32_t destPaletteSize = stagingImage.m_format.GetPaletteSize();
    const uint8_t* pDestChannelBitCounts = stagingImage.m_format.GetChannelBitCounts();
    const uint8_t* pDestChannelBitOffsets = stagingImage.m_format.GetChannelBitOffsets();

    uint32_t sourceChannelMaxValues[ CHANNEL_MAX ];
    if( pSourcePalette )
    {
        sourceChannelMaxValues[ CHANNEL_RED ] = 0xff;
        sourceChannelMaxValues[ CHANNEL_GREEN ] = 0xff;
        sourceChannelMaxValues[ CHANNEL_BLUE ] = 0xff;
        sourceChannelMaxValues[ CHANNEL_ALPHA ] = 0xff;
    }
    else
    {
        sourceChannelMaxValues[ CHANNEL_RED ] = ( 1U << pSourceChannelBitCounts[ CHANNEL_RED ] ) - 1;
        sourceChannelMaxValues[ CHANNEL_GREEN ] = ( 1U << pSourceChannelBitCounts[ CHANNEL_GREEN ] ) - 1;
        sourceChannelMaxValues[ CHANNEL_BLUE ] = ( 1U << pSourceChannelBitCounts[ CHANNEL_BLUE ] ) - 1;
        sourceChannelMaxValues[ CHANNEL_ALPHA ] = ( 1U << pSourceChannelBitCounts[ CHANNEL_ALPHA ] ) - 1;
    }

    uint32_t destChannelMaxValues[ CHANNEL_MAX ];
    if( pDestPalette )
    {
        destChannelMaxValues[ CHANNEL_RED ] = 0xff;
        destChannelMaxValues[ CHANNEL_GREEN ] = 0xff;
        destChannelMaxValues[ CHANNEL_BLUE ] = 0xff;
        destChannelMaxValues[ CHANNEL_ALPHA ] = 0xff;
    }
    else
    {
        destChannelMaxValues[ CHANNEL_RED ] = ( 1U << pDestChannelBitCounts[ CHANNEL_RED ] ) - 1;
        destChannelMaxValues[ CHANNEL_GREEN ] = ( 1U << pDestChannelBitCounts[ CHANNEL_GREEN ] ) - 1;
        destChannelMaxValues[ CHANNEL_BLUE ] = ( 1U << pDestChannelBitCounts[ CHANNEL_BLUE ] ) - 1;
        destChannelMaxValues[ CHANNEL_ALPHA ] = ( 1U << pDestChannelBitCounts[ CHANNEL_ALPHA ] ) - 1;
    }

    uint32_t channelAdjustments[ CHANNEL_MAX ] = { 0, 0, 0, 0 };

    if( !sourceChannelMaxValues[ CHANNEL_RED ] )
    {
        sourceChannelMaxValues[ CHANNEL_RED ] = 1;
        channelAdjustments[ CHANNEL_RED ] = destChannelMaxValues[ CHANNEL_RED ];
    }

    if( !sourceChannelMaxValues[ CHANNEL_GREEN ] )
    {
        sourceChannelMaxValues[ CHANNEL_GREEN ] = 1;
        channelAdjustments[ CHANNEL_GREEN ] = destChannelMaxValues[ CHANNEL_GREEN ];
    }

    if( !sourceChannelMaxValues[ CHANNEL_BLUE ] )
    {
        sourceChannelMaxValues[ CHANNEL_BLUE ] = 1;
        channelAdjustments[ CHANNEL_BLUE ] = destChannelMaxValues[ CHANNEL_BLUE ];
    }

    if( !sourceChannelMaxValues[ CHANNEL_ALPHA ] )
    {
        sourceChannelMaxValues[ CHANNEL_ALPHA ] = 1;
        channelAdjustments[ CHANNEL_ALPHA ] = destChannelMaxValues[ CHANNEL_ALPHA ];
    }

    if( pSourcePalette )
    {
        PalettizedColorReader colorReader( pSourcePalette, sourcePaletteSize );

        if( pDestPalette )
        {
            PalettizedColorWriter colorWriter( pDestPalette, destPaletteSize );

            ConvertImageSourcePixelSizeSwitch(
                colorReader,
                colorWriter,
                m_pPixelData,
                sourceBytesPerPixel,
                m_pitch,
                sourceChannelMaxValues,
                stagingImage.m_pPixelData,
                destBytesPerPixel,
                stagingImage.m_pitch,
                destChannelMaxValues,
                channelAdjustments,
                m_width,
                m_height );
        }
        else
        {
            DirectColorWriter colorWriter( pDestChannelBitOffsets );

            ConvertImageSourcePixelSizeSwitch(
                colorReader,
                colorWriter,
                m_pPixelData,
                sourceBytesPerPixel,
                m_pitch,
                sourceChannelMaxValues,
                stagingImage.m_pPixelData,
                destBytesPerPixel,
                stagingImage.m_pitch,
                destChannelMaxValues,
                channelAdjustments,
                m_width,
                m_height );
        }
    }
    else
    {
        DirectColorReader colorReader( pSourceChannelBitCounts, pSourceChannelBitOffsets );

        if( pDestPalette )
        {
            PalettizedColorWriter colorWriter( pDestPalette, destPaletteSize );

            ConvertImageSourcePixelSizeSwitch(
                colorReader,
                colorWriter,
                m_pPixelData,
                sourceBytesPerPixel,
                m_pitch,
                sourceChannelMaxValues,
                stagingImage.m_pPixelData,
                destBytesPerPixel,
                stagingImage.m_pitch,
                destChannelMaxValues,
                channelAdjustments,
                m_width,
                m_height );
        }
        else
        {
            DirectColorWriter colorWriter( pDestChannelBitOffsets );

            ConvertImageSourcePixelSizeSwitch(
                colorReader,
                colorWriter,
                m_pPixelData,
                sourceBytesPerPixel,
                m_pitch,
                sourceChannelMaxValues,
                stagingImage.m_pPixelData,
                destBytesPerPixel,
                stagingImage.m_pitch,
                destChannelMaxValues,
                channelAdjustments,
                m_width,
                m_height );
        }
    }

    // Store the converted image data in the destination image.
    rDestination.Swap( stagingImage );

    return true;
}

/// Assignment operator.
///
/// @param[in] rSource  Source object from which to copy.
///
/// @return  Reference to this object.
Image& Image::operator=( const Image& rSource )
{
    if( this != &rSource )
    {
        PrivateFree();
        PrivateCopy( rSource );
    }

    return *this;
}

/// Copy data from the given object into this object, assuming this object is in an entirely uninitialized state.
///
/// @param[in] rSource  Source object from which to copy.
///
/// @see PrivateFree()
void Image::PrivateCopy( const Image& rSource )
{
    m_pPixelData = NULL;
    if( rSource.m_pPixelData )
    {
        size_t imageSize = rSource.m_pitch * rSource.m_height;
        m_pPixelData = new uint8_t [ imageSize ];
        HELIUM_ASSERT( m_pPixelData );
        MemoryCopy( m_pPixelData, rSource.m_pPixelData, imageSize );
    }

    m_width = rSource.m_width;
    m_height = rSource.m_height;
    m_pitch = rSource.m_pitch;

    m_format = rSource.m_format;

    const Color* pPalette = m_format.GetPalette();
    if( pPalette )
    {
        uint32_t paletteSize = m_format.GetPaletteSize();
        Color* pPaletteCopy = new Color [ paletteSize ];
        HELIUM_ASSERT( pPaletteCopy );
        ArrayCopy( pPaletteCopy, pPalette, paletteSize );
        m_format.SetPalette( pPaletteCopy, paletteSize );
    }
}

/// Free all allocated data without resetting data values.
///
/// @see PrivateCopy()
void Image::PrivateFree()
{
    delete [] static_cast< uint8_t* >( m_pPixelData );

    // The internally stored palette is always allocated by this object, so it is safe for us to const_cast and
    // delete the palette memory here.
    delete [] const_cast< Color* >( m_format.GetPalette() );
}

/// Validate the parameters in this image format.
///
/// Format validation performs the following checks:
/// - The number of bytes per pixel is between 1 and 4 (inclusive).
/// - The range of bits for any given color channel does not exceed the range allowed by the given pixel size.  Note
///   that individual color channel bit ranges are ignored if a palette is specified (this check will not be
///   peformed for palettized formats).
///
/// Validation does not check for the following:
/// - Whether a non-null palette array was specified if the palette size is non-zero (image initialization and
///   conversion will automatically ignore any palette data if either the palette size is zero or the palette array
///   is null).
/// - Whether color channels overlap (image support does not make any assumptions as to whether this is the intended
///   behavior of the user).
///
/// @return  True if this is a valid image format, false if not.
bool Image::Format::Validate() const
{
    if( m_bytesPerPixel < 1 || m_bytesPerPixel > 4 )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "Image::Format::Validate(): Invalid bytes per pixel (%" ) TPRIu8 TXT( ") specified.  Pixel " )
            TXT( "size must be from 1 to 4 bytes.\n" ) ),
            m_bytesPerPixel );

        return false;
    }

    if( m_pPalette && m_paletteSize )
    {
        uint8_t bitsPerPixel = m_bytesPerPixel * 8;
        for( size_t channelIndex = 0; channelIndex < CHANNEL_MAX; ++channelIndex )
        {
            uint8_t channelBitOffset = m_channelBitOffsets[ channelIndex ];
            if( channelBitOffset >= bitsPerPixel )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "Image::Format::Validate(): Color channel %" ) TPRIuSZ TXT( " (%" ) TPRIu8 TXT( ") " )
                    TXT( "begins past the range of bits per pixel (%" ) TPRIu8 TXT( ").\n" ) ),
                    channelIndex,
                    channelBitOffset,
                    bitsPerPixel );

                return false;
            }

            uint8_t channelBitCount = m_channelBitCounts[ channelIndex ];
            uint8_t channelBitCountMax = bitsPerPixel - channelBitOffset;
            if( channelBitCount > channelBitCountMax )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "Image::Format::Validate(): Pixel bit range for color channel %" ) TPRIuSZ TXT( " (start " )
                    TXT( "offset: %" ) TPRIu8 TXT( "; bit count: %" ) TPRIu8 TXT( ") exceeds the range supported " )
                    TXT( "by the total number of bits per pixel (%" ) TPRIu8 TXT( ").\n" ) ),
                    channelIndex,
                    channelBitOffset,
                    channelBitCount,
                    bitsPerPixel );

                return false;
            }
        }
    }

    return true;
}

#endif  // HELIUM_TOOLS
