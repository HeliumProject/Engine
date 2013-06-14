namespace Helium
{
    /// Access the raw pixel data in this image.
    ///
    /// @return  Pointer to the raw image pixel data.
    void* Image::GetPixelData()
    {
        return m_pPixelData;
    }

    /// Access the raw pixel data in this image.
    ///
    /// @return  Pointer to the raw image pixel data.
    const void* Image::GetPixelData() const
    {
        return m_pPixelData;
    }

    /// Get the width of this image.
    ///
    /// @return  Image width, in pixels.
    ///
    /// @see GetHeight(), GetPitch()
    uint32_t Image::GetWidth() const
    {
        return m_width;
    }

    /// Get the height of this image.
    ///
    /// @return  Image height, in pixels.
    ///
    /// @see GetWidth(), GetPitch()
    uint32_t Image::GetHeight() const
    {
        return m_height;
    }

    /// Get the number of bytes per row of pixels in this image.
    ///
    /// @return  Number of bytes per row of pixels.
    ///
    /// @see GetWidth(), GetHeight()
    uint32_t Image::GetPitch() const
    {
        return m_pitch;
    }

    /// Get the image format data.
    ///
    /// @return  Image format.
    const Image::Format& Image::GetFormat() const
    {
        return m_format;
    }

    /// Constructor.
    Image::Format::Format()
        : m_pPalette( NULL )
        , m_paletteSize( 0 )
        , m_bytesPerPixel( 0 )
    {
        MemoryZero( m_channelBitCounts, sizeof( m_channelBitCounts ) );
        MemoryZero( m_channelBitOffsets, sizeof( m_channelBitOffsets ) );
    }

    /// Set the number of bytes per pixel.
    ///
    /// @param[in] bytesPerPixel  Bytes per pixel.
    ///
    /// @see GetBytesPerPixel()
    void Image::Format::SetBytesPerPixel( uint8_t bytesPerPixel )
    {
        m_bytesPerPixel = bytesPerPixel;
    }

    /// Get the number of bytes per pixel.
    ///
    /// @return  Bytes per pixel.
    ///
    /// @see SetBytesPerPixel()
    uint8_t Image::Format::GetBytesPerPixel() const
    {
        return m_bytesPerPixel;
    }

    /// Set the number of bits used for a given color channel in each pixel.
    ///
    /// @param[in] channel   Color channel.
    /// @param[in] bitCount  Number of bits in the color channel.
    ///
    /// @see SetChannelBitOffset(), GetChannelBitCount(), GetChannelBitCounts(), GetChannelBitOffset(),
    ///      GetChannelBitOffsets()
    void Image::Format::SetChannelBitCount( EChannel channel, uint8_t bitCount )
    {
        HELIUM_ASSERT( static_cast< size_t >( channel ) < static_cast< size_t >( CHANNEL_MAX ) );

        m_channelBitCounts[ channel ] = bitCount;
    }

    /// Get the number of bits used for a given color channel in each pixel.
    ///
    /// @param[in] channel  Color channel.
    ///
    /// @return  Number of bits in the color channel.
    ///
    /// @see GetChannelBitCounts(), GetChannelBitOffset(), GetChannelBitOffsets(), SetChannelBitCount(),
    ///      SetChannelBitOffset()
    uint8_t Image::Format::GetChannelBitCount( EChannel channel ) const
    {
        HELIUM_ASSERT( static_cast< size_t >( channel ) < static_cast< size_t >( CHANNEL_MAX ) );

        return m_channelBitCounts[ channel ];
    }

    /// Get the array of bit counts per pixel for each color channel.
    ///
    /// @return  Bit counts for each color channel.
    ///
    /// @see GetChannelBitCount(), GetChannelBitOffsets(), GetChannelBitOffset(), SetChannelBitCount(),
    ///      SetChannelBitOffset()
    const uint8_t* Image::Format::GetChannelBitCounts() const
    {
        return m_channelBitCounts;
    }

    /// Set the bit offset of a given color channel in each pixel.
    ///
    /// @param[in] channel    Color channel.
    /// @param[in] bitOffset  Bit offset of the color channel in each pixel.
    ///
    /// @see SetChannelBitCount(), GetChannelBitOffset(), GetChannelBitOffsets(), GetChannelBitCount(),
    ///      GetChannelBitCounts()
    void Image::Format::SetChannelBitOffset( EChannel channel, uint8_t bitOffset )
    {
        HELIUM_ASSERT( static_cast< size_t >( channel ) < static_cast< size_t >( CHANNEL_MAX ) );

        m_channelBitOffsets[ channel ] = bitOffset;
    }

    /// Get the bit offset of a given color channel in each pixel.
    ///
    /// @param[in] channel  Color channel.
    ///
    /// @return  Bit offset of the color channel in each pixel.
    ///
    /// @see GetChannelBitOffsets(), GetChannelBitCount(), GetChannelBitCounts(), SetChannelBitOffset(),
    ///      SetChannelBitCount()
    uint8_t Image::Format::GetChannelBitOffset( EChannel channel ) const
    {
        HELIUM_ASSERT( static_cast< size_t >( channel ) < static_cast< size_t >( CHANNEL_MAX ) );

        return m_channelBitOffsets[ channel ];
    }

    /// Get the array of bit offsets within a pixel for each color channel.
    ///
    /// @return  Bit offsets for each color channel.
    ///
    /// @see GetChannelBitOffset(), GetChannelBitCounts(), GetChannelBitCount(), SetChannelBitOffset(),
    ///      SetChannelBitCount()
    const uint8_t* Image::Format::GetChannelBitOffsets() const
    {
        return m_channelBitOffsets;
    }

    /// Set the color palette information.
    ///
    /// @param[in] pPalette     Array of colors in the palette.  Note that this does not make a copy of the array or
    ///                         take ownership of its memory, so the array memory must remain allocated for the duration
    ///                         in which it is set as the palette in this object.
    /// @param[in] paletteSize  Number of colors in the specified palette.
    ///
    /// @see GetPalette(), GetPaletteSize()
    void Image::Format::SetPalette( const Color* pPalette, uint32_t paletteSize )
    {
        m_pPalette = pPalette;
        m_paletteSize = paletteSize;
    }

    /// Get the array of colors in the color palette.
    ///
    /// @return  Array of colors in the palette.
    ///
    /// @see GetPaletteSize(), SetPalette()
    const Color* Image::Format::GetPalette() const
    {
        return m_pPalette;
    }

    /// Get the number of colors in the color palette.
    ///
    /// @return  Color palette size.
    ///
    /// @see GetPalette(), SetPalette()
    uint32_t Image::Format::GetPaletteSize() const
    {
        return m_paletteSize;
    }

    /// Compute the bit mask for a color channel based on the specified number of bits in the channel and the offset of
    /// the channel within each pixel.
    ///
    /// @param[in] bitCount   Number of bits in the color channel.
    /// @param[in] bitOffset  Bit offset of the color channel in each pixel.
    uint32_t Image::Format::ComputeChannelMask( uint8_t bitCount, uint8_t bitOffset )
    {
        return ( ( ( 1U << bitCount ) - 1 ) << bitOffset );
    }

    /// Constructor.
    Image::InitParameters::InitParameters()
        : pPixelData( NULL )
        , width( 0 )
        , height( 0 )
        , pitch( 0 )
    {
    }
}
