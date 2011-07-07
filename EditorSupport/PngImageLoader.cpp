//----------------------------------------------------------------------------------------------------------------------
// PngImageLoader.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EditorSupportPch.h"

#if HELIUM_EDITOR

#include "EditorSupport/PngImageLoader.h"

#include "Foundation/Stream/Stream.h"
#include "Foundation/StringConverter.h"
#include "Rendering/Color.h"
#include "EditorSupport/Image.h"

#define PNG_USER_MEM_SUPPORTED
#include <png.h>

using namespace Helium;

/// libpng memory allocation callback.
///
/// @param[in] pPng  PNG interface struct.
/// @param[in] size  Allocation size.
///
/// @return  Pointer to the allocated memory block if allocation was successful, null if allocation failed.
///
/// @see PngFree()
static png_voidp PngMalloc( png_structp /*pPng*/, png_alloc_size_t size )
{
    // setjmp()/longjmp() on certain platform configurations (i.e. Windows on x86-64) saves and restores SIMD
    // register states, so ensure SIMD alignment for allocations so libpng allocates its jmp_buf with the proper
    // alignment requirements.
    void* pMemory = DefaultAllocator().AllocateAligned( HELIUM_SIMD_ALIGNMENT, size );

    return pMemory;
}

/// libpng memory release callback.
///
/// @param[in] pPng     PNG interface struct.
/// @param[in] pMemory  Pointer to the block of memory to free.
///
/// @see PngMalloc()
static void PngFree( png_structp /*pPng*/, png_voidp pMemory )
{
    DefaultAllocator().Free( pMemory );
}

/// libpng error callback.
///
/// @param[in] pPng      PNG interface struct.
/// @param[in] pMessage  Error message.
///
/// @see PngWarning()
static void PNGAPI PngError( png_structp /*pPng*/, png_const_charp pMessage )
{
    HELIUM_UNREF( pMessage );

#if HELIUM_ENABLE_TRACE

    HELIUM_ASSERT( pMessage );

#if HELIUM_UNICODE
    String convertedMessage;
    StringConverter< char, tchar_t >::Convert( convertedMessage, pMessage );
    const tchar_t* pConvertedMessage = *convertedMessage;
#else
    const tchar_t* pConvertedMessage = pMessage;
#endif

    HELIUM_TRACE( TRACE_ERROR, TXT( "libpng error: %s\n" ), pConvertedMessage );

#endif  // HELIUM_ENABLE_TRACE
}

/// libpng warning callback.
///
/// @param[in] pPng      PNG interface struct.
/// @param[in] pMessage  Warning message.
///
/// @see PngError()
static void PNGAPI PngWarning( png_structp /*pPng*/, png_const_charp pMessage )
{
    HELIUM_ASSERT( pMessage );

#if HELIUM_UNICODE
    String convertedMessage;
    StringConverter< char, tchar_t >::Convert( convertedMessage, pMessage );
    const tchar_t* pConvertedMessage = *convertedMessage;
#else
    const tchar_t* pConvertedMessage = pMessage;
#endif

    HELIUM_TRACE( TRACE_WARNING, TXT( "libpng warning: %s\n" ), pConvertedMessage );
}

/// libpng read callback.
///
/// @param[in] pPng   PNG interface struct.
/// @param[in] pData  Buffer in which to read the data.
/// @param[in] size   Number of bytes to read.
static void PNGAPI PngReadData( png_structp pPng, png_bytep pData, png_size_t size )
{
    HELIUM_ASSERT( pPng );
    HELIUM_ASSERT( pData );

    // The input stream is stored as the IO pointer in the PNG struct.
    Stream* pStream = static_cast< Stream* >( png_get_io_ptr( pPng ) );
    HELIUM_ASSERT( pStream );

    size_t bytesRead = pStream->Read( pData, 1, size );
    if( bytesRead != size )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "Attempted to read %" ) TPRIuSZ TXT( " bytes from PNG image, but only %" ) TPRIuSZ
            TXT( " bytes could be read.\n" ) ),
            size,
            bytesRead );
        png_error( pPng, "Read Error" );
    }
}

/// Load a PNG image from a file.
///
/// @param[out] rImage         Loaded image data if loading was successful.
/// @param[in]  pSourceStream  Stream through which to load the image.  Note that this function does not buffer
///                            reads, so it is recommended to use a BufferedStream when loading from a file.
///
/// @return  True if loading was successful, false if not.
bool PngImageLoader::Load( Image& rImage, Stream* pSourceStream )
{
    HELIUM_ASSERT( pSourceStream );

    Image::InitParameters imageParameters;

    // Read the first 8 bytes from the source stream to verify that we are dealing with a PNG file.
    uint8_t header[ 8 ];
    size_t bytesRead = pSourceStream->Read( header, 1, sizeof( header ) );
    if( bytesRead != sizeof( header ) || png_sig_cmp( header, 0, bytesRead ) != 0 )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "PngImageLoader::Load(): Source stream does not contain a valid PNG image.\n" ) );

        return false;
    }

    // Initialize PNG reader and info structs.
    png_structp pPng = png_create_read_struct_2(
        PNG_LIBPNG_VER_STRING,
        NULL,
        PngError,
        PngWarning,
        NULL,
        PngMalloc,
        PngFree );
    if( !pPng )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "PngImageLoader::Load(): Failed to create PNG reader struct.\n" ) );

        return false;
    }

    png_infop pPngInfo = png_create_info_struct( pPng );
    if( !pPngInfo )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "PngImageLoader::Load(): Failed to create PNG info struct.\n" ) );

        png_destroy_read_struct( &pPng, NULL, NULL );

        return false;
    }

    // Handle longjmp() calls that may be made by libpng later on if an error occurs.
#if HELIUM_CC_CL
    // XXX TMC: The only C++ object we create locally is the Image::InitParameters struct, which has a non-trivial
    // constructor, but needs only a trivial destructor (no data is cleared or memory freed on destruction).  Either way, we
    // declare it at the top of the function just to be safe.
#pragma warning( push )
#pragma warning( disable : 4611 )  // interaction between '_setjmp' and C++ object destruction is non-portable
#endif
    if( setjmp( png_jmpbuf( pPng ) ) )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "PngImageLoader::Load(): Error occurred while reading PNG image data.\n" ) );

        png_destroy_read_struct( &pPng, &pPngInfo, NULL );

        return false;
    }
#if HELIUM_CC_CL
#pragma warning( pop )
#endif

    // Read the PNG image header.
    png_set_read_fn( pPng, pSourceStream, PngReadData );
    png_set_sig_bytes( pPng, static_cast< int >( bytesRead ) );
    png_set_keep_unknown_chunks( pPng, 1, NULL, 0 );

    png_read_info( pPng, pPngInfo );

    // Expand color-mappend and grayscale to RGB or RGBA data automatically to make things easier.
    png_set_expand( pPng );

    // Always convert 16-bit image data to 8-bit, as Image currently does not support more than 8 bits per channel.
    png_set_strip_16( pPng );

    // Get the number of passes for interlaced image handling (must be done before we call png_read_update_info()).
    int passCount = png_set_interlace_handling( pPng );

    // Update the PNG info based on the transformations we will be performing.
    png_read_update_info( pPng, pPngInfo );

    // Set up the target image object based on the dimensions and format of the PNG image.
    png_uint_32 width = png_get_image_width( pPng, pPngInfo );
    png_uint_32 height = png_get_image_height( pPng, pPngInfo );
    size_t pitch = png_get_rowbytes( pPng, pPngInfo );

    png_byte bitDepth = png_get_bit_depth( pPng, pPngInfo );
    HELIUM_ASSERT( bitDepth % 8 == 0 );
    HELIUM_ASSERT( bitDepth == 8 );

    png_byte channels = png_get_channels( pPng, pPngInfo );
    HELIUM_ASSERT( channels >= 1 );
    HELIUM_ASSERT( channels <= 4 );

    uint8_t bytesPerPixel = bitDepth * channels / 8;

    imageParameters.width = width;
    imageParameters.height = height;
    imageParameters.pitch = static_cast< uint32_t >( pitch );
    imageParameters.format.SetBytesPerPixel( bytesPerPixel );

    switch( bytesPerPixel )
    {
        // Grayscale.
    case 1:
        {
            imageParameters.format.SetChannelBitCount( Image::CHANNEL_RED, 8 );
            imageParameters.format.SetChannelBitOffset( Image::CHANNEL_RED, 0 );

            break;
        }

        // Grayscale with alpha.
    case 2:
        {
            imageParameters.format.SetChannelBitCount( Image::CHANNEL_RED, 8 );
            imageParameters.format.SetChannelBitCount( Image::CHANNEL_ALPHA, 8 );
#if HELIUM_ENDIAN_LITTLE
            imageParameters.format.SetChannelBitOffset( Image::CHANNEL_RED, 0 );
            imageParameters.format.SetChannelBitOffset( Image::CHANNEL_ALPHA, 8 );
#else
            imageParameters.format.SetChannelBitOffset( Image::CHANNEL_RED, 8 );
            imageParameters.format.SetChannelBitOffset( Image::CHANNEL_ALPHA, 0 );
#endif

            break;
        }

        // RGB.
    case 3:
        {
            imageParameters.format.SetChannelBitCount( Image::CHANNEL_RED, 8 );
            imageParameters.format.SetChannelBitCount( Image::CHANNEL_GREEN, 8 );
            imageParameters.format.SetChannelBitCount( Image::CHANNEL_BLUE, 8 );
#if HELIUM_ENDIAN_LITTLE
            imageParameters.format.SetChannelBitOffset( Image::CHANNEL_RED, 0 );
            imageParameters.format.SetChannelBitOffset( Image::CHANNEL_GREEN, 8 );
            imageParameters.format.SetChannelBitOffset( Image::CHANNEL_BLUE, 16 );
#else
            imageParameters.format.SetChannelBitOffset( Image::CHANNEL_RED, 16 );
            imageParameters.format.SetChannelBitOffset( Image::CHANNEL_GREEN, 8 );
            imageParameters.format.SetChannelBitOffset( Image::CHANNEL_BLUE, 0 );
#endif

            break;
        }

        // RGBA
    default:
        {
            HELIUM_ASSERT( bytesPerPixel == 4 );

            imageParameters.format.SetChannelBitCount( Image::CHANNEL_RED, 8 );
            imageParameters.format.SetChannelBitCount( Image::CHANNEL_GREEN, 8 );
            imageParameters.format.SetChannelBitCount( Image::CHANNEL_BLUE, 8 );
            imageParameters.format.SetChannelBitCount( Image::CHANNEL_ALPHA, 8 );
#if HELIUM_ENDIAN_LITTLE
            imageParameters.format.SetChannelBitOffset( Image::CHANNEL_RED, 0 );
            imageParameters.format.SetChannelBitOffset( Image::CHANNEL_GREEN, 8 );
            imageParameters.format.SetChannelBitOffset( Image::CHANNEL_BLUE, 16 );
            imageParameters.format.SetChannelBitOffset( Image::CHANNEL_ALPHA, 24 );
#else
            imageParameters.format.SetChannelBitOffset( Image::CHANNEL_RED, 24 );
            imageParameters.format.SetChannelBitOffset( Image::CHANNEL_GREEN, 16 );
            imageParameters.format.SetChannelBitOffset( Image::CHANNEL_BLUE, 8 );
            imageParameters.format.SetChannelBitOffset( Image::CHANNEL_ALPHA, 0 );
#endif

            break;
        }
    }

    if( !rImage.Initialize( imageParameters ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "PngImageLoader::Load(): Failed to initialize target image object based on source image " )
            TXT( "parameters.\n" ) ) );

        png_destroy_read_struct( &pPng, &pPngInfo, NULL );

        return false;
    }

    // Read the image data.
    void* pDestImageData = rImage.GetPixelData();
    HELIUM_ASSERT( pDestImageData );

    for( int passIndex = 0; passIndex < passCount; ++passIndex )
    {
        uint8_t* pRow = static_cast< uint8_t* >( pDestImageData );

        for( uint32_t rowIndex = 0; rowIndex < height; ++rowIndex )
        {
            png_read_row( pPng, pRow, NULL );
            pRow += pitch;
        }
    }

    png_read_end( pPng, NULL );
    png_destroy_read_struct( &pPng, &pPngInfo, NULL );

    return true;
}

#endif  // HELIUM_EDITOR
