//----------------------------------------------------------------------------------------------------------------------
// Utility.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// ArrayCopy() implementation for types with a trivial assignment operator.
    ///
    /// @param[in] pDest              Base address of the destination array.
    /// @param[in] pSource            Base address of the source array.
    /// @param[in] count              Number of elements to copy.
    /// @param[in] rHasTrivialAssign  boost::true_type.
    template< typename T >
    void _ArrayCopy( T* pDest, const T* pSource, size_t count, const boost::true_type& /*rHasTrivialAssign*/ )
    {
        MemoryCopy( pDest, pSource, sizeof( T ) * count );
    }

    /// ArrayCopy() implementation for types without a trivial assignment operator.
    ///
    /// @param[in] pDest              Base address of the destination array.
    /// @param[in] pSource            Base address of the source array.
    /// @param[in] count              Number of elements to copy.
    /// @param[in] rHasTrivialAssign  boost::false_type.
    template< typename T >
    void _ArrayCopy( T* pDest, const T* pSource, size_t count, const boost::false_type& /*rHasTrivialAssign*/ )
    {
        for( size_t elementIndex = 0; elementIndex < count; ++elementIndex )
        {
            pDest[ elementIndex ] = pSource[ elementIndex ];
        }
    }

    /// ArrayMove() implementation for types with a trivial assignment operator.
    ///
    /// @param[in] pDest              Base address of the destination array.
    /// @param[in] pSource            Base address of the source array.
    /// @param[in] count              Number of elements to copy.
    /// @param[in] rHasTrivialAssign  boost::true_type.
    template< typename T >
    void _ArrayMove( T* pDest, const T* pSource, size_t count, const boost::true_type& /*rHasTrivialAssign*/ )
    {
        MemoryMove( pDest, pSource, sizeof( T ) * count );
    }

    /// ArrayMove() implementation for types without a trivial assignment operator.
    ///
    /// @param[in] pDest              Base address of the destination array.
    /// @param[in] pSource            Base address of the source array.
    /// @param[in] count              Number of elements to copy.
    /// @param[in] rHasTrivialAssign  boost::false_type.
    template< typename T >
    void _ArrayMove( T* pDest, const T* pSource, size_t count, const boost::false_type& /*rHasTrivialAssign*/ )
    {
        if( pDest <= pSource )
        {
            for( size_t elementIndex = 0; elementIndex < count; ++elementIndex )
            {
                pDest[ elementIndex ] = pSource[ elementIndex ];
            }
        }
        else
        {
            size_t elementIndex = count;
            while( elementIndex != 0 )
            {
                --elementIndex;
                pDest[ elementIndex ] = pSource[ elementIndex ];
            }
        }
    }

    /// ArrayInPlaceConstruct() implementation for types with a trivial constructor.
    ///
    /// @param[in] pMemory                 Base address of the memory buffer in which the objects should be constructed.
    /// @param[in] count                   Number of objects to construct.
    /// @param[in] rHasTrivialConstructor  boost::true_type.
    ///
    /// @return  Pointer to the first element in the array.  This address will actually be the same as @c pMemory, but
    ///          cast to the template type.
    template< typename T >
    T* _ArrayInPlaceConstruct( void* pMemory, size_t /*count*/, const boost::true_type& /*rHasTrivialConstructor*/ )
    {
        // Nothing needs to be done.
        return static_cast< T* >( pMemory );
    }

    /// ArrayInPlaceConstruct() implementation for types without a trivial constructor.
    ///
    /// @param[in] pMemory                 Base address of the memory buffer in which the objects should be constructed.
    /// @param[in] count                   Number of objects to construct.
    /// @param[in] rHasTrivialConstructor  boost::false_type.
    ///
    /// @return  Pointer to the first element in the array.  This address will actually be the same as @c pMemory, but
    ///          cast to the template type.
    template< typename T >
    T* _ArrayInPlaceConstruct( void* pMemory, size_t count, const boost::false_type& /*rHasTrivialConstructor*/ )
    {
        L_ASSERT( pMemory || count == 0 );

        T* pArray = static_cast< T* >( pMemory );

        T* pObject = pArray;
        for( size_t elementIndex = 0; elementIndex < count; ++elementIndex )
        {
            new( pObject ) T;
            ++pObject;
        }

        return pArray;
    }

    /// ArrayInPlaceDestroy() implementation for types with a trivial destructor.
    ///
    /// @param[in] pMemory                Base address of the array of objects being destroyed.
    /// @param[in] count                  Number of objects being destroyed.
    /// @param[in] rHasTrivialDestructor  boost::true_type.
    template< typename T >
    void _ArrayInPlaceDestroy( T* /*pMemory*/, size_t /*count*/, const boost::true_type& /*rHasTrivialDestructor*/ )
    {
        // Nothing needs to be done...
    }

    /// ArrayInPlaceDestroy() implementation for types without a trivial destructor.
    ///
    /// @param[in] pMemory                Base address of the array of objects being destroyed.
    /// @param[in] count                  Number of objects being destroyed.
    /// @param[in] rHasTrivialDestructor  boost::false_type.
    template< typename T >
    void _ArrayInPlaceDestroy( T* pMemory, size_t count, const boost::false_type& /*rHasTrivialDestructor*/ )
    {
        for( size_t elementIndex = 0; elementIndex < count; ++elementIndex )
        {
            pMemory[ elementIndex ].~T();
        }
    }

    /// ArrayUninitializedCopy() implementation for types with a trivial copy constructor.
    ///
    /// @param[in] pDest            Destination buffer in which each element should be constructed.
    /// @param[in] pSource          Source array from which to copy.
    /// @param[in] count            Number of elements to copy.
    /// @param[in] rHasTrivialCopy  boost::true_type.
    template< typename T >
    void _ArrayUninitializedCopy(
        T* pDest,
        const T* pSource,
        size_t count,
        const boost::true_type& /*rHasTrivialCopy*/ )
    {
        MemoryCopy( pDest, pSource, sizeof( T ) * count );
    }

    /// ArrayUninitializedCopy() implementation for types without a trivial copy constructor.
    ///
    /// @param[in] pDest            Destination buffer in which each element should be constructed.
    /// @param[in] pSource          Source array from which to copy.
    /// @param[in] count            Number of elements to copy.
    /// @param[in] rHasTrivialCopy  boost::false_type.
    template< typename T >
    void _ArrayUninitializedCopy(
        T* pDest,
        const T* pSource,
        size_t count,
        const boost::false_type& /*rHasTrivialCopy*/ )
    {
        for( size_t elementIndex = 0; elementIndex < count; ++elementIndex )
        {
            new( pDest + elementIndex ) T( pSource[ elementIndex ] );
        }
    }

    /// ArrayUninitializedFill() implementation for single-byte types with a trivial copy constructor.
    ///
    /// @param[in] pDest              Destination buffer in which each element should be constructed.
    /// @param[in] rValue             Value to copy.
    /// @param[in] count              Number of copies to fill.
    /// @param[in] rCopyTrivialBytes  boost::true_type.
    template< typename T >
    void _ArrayUninitializedFill(
        T* pDest,
        const T& rValue,
        size_t count,
        const boost::true_type& /*rCopyTrivialBytes*/ )
    {
        MemorySet( pDest, rValue, count );
    }

    /// ArrayUninitializedFill() implementation for multi-byte types and types without a trivial copy constructor.
    ///
    /// @param[in] pDest              Destination buffer in which each element should be constructed.
    /// @param[in] rValue             Value to copy.
    /// @param[in] count              Number of copies to fill.
    /// @param[in] rCopyTrivialBytes  boost::false_type.
    template< typename T >
    void _ArrayUninitializedFill(
        T* pDest,
        const T& rValue,
        size_t count,
        const boost::false_type& /*rCopyTrivialBytes*/ )
    {
        for( size_t elementIndex = 0; elementIndex < count; ++elementIndex )
        {
            new( pDest + elementIndex ) T( rValue );
        }
    }

    /// Align() implementation for pointer types.
    ///
    /// @param[in] rValue      Pointer to align.
    /// @param[in] alignment   Byte alignment (must be a power of two).
    /// @param[in] rIsPointer  boost::true_type.
    ///
    /// @return  Aligned value.
    template< typename T >
    T _Align( const T& rValue, size_t alignment, const boost::true_type& /*rIsPointer*/ )
    {
        uintptr_t valueInt = reinterpret_cast< uintptr_t >( rValue );

        return reinterpret_cast< T >( ( valueInt + alignment - 1 ) & ~( alignment - 1 ) );
    }

    /// Align() implementation for non-pointer types.
    ///
    /// @param[in] rValue      Integer to align.
    /// @param[in] alignment   Byte alignment (must be a power of two).
    /// @param[in] rIsPointer  boost::false_type.
    ///
    /// @return  Aligned value.
    template< typename T >
    T _Align( const T& rValue, size_t alignment, const boost::false_type& /*rIsPointer*/ )
    {
        return ( rValue + alignment - 1 ) & ~( alignment - 1 );
    }

    /// CastIndex() implementation when casting to a larger unsigned integer type.
    ///
    /// @param[in] index          Index to cast.
    /// @param[in] rIsDestLarger  boost::true_type.
    ///
    /// @return  Cast index type, with invalid values properly retained when casting to larger types.
    template< typename DestIndexType, typename SourceIndexType >
    DestIndexType _CastIndex( SourceIndexType index, const boost::true_type& /*rIsDestLarger*/ )
    {
        return ( index == static_cast< SourceIndexType >( -1 )
                 ? static_cast< DestIndexType >( -1 )
                 : static_cast< DestIndexType >( index ) );
    }

    /// CastIndex() implementation when not casting to a larger unsigned integer type.
    ///
    /// @param[in] index          Index to cast.
    /// @param[in] rIsDestLarger  boost::false_type.
    ///
    /// @return  Cast index type, with invalid values properly retained when casting to larger types.
    template< typename DestIndexType, typename SourceIndexType >
    DestIndexType _CastIndex( SourceIndexType index, const boost::false_type& /*rIsDestLarger*/ )
    {
        return static_cast< DestIndexType >( index );
    }

    /// Copy data from one region of memory to another.
    ///
    /// Note that the source and destination regions of memory should not overlap.  For such cases, MemoryMove() should
    /// be used instead.
    ///
    /// @param[in] pDest    Base address of the destination buffer.
    /// @param[in] pSource  Base address of the source buffer.
    /// @param[in] size     Number of bytes to copy.
    ///
    /// @see MemoryMove(), ArrayCopy()
    void MemoryCopy( void* pDest, const void* pSource, size_t size )
    {
#if HELIUM_CC_MSC
        memcpy_s( pDest, size, pSource, size );
#else
        memcpy( pDest, pSource, size );
#endif
    }

    /// Copy data from one region of memory to another, with support for overlapping regions of memory.
    ///
    /// @param[in] pDest    Base address of the destination buffer.
    /// @param[in] pSource  Base address of the source buffer.
    /// @param[in] size     Number of bytes to copy.
    ///
    /// @see MemoryCopy(), ArrayMove()
    void MemoryMove( void* pDest, const void* pSource, size_t size )
    {
#if HELIUM_CC_MSC
        memmove_s( pDest, size, pSource, size );
#else
        memmove( pDest, pSource, size );
#endif
    }

    /// Set each byte in a region of memory to a given value.
    ///
    /// @param[in] pDest  Base address of the destination buffer.
    /// @param[in] value  Value to which each byte should be set.
    /// @param[in] size   Number of bytes to set.
    ///
    /// @see ArraySet()
    void MemorySet( void* pDest, int value, size_t size )
    {
        memset( pDest, value, size );
    }

    /// Set each byte in a region of memory to zero.
    ///
    /// @param[in] pDest  Base address of the destination buffer.
    /// @param[in] size   Number of bytes to clear.
    void MemoryZero( void* pDest, size_t size )
    {
        memset( pDest, 0, size );
    }

    /// Compare each byte between two regions of memory.
    ///
    /// @param[in] pMemory0  Base address of the first buffer.
    /// @param[in] pMemory1  Base address of the second buffer.
    /// @param[in] size      Number of bytes to compare.
    ///
    /// @return  Zero if the contents match, a value greater than zero if the first non-matching byte found is larger in
    ///          the first memory buffer, or a value less than zero if the first non-matching byte found is larger in
    ///          the second memory buffer.
    int MemoryCompare( const void* pMemory0, const void* pMemory1, size_t size )
    {
        return memcmp( pMemory0, pMemory1, size );
    }

    /// Copy an array of data from one region of memory to another.
    ///
    /// This takes advantage of compiler type trait support to determine whether the type being copied has a trivial
    /// assignment operator.  For types with trivial assignment, this has the equivalent of calling MemoryCopy(),
    /// whereas for types without trivial assignment, this safely copies each element individually using the assignment
    /// operator.  Since type checking is performed at compile time, this does not have a negative impact on
    /// performance.
    ///
    /// Note that the source and destination regions of memory should not overlap.  For such cases, ArrayMove() should
    /// be used instead.
    ///
    /// @param[in] pDest    Base address of the destination array.
    /// @param[in] pSource  Base address of the source array.
    /// @param[in] count    Number of elements to copy.
    ///
    /// @see ArrayMove(), MemoryCopy()
    template< typename T >
    void ArrayCopy( T* pDest, const T* pSource, size_t count )
    {
        _ArrayCopy( pDest, pSource, count, boost::has_trivial_assign< T >() );
    }

    /// Copy an array of data from one region of memory to another, with support for overlapping regions of memory.
    ///
    /// This takes advantage of compiler type trait support to determine whether the type being copied has a trivial
    /// assignment operator.  For types with trivial assignment, this has the equivalent of calling MemoryCopy(),
    /// whereas for types without trivial assignment, this safely copies each element individually using the assignment
    /// operator.  Since type checking is performed at compile time, this does not have a negative impact on
    /// performance.
    ///
    /// @param[in] pDest    Base address of the destination array.
    /// @param[in] pSource  Base address of the source array.
    /// @param[in] count    Number of elements to copy.
    ///
    /// @see ArrayCopy(), MemoryMove()
    template< typename T >
    void ArrayMove( T* pDest, const T* pSource, size_t count )
    {
        _ArrayMove( pDest, pSource, count, boost::has_trivial_assign< T >() );
    }

    /// Set each element in an array to a given value.
    ///
    /// @param[in] pDest   Base address of the destination array.
    /// @param[in] rValue  Value to which each element should be set.
    /// @param[in] count   Number of elements to set.
    ///
    /// @see MemorySet()
    template< typename T >
    void ArraySet( T* pDest, const T& rValue, size_t count )
    {
        for( size_t elementIndex = 0; elementIndex < count; ++elementIndex )
        {
            pDest[ elementIndex ] = rValue;
        }
    }

    /// Call the constructor for the given type on each element in the given array.
    ///
    /// When using placement new to construct an array of elements within a given buffer, the given C++ implementation
    /// may actually offset the start of the array within the buffer in order to store a value specifying the number of
    /// elements in the array (particularly when constructing arrays of types with non-trivial destructors), effectively
    /// making it difficult and/or error-prone to allocate the exact amount of memory that is actually needed for the
    /// array.  This function provides an alternative to using the array placement new on a memory buffer if the number
    /// of elements will always be known by explicitly constructing each element individually (if necessary; compiler
    /// support for type traits are used where supported in order to avoid this overhead for types with a trivial
    /// constructor).
    ///
    /// This should only be called if the destination buffer is allocated as uninitialized memory (not created using
    /// "new T" or "new T []", where "T" is the type of array element).  Elements should be destroyed later using
    /// ArrayInPlaceDestroy() prior to freeing the buffer memory.
    ///
    /// @param[in] pMemory  Base address of the memory buffer in which the objects should be constructed.
    /// @param[in] count    Number of objects to construct.
    ///
    /// @return  Pointer to the first element in the array.  This address will actually be the same as @c pMemory, but
    ///          cast to the template type.
    ///
    /// @see ArrayInPlaceDestroy()
    template< typename T >
    T* ArrayInPlaceConstruct( void* pMemory, size_t count )
    {
        return _ArrayInPlaceConstruct< T >( pMemory, count, boost::has_trivial_constructor< T >() );
    }

    /// Call the destructor on each element in the given array without deallocating the array.
    ///
    /// This should only be called if the destination buffer was originally allocated as uninitialized memory (not
    /// created using "new T" or "new T []", where "T" is the type of array element) and later initialized manually
    /// (i.e. using the in-place "new" operator, ArrayInPlaceConstruct(), ArrayUninitializedCopy(), or
    /// ArrayUninitializedFill()).
    ///
    /// @param[in] pMemory  Base address of the array of objects being destroyed.
    /// @param[in] count    Number of objects being destroyed.
    ///
    /// @see ArrayInPlaceConstruct()
    template< typename T >
    void ArrayInPlaceDestroy( T* pMemory, size_t count )
    {
        _ArrayInPlaceDestroy( pMemory, count, boost::has_trivial_destructor< T >() );
    }

    /// Construct copies of objects from the source array in the uninitialized destination buffer.
    ///
    /// This should only be called if the destination buffer is uninitialized memory (not created using "new T" or
    /// "new T []", where "T" is the type of array element).
    ///
    /// @param[in] pDest    Destination buffer in which each element should be constructed.
    /// @param[in] pSource  Source array from which to copy.
    /// @param[in] count    Number of elements to copy.
    template< typename T >
    void ArrayUninitializedCopy( T* pDest, const T* pSource, size_t count )
    {
        _ArrayUninitializedCopy( pDest, pSource, count, boost::has_trivial_copy< T >() );
    }

    /// Construct copies of the given object in the uninitialized destination buffer.
    ///
    /// This should only be called if the destination buffer is uninitialized memory (not created using "new T" or
    /// "new T []", where "T" is the type of array element).
    ///
    /// @param[in] pDest   Destination buffer in which each element should be constructed.
    /// @param[in] rValue  Value to copy.
    /// @param[in] count   Number of copies to fill.
    template< typename T >
    void ArrayUninitializedFill( T* pDest, const T& rValue, size_t count )
    {
        _ArrayUninitializedFill(
            pDest,
            rValue,
            count,
            boost::integral_constant< bool, boost::has_trivial_copy< T >::value && sizeof( T ) == 1 >() );
    }

    /// Align a value (size or memory address) up to a given byte alignment.
    ///
    /// @param[in] rValue     Integral value or pointer to align.
    /// @param[in] alignment  Byte alignment (must be a power of two).
    ///
    /// @return  Aligned value.
    template< typename T >
    T Align( const T& rValue, size_t alignment )
    {
        L_ASSERT( IsPowerOfTwo( alignment ) );

        return _Align( rValue, alignment, boost::is_pointer< T >() );
    }

    /// Swap two values.
    ///
    /// This will make a temporary copy of the first parameter's value (copy construction), copy the second parameter's
    /// value over into the first parameter (assignment), and finally copy the temporary copy of the first parameter
    /// over into the second parameter (assignment, then automatic destruction of the temporary value).
    ///
    /// @param[in,out] rValue0  First value to swap.
    /// @param[in,out] rValue1  Second value to swap.
    template< typename T >
    void Swap( T& rValue0, T& rValue1 )
    {
        T temp( rValue0 );
        rValue0 = rValue1;
        rValue1 = temp;
    }

    /// Get the length of a C-style string.
    ///
    /// Note that this only counts the number of elements in the given array up to, but not including, the first null
    /// element.  For strings types that may represent certain characters using multiple array elements (i.e. UTF-16),
    /// the number returned might not represent the actual number of characters in the string.
    ///
    /// @param[in] pString  C-style string.
    ///
    /// @return  Number of array elements in the string up to the null terminator.
    template< typename T >
    size_t StringLength( const T* pString )
    {
        L_ASSERT( pString );
        const T* pEnd = pString;
        while( *pEnd != static_cast< T >( 0 ) )
        {
            ++pEnd;
        }

        return static_cast< size_t >( pEnd - pString );
    }

    /// Compare two null-terminated C-style strings.
    ///
    /// @param[in] pString0  First C-style string.
    /// @param[in] pString1  Second C-style string.
    ///
    /// @return  Zero if the contents match, a value greater than zero if the first non-matching character found is
    ///          larger in the first memory buffer, or a value less than zero if the first non-matching character found
    ///          is larger in the second memory buffer.
    ///
    /// @see StringNCompare()
    template< typename T >
    int StringCompare( const T* pString0, const T* pString1 )
    {
        L_ASSERT( pString0 );
        L_ASSERT( pString1 );

        for( ; ; )
        {
            int char0 = *pString0;
            int char1 = *pString1;
            int charDiff = char0 - char1;
            if( charDiff != 0 || char0 == static_cast< T >( 0 ) )
            {
                return charDiff;
            }

            ++pString0;
            ++pString1;
        }
    }

    /// Compare two null-terminated C-style strings, limiting the number of characters checked to a specific range.
    ///
    /// @param[in] pString0  First C-style string.
    /// @param[in] pString1  Second C-style string.
    /// @param[in] count     Maximum number of characters to compare.
    ///
    /// @return  Zero if the contents match, a value greater than zero if the first non-matching character found is
    ///          larger in the first memory buffer, or a value less than zero if the first non-matching character found
    ///          is larger in the second memory buffer.
    ///
    /// @see StringCompare()
    template< typename T >
    int StringNCompare( const T* pString0, const T* pString1, size_t count )
    {
        L_ASSERT( pString0 );
        L_ASSERT( pString1 );

        for( ; count != 0; --count )
        {
            int char0 = *pString0;
            int char1 = *pString1;
            int charDiff = char0 - char1;
            if( charDiff != 0 || char0 == static_cast< T >( 0 ) )
            {
                return charDiff;
            }

            ++pString0;
            ++pString1;
        }

        return 0;
    }

    /// Write a formatted string into a given buffer.
    ///
    /// @param[in] pBuffer     Buffer into which the formatted string should be written.
    /// @param[in] bufferSize  Total number of elements that can be stored in the given buffer (including a null
    ///                        terminator).
    /// @param[in] pFormat     Format string.
    /// @param[in] ...         Format arguments.
    ///
    /// @return  The number of characters that need to be written for the final formatted string, not including the null
    ///          terminator.  If the result string does not fit in the buffer, the contents will be truncated, and no
    ///          null terminator will be written.  If an error occurred while writing the formatted string, -1 is
    ///          returned.
    int StringFormat( char* pBuffer, size_t bufferSize, const char* pFormat, ... )
    {
        L_ASSERT( pBuffer || bufferSize == 0 );
        L_ASSERT( pFormat );

        va_list argList;
        va_start( argList, pFormat );
        int result = StringFormatVa( pBuffer, bufferSize, pFormat, argList );
        va_end( argList );

        return result;
    }

    /// Write a formatted string into a given buffer.
    ///
    /// @param[in] pBuffer     Buffer into which the formatted string should be written.
    /// @param[in] bufferSize  Total number of elements that can be stored in the given buffer (including a null
    ///                        terminator).
    /// @param[in] pFormat     Format string.
    /// @param[in] ...         Format arguments.
    ///
    /// @return  The number of characters that need to be written for the final formatted string, not including the null
    ///          terminator.  If the result string does not fit in the buffer, the contents will be truncated, and no
    ///          null terminator will be written.  If an error occurred while writing the formatted string, -1 is
    ///          returned.
    int StringFormat( wchar_t* pBuffer, size_t bufferSize, const wchar_t* pFormat, ... )
    {
        L_ASSERT( pBuffer || bufferSize == 0 );
        L_ASSERT( pFormat );

        va_list argList;
        va_start( argList, pFormat );
        int result = StringFormatVa( pBuffer, bufferSize, pFormat, argList );
        va_end( argList );

        return result;
    }

#if HELIUM_CC_MSC
// We don't use the secure CRT versions of vsnprintf() and _vsnwprintf() here since we can't use them to compute the
// size that would be needed for a format string if it doesn't fit (vsnprintf_s() and _vsnwprintf_s() simply return -1
// if the string doesn't fit, even if _TRUNCATE is specified for the character count).
#pragma warning( push )
#pragma warning( disable : 4996 )  // 'function': This function or variable may be unsafe.
#endif
    /// Write a formatted string into a given buffer.
    ///
    /// @param[in] pBuffer     Buffer into which the formatted string should be written.
    /// @param[in] bufferSize  Total number of elements that can be stored in the given buffer (including a null
    ///                        terminator).
    /// @param[in] pFormat     Format string.
    /// @param[in] argList     Initialized variable argument list for the format arguments.
    ///
    /// @return  The number of characters that need to be written for the final formatted string, not including the null
    ///          terminator.  If the result string does not fit in the buffer, the contents will be truncated, and no
    ///          null terminator will be written.  If an error occurred while writing the formatted string, -1 is
    ///          returned.
    int StringFormatVa( char* pBuffer, size_t bufferSize, const char* pFormat, va_list argList )
    {
        L_ASSERT( pBuffer || bufferSize == 0 );
        L_ASSERT( pFormat );
        int result = vsnprintf( pBuffer, bufferSize, pFormat, argList );
        return result;
    }

    /// Write a formatted string into a given buffer.
    ///
    /// @param[in] pBuffer     Buffer into which the formatted string should be written.
    /// @param[in] bufferSize  Total number of elements that can be stored in the given buffer (including a null
    ///                        terminator).
    /// @param[in] pFormat     Format string.
    /// @param[in] argList     Initialized variable argument list for the format arguments.
    ///
    /// @return  The number of characters that need to be written for the final formatted string, not including the null
    ///          terminator.  If the result string does not fit in the buffer, the contents will be truncated, and no
    ///          null terminator will be written.  If an error occurred while writing the formatted string, -1 is
    ///          returned.
    int StringFormatVa( wchar_t* pBuffer, size_t bufferSize, const wchar_t* pFormat, va_list argList )
    {
        L_ASSERT( pBuffer || bufferSize == 0 );
        L_ASSERT( pFormat );
        int result = _vsnwprintf( pBuffer, bufferSize, pFormat, argList );
        return result;
    }
#if HELIUM_CC_MSC
#pragma warning( pop )
#endif

    /// Compute a 32-bit hash value for a string.
    ///
    /// @param[in] pString  Null-terminated C-string (can be null).
    ///
    /// @return  32-bit hash for the given string.
    template< typename T >
    uint32_t StringHash( const T* pString )
    {
        // djb2...GO!
        uint32_t hash = 5381;
        if( pString )
        {
            for( T character = *pString; character != static_cast< T >( '\0' ); character = *( ++pString ) )
            {
                hash = ( ( hash * 33 ) ^ static_cast< uint32_t >( character ) );
            }
        }

        return hash;
    }

    /// Get the invalid index value for the template integer type.
    ///
    /// @return  Invalid index value.
    ///
    /// @see IsValid(), IsInvalid(), SetInvalid()
    template< typename IndexType >
    IndexType Invalid()
    {
        // NOT operator implicitly casts 8-bit types to ints, so we will wrap the result with another static cast to the
        // index type.
        return static_cast< IndexType >( ~static_cast< IndexType >( 0 ) );
    }

    /// Get whether the given value is a valid index value.
    ///
    /// @param[in] index  Index value.
    ///
    /// @return  True if the index is valid, false if not.
    ///
    /// @see Invalid(), IsInvalid(), SetInvalid()
    template< typename IndexType >
    bool IsValid( IndexType index )
    {
        return( index != Invalid< typename boost::remove_cv< IndexType >::type >() );
    }

    /// Get whether the given value is an invalid index value.
    ///
    /// @param[in] index  Index value.
    ///
    /// @return  True if the index is invalid, false if it is valid.
    ///
    /// @see Invalid(), IsValid(), SetInvalid()
    template< typename IndexType >
    bool IsInvalid( IndexType index )
    {
        return( index == Invalid< typename boost::remove_cv< IndexType >::type >() );
    }

    /// Set the given integer to the reserved invalid index value.
    ///
    /// @param[in] rIndex  Integer to set.
    ///
    /// @see Invalid(), IsValid(), IsInvalid()
    template< typename IndexType >
    void SetInvalid( IndexType& rIndex )
    {
        rIndex = Invalid< typename boost::remove_cv< IndexType >::type >();
    }

    /// Cast an index value to a different integer type, retaining invalid index values.
    ///
    /// @param[in] index  Index to cast.
    ///
    /// @return  Cast index type, with invalid values properly retained when casting to larger types.
    template< typename DestIndexType, typename SourceIndexType >
    DestIndexType CastIndex( SourceIndexType index )
    {
        return _CastIndex< DestIndexType >(
            index,
            boost::integral_constant< bool, ( sizeof( DestIndexType ) > sizeof( SourceIndexType ) ) >() );
    }

    /// Get the integer array offset and the mask bit index for manipulating the specified bit in a bit array.
    ///
    /// @param[in]  bitIndex       Bit index.
    /// @param[out] rElementIndex  Index of the array element containing the specified bit.
    /// @param[out] rMaskIndex     Index of the bit within the array element.
    template< typename ElementType >
    void GetBitElementAndMaskIndex(
        size_t bitIndex,
        size_t& rElementIndex,
        size_t& rMaskIndex )
    {
        rElementIndex = bitIndex / ( sizeof( ElementType ) * 8 );
        rMaskIndex = bitIndex % ( sizeof( ElementType ) * 8 );
    }

    /// Get the state of a bit in the given integer element.
    ///
    /// @param[in] rElement   Integer to test.
    /// @param[in] maskIndex  Index of the bit to test.
    ///
    /// @return  True if the bit is set, false if it is unset.
    ///
    /// @see SetBit(), ClearBit(), ToggleBit()
    template< typename ElementType >
    bool GetBit( const ElementType& rElement, size_t maskIndex )
    {
        return( ( rElement & ( static_cast< ElementType >( 1 ) << maskIndex ) ) != 0 );
    }

    /// Set a bit in the given integer element.
    ///
    /// @param[in] rElement   Integer to update.
    /// @param[in] maskIndex  Index of the bit to set.
    ///
    /// @see ClearBit(), ToggleBit(), GetBit()
    template< typename ElementType >
    void SetBit( ElementType& rElement, size_t maskIndex )
    {
        rElement |= ( static_cast< ElementType >( 1 ) << maskIndex );
    }

    /// Unset a bit in the given integer element.
    ///
    /// @param[in] rElement   Integer to update.
    /// @param[in] maskIndex  Index of the bit to set.
    ///
    /// @see SetBit(), ToggleBit(), GetBit()
    template< typename ElementType >
    void ClearBit( ElementType& rElement, size_t maskIndex )
    {
        rElement &= ~( static_cast< ElementType >( 1 ) << maskIndex );
    }

    /// Toggle a bit in the given integer element.
    ///
    /// @param[in] rElement   Integer to update.
    /// @param[in] maskIndex  Index of the bit to toggle.
    ///
    /// @see SetBit(), ClearBit(), GetBit()
    template< typename ElementType >
    void ToggleBit( ElementType& rElement, size_t maskIndex )
    {
        rElement ^= ( static_cast< ElementType >( 1 ) << maskIndex );
    }

    /// Set a range of bits in a bit array.
    ///
    /// @param[in] pElements  Bit array to update.
    /// @param[in] bitStart   Index of the first bit to set.
    /// @param[in] bitCount   Number of bits to set.
    ///
    /// @see ClearBitRange(), ToggleBitRange()
    template< typename ElementType >
    void SetBitRange( ElementType* pElements, size_t bitStart, size_t bitCount )
    {
        if( bitCount == 0 )
        {
            return;
        }

        L_ASSERT( pElements );

        size_t startElementIndex, startMaskIndex;
        GetBitElementAndMaskIndex< ElementType >( bitStart, startElementIndex, startMaskIndex );

        size_t endElementIndex, endMaskIndex;
        GetBitElementAndMaskIndex< ElementType >( bitStart + bitCount - 1, endElementIndex, endMaskIndex );

        if( startMaskIndex != 0 )
        {
            if( startElementIndex == endElementIndex )
            {
                ElementType mask =
                    ( ( static_cast< ElementType >( 1 ) << ( endMaskIndex + 1 ) ) - 1 ) &
                    ~( ( static_cast< ElementType >( 1 ) << startMaskIndex ) - 1 );
                pElements[ startElementIndex ] |= mask;

                return;
            }

            ElementType mask = ~( ( static_cast< ElementType >( 1 ) << startMaskIndex ) - 1 );
            pElements[ startElementIndex ] |= mask;

            ++startElementIndex;
        }

        MemorySet(
            pElements + startElementIndex,
            0xff,
            ( endElementIndex - startElementIndex ) * sizeof( ElementType ) );

        ElementType mask = ( static_cast< ElementType >( 1 ) << ( endMaskIndex + 1 ) ) - 1;
        pElements[ endElementIndex ] |= mask;
    }

    /// Unset a range of bits in a bit array.
    ///
    /// @param[in] pElements  Bit array to update.
    /// @param[in] bitStart   Index of the first bit to set.
    /// @param[in] bitCount   Number of bits to set.
    ///
    /// @see SetBitRange(), ToggleBitRange()
    template< typename ElementType >
    void ClearBitRange( ElementType* pElements, size_t bitStart, size_t bitCount )
    {
        if( bitCount == 0 )
        {
            return;
        }

        L_ASSERT( pElements );

        size_t startElementIndex, startMaskIndex;
        GetBitElementAndMaskIndex< ElementType >( bitStart, startElementIndex, startMaskIndex );

        size_t endElementIndex, endMaskIndex;
        GetBitElementAndMaskIndex< ElementType >( bitStart + bitCount - 1, endElementIndex, endMaskIndex );

        if( startMaskIndex != 0 )
        {
            if( startElementIndex == endElementIndex )
            {
                ElementType mask =
                    ~( ( static_cast< ElementType >( 1 ) << ( endMaskIndex + 1 ) ) - 1 ) |
                    ( ( static_cast< ElementType >( 1 ) << startMaskIndex ) - 1 );
                pElements[ startElementIndex ] &= mask;

                return;
            }

            ElementType mask = ( static_cast< ElementType >( 1 ) << startMaskIndex ) - 1;
            pElements[ startElementIndex ] &= mask;

            ++startElementIndex;
        }

        MemoryZero( pElements + startElementIndex, ( endElementIndex - startElementIndex ) * sizeof( ElementType ) );

        ElementType mask = ~( ( static_cast< ElementType >( 1 ) << ( endMaskIndex + 1 ) ) - 1 );
        pElements[ endElementIndex ] &= mask;
    }

    /// Toggle a range of bits in a bit array.
    ///
    /// @param[in] pElements  Bit array to update.
    /// @param[in] bitStart   Index of the first bit to set.
    /// @param[in] bitCount   Number of bits to set.
    ///
    /// @see SetBitRange(), ClearBitRange()
    template< typename ElementType >
    void ToggleBitRange( ElementType* pElements, size_t bitStart, size_t bitCount )
    {
        if( bitCount == 0 )
        {
            return;
        }

        L_ASSERT( pElements );

        size_t startElementIndex, startMaskIndex;
        GetBitElementAndMaskIndex< ElementType >( bitStart, startElementIndex, startMaskIndex );

        size_t endElementIndex, endMaskIndex;
        GetBitElementAndMaskIndex< ElementType >( bitStart + bitCount - 1, endElementIndex, endMaskIndex );

        if( startMaskIndex != 0 )
        {
            if( startElementIndex == endElementIndex )
            {
                ElementType mask =
                    ( ( static_cast< ElementType >( 1 ) << ( endMaskIndex + 1 ) ) - 1 ) &
                    ~( ( static_cast< ElementType >( 1 ) << startMaskIndex ) - 1 );
                pElements[ startElementIndex ] ^= mask;

                return;
            }

            ElementType mask = ~( ( static_cast< ElementType >( 1 ) << startMaskIndex ) - 1 );
            pElements[ startElementIndex ] ^= mask;

            ++startElementIndex;
        }

        ElementType mask = ~static_cast< ElementType >( 0 );
        for( size_t elementIndex = startElementIndex; elementIndex < endElementIndex; ++elementIndex )
        {
            pElements[ elementIndex ] ^= mask;
        }

        mask = ( static_cast< ElementType >( 1 ) << ( endMaskIndex + 1 ) ) - 1;
        pElements[ endElementIndex ] ^= mask;
    }

    /// Load a value from a byte stream without performing byte swapping.
    ///
    /// @param[out] rDest    Value read from the byte stream.
    /// @param[in]  pSource  Byte stream from which to read the data.
    ///
    /// @return  Pointer to the memory location directly after the data read from the input byte stream.
    ///
    /// @see LoadValueSwapped()
    template< typename T >
    const void* LoadValue( T& rDest, const void* pSource )
    {
        L_ASSERT( pSource );

        MemoryCopy( &rDest, pSource, sizeof( T ) );

        return static_cast< const uint8_t* >( pSource ) + sizeof( T );
    }

    /// Load a value from a byte stream, reversing the byte order of the read value.
    ///
    /// @param[out] rDest    Value read from the byte stream.
    /// @param[in]  pSource  Byte stream from which to read the data.
    ///
    /// @return  Pointer to the memory location directly after the data read from the input byte stream.
    ///
    /// @see LoadValue(), ReverseByteOrder()
    template< typename T >
    const void* LoadValueSwapped( T& rDest, const void* pSource )
    {
        L_ASSERT( pSource );

        ReverseByteOrder( &rDest, pSource, sizeof( T ) );

        return static_cast< const uint8_t* >( pSource ) + sizeof( T );
    }

    /// Reverse the order of each byte in the source memory buffer, storing the result in the destination buffer.
    ///
    /// Note that in-place byte swapping (where the source and destination addresses are the same) is supported.
    ///
    /// @param[out] pDest    Byte swapped data.
    /// @param[in]  pSource  Byte stream from which to read the data.
    /// @param[in]  size     Number of bytes to copy and swap.
    ///
    /// @see LoadValueSwapped()
    void ReverseByteOrder( void* pDest, const void* pSource, size_t size )
    {
        L_ASSERT( pDest );
        L_ASSERT( pSource );

        const uint8_t* pSourceLow = static_cast< const uint8_t* >( pSource );
        const uint8_t* pSourceHigh = pSourceLow + size;

        uint8_t* pDestLow = static_cast< uint8_t* >( pDest );
        uint8_t* pDestHigh = pDestLow + size;

        size_t halfSize = ( size + 1 ) / 2;

        for( size_t byteIndex = 0; byteIndex < halfSize; ++byteIndex )
        {
            --pSourceHigh;
            --pDestHigh;

            uint8_t byteLow = *pSourceLow;
            uint8_t byteHigh = *pSourceHigh;

            *pDestLow = byteHigh;
            *pDestHigh = byteLow;

            ++pSourceLow;
            ++pDestLow;
        }
    }

    /// Constructor.
    NonCopyable::NonCopyable()
    {
    }
}
