/// Constructor.
///
/// This creates an empty string without allocating any memory.
template< typename CharType, typename Allocator >
Helium::StringBase< CharType, Allocator >::StringBase()
{
}

/// Constructor.
///
/// This creates a copy of a null-terminated C-style string.  The length is determined automatically based on the
/// location of the first null terminating character.
///
/// @param[in] pString  C-style string from which to copy.  This can be null.
template< typename CharType, typename Allocator >
Helium::StringBase< CharType, Allocator >::StringBase( const CharType* pString )
{
    if( pString )
    {
        size_t length = StringLength( pString );
        if( length )
        {
            m_buffer.Set( pString, length + 1 );
        }
    }
}

/// Constructor.
///
/// This creates a copy of a C-style string with an explicit length specified.
///
/// @param[in] pString  C-style string from which to copy.  This can be null as long as the size specified is zero.
/// @param[in] size     Number of character type elements in the string, not including the null terminator.
template< typename CharType, typename Allocator >
Helium::StringBase< CharType, Allocator >::StringBase( const CharType* pString, size_t size )
{
    if( size != 0 )
    {
        HELIUM_ASSERT( pString );
        m_buffer.Reserve( size + 1 );
        m_buffer.Set( pString, size );
        m_buffer.Add( static_cast< CharType >( '\0' ) );
    }
}

/// Get the size of this string.
///
/// Note that this only counts the number of character type elements in the internal string buffer up to, but not
/// including, the final null terminator element appended to the end of the string.  For strings types that may
/// represent certain characters using multiple array elements (i.e. UTF-16), the number returned might not
/// represent the actual number of characters in the string.
///
/// @return  Number of character type elements in this string.
///
/// @see GetCapacity(), Resize(), IsEmpty()
template< typename CharType, typename Allocator >
size_t Helium::StringBase< CharType, Allocator >::GetSize() const
{
    size_t bufferSize = m_buffer.GetSize();
    return( bufferSize == 0 ? 0 : bufferSize - 1 );
}

/// Get whether this string is empty.
///
/// @return  True if this string is empty, false if not.
///
/// @see GetSize()
template< typename CharType, typename Allocator >
bool Helium::StringBase< CharType, Allocator >::IsEmpty() const
{
    return( m_buffer.GetSize() <= 1 );
}

/// Resize this array, retaining any existing data that fits within the new size.
///
/// If the new size is smaller than the current size, no memory will be freed for the string buffer itself, and any
/// existing string contents will be truncated.
///
/// If the new size is larger than the current capacity, the string memory will be reallocated according to the normal
/// array growth rules.  This can be avoided by calling Reserve() to increase the capacity to an explicit value prior to
/// calling this function.
///
/// @param[in] size  New array size.
/// @param[in] fill  Character with which to fill new string elements if the new string size is larger than the current
///                  capacity.
///
/// @see GetSize()
template< typename CharType, typename Allocator >
void Helium::StringBase< CharType, Allocator >::Resize( size_t size, CharType fill )
{
    size_t oldSize = m_buffer.GetSize();
    m_buffer.Resize( size );

    if( size > oldSize )
    {
        size_t newCharacterCount = size - oldSize;
        ArraySet( m_buffer.GetData() + oldSize, fill, newCharacterCount );
    }
}

/// Get the number of character type elements that can be held by this string without reallocating memory.
///
/// Note that this only counts the number of character type elements allocated for the internal string buffer up to,
/// but not including, the final null terminator element appended to the end of the string.
///
/// @return  Current string capacity.
///
/// @see GetSize(), Reserve()
template< typename CharType, typename Allocator >
size_t Helium::StringBase< CharType, Allocator >::GetCapacity() const
{
    size_t bufferCapacity = m_buffer.GetCapacity();
    return( bufferCapacity == 0 ? 0 : bufferCapacity - 1 );
}

/// Explicitly increase the capacity of this string to support at least the specified number of character type
/// elements.
///
/// If the requested capacity is less than the current capacity, no memory will be reallocated.
///
/// @param[in] capacity  Desired capacity.
///
/// @see GetCapacity()
template< typename CharType, typename Allocator >
void Helium::StringBase< CharType, Allocator >::Reserve( size_t capacity )
{
    if( capacity )
    {
        m_buffer.Reserve( capacity + 1 );
    }
}

/// Resize the allocated string memory to match the size actually in use.
///
/// @see GetCapacity()
template< typename CharType, typename Allocator >
void Helium::StringBase< CharType, Allocator >::Trim()
{
    m_buffer.Trim();
}

/// Get a pointer to the base of the allocated string buffer.
///
/// @return  String buffer.
template< typename CharType, typename Allocator >
const CharType* Helium::StringBase< CharType, Allocator >::GetData() const
{
    // The internal string buffer may contain an old string if the string has been resized to zero, so return a null
    // pointer if this string is empty.
    return ( m_buffer.GetSize() ? m_buffer.GetData() : NULL );
}

/// Empty the string contents and free all allocated memory.
template< typename CharType, typename Allocator >
void Helium::StringBase< CharType, Allocator >::Clear()
{
    m_buffer.Clear();
}

/// Get the string element at the specified index.
///
/// @param[in] index  Character element index.
///
/// @return  Reference to the element at the specified index.
template< typename CharType, typename Allocator >
CharType& Helium::StringBase< CharType, Allocator >::GetElement( size_t index )
{
    HELIUM_ASSERT( index < GetSize() );
    return m_buffer.GetElement( index );
}

/// Get the string element at the specified index.
///
/// @param[in] index  Character element index.
///
/// @return  Constant reference to the element at the specified index.
template< typename CharType, typename Allocator >
const CharType& Helium::StringBase< CharType, Allocator >::GetElement( size_t index ) const
{
    HELIUM_ASSERT( index < GetSize() );
    return m_buffer.GetElement( index );
}

/// Append a character to the end of this string.
///
/// @param[in] character  Character to append.
/// @param[in] count      Number of copies of the specified character to append.
template< typename CharType, typename Allocator >
void Helium::StringBase< CharType, Allocator >::Add( CharType character, size_t count )
{
    size_t bufferSize = m_buffer.GetSize();
    if( bufferSize == 0 )
    {
        m_buffer.Add( character, count );
        m_buffer.Add( static_cast< CharType >( 0 ) );
    }
    else
    {
        m_buffer.Insert( bufferSize - 1, character, count );
    }
}

/// Append the contents of a null-terminated C-style string to the end of this string.
///
/// Note that it is not safe to append a string to itself using this function.
///
/// @param[in] pString  String to append.
template< typename CharType, typename Allocator >
void Helium::StringBase< CharType, Allocator >::Add( const CharType* pString )
{
    if( pString )
    {
        size_t stringLength = StringLength( pString );
        if( stringLength != 0 )
        {
            HELIUM_ASSERT( pString < m_buffer.GetData() || pString >= m_buffer.GetData() + m_buffer.GetSize() );

            size_t bufferSize = m_buffer.GetSize();
            if( bufferSize == 0 )
            {
                // Given string ends with a null terminator, so we can copy it over to terminate our string.
                m_buffer.AddArray( pString, stringLength + 1 );
            }
            else
            {
                m_buffer.InsertArray( bufferSize - 1, pString, stringLength );
            }
        }
    }
}

/// Insert copies of a character at the specified index in this string.
///
/// @param[in] index      Index at which to insert copies of the character.
/// @param[in] character  Character to insert.
/// @param[in] count      Number of copies of the character to insert.
template< typename CharType, typename Allocator >
void Helium::StringBase< CharType, Allocator >::Insert( size_t index, CharType character, size_t count )
{
    HELIUM_ASSERT( index <= GetSize() );
    size_t bufferSize = m_buffer.GetSize();
    if( bufferSize == 0 )
    {
        m_buffer.Add( character, count );
        m_buffer.Add( static_cast< CharType >( 0 ) );
    }
    else
    {
        m_buffer.Insert( index, character, count );
    }
}

/// Insert a copy of a null-terminated C-style string at the specified index in this string.
///
/// Note that it is not safe to insert a string to itself using this function.
///
/// @param[in] index    Index at which to insert a copy of the string.
/// @param[in] pString  String to insert.
template< typename CharType, typename Allocator >
void Helium::StringBase< CharType, Allocator >::Insert( size_t index, const CharType* pString )
{
    HELIUM_ASSERT( index <= GetSize() );
    if( pString )
    {
        size_t stringLength = StringLength( pString );
        if( stringLength != 0 )
        {
            HELIUM_ASSERT( pString < m_buffer.GetData() || pString >= m_buffer.GetData() + m_buffer.GetSize() );

            size_t bufferSize = m_buffer.GetSize();
            if( bufferSize == 0 )
            {
                // Given string ends with a null terminator, so we can copy it over to terminate our string.
                m_buffer.AddArray( pString, stringLength + 1 );
            }
            else
            {
                m_buffer.InsertArray( index, pString, stringLength );
            }
        }
    }
}

/// Remove characters from this string.
///
/// @param[in] index  Index from which to remove character type elements.
/// @param[in] count  Number of elements to remove.
template< typename CharType, typename Allocator >
void Helium::StringBase< CharType, Allocator >::Remove( size_t index, size_t count )
{
    HELIUM_ASSERT( index <= GetSize() );
    HELIUM_ASSERT( index + count <= GetSize() );
    m_buffer.Remove( index, count );
}

/// Extract a substring from this string.
///
/// @param[out] rOutput  Extracted substring.
/// @param[in]  index    Starting character index.
/// @param[in]  count    Maximum number of characters to extract.
template< typename CharType, typename Allocator >
template< typename OtherAllocator >
void Helium::StringBase< CharType, Allocator >::Substring(
    StringBase< CharType, OtherAllocator >& rOutput,
    size_t index,
    size_t count ) const
{
    size_t stringSize = GetSize();
    HELIUM_ASSERT( index <= stringSize );

    if( index >= stringSize || count == 0 )
    {
        rOutput.Clear();

        return;
    }

    count = Min( count, stringSize - index );
    size_t endIndex = index + count;

    if( &rOutput == this )
    {
        rOutput.Remove( endIndex, stringSize - endIndex );
        rOutput.Remove( 0, index );

        return;
    }

    rOutput.Clear();
    rOutput.Reserve( count );

    for( ; index < endIndex; ++index )
    {
        rOutput.Push( m_buffer[ index ] );
    }
}

/// Extract a substring from this string.
///
/// @param[in] index  Starting character index.
/// @param[in] count  Maximum number of characters to extract.
///
/// @return  Extracted substring.
template< typename CharType, typename Allocator >
Helium::StringBase< CharType, Allocator > Helium::StringBase< CharType, Allocator >::Substring(
    size_t index,
    size_t count ) const
{
    StringBase output;
    Substring( output, index, count );

    return output;
}

/// Get the first character in this string.
///
/// @return  Reference to the first element in this string.
///
/// @see GetLast()
template< typename CharType, typename Allocator >
CharType& Helium::StringBase< CharType, Allocator >::GetFirst()
{
    HELIUM_ASSERT( GetSize() != 0 );
    return m_buffer.GetFirst();
}

/// Get the first character in this string.
///
/// @return  Constant reference to the first element in this string.
///
/// @see GetLast()
template< typename CharType, typename Allocator >
const CharType& Helium::StringBase< CharType, Allocator >::GetFirst() const
{
    HELIUM_ASSERT( GetSize() != 0 );
    return m_buffer.GetFirst();
}

/// Get the last character in this string.
///
/// @return  Reference to the last element in this string.
///
/// @see GetFirst()
template< typename CharType, typename Allocator >
CharType& Helium::StringBase< CharType, Allocator >::GetLast()
{
    size_t bufferSize = m_buffer.GetSize();
    HELIUM_ASSERT( bufferSize >= 2 );
    return m_buffer.GetElement( bufferSize - 2 );
}

/// Get the last character in this string.
///
/// @return  Constant reference to the last element in this string.
///
/// @see GetFirst()
template< typename CharType, typename Allocator >
const CharType& Helium::StringBase< CharType, Allocator >::GetLast() const
{
    size_t bufferSize = m_buffer.GetSize();
    HELIUM_ASSERT( bufferSize >= 2 );
    return m_buffer.GetElement( bufferSize - 2 );
}

/// Push a character element onto the end of this string.
///
/// @param[in] character  Character to push.
///
/// @return  Index of the pushed character.
///
/// @see Pop()
template< typename CharType, typename Allocator >
size_t Helium::StringBase< CharType, Allocator >::Push( CharType character )
{
    size_t bufferSize = m_buffer.GetSize();
    if( bufferSize == 0 )
    {
        m_buffer.Push( character );
        m_buffer.Push( static_cast< CharType >( 0 ) );

        return 0;
    }

    size_t insertIndex = bufferSize - 1;
    m_buffer.Insert( insertIndex, character );

    return insertIndex;
}

/// Remove the last character element from this string.
///
/// @see Push()
template< typename CharType, typename Allocator >
void Helium::StringBase< CharType, Allocator >::Pop()
{
    size_t bufferSize = m_buffer.GetSize();
    HELIUM_ASSERT( bufferSize >= 2 );

    if( bufferSize == 2 )
    {
        // Only the null terminator will remain if we pop the last character, so empty out the string.
        m_buffer.Resize( 0 );
    }
    else
    {
        // Remove the character prior to the final null terminator.
        m_buffer.Remove( bufferSize - 2 );
    }
}

/// Set this string using "printf"-style formatting.
///
/// @param[in] pFormatString  Format string.
/// @param[in] ...            Format string arguments.
template< typename CharType, typename Allocator >
void Helium::StringBase< CharType, Allocator >::Format( const CharType* pFormatString, ... )
{
    HELIUM_ASSERT( pFormatString );

    va_list argList;
    va_start( argList, pFormatString );
    int resultLength = StringFormatVa( NULL, 0, pFormatString, argList );
    va_end( argList );

    if( resultLength <= 0 )
    {
        m_buffer.Resize( 0 );
        return;
    }

    m_buffer.Resize( resultLength + 1 );
    CharType* pBufferData = m_buffer.GetData();
    HELIUM_ASSERT( pBufferData );

    va_start( argList, pFormatString );
    int finalResult = StringFormatVa( pBufferData, resultLength + 1, pFormatString, argList );
    HELIUM_ASSERT( finalResult == resultLength );
    HELIUM_UNREF( finalResult );
    va_end( argList );
}

/// Find the first instance of the specified character, starting from the given offset.
///
/// @param[in] character   Character to locate.
/// @param[in] startIndex  Index from which to start searching.
///
/// @return  Index of the first instance of the specified character if found, or an invalid index if not found.
///
/// @see FindReverse(), FindAny(), FindAnyReverse()
template< typename CharType, typename Allocator >
size_t Helium::StringBase< CharType, Allocator >::Find( CharType character, size_t startIndex ) const
{
    size_t size = GetSize();
    for( size_t index = startIndex; index < size; ++index )
    {
        if( m_buffer[ index ] == character )
        {
            return index;
        }
    }

    return Invalid< size_t >();
}

/// Find the last instance of the specified character, starting from the given offset and searching in reverse.
///
/// @param[in] character   Character to locate.
/// @param[in] startIndex  Index from which to start searching, or an invalid index to start searching from the end
///                        of the string.
///
/// @return  Index of the last instance of the specified character if found, or an invalid index if not found.
///
/// @see Find(), FindAny(), FindAnyReverse()
template< typename CharType, typename Allocator >
size_t Helium::StringBase< CharType, Allocator >::FindReverse( CharType character, size_t startIndex ) const
{
    size_t size = GetSize();
    if( size != 0 )
    {
        size_t index = ( startIndex >= size ? size : startIndex + 1 );
        while( index != 0 )
        {
            --index;
            if( m_buffer[ index ] == character )
            {
                return index;
            }
        }
    }

    return Invalid< size_t >();
}

/// Find the first instance of any of the characters in the given string, starting from the given offset.
///
/// @param[in] pCharacters     String containing the characters to locate.
/// @param[in] startIndex      Index from which to start searching.
/// @param[in] characterCount  Number of characters in the @c pCharacters string, or an invalid index to check for
///                            each character up to the first null terminator encountered (default behavior).
///
/// @return  Index of the first instance of any of the specified characters if found, or an invalid index if not
///          found.
///
/// @see FindAnyReverse(), Find(), FindReverse()
template< typename CharType, typename Allocator >
size_t Helium::StringBase< CharType, Allocator >::FindAny(
    const CharType* pCharacters,
    size_t startIndex,
    size_t characterCount ) const
{
    HELIUM_ASSERT( pCharacters || characterCount == 0 );

    if( IsInvalid( characterCount ) )
    {
        characterCount = StringLength( pCharacters );
    }

    if( characterCount != 0 )
    {
        size_t size = GetSize();
        for( size_t index = startIndex; index < size; ++index )
        {
            CharType testCharacter = m_buffer[ index ];
            for( size_t characterIndex = 0; characterIndex < characterCount; ++characterIndex )
            {
                if( testCharacter == pCharacters[ characterIndex ] )
                {
                    return index;
                }
            }
        }
    }

    return Invalid< size_t >();
}

/// Find the last instance of any of the characters in the given string, starting from the given offset and
/// searching in reverse.
///
/// @param[in] pCharacters     String containing the characters to locate.
/// @param[in] startIndex      Index from which to start searching.
/// @param[in] characterCount  Number of characters in the @c pCharacters string, or an invalid index to check for
///                            each character up to the first null terminator encountered (default behavior).
///
/// @return  Index of the last instance of any of the specified characters if found, or an invalid index if not
///          found.
///
/// @see FindAny(), Find(), FindReverse()
template< typename CharType, typename Allocator >
size_t Helium::StringBase< CharType, Allocator >::FindAnyReverse(
    const CharType* pCharacters,
    size_t startIndex,
    size_t characterCount ) const
{
    HELIUM_ASSERT( pCharacters || characterCount == 0 );

    if( IsInvalid( characterCount ) )
    {
        characterCount = StringLength( pCharacters );
    }

    if( characterCount != 0 )
    {
        size_t size = GetSize();
        if( size != 0 )
        {
            size_t index = ( startIndex >= size ? size : startIndex + 1 );
            while( index != 0 )
            {
                --index;

                CharType testCharacter = m_buffer[ index ];
                for( size_t characterIndex = 0; characterIndex < characterCount; ++characterIndex )
                {
                    if( testCharacter == pCharacters[ characterIndex ] )
                    {
                        return index;
                    }
                }
            }
        }
    }

    return Invalid< size_t >();
}

/// Check whether this string contains a character.
///
/// @param[in] character  Character for which to check.
///
/// @return  True if this string contains the specified character, false if not.
template< typename CharType, typename Allocator >
bool Helium::StringBase< CharType, Allocator >::Contains( CharType character ) const
{
    size_t size = GetSize();
    for( size_t index = 0; index < size; ++index )
    {
        if( m_buffer[ index ] == character )
        {
            return true;
        }
    }

    return false;
}

/// Check whether this string contains another string.
///
/// @param[in] rString  String for which to check.
///
/// @return  True if this string contains the specified string, false if not.
template< typename CharType, typename Allocator >
template< typename OtherAllocator >
bool Helium::StringBase< CharType, Allocator >::Contains( const StringBase< CharType, OtherAllocator >& rString ) const
{
    size_t otherSize = rString.GetSize();
    if( otherSize == 0 )
    {
        // We always contain an empty string.
        return true;
    }

    size_t size = GetSize();
    if( otherSize > size )
    {
        return false;
    }

    const CharType* pString = rString.m_buffer.GetData();

    size -= otherSize;
    size_t otherByteCount = sizeof( CharType ) * otherSize;
    for( size_t index = 0; index < size; ++index )
    {
        if( MemoryCompare( &m_buffer[ index ], pString, otherByteCount ) == 0 )
        {
            return true;
        }
    }

    return false;
}

/// Check whether this string contains another string.
///
/// @param[in] pString  Null-terminated string for which to check.
///
/// @return  True if this string contains the specified string, false if not.
template< typename CharType, typename Allocator >
bool Helium::StringBase< CharType, Allocator >::Contains( const CharType* pString ) const
{
    // We always contain an empty string.
    if( !pString )
    {
        return false;
    }

    size_t otherSize = StringLength( pString );
    if( otherSize == 0 )
    {
        return true;
    }

    size_t size = GetSize();
    if( otherSize > size )
    {
        return false;
    }

    size -= otherSize;
    size_t otherByteCount = sizeof( CharType ) * otherSize;
    for( size_t index = 0; index < size; ++index )
    {
        if( MemoryCompare( &m_buffer[ index ], pString, otherByteCount ) == 0 )
        {
            return true;
        }
    }

    return false;
}

/// Check whether this string starts with a given string.
///
/// @param[in] rString  String with which to check.
///
/// @return  True if this string starts with the given string, false if not.
///
/// @see EndsWith()
template< typename CharType, typename Allocator >
template< typename OtherAllocator >
bool Helium::StringBase< CharType, Allocator >::StartsWith(
    const StringBase< CharType, OtherAllocator >& rString ) const
{
    size_t stringSize = rString.GetSize();
    if( stringSize > GetSize() )
    {
        return false;
    }

    int compareResult = MemoryCompare(
        m_buffer.GetData(),
        rString.m_buffer.GetData(),
        sizeof( CharType ) * stringSize );

    return ( compareResult == 0 );
}

/// Check whether this string starts with a given string.
///
/// @param[in] pString  Null-terminated string with which to check.
///
/// @return  True if this string starts with the given string, false if not.
///
/// @see EndsWith()
template< typename CharType, typename Allocator >
bool Helium::StringBase< CharType, Allocator >::StartsWith( const CharType* pString ) const
{
    HELIUM_ASSERT( pString );

    size_t stringSize = StringLength( pString );
    if( stringSize > GetSize() )
    {
        return false;
    }

    int compareResult = MemoryCompare( m_buffer.GetData(), pString, sizeof( CharType ) * stringSize );

    return ( compareResult == 0 );
}

/// Check whether this string ends with a given string.
///
/// @param[in] rString  String with which to check.
///
/// @return  True if this string ends with the given string, false if not.
///
/// @see StartsWith()
template< typename CharType, typename Allocator >
template< typename OtherAllocator >
bool Helium::StringBase< CharType, Allocator >::EndsWith( const StringBase< CharType, OtherAllocator >& rString ) const
{
    size_t stringSize = rString.GetSize();
    size_t thisSize = GetSize();
    if( stringSize > thisSize )
    {
        return false;
    }

    int compareResult = MemoryCompare(
        m_buffer.GetData() + thisSize - stringSize,
        rString.m_buffer.GetData(),
        sizeof( CharType ) * stringSize );

    return ( compareResult == 0 );
}

/// Check whether this string ends with a given string.
///
/// @param[in] pString  Null-terminated string with which to check.
///
/// @return  True if this string ends with the given string, false if not.
///
/// @see StartsWith()
template< typename CharType, typename Allocator >
bool Helium::StringBase< CharType, Allocator >::EndsWith( const CharType* pString ) const
{
    HELIUM_ASSERT( pString );

    size_t stringSize = StringLength( pString );
    size_t thisSize = GetSize();
    if( stringSize > thisSize )
    {
        return false;
    }

    int compareResult = MemoryCompare(
        m_buffer.GetData() + thisSize - stringSize,
        pString,
        sizeof( CharType ) * stringSize );

    return ( compareResult == 0 );
}

/// Split a string into an array of strings based on the specified character separator.
///
/// @param[out] rStringResults              List of resulting strings.
/// @param[in]  separator                   Separator character.
/// @param[in]  bCombineAdjacentSeparators  True if adjacent separator characters should be combined to only split
///                                         the neighboring string components that are not separator characters,
///                                         false if the string should be split at each and every separator
///                                         character even if empty strings are generated.  Note that empty strings
///                                         may still be generated for the first and last elements of the output
///                                         array if this string starts or ends with a separator character
///                                         regardless of the setting of this flag.
template< typename CharType, typename Allocator >
template< typename ArrayType, typename ArrayAllocator >
void Helium::StringBase< CharType, Allocator >::Split(
    DynArray< ArrayType, ArrayAllocator >& rStringResults,
    CharType separator,
    bool bCombineAdjacentSeparators ) const
{
    rStringResults.Resize( 0 );

    // Always allow splitting off an empty string at the start if this string starts with a separator character.
    bool bHaveNonSeparator = true;

    size_t startIndex = 0;
    size_t characterCount = GetSize();
    for( size_t characterIndex = 0; characterIndex < characterCount; ++characterIndex )
    {
        if( m_buffer[ characterIndex ] == separator )
        {
            if( !bCombineAdjacentSeparators || bHaveNonSeparator )
            {
                HELIUM_VERIFY( rStringResults.New( m_buffer.GetData() + startIndex, characterIndex - startIndex ) );
                bHaveNonSeparator = false;
            }

            startIndex = characterIndex + 1;
        }
        else
        {
            bHaveNonSeparator = true;
        }
    }

    // Always allow splitting off an empty string at the end if this string ends with a separator character.
    HELIUM_VERIFY( rStringResults.New( m_buffer.GetData() + startIndex, characterCount - startIndex ) );
}

/// Split a string into an array of strings based on the specified character separators.
///
/// @param[out] rStringResults              List of resulting strings.
/// @param[in]  pSeparators                 Separator characters.
/// @param[in]  separatorCount              Number of characters in the @c pSeparators string, or an invalid index
///                                         to check for each character up to the first null terminator encountered
///                                         (default behavior).
/// @param[in]  bCombineAdjacentSeparators  True if adjacent separator characters should be combined to only split
///                                         the neighboring string components that are not separator characters,
///                                         false if the string should be split at each and every separator
///                                         character even if empty strings are generated.  Note that empty strings
///                                         may still be generated for the first and last elements of the output
///                                         array if this string starts or ends with a separator character
///                                         regardless of the setting of this flag.
template< typename CharType, typename Allocator >
template< typename ArrayType, typename ArrayAllocator >
void Helium::StringBase< CharType, Allocator >::Split(
    DynArray< ArrayType, ArrayAllocator >& rStringResults,
    const CharType* pSeparators,
    size_t separatorCount,
    bool bCombineAdjacentSeparators ) const
{
    HELIUM_ASSERT( pSeparators || separatorCount == 0 );

    rStringResults.Resize( 0 );

    if( IsInvalid( separatorCount ) )
    {
        separatorCount = StringLength( pSeparators );
    }

    // Always allow splitting off an empty string at the start if this string starts with a separator character.
    bool bHaveNonSeparator = true;

    size_t startIndex = 0;
    size_t characterCount = GetSize();
    for( size_t characterIndex = 0; characterIndex < characterCount; ++characterIndex )
    {
        CharType character = m_buffer[ characterIndex ];
        size_t separatorIndex;
        for( separatorIndex = 0; separatorIndex < separatorCount; ++separatorIndex )
        {
            if( character == pSeparators[ separatorIndex ] )
            {
                if( !bCombineAdjacentSeparators || bHaveNonSeparator )
                {
                    HELIUM_VERIFY( rStringResults.New( m_buffer.GetData() + startIndex, characterIndex - startIndex ) );
                    bHaveNonSeparator = false;
                }

                startIndex = characterIndex + 1;

                break;
            }
        }

        if( separatorIndex >= separatorCount )
        {
            bHaveNonSeparator = true;
        }
    }

    // Always allow splitting off an empty string at the end if this string ends with a separator character.
    HELIUM_VERIFY( rStringResults.New( m_buffer.GetData() + startIndex, characterCount - startIndex ) );
}

/// Split a string into an array of strings based on the specified character separators.
///
/// @param[out] rStringResults              List of resulting strings.
/// @param[in]  rSeparators                 Separator characters.
/// @param[in]  bCombineAdjacentSeparators  True if adjacent separator characters should be combined to only split
///                                         the neighboring string components that are not separator characters,
///                                         false if the string should be split at each and every separator
///                                         character even if empty strings are generated.  Note that empty strings
///                                         may still be generated for the first and last elements of the output
///                                         array if this string starts or ends with a separator character
///                                         regardless of the setting of this flag.
template< typename CharType, typename Allocator >
template< typename ArrayType, typename ArrayAllocator, typename StringAllocator >
void Helium::StringBase< CharType, Allocator >::Split(
    DynArray< ArrayType, ArrayAllocator >& rStringResults,
    const StringBase< CharType, StringAllocator >& rSeparators,
    bool bCombineAdjacentSeparators ) const
{
    Split( rStringResults, rSeparators.GetData(), rSeparators.GetSize(), bCombineAdjacentSeparators );
}

/// Get the contents of this string as a null-terminated C-string.
///
/// This is always guaranteed to return a pointer to a string, even if the string is empty (in which case a buffer
/// for string data may not have actually been allocated).
///
/// @return  Pointer to a null-terminated C-string representation of this string.  Note that if this string is
///          empty, the pointer returned may not actually point to the string's buffer.
template< typename CharType, typename Allocator >
const CharType* Helium::StringBase< CharType, Allocator >::operator*() const
{
    static const CharType emptyString[] = { static_cast< CharType >( '\0' ) };

    // Use StringBase::GetData() instead of getting the buffer data directly in order to check for empty strings.
    const CharType* pString = GetData();

    return ( pString ? pString : emptyString );
}

/// Get the string element at the specified index.
///
/// @param[in] index  Character element index.
///
/// @return  Reference to the element at the specified index.
template< typename CharType, typename Allocator >
CharType& Helium::StringBase< CharType, Allocator >::operator[]( ptrdiff_t index )
{
    HELIUM_ASSERT( static_cast< size_t >( index ) < GetSize() );
    return m_buffer.GetElement( index );
}

/// Get the string element at the specified index.
///
/// @param[in] index  Character element index.
///
/// @return  Constant reference to the element at the specified index.
template< typename CharType, typename Allocator >
const CharType& Helium::StringBase< CharType, Allocator >::operator[]( ptrdiff_t index ) const
{
    HELIUM_ASSERT( static_cast< size_t >( index ) < GetSize() );
    return m_buffer.GetElement( index );
}

/// Set this string to a single character.
///
/// This will always destroy the current string contents and allocate a fresh buffer whose capacity matches that
/// needed to hold onto a string of only the given character.
///
/// @param[in] character  Character to set.
///
/// @return  Reference to this string.
template< typename CharType, typename Allocator >
Helium::StringBase< CharType, Allocator >& Helium::StringBase< CharType, Allocator >::operator=( CharType character )
{
    m_buffer.Clear();
    m_buffer.Reserve( 2 );
    m_buffer.Resize( 2 );
    m_buffer[ 0 ] = character;
    m_buffer[ 1 ] = static_cast< CharType >( 0 );

    return *this;
}

/// Set this string to a copy of the given C-style string.
///
/// The length is determined automatically based on the location of the first null terminating character.  If the
/// given string is not the same as this string, this will always destroy the current string contents and allocate a
/// fresh buffer whose capacity matches the size of the given string.
///
/// @param[in] pString  String from which to copy.  This can be null.
///
/// @return  Reference to this string.
template< typename CharType, typename Allocator >
Helium::StringBase< CharType, Allocator >& Helium::StringBase< CharType, Allocator >::operator=(
    const CharType* pString )
{
    if( m_buffer.GetData() != pString )
    {
        m_buffer.Clear();
        if( pString )
        {
            size_t length = StringLength( pString );
            if( length != 0 )
            {
                ++length;
                m_buffer.Reserve( length );
                m_buffer.Resize( length );
                MemoryCopy( m_buffer.GetData(), pString, sizeof( CharType ) * length );
            }
        }
    }

    return *this;
}

/// Set this string to the contents of the given string.
///
/// If the given string is not the same as this string, this will always destroy the current string contents and
/// allocate a fresh buffer whose capacity matches the size of the given string.
///
/// @param[in] rSource  String from which to copy.
///
/// @return  Reference to this string.
template< typename CharType, typename Allocator >
template< typename OtherAllocator >
Helium::StringBase< CharType, Allocator >& Helium::StringBase< CharType, Allocator >::operator=(
    const StringBase< CharType, OtherAllocator >& rSource )
{
    m_buffer = rSource.m_buffer;
    return *this;
}

/// Check whether the contents of this string match the contents of a given null-terminated C-style string.
///
/// @param[in] pString  String with which to compare.  This can be null.
///
/// @return  True if the strings match, false if not.
template< typename CharType, typename Allocator >
bool Helium::StringBase< CharType, Allocator >::operator==( const CharType* pString ) const
{
    size_t bufferSize = m_buffer.GetSize();

    // Check for empty string matches.
    if( !pString || pString[ 0 ] == static_cast< CharType >( 0 ) )
    {
        return( bufferSize <= 1 );
    }

    // Perform a character-by-character comparison.
    size_t stringSize = bufferSize - 1;
    for( size_t characterIndex = 0; characterIndex < stringSize; ++characterIndex )
    {
        CharType character = pString[ characterIndex ];
        if( character == static_cast< CharType >( 0 ) || m_buffer[ characterIndex ] != character )
        {
            return false;
        }
    }

    // String should be null-terminated.
    return( pString[ stringSize ] == static_cast< CharType >( 0 ) );
}

/// Check whether the contents of this string match the contents of a given string.
///
/// @param[in] rString  String with which to compare.
///
/// @return  True if the strings match, false if not.
template< typename CharType, typename Allocator >
template< typename OtherAllocator >
bool Helium::StringBase< CharType, Allocator >::operator==(
    const StringBase< CharType, OtherAllocator >& rString ) const
{
    size_t bufferSize = m_buffer.GetSize();

    return( bufferSize == rString.m_buffer.GetSize() &&
            MemoryCompare( m_buffer.GetData(), rString.m_buffer.GetData(), bufferSize * sizeof( CharType ) ) == 0 );
}

/// Check whether the contents of this string do not match the contents of a given null-terminated C-style string.
///
/// @param[in] pString  String with which to compare.  This can be null.
///
/// @return  True if the strings do not match, false if they do.
template< typename CharType, typename Allocator >
bool Helium::StringBase< CharType, Allocator >::operator!=( const CharType* pString ) const
{
    return !( *this == pString );
}

/// Check whether the contents of this string do not match the contents of a given string.
///
/// @param[in] rString  String with which to compare.
///
/// @return  True if the strings do not match, false if they do.
template< typename CharType, typename Allocator >
template< typename OtherAllocator >
bool Helium::StringBase< CharType, Allocator >::operator!=(
    const StringBase< CharType, OtherAllocator >& rString ) const
{
    return !( *this == rString );
}

/// Append the contents of a string to the end of this string.
///
/// @param[in] rString  String to append.
template< typename CharType, typename Allocator >
template< typename OtherAllocator >
void Helium::StringBase< CharType, Allocator >::Add( const StringBase< CharType, OtherAllocator >& rString )
{
    // We cache the size locally, but not the string buffer data, so that we can support appending a string to
    // itself (buffer address may change when resizing this string).
    size_t otherBufferSize = rString.m_buffer.GetSize();
    if( otherBufferSize > 1 )
    {
        size_t currentSize = GetSize();
        m_buffer.Resize( currentSize + otherBufferSize );
        MemoryCopy(
            m_buffer.GetData() + currentSize,
            rString.m_buffer.GetData(),
            sizeof( CharType ) * otherBufferSize );
    }
}

/// Insert a copy of a string at the specified index in this string.
///
/// Note that it is not safe to insert a string to itself using this function.
///
/// @param[in] index    Index at which to insert a copy of the string.
/// @param[in] rString  String to insert.
template< typename CharType, typename Allocator >
template< typename OtherAllocator >
void Helium::StringBase< CharType, Allocator >::Insert(
    size_t index,
    const StringBase< CharType, OtherAllocator >& rString )
{
    HELIUM_ASSERT( index <= GetSize() );
    size_t otherBufferSize = rString.m_buffer.GetSize();
    if( otherBufferSize > 1 )
    {
        size_t bufferSize = m_buffer.GetSize();
        if( bufferSize > 1 )
        {
            m_buffer.InsertArray( index, rString.m_buffer.GetData(), otherBufferSize - 1 );
        }
        else
        {
            m_buffer.Resize( otherBufferSize );
            MemoryCopy(
                m_buffer.GetData(),
                rString.m_buffer.GetData(),
                sizeof( CharType ) * otherBufferSize );
        }
    }
}

/// Write a string to the given output stream.
///
/// @param[in] rStream  Output stream.
/// @param[in] rString  String to write.
///
/// @return  Reference to the given output stream.
template< typename CharType, typename CharTypeTraits, typename Allocator >
std::basic_ostream< CharType, CharTypeTraits >& Helium::operator<<(
    std::basic_ostream< CharType, CharTypeTraits >& rStream,
    const StringBase< CharType, Allocator >& rString )
{
    return ( rStream << *rString );
}

/// Read a string from the given input stream.
///
/// @param[in]  rStream  Input stream.
/// @param[out] rString  Read string.
///
/// @return  Reference to the given input stream.
template< typename CharType, typename CharTypeTraits, typename Allocator >
std::basic_istream< CharType, CharTypeTraits >& Helium::operator>>(
    std::basic_istream< CharType, CharTypeTraits >& rStream,
    StringBase< CharType, Allocator >& rString )
{
    // Could definitely improve this...
    std::basic_string< CharType, CharTypeTraits, std::allocator< char > > tempString;
    rStream >> tempString;
    rString = tempString.c_str();

    return rStream;
}
