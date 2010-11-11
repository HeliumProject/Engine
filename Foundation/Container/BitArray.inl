/// Constructor.
///
/// @param[in] rElement  Reference to the current array element.
/// @param[in] mask      Bit mask for the current array element.
Helium::ConstBitArrayElementProxy::ConstBitArrayElementProxy( const uint32_t& rElement, uint32_t mask )
    : m_rElement( const_cast< uint32_t& >( rElement ) )
    , m_mask( mask )
{
    HELIUM_ASSERT( mask );
    HELIUM_ASSERT( ( mask & ( mask - 1 ) ) == 0 );
}

/// Get the current bit value.
///
/// @return  True if the referenced bit is set, false if it is unset.
Helium::ConstBitArrayElementProxy::operator bool() const
{
    return( ( m_rElement & m_mask ) != 0 );
}

/// Equality comparison.
///
/// @param[in] bValue  True to test if this bit is set, false to test if this bit is unset.
///
/// @return  True if the current bit state matches the given value, false if not.
bool Helium::ConstBitArrayElementProxy::operator==( bool bValue ) const
{
    uint32_t bitValue = m_rElement & m_mask;

    return( bValue ? bitValue != 0 : bitValue == 0 );
}

/// Equality comparison.
///
/// @param[in] rOther  Reference to a bit array element with which to compare.
///
/// @return  True if the current bit state matches that of the given proxy, false if not.
bool Helium::ConstBitArrayElementProxy::operator==( const ConstBitArrayElementProxy& rOther ) const
{
    uint32_t bitValue = m_rElement & m_mask;

    return( static_cast< bool >( rOther ) ? bitValue != 0 : bitValue == 0 );
}

/// Inequality comparison.
///
/// @param[in] bValue  True to test if this bit is unset, false to test if it is set.
///
/// @return  True if the current bit state does not match the given value, false if they do match.
bool Helium::ConstBitArrayElementProxy::operator!=( bool bValue ) const
{
    uint32_t bitValue = m_rElement & m_mask;

    return( bValue ? bitValue == 0 : bitValue != 0 );
}

/// Inequality comparison.
///
/// @param[in] rOther  Reference to a bit array element with which to compare.
///
/// @return  True if the current bit state does not match that of the given proxy, false if they do match
bool Helium::ConstBitArrayElementProxy::operator!=( const ConstBitArrayElementProxy& rOther ) const
{
    uint32_t bitValue = m_rElement & m_mask;

    return( static_cast< bool >( rOther ) ? bitValue == 0 : bitValue != 0 );
}

/// Constructor.
///
/// @param[in] rElement  Reference to the current array element.
/// @param[in] mask      Bit mask for the current array element.
Helium::BitArrayElementProxy::BitArrayElementProxy( uint32_t& rElement, uint32_t mask )
    : ConstBitArrayElementProxy( rElement, mask )
{
}

/// Set the current bit value.
///
/// @param[in] bValue  True to set the current bit, false to unset it.
///
/// @return  Reference to this proxy object.
Helium::BitArrayElementProxy& Helium::BitArrayElementProxy::operator=( bool bValue )
{
    if( bValue )
    {
        m_rElement |= m_mask;
    }
    else
    {
        m_rElement &= ~m_mask;
    }

    return *this;
}

/// Set the current bit value.
///
/// @param[in] rOther  Bit array element whose state should be replicated to the current bit.
///
/// @return  Reference to this proxy object.
Helium::BitArrayElementProxy& Helium::BitArrayElementProxy::operator=( const ConstBitArrayElementProxy& rOther )
{
    if( static_cast< bool >( rOther ) )
    {
        m_rElement |= m_mask;
    }
    else
    {
        m_rElement &= ~m_mask;
    }

    return *this;
}

/// Constructor.
///
/// Creates an uninitialized iterator.  Using this is not safe until it is initialized.
Helium::ConstBitArrayIterator::ConstBitArrayIterator()
{
}

/// Constructor.
///
/// @param[in] pElement  Pointer to the current array element.
/// @param[in] mask      Bit mask for the current array element.
Helium::ConstBitArrayIterator::ConstBitArrayIterator( const uint32_t* pElement, uint32_t mask )
    : m_pElement( const_cast< uint32_t* >( pElement ) )
    , m_mask( mask )
{
    HELIUM_ASSERT( pElement );
    HELIUM_ASSERT( mask );
    HELIUM_ASSERT( ( mask & ( mask - 1 ) ) == 0 );
}

/// Access the current bit.
///
/// @return  Proxy for non-mutable access to the current bit.
Helium::ConstBitArrayIterator::ConstReferenceType Helium::ConstBitArrayIterator::operator*() const
{
    return ConstReferenceType( *m_pElement, m_mask );
}

/// Pre-increment operator.
///
/// Increments this iterator's position by one.
///
/// @return  Reference to this iterator.
Helium::ConstBitArrayIterator& Helium::ConstBitArrayIterator::operator++()
{
    if( !( m_mask <<= 1 ) )
    {
        ++m_pElement;
        m_mask = 1;
    }

    return *this;
}

/// Post-increment operator.
///
/// Increments this iterator's position by one.
///
/// @return  Copy of this iterator prior to incrementing.
Helium::ConstBitArrayIterator Helium::ConstBitArrayIterator::operator++( int )
{
    ConstBitArrayIterator result = *this;
    ++( *this );

    return result;
}

/// Pre-decrement operator.
///
/// Decrements this iterator's position by one.
///
/// @return  Reference to this iterator.
Helium::ConstBitArrayIterator& Helium::ConstBitArrayIterator::operator--()
{
    if( !( m_mask >>= 1 ) )
    {
        --m_pElement;
        m_mask = 1U << ( sizeof( m_mask ) * 8 - 1 );
    }

    return *this;
}

/// Post-decrement operator.
///
/// Decrements this iterator's position by one.
///
/// @return  Copy of this iterator prior to decrementing.
Helium::ConstBitArrayIterator Helium::ConstBitArrayIterator::operator--( int )
{
    ConstBitArrayIterator result = *this;
    --( *this );

    return result;
}

/// In-place addition operator.
///
/// @param[in] offset  Amount by which to increase this iterator's position.
///
/// @return  Reference to this iterator.
Helium::ConstBitArrayIterator& Helium::ConstBitArrayIterator::operator+=( ptrdiff_t offset )
{
    if( offset < 0 )
    {
        UnsignedDecrease( static_cast< size_t >( -offset ) );
    }
    else
    {
        UnsignedIncrease( static_cast< size_t >( offset ) );
    }

    return *this;
}

/// Addition operator.
///
/// @param[in] offset  Amount by which to increase this iterator's position.
///
/// @return  Copy of this iterator, increased by the specified amount.
Helium::ConstBitArrayIterator Helium::ConstBitArrayIterator::operator+( ptrdiff_t offset ) const
{
    ConstBitArrayIterator result = *this;
    result += offset;

    return result;
}

/// In-place subtraction operator.
///
/// @param[in] offset  Amount by which to decrease this iterator's position.
///
/// @return  Reference to this iterator.
Helium::ConstBitArrayIterator& Helium::ConstBitArrayIterator::operator-=( ptrdiff_t offset )
{
    if( offset < 0 )
    {
        UnsignedIncrease( static_cast< size_t >( -offset ) );
    }
    else
    {
        UnsignedDecrease( static_cast< size_t >( offset ) );
    }

    return *this;
}

/// Subtraction operator.
///
/// @param[in] offset  Amount by which to decrease this iterator's position.
///
/// @return  Copy of this iterator, decreased by the specified amount.
Helium::ConstBitArrayIterator Helium::ConstBitArrayIterator::operator-( ptrdiff_t offset ) const
{
    ConstBitArrayIterator result = *this;
    result -= offset;

    return result;
}

/// Subtraction operator.
///
/// @param[in] rOther  Bit array iterator to subtract.
///
/// @return  Offset from the given bit array iterator to this iterator.
ptrdiff_t Helium::ConstBitArrayIterator::operator-( const ConstBitArrayIterator& rOther ) const
{
    return( ( m_pElement - rOther.m_pElement ) * sizeof( m_mask ) * 8 + Log2( m_mask ) - Log2( rOther.m_mask ) );
}

/// Equality comparison operator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator and the given iterator reference the same bit, false if not.
bool Helium::ConstBitArrayIterator::operator==( const ConstBitArrayIterator& rOther ) const
{
    return( m_pElement == rOther.m_pElement && m_mask == rOther.m_mask );
}

/// Inequality comparison operator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator and the given iterator do not reference the same bit, false if they do.
bool Helium::ConstBitArrayIterator::operator!=( const ConstBitArrayIterator& rOther ) const
{
    return( m_pElement != rOther.m_pElement || m_mask != rOther.m_mask );
}

/// Less-than comparison operator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator precedes the given iterator, false if not.
bool Helium::ConstBitArrayIterator::operator<( const ConstBitArrayIterator& rOther ) const
{
    return( m_pElement < rOther.m_pElement || ( m_pElement == rOther.m_pElement && m_mask < rOther.m_mask ) );
}

/// Greater-than comparison operator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator follows the given iterator, false if not.
bool Helium::ConstBitArrayIterator::operator>( const ConstBitArrayIterator& rOther ) const
{
    return( m_pElement > rOther.m_pElement || ( m_pElement == rOther.m_pElement && m_mask > rOther.m_mask ) );
}

/// Less-than-or-equals comparison operator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator precedes or matches the given iterator, false if not.
bool Helium::ConstBitArrayIterator::operator<=( const ConstBitArrayIterator& rOther ) const
{
    return( m_pElement < rOther.m_pElement || ( m_pElement == rOther.m_pElement && m_mask <= rOther.m_mask ) );
}

/// Greater-than-or-equals comparison operator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator follows or matches the given iterator, false if not.
bool Helium::ConstBitArrayIterator::operator>=( const ConstBitArrayIterator& rOther ) const
{
    return( m_pElement > rOther.m_pElement || ( m_pElement == rOther.m_pElement && m_mask >= rOther.m_mask ) );
}

/// Increase this iterator's position by the specified amount.
///
/// @param[in] offset  Amount by which to increase this iterator's position.
void Helium::ConstBitArrayIterator::UnsignedIncrease( size_t offset )
{
    size_t elementIncrement = offset / ( sizeof( m_mask ) * 8 );
    size_t bitIncrement = offset % ( sizeof( m_mask ) * 8 );

    uint32_t* pNewElement = m_pElement + elementIncrement;

    uint32_t currentMask = m_mask;
    uint32_t newMask = currentMask << bitIncrement;
    if( !newMask )
    {
        ++pNewElement;
        newMask = currentMask >> ( sizeof( m_mask ) * 8 - bitIncrement );
    }

    m_pElement = pNewElement;
    m_mask = newMask;
}

/// Decrease this iterator's position by the specified amount.
///
/// @param[in] offset  Amount by which to decrease this iterator's position.
void Helium::ConstBitArrayIterator::UnsignedDecrease( size_t offset )
{
    size_t elementIncrement = offset / ( sizeof( m_mask ) * 8 );
    size_t bitIncrement = offset % ( sizeof( m_mask ) * 8 );

    uint32_t* pNewElement = m_pElement - elementIncrement;

    uint32_t currentMask = m_mask;
    uint32_t newMask = currentMask >> bitIncrement;
    if( !newMask )
    {
        --pNewElement;
        newMask = currentMask << ( sizeof( m_mask ) * 8 - bitIncrement );
    }

    m_pElement = pNewElement;
    m_mask = newMask;
}

/// Constructor.
///
/// Creates an uninitialized iterator.  Using this is not safe until it is initialized.
Helium::BitArrayIterator::BitArrayIterator()
{
}

/// Constructor.
///
/// @param[in] pElement  Pointer to the current array element.
/// @param[in] mask      Bit mask for the current array element.
Helium::BitArrayIterator::BitArrayIterator( uint32_t* pElement, uint32_t mask )
    : ConstBitArrayIterator( pElement, mask )
{
}

/// Access the current bit.
///
/// @return  Proxy for mutable access to the current bit.
Helium::BitArrayIterator::ReferenceType Helium::BitArrayIterator::operator*() const
{
    return ReferenceType( *m_pElement, m_mask );
}

/// Pre-increment operator.
///
/// Increments this iterator's position by one.
///
/// @return  Reference to this iterator.
Helium::BitArrayIterator& Helium::BitArrayIterator::operator++()
{
    if( !( m_mask <<= 1 ) )
    {
        ++m_pElement;
        m_mask = 1;
    }

    return *this;
}

/// Post-increment operator.
///
/// Increments this iterator's position by one.
///
/// @return  Copy of this iterator prior to incrementing.
Helium::BitArrayIterator Helium::BitArrayIterator::operator++( int )
{
    BitArrayIterator result = *this;
    ++( *this );

    return result;
}

/// Pre-decrement operator.
///
/// Decrements this iterator's position by one.
///
/// @return  Reference to this iterator.
Helium::BitArrayIterator& Helium::BitArrayIterator::operator--()
{
    if( !( m_mask >>= 1 ) )
    {
        --m_pElement;
        m_mask = 1U << ( sizeof( m_mask ) * 8 - 1 );
    }

    return *this;
}

/// Post-decrement operator.
///
/// Decrements this iterator's position by one.
///
/// @return  Copy of this iterator prior to decrementing.
Helium::BitArrayIterator Helium::BitArrayIterator::operator--( int )
{
    BitArrayIterator result = *this;
    --( *this );

    return result;
}

/// In-place addition operator.
///
/// @param[in] offset  Amount by which to increase this iterator's position.
///
/// @return  Reference to this iterator.
Helium::BitArrayIterator& Helium::BitArrayIterator::operator+=( ptrdiff_t offset )
{
    if( offset < 0 )
    {
        UnsignedDecrease( static_cast< size_t >( -offset ) );
    }
    else
    {
        UnsignedIncrease( static_cast< size_t >( offset ) );
    }

    return *this;
}

/// Addition operator.
///
/// @param[in] offset  Amount by which to increase this iterator's position.
///
/// @return  Copy of this iterator, increased by the specified amount.
Helium::BitArrayIterator Helium::BitArrayIterator::operator+( ptrdiff_t offset ) const
{
    BitArrayIterator result = *this;
    result += offset;

    return result;
}

/// In-place subtraction operator.
///
/// @param[in] offset  Amount by which to decrease this iterator's position.
///
/// @return  Reference to this iterator.
Helium::BitArrayIterator& Helium::BitArrayIterator::operator-=( ptrdiff_t offset )
{
    if( offset < 0 )
    {
        UnsignedIncrease( static_cast< size_t >( -offset ) );
    }
    else
    {
        UnsignedDecrease( static_cast< size_t >( offset ) );
    }

    return *this;
}

/// Subtraction operator.
///
/// @param[in] offset  Amount by which to decrease this iterator's position.
///
/// @return  Copy of this iterator, decreased by the specified amount.
Helium::BitArrayIterator Helium::BitArrayIterator::operator-( ptrdiff_t offset ) const
{
    BitArrayIterator result = *this;
    result -= offset;

    return result;
}

/// Constructor.
///
/// This creates an empty array.  No memory is allocated at this time.
template< typename Allocator >
Helium::BitArray< Allocator >::BitArray()
    : m_pBuffer( NULL )
    , m_size( 0 )
    , m_capacity( 0 )
{
}

/// Copy constructor.
///
/// This creates a copy of the given array.
///
/// @param[in] pSource  Array from which to copy.
/// @param[in] size     Number of elements in the given array.
template< typename Allocator >
Helium::BitArray< Allocator >::BitArray( const BitArray& rSource )
    : m_pBuffer( NULL )
    , m_size( rSource.m_size )
    , m_capacity( Align( rSource.m_size, sizeof( uint32_t ) * 8 ) )
{
    if( m_capacity )
    {
        size_t byteCount = m_capacity / 8;

        m_pBuffer = static_cast< uint32_t* >( Allocator().Allocate( byteCount ) );
        HELIUM_ASSERT( m_pBuffer );
        HELIUM_ASSERT( rSource.m_pBuffer );
        MemoryCopy( m_pBuffer, rSource.m_pBuffer, byteCount );
    }
}

/// Destructor.
template< typename Allocator >
Helium::BitArray< Allocator >::~BitArray()
{
    Allocator().Free( m_pBuffer );
}

/// Get the number of bits in this array.
///
/// @return  Number of bits in this array.
///
/// @see GetCapacity(), Resize()
template< typename Allocator >
size_t Helium::BitArray< Allocator >::GetSize() const
{
    return m_size;
}

/// Get whether this array is currently empty.
///
/// @return  True if this array is empty, false if not.
///
/// @see GetSize()
template< typename Allocator >
bool Helium::BitArray< Allocator >::IsEmpty() const
{
    return( m_size == 0 );
}

/// Resize this array, retaining any existing data that fits within the new size.
///
/// If the new size is smaller than the current size, allocated memory no longer in use will not be automatically freed.
/// If the new size is larger than the current capacity, the array memory will be reallocated according to the normal
/// array growth rules.  This can be avoided by calling Reserve() to increase the capacity to an explicit value prior to
/// calling this function.
///
/// New bits will be left in an uninitialized state.
///
/// @param[in] size  New array size.
///
/// @see GetSize()
template< typename Allocator >
void Helium::BitArray< Allocator >::Resize( size_t size )
{
    if( size > m_size )
    {
        Grow( size );
    }

    m_size = size;
}

/// Get the maximum number of bits which this array can contain without requiring reallocation of memory.
///
/// @return  Current array capacity.
///
/// @see GetSize(), Reserve()
template< typename Allocator >
size_t Helium::BitArray< Allocator >::GetCapacity() const
{
    return m_capacity;
}

/// Explicitly increase the capacity of this array to support at least the specified number of bits.
///
/// Note that since memory is allocated in chunks of 32-bit integers, the capacity will be clamped to the lowest
/// multiple of 32 that is larger than the necessary bit capacity.  If the requested capacity is less than the current
/// capacity, no memory will be reallocated.
template< typename Allocator >
void Helium::BitArray< Allocator >::Reserve( size_t capacity )
{
    capacity = Align( capacity, sizeof( uint32_t ) * 8 );

    if( capacity > m_capacity )
    {
        size_t byteCount = capacity / 8;

        m_pBuffer = static_cast< uint32_t* >( Allocator().Reallocate( m_pBuffer, byteCount ) );
        HELIUM_ASSERT( m_pBuffer );
        m_capacity = capacity;
    }
}

/// Resize the allocated array memory to match the size actually in use.
///
/// Note that the since memory is allocated in chunks of 32-bit integers, the capacity will be clamped to the lowest
/// multiple of 32 that is larger than the current size.
template< typename Allocator >
void Helium::BitArray< Allocator >::Trim()
{
    size_t capacity = Align( m_size, sizeof( uint32_t ) * 8 );
    if( capacity != m_capacity )
    {
        size_t byteCount = capacity / 8;

        m_pBuffer = static_cast< uint32_t* >( Allocator().Reallocate( m_pBuffer, byteCount ) );
        HELIUM_ASSERT( m_pBuffer || capacity == 0 );
        m_capacity = capacity;
    }
}

/// Resize the array to zero and free all allocated memory.
template< typename Allocator >
void Helium::BitArray< Allocator >::Clear()
{
    Allocator().Free( m_pBuffer );
    m_pBuffer = NULL;
    m_size = 0;
    m_capacity = 0;
}

/// Retrieve an iterator referencing the beginning of this array.
///
/// @return  Iterator at the beginning of this array.
///
/// @see End()
template< typename Allocator >
typename Helium::BitArray< Allocator >::Iterator Helium::BitArray< Allocator >::Begin()
{
    return Iterator( m_pBuffer, 1 );
}

/// Retrieve a constant iterator referencing the beginning of this array.
///
/// @return  Constant iterator at the beginning of this array.
///
/// @see End()
template< typename Allocator >
typename Helium::BitArray< Allocator >::ConstIterator Helium::BitArray< Allocator >::Begin() const
{
    return ConstIterator( m_pBuffer, 1 );
}

/// Retrieve an iterator referencing the end of this array.
///
/// @return  Iterator at the end of this array.
///
/// @see Begin()
template< typename Allocator >
typename Helium::BitArray< Allocator >::Iterator Helium::BitArray< Allocator >::End()
{
    size_t elementIndex = m_size / ( sizeof( uint32_t ) * 8 );
    size_t bitIndex = m_size % ( sizeof( uint32_t ) * 8 );

    return Iterator( m_pBuffer + elementIndex, 1U << bitIndex );
}

/// Retrieve a constant iterator referencing the end of this array.
///
/// @return  Constant iterator at the end of this array.
///
/// @see Begin()
template< typename Allocator >
typename Helium::BitArray< Allocator >::ConstIterator Helium::BitArray< Allocator >::End() const
{
    size_t elementIndex = m_size / ( sizeof( uint32_t ) * 8 );
    size_t bitIndex = m_size % ( sizeof( uint32_t ) * 8 );

    return ConstIterator( m_pBuffer + elementIndex, 1U << bitIndex );
}

/// Get the array element at the specified index.
///
/// @param[in] index  Bit index.
///
/// @return  Reference to the bit at the specified index.
template< typename Allocator >
typename Helium::BitArray< Allocator >::ReferenceType Helium::BitArray< Allocator >::GetElement( size_t index )
{
    HELIUM_ASSERT( index < m_size );
    HELIUM_ASSERT( m_pBuffer );

    size_t elementIndex = index / ( sizeof( uint32_t ) * 8 );
    size_t bitIndex = index % ( sizeof( uint32_t ) * 8 );

    return ReferenceType( *( m_pBuffer + elementIndex ), 1U << bitIndex );
}

/// Set the bit at the specified index.
///
/// @param[in] index  Index of the bit to set.
///
/// @see UnsetElement(), ToggleElement()
template< typename Allocator >
void Helium::BitArray< Allocator >::SetElement( size_t index )
{
    HELIUM_ASSERT( index < m_size );
    HELIUM_ASSERT( m_pBuffer );

    size_t elementIndex = index / ( sizeof( uint32_t ) * 8 );
    size_t bitIndex = index % ( sizeof( uint32_t ) * 8 );

    m_pBuffer[ elementIndex ] |= ( 1U << bitIndex );
}

/// Unset the bit at the specified index.
///
/// @param[in] index  Index of the bit to clear.
///
/// @see SetElement(), ToggleElement()
template< typename Allocator >
void Helium::BitArray< Allocator >::UnsetElement( size_t index )
{
    HELIUM_ASSERT( index < m_size );
    HELIUM_ASSERT( m_pBuffer );

    size_t elementIndex = index / ( sizeof( uint32_t ) * 8 );
    size_t bitIndex = index % ( sizeof( uint32_t ) * 8 );

    m_pBuffer[ elementIndex ] &= ~( 1U << bitIndex );
}

/// Toggle the bit at the specified index.
///
/// @param[in] index  Index of the bit to toggle.
///
/// @see SetElement(), UnsetElement()
template< typename Allocator >
void Helium::BitArray< Allocator >::ToggleElement( size_t index )
{
    HELIUM_ASSERT( index < m_size );
    HELIUM_ASSERT( m_pBuffer );

    size_t elementIndex = index / ( sizeof( uint32_t ) * 8 );
    size_t bitIndex = index % ( sizeof( uint32_t ) * 8 );

    m_pBuffer[ elementIndex ] ^= ( 1U << bitIndex );
}

/// Get the array element at the specified index.
///
/// @param[in] index  Bit index.
///
/// @return  Constant reference to the bit at the specified index.
template< typename Allocator >
typename Helium::BitArray< Allocator >::ConstReferenceType Helium::BitArray< Allocator >::GetElement(
    size_t index ) const
{
    HELIUM_ASSERT( index < m_size );
    HELIUM_ASSERT( m_pBuffer );

    size_t elementIndex = index / ( sizeof( uint32_t ) * 8 );
    size_t bitIndex = index % ( sizeof( uint32_t ) * 8 );

    return ConstReferenceType( *( m_pBuffer + elementIndex ), 1U << bitIndex );
}

/// Add an element to the end of this array.
///
/// @param[in] bValue  True to add set bits, false to add unset bits.
/// @param[in] count   Number of copies of the specified bit to add.
template< typename Allocator >
void Helium::BitArray< Allocator >::Add( bool bValue, size_t count )
{
    size_t newSize = m_size + count;
    Grow( newSize );

    if( bValue )
    {
        SetBitRange( m_pBuffer, m_size, count );
    }
    else
    {
        ClearBitRange( m_pBuffer, m_size, count );
    }

    m_size = newSize;
}

/// Set all bits in this array to the specified value.
///
/// @param[in] bValue  True to set all bits, false to unset all bits (default is to set all bits).
///
/// @see UnsetAll(), ToggleAll()
template< typename Allocator >
void Helium::BitArray< Allocator >::SetAll( bool bValue )
{
    size_t size = m_size;
    if( size )
    {
        size_t byteCount = ( size + 7 ) / 8;
        HELIUM_ASSERT( m_pBuffer );
        MemorySet( m_pBuffer, ( bValue ? 0xff : 0 ), byteCount );
    }
}

/// Unset all bits in this array.
///
/// @see SetAll(), ToggleAll()
template< typename Allocator >
void Helium::BitArray< Allocator >::UnsetAll()
{
    size_t size = m_size;
    if( size )
    {
        size_t byteCount = ( size + 7 ) / 8;
        HELIUM_ASSERT( m_pBuffer );
        MemoryZero( m_pBuffer, byteCount );
    }
}

/// Toggle all bits in this array.
///
/// @see SetAll(), UnsetAll()
template< typename Allocator >
void Helium::BitArray< Allocator >::ToggleAll()
{
    size_t size = m_size;
    if( size )
    {
        HELIUM_ASSERT( m_pBuffer );

        size_t elementCount = ( size + sizeof( uint32_t ) * 8 - 1 ) / ( sizeof( uint32_t ) * 8 );
        for( size_t elementIndex = 0; elementIndex < elementCount; ++elementIndex )
        {
            m_pBuffer[ elementIndex ] ^= ~static_cast< uint32_t >( 0 );
        }
    }
}

/// Get the first bit in this array.
///
/// @return  Reference to the first bit in this array.
///
/// @see GetLast()
template< typename Allocator >
typename Helium::BitArray< Allocator >::ReferenceType Helium::BitArray< Allocator >::GetFirst()
{
    HELIUM_ASSERT( m_size != 0 );
    HELIUM_ASSERT( m_pBuffer );

    return ReferenceType( *m_pBuffer, 1 );
}

/// Get the first bit in this array.
///
/// @return  Constant reference to the first bit in this array.
///
/// @see GetLast()
template< typename Allocator >
typename Helium::BitArray< Allocator >::ConstReferenceType Helium::BitArray< Allocator >::GetFirst() const
{
    HELIUM_ASSERT( m_size != 0 );
    HELIUM_ASSERT( m_pBuffer );

    return ConstReferenceType( *m_pBuffer, 1 );
}

/// Get the last bit in this array.
///
/// @return  Reference to the last bit in this array.
///
/// @see GetFirst()
template< typename Allocator >
typename Helium::BitArray< Allocator >::ReferenceType Helium::BitArray< Allocator >::GetLast()
{
    HELIUM_ASSERT( m_size != 0 );
    HELIUM_ASSERT( m_pBuffer );

    size_t elementIndex = m_size / ( sizeof( uint32_t ) * 8 );
    size_t bitIndex = m_size % ( sizeof( uint32_t ) * 8 );

    return ReferenceType( *( m_pBuffer + elementIndex ), 1U << bitIndex );
}

/// Get the last bit in this array.
///
/// @return  Constant reference to the last bit in this array.
///
/// @see GetFirst()
template< typename Allocator >
typename Helium::BitArray< Allocator >::ConstReferenceType Helium::BitArray< Allocator >::GetLast() const
{
    HELIUM_ASSERT( m_size != 0 );
    HELIUM_ASSERT( m_pBuffer );

    size_t elementIndex = m_size / ( sizeof( uint32_t ) * 8 );
    size_t bitIndex = m_size % ( sizeof( uint32_t ) * 8 );

    return ConstReferenceType( *( m_pBuffer + elementIndex ), 1U << bitIndex );
}

/// Push a bit onto the end of this array.
///
/// @param[in] bValue  True to push a set bit, false to push an unset bit.
///
/// @see Pop()
template< typename Allocator >
size_t Helium::BitArray< Allocator >::Push( bool bValue )
{
    size_t index = m_size;
    Add( bValue );

    return index;
}

/// Remove the last element from this array.
///
/// @see Push()
template< typename Allocator >
void Helium::BitArray< Allocator >::Pop()
{
    HELIUM_ASSERT( m_size != 0 );

    Resize( m_size - 1 );
}

/// Set this array to the contents of the given array.
///
/// If the given array is not the same as this array, this will always destroy the current contents of this array and
/// allocate a fresh array whose capacity matches the size of the given array.
///
/// @param[in] rSource  Array from which to copy.
///
/// @return  Reference to this array.
template< typename Allocator >
Helium::BitArray< Allocator >& Helium::BitArray< Allocator >::operator=( const BitArray& rSource )
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
template< typename Allocator >
template< typename OtherAllocator >
Helium::BitArray< Allocator >& Helium::BitArray< Allocator >::operator=( const BitArray< OtherAllocator >& rSource )
{
    return Assign( rSource );
}

/// Get the bit at the specified index.
///
/// @param[in] index  Array index.
///
/// @return  Reference to the bit at the specified index.
template< typename Allocator >
typename Helium::BitArray< Allocator >::ReferenceType Helium::BitArray< Allocator >::operator[]( ptrdiff_t index )
{
    return GetElement( static_cast< size_t >( index ) );
}

/// Get the bit at the specified index.
///
/// @param[in] index  Array index.
///
/// @return  Constant reference to the bit at the specified index.
template< typename Allocator >
typename Helium::BitArray< Allocator >::ConstReferenceType Helium::BitArray< Allocator >::operator[](
    ptrdiff_t index ) const
{
    return GetElement( static_cast< size_t >( index ) );
}

/// Get the capacity to which this array should grow if growing to support the desired number of bits.
///
/// @param[in] desiredCount  Desired minimum capacity.
///
/// @return  Recommended capacity.
template< typename Allocator >
size_t Helium::BitArray< Allocator >::GetGrowCapacity( size_t desiredCount ) const
{
    HELIUM_ASSERT( desiredCount > m_capacity );
    return Align( Max< size_t >( desiredCount, m_capacity + m_capacity / 2 + 1 ), sizeof( uint32_t ) * 8 );
}

/// Increase the capacity of this array according to the normal growth rules.
///
/// @param[in] capacity  New capacity.
template< typename Allocator >
void Helium::BitArray< Allocator >::Grow( size_t capacity )
{
    if( capacity > m_capacity )
    {
        capacity = GetGrowCapacity( capacity );
        size_t byteCount = capacity / 8;

        m_pBuffer = static_cast< uint32_t* >( Allocator().Reallocate( m_pBuffer, byteCount ) );
        HELIUM_ASSERT( m_pBuffer );

        m_capacity = capacity;
    }
}

/// Assignment operator implementation.
///
/// This is separated out to help deal with the fact that the default (shallow-copy) assignment operator is used if we
/// define just a template assignment operator overload for any allocator type in the source array and not one that only
/// takes the same exact BitArray type (non-templated) as well.
///
/// @param[in] rSource  Array from which to copy.
///
/// @return  Reference to this array.
template< typename Allocator >
template< typename OtherAllocator >
Helium::BitArray< Allocator >& Helium::BitArray< Allocator >::Assign( const BitArray< OtherAllocator >& rSource )
{
    if( this != &rSource )
    {
        Clear();

        m_size = rSource.m_size;
        m_capacity = Align( m_size, sizeof( uint32_t ) * 8 );
        if( m_size != 0 )
        {
            size_t byteCount = m_capacity / 8;
            m_pBuffer = static_cast< uint32_t* >( Allocator().Allocate( byteCount ) );
            HELIUM_ASSERT( m_pBuffer );
            HELIUM_ASSERT( rSource.m_pBuffer );
            MemoryCopy( m_pBuffer, rSource.m_pBuffer, ( m_size + 7 ) / 8 );
        }
    }

    return *this;
}
