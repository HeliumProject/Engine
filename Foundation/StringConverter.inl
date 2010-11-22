/// Convert a single-byte character string to a wide character string.
///
/// @param[out] pDestString     Destination string buffer.
/// @param[in]  destBufferSize  Size of the destination string buffer in characters.
/// @param[in]  pSourceString   String to convert.
///
/// @return  If the destination string buffer is not null, the number of wide characters written to the destination
///          buffer (not including a null terminator) if no errors were encountered, otherwise Invalid< size_t >()
///          if either an invalid multibyte character was encountered in the source string or a character was
///          encountered that could not be converted properly.  If the destination buffer is null, this will only
///          validate the conversion and return either the number of wide characters needed to convert and store the
///          string in the destination buffer (not including a null terminator) or Invalid< size_t >() if an invalid
///          or incompatible character was encountered.  A null terminator will always be written to the destination
///          buffer if one is provided and the size is not zero.
size_t Helium::StringConverter< char, wchar_t >::Convert(
    wchar_t* pDestString,
    size_t destBufferSize,
    const char* pSourceString )
{
    HELIUM_ASSERT( pSourceString );

#if HELIUM_CC_MSC
    size_t charactersConverted = 0;
    errno_t result = mbstowcs_s(
        &charactersConverted,
        pDestString,
        ( pDestString ? destBufferSize : 0 ),
        pSourceString,
        _TRUNCATE );
    if( result != 0 )
    {
        return Invalid< size_t >();
    }

    // Subtract the null terminator count.
    if( charactersConverted != 0 )
    {
        --charactersConverted;
    }

    return charactersConverted;
#else
    size_t charactersConverted = mbstowcs( pDestString, pSourceString, destBufferSize );
    if( pDestString && destBufferSize != 0 && charactersConverted == destBufferSize )
    {
        pDestString[ destBufferSize - 1 ] = L'\0';
        --charactersConverted;
    }

    return charactersConverted;
#endif
}

/// Convert a single-byte character string to a wide character string.
///
/// @param[out] pDestString     Destination string buffer.
/// @param[in]  destBufferSize  Size of the destination string buffer in characters.
/// @param[in]  rSourceString   String to convert.
///
/// @return  If the destination string buffer is not null, the number of wide characters written to the destination
///          buffer (not including a null terminator) if no errors were encountered, otherwise Invalid< size_t >()
///          if either an invalid multibyte character was encountered in the source string or a character was
///          encountered that could not be converted properly.  If the destination buffer is null, this will only
///          validate the conversion and return either the number of wide characters needed to convert and store the
///          string in the destination buffer (not including a null terminator) or Invalid< size_t >() if an invalid
///          or incompatible character was encountered.  A null terminator will always be written to the destination
///          buffer if one is provided and the size is not zero.
template< typename SourceAllocator >
size_t Helium::StringConverter< char, wchar_t >::Convert(
    wchar_t* pDestString,
    size_t destBufferSize,
    const StringBase< char, SourceAllocator >& rSourceString )
{
    return Convert( pDestString, destBufferSize, *rSourceString );
}

/// Convert a single-byte character string to a wide character string.
///
/// @param[out] rDestString    Destination string buffer.
/// @param[in]  pSourceString  String to convert.
///
/// @return  True if the conversion was successful, false if not.
template< typename DestAllocator >
bool Helium::StringConverter< char, wchar_t >::Convert(
    StringBase< wchar_t, DestAllocator >& rDestString,
    const char* pSourceString )
{
    size_t destStringSize = Convert( NULL, 0, pSourceString );
    if( IsInvalid( destStringSize ) )
    {
        return false;
    }

    size_t currentStringSize = rDestString.GetSize();
    if( destStringSize == 0 )
    {
        rDestString.Remove( 0, currentStringSize );

        return true;
    }

    rDestString.Reserve( destStringSize );
    if( currentStringSize < destStringSize )
    {
        rDestString.Add( L' ', destStringSize - currentStringSize );
    }
    else
    {
        rDestString.Remove( destStringSize, currentStringSize - destStringSize );
    }

    Convert( &rDestString[ 0 ], destStringSize + 1, pSourceString );

    return true;
}

/// Convert a single-byte character string to a wide character string.
///
/// @param[out] rDestString    Destination string buffer.
/// @param[in]  rSourceString  String to convert.
///
/// @return  True if the conversion was successful, false if not.
template< typename DestAllocator, typename SourceAllocator >
bool Helium::StringConverter< char, wchar_t >::Convert(
    StringBase< wchar_t, DestAllocator >& rDestString,
    const StringBase< char, SourceAllocator >& rSourceString )
{
    return Convert( rDestString, *rSourceString );
}

/// Convert a wide character string to a single-byte character string.
///
/// @param[out] pDestString     Destination string buffer.
/// @param[in]  destBufferSize  Size of the destination string buffer in characters.
/// @param[in]  pSourceString   String to convert.
///
/// @return  If the destination string buffer is not null, the number of single-byte characters written to the
///          destination buffer (not including a null terminator) if no errors were encountered, otherwise
///          Invalid< size_t >() if either an invalid wide character was encountered in the source string or a
///          character was encountered that could not be converted properly.  If the destination buffer is null,
///          this will only validate the conversion and return either the number of single-byte characters needed to
///          convert and store the string in the destination buffer (not including a null terminator) or
///          Invalid< size_t >() if an invalid or incompatible character was encountered.  A null terminator will
///          always be written to the destination buffer if one is provided and the size is not zero.
size_t Helium::StringConverter< wchar_t, char >::Convert(
    char* pDestString,
    size_t destBufferSize,
    const wchar_t* pSourceString )
{
    HELIUM_ASSERT( pSourceString );

#if HELIUM_CC_MSC
    size_t charactersConverted = 0;
    errno_t result = wcstombs_s(
        &charactersConverted,
        pDestString,
        ( pDestString ? destBufferSize : 0 ),
        pSourceString,
        _TRUNCATE );
    if( result != 0 )
    {
        return Invalid< size_t >();
    }

    // Subtract the null terminator count.
    if( charactersConverted != 0 )
    {
        --charactersConverted;
    }

    return charactersConverted;
#else
    size_t charactersConverted = wcstombs( pDestString, pSourceString, destBufferSize );
    if( pDestString && destBufferSize != 0 && charactersConverted == destBufferSize )
    {
        pDestString[ destBufferSize - 1 ] = '\0';
        --charactersConverted;
    }

    return charactersConverted;
#endif
}

/// Convert a wide character string to a single-byte character string.
///
/// @param[out] pDestString     Destination string buffer.
/// @param[in]  destBufferSize  Size of the destination string buffer in characters.
/// @param[in]  rSourceString   String to convert.
///
/// @return  If the destination string buffer is not null, the number of single-byte characters written to the
///          destination buffer (not including a null terminator) if no errors were encountered, otherwise
///          Invalid< size_t >() if either an invalid wide character was encountered in the source string or a
///          character was encountered that could not be converted properly.  If the destination buffer is null,
///          this will only validate the conversion and return either the number of single-byte characters needed to
///          convert and store the string in the destination buffer (not including a null terminator) or
///          Invalid< size_t >() if an invalid or incompatible character was encountered.  A null terminator will
///          always be written to the destination buffer if one is provided and the size is not zero.
template< typename SourceAllocator >
size_t Helium::StringConverter< wchar_t, char >::Convert(
    char* pDestString,
    size_t destBufferSize,
    const StringBase< wchar_t, SourceAllocator >& rSourceString )
{
    return Convert( pDestString, destBufferSize, *rSourceString );
}

/// Convert a single-byte character string to a wide character string.
///
/// @param[out] rDestString    Destination string buffer.
/// @param[in]  pSourceString  String to convert.
///
/// @return  True if the conversion was successful, false if not.
template< typename DestAllocator >
bool Helium::StringConverter< wchar_t, char >::Convert(
    StringBase< char, DestAllocator >& rDestString,
    const wchar_t* pSourceString )
{
    size_t destStringSize = Convert( NULL, 0, pSourceString );
    if( IsInvalid( destStringSize ) )
    {
        return false;
    }

    size_t currentStringSize = rDestString.GetSize();
    if( destStringSize == 0 )
    {
        rDestString.Remove( 0, currentStringSize );

        return true;
    }

    rDestString.Reserve( destStringSize );
    if( currentStringSize < destStringSize )
    {
        rDestString.Add( ' ', destStringSize - currentStringSize );
    }
    else
    {
        rDestString.Remove( destStringSize, currentStringSize - destStringSize );
    }

    Convert( &rDestString[ 0 ], destStringSize + 1, pSourceString );

    return true;
}

/// Convert a single-byte character string to a wide character string.
///
/// @param[out] rDestString    Destination string buffer.
/// @param[in]  rSourceString  String to convert.
///
/// @return  True if the conversion was successful, false if not.
template< typename DestAllocator, typename SourceAllocator >
bool Helium::StringConverter< wchar_t, char >::Convert(
    StringBase< char, DestAllocator >& rDestString,
    const StringBase< wchar_t, SourceAllocator >& rSourceString )
{
    return Convert( rDestString, *rSourceString );
}

/// Null string conversion.
///
/// @param[out] pDestString     Destination string buffer.
/// @param[in]  destBufferSize  Size of the destination string buffer in characters.
/// @param[in]  pSourceString   String to convert.
///
/// @return  If the destination string buffer is not null, the number of characters copied to the destination buffer
///          (not including a null terminator).  If the destination buffer is null, this will only return the number
///          of characters needed to copy the string in the destination buffer (not including a null terminator).
///          A null terminator will always be written to the destination buffer if one is provided and the size is
///          not zero.
template< typename CharType >
size_t Helium::NullStringConverter< CharType >::Convert(
    CharType* pDestString,
    size_t destBufferSize,
    const CharType* pSourceString )
{
    HELIUM_ASSERT( pSourceString );

    size_t sourceLength = StringLength( pSourceString );
    if( !pDestString )
    {
        return sourceLength;
    }

    if( destBufferSize == 0 )
    {
        return 0;
    }

    size_t copyCount = Min( destBufferSize - 1, sourceLength );
    ArrayCopy( pDestString, pSourceString, copyCount );
    pDestString[ copyCount ] = static_cast< CharType >( '\0' );

    return copyCount;
}

/// Null string conversion.
///
/// @param[out] pDestString     Destination string buffer.
/// @param[in]  destBufferSize  Size of the destination string buffer in characters.
/// @param[in]  rSourceString   String to convert.
///
/// @return  If the destination string buffer is not null, the number of characters copied to the destination buffer
///          (not including a null terminator).  If the destination buffer is null, this will only return the number
///          of characters needed to copy the string in the destination buffer (not including a null terminator).
///          A null terminator will always be written to the destination buffer if one is provided and the size is
///          not zero.
template< typename CharType >
template< typename SourceAllocator >
size_t Helium::NullStringConverter< CharType >::Convert(
    CharType* pDestString,
    size_t destBufferSize,
    const StringBase< CharType, SourceAllocator >& rSourceString )
{
    return Convert( pDestString, destBufferSize, *rSourceString );
}

/// Null string conversion.
///
/// @param[out] rDestString    Destination string buffer.
/// @param[in]  pSourceString  String to convert.
///
/// @return  True.
template< typename CharType >
template< typename DestAllocator >
bool Helium::NullStringConverter< CharType >::Convert(
    StringBase< CharType, DestAllocator >& rDestString,
    const CharType* pSourceString )
{
    HELIUM_ASSERT( pSourceString );

    rDestString = pSourceString;

    return true;
}

/// Convert a single-byte character string to a wide character string.
///
/// @param[out] pDestString    Destination string buffer.
/// @param[in]  rSourceString  String to convert.
///
/// @return  True if the conversion was successful, false if not.
template< typename CharType >
template< typename DestAllocator, typename SourceAllocator >
bool Helium::NullStringConverter< CharType >::Convert(
    StringBase< CharType, DestAllocator >& rDestString,
    const StringBase< CharType, SourceAllocator >& rSourceString )
{
    rDestString = rSourceString;

    return true;
}
