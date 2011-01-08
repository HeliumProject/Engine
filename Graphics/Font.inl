/// Get the point size of this font.
///
/// The actual pixel size of a font is determined by a combination of its point size and its display resolution (which
/// can be accessed using GetDpi()).
///
/// @return  Font size, in points.
///
/// @see GetDpi()
float32_t Lunar::Font::GetPointSize() const
{
    return m_pointSize;
}

/// Get the resolution at which this font is rendered.
///
/// The actual pixel size of a font is determined by a combination of its point size (which can be accessed using
/// GetPointSize()) and its display resolution.
///
/// @return  Display resolution, in DPI (dots per inch).
///
/// @see GetPointSize()
uint32_t Lunar::Font::GetDpi() const
{
    return m_dpi;
}

/// Get the width of each texture sheet used by this font.
///
/// @return  Texture sheet width, in texels.
///
/// @see GetTextureSheetHeight()
uint16_t Lunar::Font::GetTextureSheetWidth() const
{
    return m_textureSheetWidth;
}

/// Get the height of each texture sheet used by this font.
///
/// @return  Texture sheet height, in texels.
///
/// @see GetTextureSheetWidth()
uint16_t Lunar::Font::GetTextureSheetHeight() const
{
    return m_textureSheetHeight;
}

/// Get the compression scheme to use for texture sheets for this font.
///
/// @return  Texture compression scheme.
Lunar::Font::ECompression Lunar::Font::GetTextureCompression() const
{
    return m_textureCompression;
}

/// Get whether this font should be rendered with antialiased edges.
///
/// @return  True if antialiasing should be used to smooth font edges, false if not.
bool Lunar::Font::GetAntialiased() const
{
    return m_bAntialiased;
}

/// Get the maximum ascender height of this font in pixels, as a 26.6 fixed-point value.
///
/// @return  Maximum ascender height from the baseline, in pixels.
///
/// @see GetAscenderFloat(), GetDescenderFixed(), GetHeightFixed(), GetMaxAdvanceFixed()
int32_t Lunar::Font::GetAscenderFixed() const
{
    return m_ascender;
}

/// Get the maximum descender depth of this font in pixels, as a 26.6 fixed-point value.
///
/// @return  Maximum descender depth from the baseline, in pixels.
///
/// @see GetDescenderFloat(), GetAscenderFixed(), GetHeightFixed(), GetMaxAdvanceFixed()
int32_t Lunar::Font::GetDescenderFixed() const
{
    return m_descender;
}

/// Get the maximum height of characters in this font in pixels, as a 26.6 fixed-point value.
///
/// @return  Maximum character height, in pixels.
///
/// @see GetHeightFloat(), GetAscenderFixed(), GetDescenderFixed(), GetMaxAdvanceFixed()
int32_t Lunar::Font::GetHeightFixed() const
{
    return m_height;
}

/// Get the maximum amount by which to advance the pen when rendering characters in pixels, as a 26.6 fixed-point value.
///
/// @return  Maximum character advance step, in pixels.
///
/// @see GetMaxAdvanceFloat(), GetAscenderFixed(), GetDescenderFixed(), GetHeightFixed()
int32_t Lunar::Font::GetMaxAdvanceFixed() const
{
    return m_maxAdvance;
}

/// Get the maximum ascender height of this font in pixels, as a floating-point value.
///
/// @return  Maximum ascender height from the baseline, in pixels.
///
/// @see GetAscenderFixed(), GetDescenderFloat(), GetHeightFloat(), GetMaxAdvanceFloat()
float32_t Lunar::Font::GetAscenderFloat() const
{
    return Fixed26x6ToFloat32( m_ascender );
}

/// Get the maximum descender depth of this font in pixels, as a floating-point value.
///
/// @return  Maximum descender depth from the baseline, in pixels.
///
/// @see GetDescenderFixed(), GetAscenderFloat(), GetHeightFloat(), GetMaxAdvanceFloat()
float32_t Lunar::Font::GetDescenderFloat() const
{
    return Fixed26x6ToFloat32( m_descender );
}

/// Get the maximum height of characters in this font in pixels, as a floating-point value.
///
/// @return  Maximum character height, in pixels.
///
/// @see GetHeightFixed(), GetAscenderFloat(), GetDescenderFloat(), GetMaxAdvanceFloat()
float32_t Lunar::Font::GetHeightFloat() const
{
    return Fixed26x6ToFloat32( m_height );
}

/// Get the maximum amount by which to advance the pen when rendering characters in pixels, as a floating-point value.
///
/// @return  Maximum character advance step, in pixels.
///
/// @see GetMaxAdvanceFixed(), GetAscenderFloat(), GetDescenderFloat(), GetHeightFloat()
float32_t Lunar::Font::GetMaxAdvanceFloat() const
{
    return Fixed26x6ToFloat32( m_maxAdvance );
}

/// Get the number of characters in this font.
///
/// @return  Character count.
///
/// @see GetCharacter(), GetCharacterIndex(), FindCharacter()
uint32_t Lunar::Font::GetCharacterCount() const
{
    return m_characterCount;
}

/// Get the data for the character associated with the specified index.
///
/// To get the data for a character based on its Unicode code point value, use FindCharacter()
///
/// @param[in] index  Character index (must be a valid character index).
///
/// @return  Reference to the information associated with the character at the given index.
///
/// @see GetCharacterCount(), GetCharacterIndex(), FindCharacter()
const Lunar::Font::Character& Lunar::Font::GetCharacter( uint32_t index ) const
{
    HELIUM_ASSERT( index < m_characterCount );

    return m_pCharacters[ index ];
}

/// Get the index associated with the specified character data.
///
/// @param[in] pCharacter  Pointer to the character data (must point to valid character data stored in this font).
///
/// @return  Index associated with the given character data.
uint32_t Lunar::Font::GetCharacterIndex( const Character* pCharacter ) const
{
    HELIUM_ASSERT( pCharacter >= m_pCharacters );
    HELIUM_ASSERT( pCharacter < m_pCharacters + m_characterCount );

    return static_cast< uint32_t >( pCharacter - m_pCharacters );
}

/// Find the character data for the given Unicode character code point.
///
/// This will perform a binary search for the character with the given code point.
///
/// @param[in] codePoint  Unicode code point value.
///
/// @return  Pointer to the code point if located successfully, null if no character exists in this font with the given
///          code point.
///
/// @see GetCharacterCount(), GetCharacter(), GetCharacterIndex()
const Lunar::Font::Character* Lunar::Font::FindCharacter( uint32_t codePoint ) const
{
    uint32_t baseIndex = 0;
    uint32_t searchCount = m_characterCount;
    while( searchCount != 0 )
    {
        uint32_t testOffset = searchCount / 2;
        uint32_t testIndex = baseIndex + testOffset;

        const Character& rCharacter = m_pCharacters[ testIndex ];
        uint32_t testCodePoint = rCharacter.codePoint;
        if( codePoint < testCodePoint )
        {
            searchCount = testOffset;
        }
        else if( codePoint > testCodePoint )
        {
            baseIndex = testIndex + 1;
            searchCount -= testOffset + 1;
        }
        else
        {
            return &rCharacter;
        }
    }

    return NULL;
}

/// Get the number of texture sheets in this font.
///
/// @return  Texture sheet count.
///
/// @see GetTextureSheet()
uint8_t Lunar::Font::GetTextureSheetCount() const
{
    return m_textureCount;
}

/// Retrieve the resource for the texture sheet associated with the specified index.
///
/// @param[in] index  Texture sheet index.
///
/// @return  Pointer to the texture sheet resource associated with the given index.
///
/// @see GetTextureSheetCount()
Lunar::RTexture2d* Lunar::Font::GetTextureSheet( uint8_t index ) const
{
    HELIUM_ASSERT( index < m_textureCount );
    HELIUM_ASSERT( m_pspTextures );

    return m_pspTextures[ index ];
}

/// Parse a string and pass valid character information to a custom handler.
///
/// @param[in] pString         String to process.
/// @param[in] characterCount  Number of characters to process.
/// @param[in] rGlyphHandler   Handler for processing characters (i.e. for display, length counting, etc.).
template< typename GlyphHandler, typename CharType >
void Lunar::Font::ProcessText( const CharType* pString, size_t characterCount, GlyphHandler& rGlyphHandler ) const
{
    HELIUM_ASSERT( pString || characterCount == 0 );

    // Convert the text to wide characters if necessary.
    ProcessTextConverter< CharType > stringConverter( pString, characterCount );
    const wchar_t* pWideString = stringConverter.GetString();
    characterCount = stringConverter.GetLength();
    HELIUM_ASSERT( pWideString || characterCount == 0 );

    // Process each individual code point encountered in the given string (remember to check for surrogate pairs for
    // UTF-16 text).
    bool bHaveSurrogate = false;
    uint32_t highSurrogate = 0;

    for( size_t characterIndex = 0; characterIndex < characterCount; ++characterIndex )
    {
        uint32_t character = pWideString[ characterIndex ];
        if( !bHaveSurrogate )
        {
            if( character >= 0xd800 && character < 0xe000 )
            {
                if( character < 0xdc00 )
                {
                    bHaveSurrogate = true;
                    highSurrogate = character;
                }

                continue;
            }
        }
        else
        {
            bHaveSurrogate = false;

            if( character < 0xdc00 || character >= 0xe000 )
            {
                continue;
            }

            character = ( ( ( highSurrogate - 0xd800 ) << 10 ) | ( character - 0xdc00 ) ) + 0x10000;
        }

        const Font::Character* pCharacter = FindCharacter( character );
        if( pCharacter )
        {
            rGlyphHandler( pCharacter );
        }
    }
}

/// Parse a string and pass valid character information to a custom handler.
///
/// @param[in] pString        Null-terminated string to process.
/// @param[in] rGlyphHandler  Handler for processing characters (i.e. for display, length counting, etc.).
template< typename GlyphHandler, typename CharType >
void Lunar::Font::ProcessText( const CharType* pString, GlyphHandler& rGlyphHandler ) const
{
    ProcessText( pString, StringLength( pString ), rGlyphHandler );
}

/// Parse a string and pass valid character information to a custom handler.
///
/// @param[in] rString        String to process.
/// @param[in] rGlyphHandler  Handler for processing characters (i.e. for display, length counting, etc.).
template< typename GlyphHandler, typename CharType, typename Allocator >
void Lunar::Font::ProcessText(
    const Helium::StringBase< CharType, Allocator >& rString,
    GlyphHandler& rGlyphHandler ) const
{
    ProcessText( rString.GetData(), rString.GetSize(), rGlyphHandler );
}

/// Convert a 26.6 fixed-point value to a 32-bit floating-point value.
///
/// @param[in] value  26.6 fixed-point value.
///
/// @return  32-bit floating-point value.
///
/// @see Float32ToFixed26x6()
float32_t Lunar::Font::Fixed26x6ToFloat32( int32_t value )
{
    float32_t result =
        static_cast< float32_t >( value >> 6 ) +
        static_cast< float32_t >( value & ( ( 1 << 6 ) - 1 ) ) * ( 1.0f / static_cast< float32_t >( 1 << 6 ) );

    return result;
}

/// Convert a 32-bit floating-point value to a 26.6 fixed-point value.
///
/// @param[in] value  32-bit floating-point value.
///
/// @return  26.6 fixed-point value.
///
/// @see Fixed26x6ToFloat32()
int32_t Lunar::Font::Float32ToFixed26x6( float32_t value )
{
    float32_t integer = 0.0f;
    float32_t fraction = Modf( value, integer );

    int32_t result =
        ( static_cast< int32_t >( integer ) << 6 ) +
        static_cast< int32_t >( floorf( fraction * static_cast< float32_t >( 1 << 6 ) + 0.5f ) );

    return result;
}

/// Constructor.
///
/// This will convert the given multi-byte character string to a wide-character string.
///
/// @param[in] pString  String to convert to a wide-character string.
/// @param[in] length   String length.
Lunar::Font::ProcessTextConverter< char >::ProcessTextConverter( const char* pString, size_t /*length*/ )
{
    m_length = Helium::StringConverter< char, wchar_t >::Convert( m_string, STRING_LENGTH_MAX, pString );
    if( IsInvalid( m_length ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "Font::ProcessTextConverter: Failed to convert multi-byte character string to a wide-character " )
              TXT( "string.\n" ) ) );

        m_string[ 0 ] = L'\0';
        m_length = 0;
    }
}

/// Get the converted string.
///
/// @return  Converted string.
///
/// @see GetLength()
const wchar_t* Lunar::Font::ProcessTextConverter< char >::GetString() const
{
    return m_string;
}

/// Get the length of the converted string.
///
/// @return  Cached string length.
///
/// @see GetString()
size_t Lunar::Font::ProcessTextConverter< char >::GetLength() const
{
    return m_length;
}

/// Constructor.
///
/// This will cache the given wide-character string, as no conversion is necessary.
///
/// @param[in] pString  String to cache.
/// @param[in] length   String length.
Lunar::Font::ProcessTextConverter< wchar_t >::ProcessTextConverter( const wchar_t* pString, size_t length )
    : m_pString( pString )
    , m_length( length )
{
}

/// Get the converted string.
///
/// @return  Converted string.
///
/// @see GetLength()
const wchar_t* Lunar::Font::ProcessTextConverter< wchar_t >::GetString() const
{
    return m_pString;
}

/// Get the length of the converted string.
///
/// @return  Cached string length.
///
/// @see GetString()
size_t Lunar::Font::ProcessTextConverter< wchar_t >::GetLength() const
{
    return m_length;
}
