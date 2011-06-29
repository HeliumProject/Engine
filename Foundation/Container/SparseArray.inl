/// Constructor.
///
/// Creates an uninitialized iterator.  Using this is not safe until it is initialized.
template< typename T >
Helium::ConstSparseArrayIterator< T >::ConstSparseArrayIterator()
{
}

/// Constructor.
///
/// @param[in] pElement          Pointer to the current array element.
/// @param[in] pUsedElementBits  Pointer to the current element of the in-use bit array.
/// @param[in] usedElementMask   Bit mask for the in-use bit for the current array element.
template< typename T >
Helium::ConstSparseArrayIterator< T >::ConstSparseArrayIterator(
    const T* pElement,
    const uint32_t* pUsedElementBits,
    uint32_t usedElementMask )
    : m_pElement( const_cast< T* >( pElement ) )
    , m_pUsedElementBits( const_cast< uint32_t* >( pUsedElementBits ) )
    , m_usedElementMask( usedElementMask )
{
    HELIUM_ASSERT( ( usedElementMask & ( usedElementMask - 1 ) ) == 0 );
}

/// Get whether this iterator is pointing to a valid array element.
///
/// @return  True if the current array element is valid, false if not.
template< typename T >
Helium::ConstSparseArrayIterator< T >::operator bool() const
{
    return ( ( *m_pUsedElementBits & m_usedElementMask ) != 0 );
}

/// Access the current array element.
///
/// Note that an iterator could be referencing an unused array slot.  You should always check whether the iterator is
/// referencing a used array slot first using the "bool" operator overload.
///
/// @return  Constant reference to the current array element.
template< typename T >
typename Helium::ConstSparseArrayIterator< T >::ConstReferenceType
    Helium::ConstSparseArrayIterator< T >::operator*() const
{
    HELIUM_ASSERT( static_cast< bool >( *this ) );
    return *( static_cast< bool >( *this ) ? m_pElement : static_cast< const T* >( NULL ) );
}

/// Access the current array element.
///
/// Note that an iterator could be referencing an unused array slot.  You should always check whether the iterator is
/// referencing a used array slot first using the "bool" operator overload.
///
/// @return  Constant pointer to the current array element.
template< typename T >
typename Helium::ConstSparseArrayIterator< T >::ConstPointerType
    Helium::ConstSparseArrayIterator< T >::operator->() const
{
    HELIUM_ASSERT( static_cast< bool >( *this ) );
    return ( static_cast< bool >( *this ) ? m_pElement : static_cast< const T* >( NULL ) );
}

/// Pre-increment operator.
///
/// Increments this iterator's position by one.
///
/// @return  Reference to this iterator.
template< typename T >
Helium::ConstSparseArrayIterator< T >& Helium::ConstSparseArrayIterator< T >::operator++()
{
    ++m_pElement;

    m_usedElementMask <<= 1;
    if( !m_usedElementMask )
    {
        ++m_pUsedElementBits;
        m_usedElementMask = 1U;
    }

    return *this;
}

/// Post-increment operator.
///
/// Increments this iterator's position by one.
///
/// @return  Copy of this iterator prior to incrementing.
template< typename T >
Helium::ConstSparseArrayIterator< T > Helium::ConstSparseArrayIterator< T >::operator++( int )
{
    ConstSparseArrayIterator result = *this;
    operator++();

    return result;
}

/// Pre-decrement operator.
///
/// Decrements this iterator's position by one.
///
/// @return  Reference to this iterator.
template< typename T >
Helium::ConstSparseArrayIterator< T >& Helium::ConstSparseArrayIterator< T >::operator--()
{
    --m_pElement;

    m_usedElementMask >>= 1;
    if( !m_usedElementMask )
    {
        --m_pUsedElementBits;
        m_usedElementMask = ( 1U << ( sizeof( uint32_t ) * 8 - 1 ) );
    }

    return *this;
}

/// Post-decrement operator.
///
/// Decrements this iterator's position by one.
///
/// @return  Copy of this iterator prior to decrementing.
template< typename T >
Helium::ConstSparseArrayIterator< T > Helium::ConstSparseArrayIterator< T >::operator--( int )
{
    ConstSparseArrayIterator result = *this;
    operator--();

    return result;
}

/// In-place addition operator.
///
/// @param[in] offset  Amount by which to increase this iterator's position.
///
/// @return  Reference to this iterator.
template< typename T >
Helium::ConstSparseArrayIterator< T >& Helium::ConstSparseArrayIterator< T >::operator+=( ptrdiff_t offset )
{
    m_pElement += offset;

    size_t elementBitsOffset = offset / ( sizeof( uint32_t ) * 8 );
    size_t elementMaskOffset = offset % ( sizeof( uint32_t ) * 8 );

    uint32_t newMask = ( m_usedElementMask << elementMaskOffset );
    if( !newMask )
    {
        ++elementBitsOffset;
        newMask = ( m_usedElementMask >> ( sizeof( uint32_t ) * 8 - elementMaskOffset - 1 ) );
    }

    m_pUsedElementBits += elementBitsOffset;
    m_usedElementMask = newMask;

    return *this;
}

/// Addition operator.
///
/// @param[in] offset  Amount by which to increase this iterator's position.
///
/// @return  Copy of this iterator, increased by the specified amount.
template< typename T >
Helium::ConstSparseArrayIterator< T > Helium::ConstSparseArrayIterator< T >::operator+( ptrdiff_t offset ) const
{
    ConstSparseArrayIterator result = *this;
    result += offset;

    return result;
}

/// In-place subtraction operator.
///
/// @param[in] offset  Amount by which to decrease this iterator's position.
///
/// @return  Reference to this iterator.
template< typename T >
Helium::ConstSparseArrayIterator< T >& Helium::ConstSparseArrayIterator< T >::operator-=( ptrdiff_t offset )
{
    m_pElement -= offset;

    size_t elementBitsOffset = offset / ( sizeof( uint32_t ) * 8 );
    size_t elementMaskOffset = offset % ( sizeof( uint32_t ) * 8 );

    uint32_t newMask = ( m_usedElementMask >> elementMaskOffset );
    if( !newMask )
    {
        ++elementBitsOffset;
        newMask = ( m_usedElementMask << ( sizeof( uint32_t ) * 8 - elementMaskOffset - 1 ) );
    }

    m_pUsedElementBits -= elementBitsOffset;
    m_usedElementMask = newMask;

    return *this;
}

/// Subtraction operator.
///
/// @param[in] offset  Amount by which to decrease this iterator's position.
///
/// @return  Copy of this iterator, decreased by the specified amount.
template< typename T >
Helium::ConstSparseArrayIterator< T > Helium::ConstSparseArrayIterator< T >::operator-( ptrdiff_t offset ) const
{
    ConstSparseArrayIterator result = *this;
    result -= offset;

    return result;
}

/// Subtraction operator.
///
/// @param[in] rOther  Bit array iterator to subtract.
///
/// @return  Offset from the given bit array iterator to this iterator.
template< typename T >
ptrdiff_t Helium::ConstSparseArrayIterator< T >::operator-( const ConstSparseArrayIterator& rOther ) const
{
    return( m_pElement - rOther.m_pElement );
}

/// Equality comparison operator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator and the given iterator reference the same element, false if not.
template< typename T >
bool Helium::ConstSparseArrayIterator< T >::operator==( const ConstSparseArrayIterator& rOther ) const
{
    return( m_pElement == rOther.m_pElement );
}

/// Inequality comparison operator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator and the given iterator do not reference the same element, false if they do.
template< typename T >
bool Helium::ConstSparseArrayIterator< T >::operator!=( const ConstSparseArrayIterator& rOther ) const
{
    return( m_pElement != rOther.m_pElement );
}

/// Less-than comparison operator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator precedes the given iterator, false if not.
template< typename T >
bool Helium::ConstSparseArrayIterator< T >::operator<( const ConstSparseArrayIterator& rOther ) const
{
    return( m_pElement < rOther.m_pElement );
}

/// Greater-than comparison operator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator follows the given iterator, false if not.
template< typename T >
bool Helium::ConstSparseArrayIterator< T >::operator>( const ConstSparseArrayIterator& rOther ) const
{
    return( m_pElement > rOther.m_pElement );
}

/// Less-than-or-equals comparison operator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator precedes or matches the given iterator, false if not.
template< typename T >
bool Helium::ConstSparseArrayIterator< T >::operator<=( const ConstSparseArrayIterator& rOther ) const
{
    return( m_pElement <= rOther.m_pElement );
}

/// Greater-than-or-equals comparison operator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator follows or matches the given iterator, false if not.
template< typename T >
bool Helium::ConstSparseArrayIterator< T >::operator>=( const ConstSparseArrayIterator& rOther ) const
{
    return( m_pElement >= rOther.m_pElement );
}

/// Constructor.
///
/// Creates an uninitialized iterator.  Using this is not safe until it is initialized.
template< typename T >
Helium::SparseArrayIterator< T >::SparseArrayIterator()
{
}

/// Constructor.
///
/// @param[in] pElement          Pointer to the current array element.
/// @param[in] pUsedElementBits  Pointer to the current element of the in-use bit array.
/// @param[in] usedElementMask   Bit mask for the in-use bit for the current array element.
template< typename T >
Helium::SparseArrayIterator< T >::SparseArrayIterator(
    T* pElement,
    uint32_t* pUsedElementBits,
    uint32_t usedElementMask )
    : ConstSparseArrayIterator< T >( pElement, pUsedElementBits, usedElementMask )
{
}

/// Access the current array element.
///
/// Note that an iterator could be referencing an unused array slot.  You should always check whether the iterator is
/// referencing a used array slot first using the "bool" operator overload.
///
/// @return  Reference to the current array element.
template< typename T >
typename Helium::SparseArrayIterator< T >::ReferenceType Helium::SparseArrayIterator< T >::operator*() const
{
    HELIUM_ASSERT( static_cast< bool >( *this ) );
    return *( static_cast< bool >( *this )
              ? ConstSparseArrayIterator< T >::m_pElement
              : static_cast< T* >( NULL ) );
}

/// Access the current array element.
///
/// Note that an iterator could be referencing an unused array slot.  You should always check whether the iterator is
/// referencing a used array slot first using the "bool" operator overload.
///
/// @return  Pointer to the current array element.
template< typename T >
typename Helium::SparseArrayIterator< T >::PointerType Helium::SparseArrayIterator< T >::operator->() const
{
    HELIUM_ASSERT( static_cast< bool >( *this ) );
    return ( static_cast< bool >( *this ) ? ConstSparseArrayIterator< T >::m_pElement : static_cast< T* >( NULL ) );
}

/// Pre-increment operator.
///
/// Increments this iterator's position by one.
///
/// @return  Reference to this iterator.
template< typename T >
Helium::SparseArrayIterator< T >& Helium::SparseArrayIterator< T >::operator++()
{
    ConstSparseArrayIterator< T >::operator++();

    return *this;
}

/// Post-increment operator.
///
/// Increments this iterator's position by one.
///
/// @return  Copy of this iterator prior to incrementing.
template< typename T >
Helium::SparseArrayIterator< T > Helium::SparseArrayIterator< T >::operator++( int )
{
    SparseArrayIterator result = *this;
    operator++();

    return result;
}

/// Pre-decrement operator.
///
/// Decrements this iterator's position by one.
///
/// @return  Reference to this iterator.
template< typename T >
Helium::SparseArrayIterator< T >& Helium::SparseArrayIterator< T >::operator--()
{
    ConstSparseArrayIterator< T >::operator--();

    return *this;
}

/// Post-decrement operator.
///
/// Decrements this iterator's position by one.
///
/// @return  Copy of this iterator prior to decrementing.
template< typename T >
Helium::SparseArrayIterator< T > Helium::SparseArrayIterator< T >::operator--( int )
{
    SparseArrayIterator result = *this;
    operator--();

    return result;
}

/// In-place addition operator.
///
/// @param[in] offset  Amount by which to increase this iterator's position.
///
/// @return  Reference to this iterator.
template< typename T >
Helium::SparseArrayIterator< T >& Helium::SparseArrayIterator< T >::operator+=( ptrdiff_t offset )
{
    ConstSparseArrayIterator< T >::operator+=( offset );

    return *this;
}

/// Addition operator.
///
/// @param[in] offset  Amount by which to increase this iterator's position.
///
/// @return  Copy of this iterator, increased by the specified amount.
template< typename T >
Helium::SparseArrayIterator< T > Helium::SparseArrayIterator< T >::operator+( ptrdiff_t offset ) const
{
    SparseArrayIterator result = *this;
    result += offset;

    return result;
}

/// In-place subtraction operator.
///
/// @param[in] offset  Amount by which to decrease this iterator's position.
///
/// @return  Reference to this iterator.
template< typename T >
Helium::SparseArrayIterator< T >& Helium::SparseArrayIterator< T >::operator-=( ptrdiff_t offset )
{
    ConstSparseArrayIterator< T >::operator-=( offset );

    return *this;
}

/// Subtraction operator.
///
/// @param[in] offset  Amount by which to decrease this iterator's position.
///
/// @return  Copy of this iterator, decreased by the specified amount.
template< typename T >
Helium::SparseArrayIterator< T > Helium::SparseArrayIterator< T >::operator-( ptrdiff_t offset ) const
{
    SparseArrayIterator result = *this;
    result -= offset;

    return result;
}

/// Constructor.
///
/// This creates an empty array.  No memory is allocated at this time.
template< typename T, typename Allocator >
Helium::SparseArray< T, Allocator >::SparseArray()
    : m_pBuffer( NULL )
    , m_pUsedElements( NULL )
    , m_size( 0 )
    , m_usedSize( 0 )
    , m_capacity( 0 )
    , m_lastUpdateIndex( 0 )
{
}

/// Constructor.
///
/// This creates a copy of the given array.
///
/// @param[in] pSource  Array from which to copy.
/// @param[in] size     Number of elements in the given array.
template< typename T, typename Allocator >
Helium::SparseArray< T, Allocator >::SparseArray( const T* pSource, size_t size )
    : m_pBuffer( NULL )
    , m_pUsedElements( NULL )
    , m_size( size )
    , m_usedSize( size )
    , m_capacity( size )
    , m_lastUpdateIndex( 0 )
{
    HELIUM_ASSERT( pSource );
    if( size != 0 )
    {
        m_pBuffer = Allocate( m_size );
        HELIUM_ASSERT( m_pBuffer );
        ArrayUninitializedCopy( m_pBuffer, pSource, size );

        size_t bitArrayByteCount = Align( size, sizeof( uint32_t ) * 8 ) / 8;
        m_pUsedElements = static_cast< uint32_t* >( m_allocator.Allocate( bitArrayByteCount ) );
        HELIUM_ASSERT( m_pUsedElements );
        MemorySet( m_pUsedElements, 0xff, bitArrayByteCount );
    }
}

/// Copy constructor.
///
/// When copying, only the memory needed to hold onto the used contents of the source array will be allocated (i.e. if
/// the source array has 10 elements but a capacity of 20, only memory for the 10 used elements will be allocated for
/// this copy).  Holes will be compacted as well.
///
/// @param[in] rSource  Array from which to copy.
template< typename T, typename Allocator >
template< typename OtherAllocator >
Helium::SparseArray< T, Allocator >::SparseArray( const SparseArray< T, OtherAllocator >& rSource )
    : m_pBuffer( NULL )
    , m_pUsedElements( NULL )
    , m_size( 0 )
    , m_usedSize( 0 )
    , m_capacity( 0 )
    , m_lastUpdateIndex( 0 )
{
    size_t usedSize = rSource.m_usedSize;
    if( usedSize != 0 )
    {
        Reserve( usedSize );

        typename SparseArray< T, OtherAllocator >::ConstIterator sourceEnd = rSource.End();
        typename SparseArray< T, OtherAllocator >::ConstIterator sourceIter;
        for( sourceIter = rSource.Begin(); sourceIter != sourceEnd; ++sourceIter )
        {
            if( sourceIter )
            {
                Add( *sourceIter );
            }
        }

        HELIUM_ASSERT( m_size == usedSize );
        HELIUM_ASSERT( m_usedSize == usedSize );
    }
}

/// Destructor.
template< typename T, typename Allocator >
Helium::SparseArray< T, Allocator >::~SparseArray()
{
    InPlaceDestroy( m_pBuffer, m_size, m_pUsedElements );
    m_allocator.Free( m_pBuffer );
    m_allocator.Free( m_pUsedElements );
}

/// Get the size of this array, including internal holes.
///
/// This can be used to determine the total range of assigned elements in this array, which can be useful for
/// iterating.  In order to retrieve the actual number of elements used, use GetUsedSize().
///
/// @return  Array size.
///
/// @see GetUsedSize(), GetCapacity()
template< typename T, typename Allocator >
size_t Helium::SparseArray< T, Allocator >::GetSize() const
{
    return m_size;
}

/// Get the number of occupied elements.
///
/// @return  Number of used elements.
///
/// @see GetSize(), GetCapacity()
template< typename T, typename Allocator >
size_t Helium::SparseArray< T, Allocator >::GetUsedSize() const
{
    return m_usedSize;
}

/// Get the maximum number of elements which this array can contain without requiring reallocation of memory.
///
/// @return  Current array capacity.
///
/// @see GetSize(), GetUsedSize()
template< typename T, typename Allocator >
size_t Helium::SparseArray< T, Allocator >::GetCapacity() const
{
    return m_capacity;
}

/// Explicitly increase the capacity of this array to support at least the specified number of elements.
///
/// If the requested capacity is less than the current capacity, no memory will be reallocated.
///
/// @param[in] capacity  Desired capacity.
///
/// @see GetCapacity()
template< typename T, typename Allocator >
void Helium::SparseArray< T, Allocator >::Reserve( size_t capacity )
{
    if( capacity > m_capacity )
    {
        m_pBuffer = ResizeBuffer( m_pBuffer, m_size, m_pUsedElements, m_capacity, capacity );
        HELIUM_ASSERT( m_pBuffer );

        size_t bitArrayByteCount = Align( capacity, sizeof( uint32_t ) * 8 ) / 8;
        m_pUsedElements = static_cast< uint32_t* >( m_allocator.Reallocate( m_pUsedElements, bitArrayByteCount ) );
        HELIUM_ASSERT( m_pUsedElements );

        m_capacity = capacity;
    }
}

/// Resize the allocated array memory to match the size actually in use. 
///
/// @see GetCapacity()
template< typename T, typename Allocator >
void Helium::SparseArray< T, Allocator >::Trim()
{
    if( m_capacity != m_size )
    {
        m_pBuffer = ResizeBuffer( m_pBuffer, m_size, m_pUsedElements, m_capacity, m_size );
        HELIUM_ASSERT( m_pBuffer || m_size == 0 );

        size_t bitArrayByteCount = Align( m_size, sizeof( uint32_t ) * 8 ) / 8;
        m_pUsedElements = static_cast< uint32_t* >( m_allocator.Reallocate( m_pUsedElements, bitArrayByteCount ) );
        HELIUM_ASSERT( m_pUsedElements || bitArrayByteCount == 0 );

        m_capacity = m_size;
    }
}

/// Get a pointer to the base of the allocated array buffer.
///
/// @return  Array buffer.
template< typename T, typename Allocator >
T* Helium::SparseArray< T, Allocator >::GetData()
{
    return m_pBuffer;
}

/// Get a pointer to the base of the allocated array buffer.
///
/// @return  Array buffer.
template< typename T, typename Allocator >
const T* Helium::SparseArray< T, Allocator >::GetData() const
{
    return m_pBuffer;
}

/// Get a pointer to the base of the bit array specifying which elements are in use.
///
/// @return  Bit array specifying which elements are in use.
template< typename T, typename Allocator >
const uint32_t* Helium::SparseArray< T, Allocator >::GetUsedElements() const
{
    return m_pUsedElements;
}

/// Resize the array to zero and free all allocated memory.
template< typename T, typename Allocator >
void Helium::SparseArray< T, Allocator >::Clear()
{
    InPlaceDestroy( m_pBuffer, m_size, m_pUsedElements );
    m_allocator.Free( m_pBuffer );
    m_pBuffer = NULL;

    m_allocator.Free( m_pUsedElements );
    m_pUsedElements = NULL;

    m_size = 0;
    m_usedSize = 0;
    m_capacity = 0;

    m_lastUpdateIndex = 0;
}

/// Retrieve an iterator referencing the beginning of this array.
///
/// @return  Iterator at the beginning of this array.
///
/// @see End()
template< typename T, typename Allocator >
typename Helium::SparseArray< T, Allocator >::Iterator Helium::SparseArray< T, Allocator >::Begin()
{
    return Iterator( m_pBuffer, m_pUsedElements, 1 );
}

/// Retrieve a constant iterator referencing the beginning of this array.
///
/// @return  Constant iterator at the beginning of this array.
///
/// @see End()
template< typename T, typename Allocator >
typename Helium::SparseArray< T, Allocator >::ConstIterator Helium::SparseArray< T, Allocator >::Begin() const
{
    return ConstIterator( m_pBuffer, m_pUsedElements, 1 );
}

/// Retrieve an iterator referencing the end of this array.
///
/// @return  Iterator at the end of this array.
///
/// @see Begin()
template< typename T, typename Allocator >
typename Helium::SparseArray< T, Allocator >::Iterator Helium::SparseArray< T, Allocator >::End()
{
    size_t size = m_size;

    size_t elementIndex, maskIndex;
    GetBitElementAndMaskIndex< uint32_t >( size, elementIndex, maskIndex );

    return Iterator( m_pBuffer + m_size, m_pUsedElements + elementIndex, ( 1 << maskIndex ) );
}

/// Retrieve a constant iterator referencing the end of this array.
///
/// @return  Constant iterator at the end of this array.
///
/// @see Begin()
template< typename T, typename Allocator >
typename Helium::SparseArray< T, Allocator >::ConstIterator Helium::SparseArray< T, Allocator >::End() const
{
    size_t size = m_size;

    size_t elementIndex, maskIndex;
    GetBitElementAndMaskIndex< uint32_t >( size, elementIndex, maskIndex );

    return ConstIterator( m_pBuffer + m_size, m_pUsedElements + elementIndex, ( 1 << maskIndex ) );
}

/// Get whether the array slot at the specified index is in use.
///
/// @return  True if the array element is in use, false if not.
///
/// @see GetElement(), GetElement()
template< typename T, typename Allocator >
bool Helium::SparseArray< T, Allocator >::IsElementValid( size_t index ) const
{
    HELIUM_ASSERT( index < m_size );
    if( index >= m_size )
    {
        return false;
    }

    HELIUM_ASSERT( m_pUsedElements );

    size_t elementIndex, maskIndex;
    GetBitElementAndMaskIndex< uint32_t >( index, elementIndex, maskIndex );

    return GetBit< uint32_t >( m_pUsedElements[ elementIndex ], maskIndex );
}

/// Get the array element at the specified index.
///
/// @param[in] index  Array index.  This must reference a valid element (can test using IsElementValid()).
///
/// @return  Reference to the element at the specified index.
///
/// @see IsElementValid()
template< typename T, typename Allocator >
T& Helium::SparseArray< T, Allocator >::GetElement( size_t index )
{
    HELIUM_ASSERT( IsElementValid( index ) );
    HELIUM_ASSERT( m_pBuffer );

    return m_pBuffer[ index ];
}

/// Get the array element at the specified index.
///
/// @param[in] index  Array index.  This must reference a valid element (can test using IsElementValid()).
///
/// @return  Constant reference to the element at the specified index.
///
/// @see IsElementValid()
template< typename T, typename Allocator >
const T& Helium::SparseArray< T, Allocator >::GetElement( size_t index ) const
{
    HELIUM_ASSERT( IsElementValid( index ) );
    HELIUM_ASSERT( m_pBuffer );

    return m_pBuffer[ index ];
}

/// Get the index of the specified element of this array.
///
/// @param[in] rElement  Reference to an element in this array.
///
/// @return  Index of the specified element.
template< typename T, typename Allocator >
size_t Helium::SparseArray< T, Allocator >::GetElementIndex( const T& rElement ) const
{
    size_t index = static_cast< size_t >( &rElement - m_pBuffer );
    HELIUM_ASSERT( index < m_size );

    return index;
}

/// Get the index of the specified elemnet of this array.
///
/// @param[in] pElement  Pointer to an element in this array.
///
/// @return  Index of the specified element.
template< typename T, typename Allocator >
size_t Helium::SparseArray< T, Allocator >::GetElementIndex( const T* pElement ) const
{
    size_t index = static_cast< size_t >( pElement - m_pBuffer );
    HELIUM_ASSERT( index < m_size );

    return index;
}

/// Set this array to a copy of a C-style array.
///
/// Note that this will reallocate the capacity of this array to match the given array size.
///
/// @param[in] pSource  Array from which to copy.
/// @param[in] size     Number of elements in the given array.
template< typename T, typename Allocator >
void Helium::SparseArray< T, Allocator >::Set( const T* pSource, size_t size )
{
    HELIUM_ASSERT( pSource );
    InPlaceDestroy( m_pBuffer, m_size, m_pUsedElements );
    if( size != m_capacity )
    {
        m_pBuffer = Reallocate( m_pBuffer, size );
        HELIUM_ASSERT( m_pBuffer || size == 0 );

        size_t bitArrayByteCount = Align( size, sizeof( uint32_t ) * 8 ) / 8;
        m_pUsedElements = static_cast< uint32_t* >( m_allocator.Reallocate( m_pUsedElements, bitArrayByteCount ) );
        HELIUM_ASSERT( m_pUsedElements || bitArrayByteCount == 0 );

        m_capacity = size;
    }

    HELIUM_ASSERT( size == 0 ? m_pBuffer == NULL : m_pBuffer != NULL );

    m_size = size;
    m_usedSize = size;
    m_lastUpdateIndex = 0;

    ArrayUninitializedCopy( m_pBuffer, pSource, size );
    MemorySet( m_pUsedElements, 0xff, size );
}

/// Add an element to this array.
///
/// If a free slot is available in this array, this will assign the given value to the first available free spot,
/// otherwise the value will be added to the end of the array.
///
/// @param[in] rValue  Value to add.
///
/// @return  Index at which the value was added.
///
/// @see Remove()
template< typename T, typename Allocator >
size_t Helium::SparseArray< T, Allocator >::Add( const T& rValue )
{
    size_t index = AllocateSlot();
    HELIUM_ASSERT( IsValid( index ) );

    new( m_pBuffer + index ) T( rValue );

    return index;
}

/// Remove an element from this array.
///
/// This will remove the element at the specified index, leaving an empty slot in its place.  Other array elements will
/// be left unmodified.
///
/// @param[in] index  Index of the element to remove.
///
/// @see Add()
template< typename T, typename Allocator >
void Helium::SparseArray< T, Allocator >::Remove( size_t index )
{
    HELIUM_ASSERT( IsElementValid( index ) );

    // Clear out the array slot.
    m_pBuffer[ index ].~T();

    size_t bitElementIndex, maskIndex;
    GetBitElementAndMaskIndex< uint32_t >( index, bitElementIndex, maskIndex );

    uint32_t bitMask = ( 1U << maskIndex );
    m_pUsedElements[ bitElementIndex ] &= ~bitMask;

    // Decrement the used element count.
    HELIUM_ASSERT( m_usedSize != 0 );
    --m_usedSize;
    if( m_usedSize == 0 )
    {
        // No elements are in use anymore, so we can be sure the array is now empty.
        m_size = 0;
        m_lastUpdateIndex = 0;

        return;
    }

    // If we removed the last element in the array, search for the new last element and update the array size.
    if( index == m_size - 1 )
    {
        while( index != 0 )
        {
            bitMask >>= 1;
            if( !bitMask )
            {
                --bitElementIndex;
                bitMask = ( 1U << ( sizeof( uint32_t ) * 8 - 1 ) );
            }

            if( m_pUsedElements[ bitElementIndex ] & bitMask )
            {
                // Found our new last element, so update the size and bail.
                m_size = index;
                if( index < m_lastUpdateIndex )
                {
                    m_lastUpdateIndex = index;
                }

                return;
            }

            --index;
        }

        // We should never reach this point, as our previous check for when the used size reaches zero should have
        // been triggered.
        HELIUM_ASSERT_MSG_FALSE(
            TXT( "SparseArray::Remove(): Failed to find a used array slot, although at least one should exist." ) );

        m_size = 0;
        m_lastUpdateIndex = 0;

        return;
    }

    // If the element just removed is less than the last marked index, update the index.
    if( index < m_lastUpdateIndex )
    {
        m_lastUpdateIndex = index;
    }
}

/// Swap the contents of this array with another array.
///
/// @param[in] rArray  Array with which to swap.
template< typename T, typename Allocator >
void Helium::SparseArray< T, Allocator >::Swap( SparseArray& rArray )
{
    T* pBuffer = m_pBuffer;
    uint32_t pUsedElements = m_pUsedElements;
    size_t size = m_size;
    size_t usedSize = m_usedSize;
    size_t capacity = m_capacity;
    size_t lastUpdateIndex = m_lastUpdateIndex;

    m_pBuffer = rArray.m_pBuffer;
    m_pUsedElements = rArray.m_pUsedElements;
    m_size = rArray.m_size;
    m_usedSize = rArray.m_usedSize;
    m_capacity = rArray.m_capacity;
    m_lastUpdateIndex = rArray.m_lastUpdateIndex;

    rArray.m_pBuffer = pBuffer;
    rArray.m_pUsedElements = pUsedElements;
    rArray.m_size = size;
    rArray.m_usedSize = usedSize;
    rArray.m_capacity = capacity;
    rArray.m_lastUpdateIndex = lastUpdateIndex;
}

/// Allocate a new object as a new element in this array.
///
/// @return  Pointer to the new object.
template< typename T, typename Allocator >
T* Helium::SparseArray< T, Allocator >::New()
{
    size_t index = AllocateSlot();
    HELIUM_ASSERT( IsValid( index ) );

    T* pElement = new( m_pBuffer + index ) T;
    HELIUM_ASSERT( pElement );

    return pElement;
}

#define L_IMPLEMENT_SPARSEARRAY_NEW_Z( Z, N, DATA ) \
template< typename T, typename Allocator > \
template< BOOST_PP_ENUM_PARAMS_Z( Z, N, typename Param ) > \
T* Helium::SparseArray< T, Allocator >::New( BOOST_PP_ENUM_BINARY_PARAMS_Z( Z, N, const Param, &rParam ) ) \
{ \
    size_t index = AllocateSlot(); \
    HELIUM_ASSERT( IsValid( index ) ); \
    \
    T* pElement = new( m_pBuffer + index ) T( BOOST_PP_ENUM_PARAMS_Z( Z, N, rParam ) ); \
    HELIUM_ASSERT( pElement ); \
    \
    return pElement; \
}

BOOST_PP_REPEAT_FROM_TO( 1, 17, L_IMPLEMENT_SPARSEARRAY_NEW_Z, )

#undef L_IMPLEMENT_SPARSEARRAY_NEW_Z

/// Set this array to the contents of the given array.
///
/// If the given array is not the same as this array, this will always destroy the current contents of this array and
/// allocate a fresh array whose capacity matches the size of the given array.
///
/// @param[in] rSource  Array from which to copy.
///
/// @return  Reference to this array.
template< typename T, typename Allocator >
Helium::SparseArray< T, Allocator >& Helium::SparseArray< T, Allocator >::operator=( const SparseArray& rSource )
{
    return Assign( rSource );
}

/// Set this array to the contents of the given array.
///
/// If the given array is not the same as this array, this will always destroy the current contents of this array and
/// allocate a fresh array whose capacity matches the size of the given array.
///
/// @param[in] rSource  Array from which to copy.
///
/// @return  Reference to this array.
template< typename T, typename Allocator >
template< typename OtherAllocator >
Helium::SparseArray< T, Allocator >& Helium::SparseArray< T, Allocator >::operator=(
    const SparseArray< T, OtherAllocator >& rSource )
{
    return Assign( rSource );
}

/// Get the array element at the specified index.
///
/// @param[in] index  Array index.
///
/// @return  Reference to the element at the specified index.
template< typename T, typename Allocator >
T& Helium::SparseArray< T, Allocator >::operator[]( ptrdiff_t index )
{
    HELIUM_ASSERT( IsElementValid( index ) );
    HELIUM_ASSERT( m_pBuffer );
    return m_pBuffer[ index ];
}

/// Get the array element at the specified index.
///
/// @param[in] index  Array index.
///
/// @return  Constant reference to the element at the specified index.
template< typename T, typename Allocator >
const T& Helium::SparseArray< T, Allocator >::operator[]( ptrdiff_t index ) const
{
    HELIUM_ASSERT( IsElementValid( index ) );
    HELIUM_ASSERT( m_pBuffer );
    return m_pBuffer[ index ];
}

/// Get the capacity to which this array should grow if growing to support the desired number of elements.
///
/// @param[in] desiredCount  Desired minimum capacity.
///
/// @return  Recommended capacity.
template< typename T, typename Allocator >
size_t Helium::SparseArray< T, Allocator >::GetGrowCapacity( size_t desiredCount ) const
{
    HELIUM_ASSERT( desiredCount > m_capacity );
    return Max< size_t >( desiredCount, m_capacity + m_capacity / 2 + 1 );
}

/// Increase the capacity of this array according to the normal growth rules.
///
/// @param[in] capacity  New capacity.
template< typename T, typename Allocator >
void Helium::SparseArray< T, Allocator >::Grow( size_t capacity )
{
    if( capacity > m_capacity )
    {
        capacity = GetGrowCapacity( capacity );

        m_pBuffer = ResizeBuffer( m_pBuffer, m_size, m_pUsedElements, m_capacity, capacity );
        HELIUM_ASSERT( m_pBuffer );

        size_t bitArrayByteCount = Align( capacity, sizeof( uint32_t ) * 8 ) / 8;
        m_pUsedElements = static_cast< uint32_t* >( m_allocator.Reallocate( m_pUsedElements, bitArrayByteCount ) );
        HELIUM_ASSERT( m_pUsedElements );

        m_capacity = capacity;
    }
}

/// Allocate an array slot for Add() or New().
///
/// @return  Index of the allocated array slot.
template< typename T, typename Allocator >
size_t Helium::SparseArray< T, Allocator >::AllocateSlot()
{
    size_t size = m_size;

    // Search for an unused slot within the array first.
    if( m_usedSize != size )
    {
        size_t bitElementIndex, maskIndex;
        GetBitElementAndMaskIndex< uint32_t >( m_lastUpdateIndex, bitElementIndex, maskIndex );
        uint32_t bitMask = ( 1U << maskIndex );

        uint32_t elementBits = 0;
        size_t lastBitElementIndex = Invalid< size_t >();
        for( size_t index = m_lastUpdateIndex; index < size; ++index )
        {
            if( bitElementIndex != lastBitElementIndex )
            {
                elementBits = m_pUsedElements[ bitElementIndex ];
                lastBitElementIndex = bitElementIndex;
            }

            if( !( elementBits & bitMask ) )
            {
                m_pUsedElements[ bitElementIndex ] = elementBits | bitMask;
                ++m_usedSize;

                m_lastUpdateIndex = index + 1;

                return index;
            }

            bitMask <<= 1;
            if( !bitMask )
            {
                ++bitElementIndex;
                bitMask = 1U;
            }
        }

        HELIUM_ASSERT_MSG_FALSE(
            ( TXT( "SparseArray::AllocateSlot(): Failed to find an unused array slot, although at least one " )
              TXT( "should exist." ) ) );
    }

    HELIUM_ASSERT( size == m_usedSize );

    // No unused slot could be found, so grow the array and add the element to the end.
    size_t newSize = size + 1;
    Grow( newSize );

    size_t bitElementIndex, maskIndex;
    GetBitElementAndMaskIndex< uint32_t >( size, bitElementIndex, maskIndex );
    SetBit( m_pUsedElements[ bitElementIndex ], maskIndex );

    m_usedSize = newSize;
    m_size = newSize;

    return size;
}

/// Assignment operator implementation.
///
/// This is separated out to help deal with the fact that the default (shallow-copy) assignment operator is used if we
/// define just a template assignment operator overload for any allocator type in the source array and not one that only
/// takes the same exact SparseArray type (non-templated) as well.
///
/// @param[in] rSource  Array from which to copy.
///
/// @return  Reference to this array.
template< typename T, typename Allocator >
template< typename OtherAllocator >
Helium::SparseArray< T, Allocator >& Helium::SparseArray< T, Allocator >::Assign(
    const SparseArray< T, OtherAllocator >& rSource )
{
    if( this != &rSource )
    {
        Clear();

        size_t usedSize = rSource.m_usedSize;
        if( usedSize != 0 )
        {
            Reserve( usedSize );

            typename SparseArray< T, OtherAllocator >::ConstIterator sourceEnd = rSource.End();
            typename SparseArray< T, OtherAllocator >::ConstIterator sourceIter;
            for( sourceIter = rSource.Begin(); sourceIter != sourceEnd; ++sourceIter )
            {
                if( sourceIter )
                {
                    Add( *sourceIter );
                }
            }

            HELIUM_ASSERT( m_size == usedSize );
            HELIUM_ASSERT( m_usedSize == usedSize );
        }
    }

    return *this;
}

/// Allocate memory for the specified number of elements, accounting for non-standard alignment requirements.
///
/// @param[in] count  Number of elements for which to allocate.
///
/// @return  Pointer to the allocated memory.
///
/// @see Reallocate()
template< typename T, typename Allocator >
T* Helium::SparseArray< T, Allocator >::Allocate( size_t count )
{
    return Allocate( count, std::integral_constant< bool, ( std::alignment_of< T >::value > 8 ) >() );
}

/// Allocate() implementation for types requiring a specific alignment.
///
/// @param[in] count            Number of elements for which to allocate.
/// @param[in] rNeedsAlignment  std::true_type.
///
/// @return  Pointer to the allocated memory.
///
/// @see Reallocate()
template< typename T, typename Allocator >
T* Helium::SparseArray< T, Allocator >::Allocate( size_t count, const std::true_type& /*rNeedsAlignment*/ )
{
    return static_cast< T* >( m_allocator.AllocateAligned( std::alignment_of< T >::value, sizeof( T ) * count ) );
}

/// Allocate() implementation for types that can use the default alignment.
///
/// @param[in] count            Number of elements for which to allocate.
/// @param[in] rNeedsAlignment  std::false_type.
///
/// @return  Pointer to the allocated memory.
///
/// @see Reallocate()
template< typename T, typename Allocator >
T* Helium::SparseArray< T, Allocator >::Allocate( size_t count, const std::false_type& /*rNeedsAlignment*/ )
{
    return static_cast< T* >( m_allocator.Allocate( sizeof( T ) * count ) );
}

/// Reallocate memory for the specified number of elements, accounting for non-standard alignment requirements.
///
/// @param[in] pMemory  Base address of the allocation to reallocate.
/// @param[in] count    Number of elements for which to reallocate.
///
/// @return  Pointer to the allocated memory.
///
/// @see Allocate()
template< typename T, typename Allocator >
T* Helium::SparseArray< T, Allocator >::Reallocate( T* pMemory, size_t count )
{
    return Reallocate(
        pMemory,
        count,
        std::integral_constant< bool, ( std::alignment_of< T >::value > 8 ) >() );
}

/// Reallocate() implementation for types requiring a specific alignment.
///
/// @param[in] pMemory          Base address of the allocation to reallocate.
/// @param[in] count            Number of elements for which to reallocate.
/// @param[in] rNeedsAlignment  std::true_type.
///
/// @return  Pointer to the allocated memory.
///
/// @see Reallocate()
template< typename T, typename Allocator >
T* Helium::SparseArray< T, Allocator >::Reallocate(
    T* pMemory,
    size_t count,
    const std::true_type& /*rNeedsAlignment*/ )
{
    size_t existingSize = m_allocator.GetMemorySize( pMemory );
    size_t newSize = sizeof( T ) * count;
    if( existingSize != newSize )
    {
        T* pNewMemory = static_cast< T* >( m_allocator.AllocateAligned(
            std::alignment_of< T >::value,
            newSize ) );
        HELIUM_ASSERT( pNewMemory || newSize == 0 );
        MemoryCopy( pNewMemory, pMemory, Min( existingSize, newSize ) );
        m_allocator.Free( pMemory );
        pMemory = pNewMemory;
    }

    return pMemory;
}

/// Reallocate() implementation for types that can use the default alignment.
///
/// @param[in] pMemory          Base address of the allocation to reallocate.
/// @param[in] count            Number of elements for which to reallocate.
/// @param[in] rNeedsAlignment  std::false_type.
///
/// @return  Pointer to the allocated memory.
///
/// @see Reallocate()
template< typename T, typename Allocator >
T* Helium::SparseArray< T, Allocator >::Reallocate(
    T* pMemory,
    size_t count,
    const std::false_type& /*rNeedsAlignment*/ )
{
    return static_cast< T* >( m_allocator.Reallocate( pMemory, sizeof( T ) * count ) );
}

/// Resize an array of elements.
///
/// @param[in] pMemory        Base address of the array being resized.
/// @param[in] elementRange   Range of elements that have actually been constructed in the buffer.
/// @param[in] pUsedElements  Bit array specifying which elements are in use.
/// @param[in] oldCapacity    Current array capacity.
/// @param[in] newCapacity    New array capacity.
///
/// @return  Pointer to the resized array.
template< typename T, typename Allocator >
T* Helium::SparseArray< T, Allocator >::ResizeBuffer(
    T* pMemory,
    size_t elementRange,
    const uint32_t* pUsedElements,
    size_t oldCapacity,
    size_t newCapacity )
{
    return ResizeBuffer(
        pMemory,
        elementRange,
        pUsedElements,
        oldCapacity,
        newCapacity,
        std::integral_constant<
            bool, std::has_trivial_copy< T >::value && std::has_trivial_destructor< T >::value >() );
}

/// ResizeBuffer() implementation for types with both a trivial copy constructor and trivial destructor.
///
/// @param[in] pMemory                       Base address of the array being resized.
/// @param[in] elementRange                  Range of elements that have actually been constructed in the buffer.
/// @param[in] pUsedElements                 Bit array specifying which elements are in use.
/// @param[in] oldCapacity                   Current array capacity.
/// @param[in] newCapacity                   New array capacity.
/// @param[in] rHasTrivialCopyAndDestructor  std::true_type.
///
/// @return  Pointer to the resized array.
template< typename T, typename Allocator >
T* Helium::SparseArray< T, Allocator >::ResizeBuffer(
    T* pMemory,
    size_t /*elementRange*/,
    const uint32_t* /*pUsedElements*/,
    size_t /*oldCapacity*/,
    size_t newCapacity,
    const std::true_type& /*rHasTrivialCopyAndDestructor*/ )
{
    return Reallocate( pMemory, newCapacity );
}

/// ResizeBuffer() implementation for types without either a trivial copy constructor or a trivial destructor.
///
/// @param[in] pMemory                       Base address of the array being resized.
/// @param[in] elementRange                  Range of elements that have actually been constructed in the buffer.
/// @param[in] pUsedElements                 Bit array specifying which elements are in use.
/// @param[in] oldCapacity                   Current array capacity.
/// @param[in] newCapacity                   New array capacity.
/// @param[in] rHasTrivialCopyAndDestructor  std::false_type.
///
/// @return  Pointer to the resized array.
template< typename T, typename Allocator >
T* Helium::SparseArray< T, Allocator >::ResizeBuffer(
    T* pMemory,
    size_t elementRange,
    const uint32_t* pUsedElements,
    size_t oldCapacity,
    size_t newCapacity,
    const std::false_type& /*rHasTrivialCopyAndDestructor*/ )
{
    HELIUM_ASSERT( elementRange <= oldCapacity );
    HELIUM_ASSERT( elementRange <= newCapacity );

    T* pNewMemory = pMemory;

    if( oldCapacity != newCapacity )
    {
        pNewMemory = NULL;
        if( newCapacity )
        {
            pNewMemory = Allocate( newCapacity );
            HELIUM_ASSERT( pNewMemory );
            UninitializedCopy( pNewMemory, pMemory, elementRange, pUsedElements );
        }

        InPlaceDestroy( pMemory, elementRange, pUsedElements );
        m_allocator.Free( pMemory );
    }

    return pNewMemory;
}

/// Call the destructor on used elements in the given sparse array without deallocating the array.
///
/// @param[in] pMemory        Base address of the array of objects being destroyed.
/// @param[in] range          Range of elements in use.
/// @param[in] pUsedElements  Bit array specifying which elements are in use.
template< typename T, typename Allocator >
void Helium::SparseArray< T, Allocator >::InPlaceDestroy( T* pMemory, size_t range, const uint32_t* pUsedElements )
{
    InPlaceDestroy( pMemory, range, pUsedElements, std::has_trivial_destructor< T >() );
}

/// InPlaceDestroy() implementation for types with a trivial destructor.
///
/// @param[in] pMemory                Base address of the array of objects being destroyed.
/// @param[in] range                  Range of elements in use.
/// @param[in] pUsedElements          Bit array specifying which elements are in use.
/// @param[in] rHasTrivialDestructor  std::true_type.
template< typename T, typename Allocator >
void Helium::SparseArray< T, Allocator >::InPlaceDestroy(
    T* /*pMemory*/,
    size_t /*range*/,
    const uint32_t* /*pUsedElements*/,
    const std::true_type& /*rHasTrivialDestructor*/ )
{
    // Nothing needs to be done...
}

/// InPlaceDestroy() implementation for types without a trivial destructor.
///
/// @param[in] pMemory                Base address of the array of objects being destroyed.
/// @param[in] range                  Range of elements in use.
/// @param[in] pUsedElements          Bit array specifying which elements are in use.
/// @param[in] rHasTrivialDestructor  std::false_type.
template< typename T, typename Allocator >
void Helium::SparseArray< T, Allocator >::InPlaceDestroy(
    T* pMemory,
    size_t range,
    const uint32_t* pUsedElements,
    const std::false_type& /*rHasTrivialDestructor*/ )
{
    HELIUM_ASSERT( pMemory || range == 0 );
    HELIUM_ASSERT( pUsedElements || range == 0 );

    uint32_t elementBits = 0;
    const uint32_t* pLastUsedElements = NULL;

    uint32_t bitMask = 1U;

    for( size_t index = 0; index < range; ++index )
    {
        if( pLastUsedElements != pUsedElements )
        {
            elementBits = *pUsedElements;
            pLastUsedElements = pUsedElements;
        }

        if( elementBits & bitMask )
        {
            pMemory->~T();
        }

        ++pMemory;

        bitMask <<= 1;
        if( !bitMask )
        {
            ++pUsedElements;
            bitMask = 1U;
        }
    }
}

/// Construct copies of used elements from the source sparse array in the uninitialized destination buffer, matching up
/// the used element slots.
///
/// @param[in] pDest          Destination buffer in which each element should be constructed.
/// @param[in] pSource        Source array from which to copy.
/// @param[in] range          Range of elements in use in the source array.
/// @param[in] pUsedElements  Bit array specifying which elements are in use in the source array.
template< typename T, typename Allocator >
void Helium::SparseArray< T, Allocator >::UninitializedCopy(
    T* pDest,
    const T* pSource,
    size_t range,
    const uint32_t* pUsedElements )
{
    UninitializedCopy( pDest, pSource, range, pUsedElements, std::has_trivial_copy< T >() );
}

/// UninitializedCopy() implementation for types with a trivial copy constructor.
///
/// @param[in] pDest            Destination buffer in which each element should be constructed.
/// @param[in] pSource          Source array from which to copy.
/// @param[in] range            Range of elements in use in the source array.
/// @param[in] pUsedElements    Bit array specifying which elements are in use in the source array.
/// @param[in] rHasTrivialCopy  std::true_type().
template< typename T, typename Allocator >
void Helium::SparseArray< T, Allocator >::UninitializedCopy(
    T* pDest,
    const T* pSource,
    size_t range,
    const uint32_t* /*pUsedElements*/,
    const std::true_type& /*rHasTrivialCopy*/ )
{
    HELIUM_ASSERT( pDest || range == 0 );
    HELIUM_ASSERT( pSource || range == 0 );

    MemoryCopy( pDest, pSource, sizeof( T ) * range );
}

/// UninitializedCopy() implementation for types without a trivial copy constructor.
///
/// @param[in] pDest            Destination buffer in which each element should be constructed.
/// @param[in] pSource          Source array from which to copy.
/// @param[in] range            Range of elements in use in the source array.
/// @param[in] pUsedElements    Bit array specifying which elements are in use in the source array.
/// @param[in] rHasTrivialCopy  std::false_type().
template< typename T, typename Allocator >
void Helium::SparseArray< T, Allocator >::UninitializedCopy(
    T* pDest,
    const T* pSource,
    size_t range,
    const uint32_t* pUsedElements,
    const std::false_type& /*rHasTrivialCopy*/ )
{
    HELIUM_ASSERT( pDest || range == 0 );
    HELIUM_ASSERT( pSource || range == 0 );
    HELIUM_ASSERT( pUsedElements || range == 0 );

    uint32_t elementBits = 0;
    const uint32_t* pLastUsedElements = NULL;

    uint32_t bitMask = 1U;

    for( size_t index = 0; index < range; ++index )
    {
        if( pLastUsedElements != pUsedElements )
        {
            elementBits = *pUsedElements;
            pLastUsedElements = pUsedElements;
        }

        if( elementBits & bitMask )
        {
            new( pDest ) T( *pSource );
        }

        ++pDest;
        ++pSource;

        bitMask <<= 1;
        if( !bitMask )
        {
            ++pUsedElements;
            bitMask = 1U;
        }
    }
}
