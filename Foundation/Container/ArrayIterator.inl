/// Constructor.
///
/// Creates an uninitialized iterator.  Using this is not safe until it is initialized.
template< typename T >
Helium::ConstArrayIterator< T >::ConstArrayIterator()
{
}

/// Constructor.
///
/// @param[in] pElement  Pointer to the current array element.
template< typename T >
Helium::ConstArrayIterator< T >::ConstArrayIterator( const T* pElement )
    : m_pElement( const_cast< T* >( pElement ) )
{
}

/// Access the current array element.
///
/// @return  Constant reference to the current array element.
template< typename T >
typename Helium::ConstArrayIterator< T >::ConstReferenceType Helium::ConstArrayIterator< T >::operator*() const
{
    return *m_pElement;
}

/// Access the current array element.
///
/// @return  Constant pointer to the current array element.
template< typename T >
typename Helium::ConstArrayIterator< T >::ConstPointerType Helium::ConstArrayIterator< T >::operator->() const
{
    return m_pElement;
}

/// Pre-increment operator.
///
/// Increments this iterator's position by one.
///
/// @return  Reference to this iterator.
template< typename T >
Helium::ConstArrayIterator< T >& Helium::ConstArrayIterator< T >::operator++()
{
    ++m_pElement;

    return *this;
}

/// Post-increment operator.
///
/// Increments this iterator's position by one.
///
/// @return  Copy of this iterator prior to incrementing.
template< typename T >
Helium::ConstArrayIterator< T > Helium::ConstArrayIterator< T >::operator++( int )
{
    ConstArrayIterator result = *this;
    ++m_pElement;

    return result;
}

/// Pre-decrement operator.
///
/// Decrements this iterator's position by one.
///
/// @return  Reference to this iterator.
template< typename T >
Helium::ConstArrayIterator< T >& Helium::ConstArrayIterator< T >::operator--()
{
    --m_pElement;

    return *this;
}

/// Post-decrement operator.
///
/// Decrements this iterator's position by one.
///
/// @return  Copy of this iterator prior to decrementing.
template< typename T >
Helium::ConstArrayIterator< T > Helium::ConstArrayIterator< T >::operator--( int )
{
    ConstArrayIterator result = *this;
    --m_pElement;

    return result;
}

/// In-place addition operator.
///
/// @param[in] offset  Amount by which to increase this iterator's position.
///
/// @return  Reference to this iterator.
template< typename T >
Helium::ConstArrayIterator< T >& Helium::ConstArrayIterator< T >::operator+=( ptrdiff_t offset )
{
    m_pElement += offset;

    return *this;
}

/// Addition operator.
///
/// @param[in] offset  Amount by which to increase this iterator's position.
///
/// @return  Copy of this iterator, increased by the specified amount.
template< typename T >
Helium::ConstArrayIterator< T > Helium::ConstArrayIterator< T >::operator+( ptrdiff_t offset ) const
{
    return ConstArrayIterator( m_pElement + offset );
}

/// In-place subtraction operator.
///
/// @param[in] offset  Amount by which to decrease this iterator's position.
///
/// @return  Reference to this iterator.
template< typename T >
Helium::ConstArrayIterator< T >& Helium::ConstArrayIterator< T >::operator-=( ptrdiff_t offset )
{
    m_pElement -= offset;

    return *this;
}

/// Subtraction operator.
///
/// @param[in] offset  Amount by which to decrease this iterator's position.
///
/// @return  Copy of this iterator, decreased by the specified amount.
template< typename T >
Helium::ConstArrayIterator< T > Helium::ConstArrayIterator< T >::operator-( ptrdiff_t offset ) const
{
    return ConstArrayIterator( m_pElement - offset );
}

/// Subtraction operator.
///
/// @param[in] rOther  Bit array iterator to subtract.
///
/// @return  Offset from the given bit array iterator to this iterator.
template< typename T >
ptrdiff_t Helium::ConstArrayIterator< T >::operator-( const ConstArrayIterator& rOther ) const
{
    return( m_pElement - rOther.m_pElement );
}

/// Equality comparison operator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator and the given iterator reference the same element, false if not.
template< typename T >
bool Helium::ConstArrayIterator< T >::operator==( const ConstArrayIterator& rOther ) const
{
    return( m_pElement == rOther.m_pElement );
}

/// Inequality comparison operator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator and the given iterator do not reference the same element, false if they do.
template< typename T >
bool Helium::ConstArrayIterator< T >::operator!=( const ConstArrayIterator& rOther ) const
{
    return( m_pElement != rOther.m_pElement );
}

/// Less-than comparison operator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator precedes the given iterator, false if not.
template< typename T >
bool Helium::ConstArrayIterator< T >::operator<( const ConstArrayIterator& rOther ) const
{
    return( m_pElement < rOther.m_pElement );
}

/// Greater-than comparison operator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator follows the given iterator, false if not.
template< typename T >
bool Helium::ConstArrayIterator< T >::operator>( const ConstArrayIterator& rOther ) const
{
    return( m_pElement > rOther.m_pElement );
}

/// Less-than-or-equals comparison operator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator precedes or matches the given iterator, false if not.
template< typename T >
bool Helium::ConstArrayIterator< T >::operator<=( const ConstArrayIterator& rOther ) const
{
    return( m_pElement <= rOther.m_pElement );
}

/// Greater-than-or-equals comparison operator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator follows or matches the given iterator, false if not.
template< typename T >
bool Helium::ConstArrayIterator< T >::operator>=( const ConstArrayIterator& rOther ) const
{
    return( m_pElement >= rOther.m_pElement );
}

/// Constructor.
///
/// Creates an uninitialized iterator.  Using this is not safe until it is initialized.
template< typename T >
Helium::ArrayIterator< T >::ArrayIterator()
{
}

/// Constructor.
///
/// @param[in] pElement  Pointer to the current array element.
template< typename T >
Helium::ArrayIterator< T >::ArrayIterator( T* pElement )
    : ConstArrayIterator< T >( pElement )
{
}

/// Access the current array element.
///
/// @return  Reference to the current array element.
template< typename T >
typename Helium::ArrayIterator< T >::ReferenceType Helium::ArrayIterator< T >::operator*() const
{
    return *ConstArrayIterator< T >::m_pElement;
}

/// Access the current array element.
///
/// @return  Pointer to the current array element.
template< typename T >
typename Helium::ArrayIterator< T >::PointerType Helium::ArrayIterator< T >::operator->() const
{
    return ConstArrayIterator< T >::m_pElement;
}

/// Pre-increment operator.
///
/// Increments this iterator's position by one.
///
/// @return  Reference to this iterator.
template< typename T >
Helium::ArrayIterator< T >& Helium::ArrayIterator< T >::operator++()
{
    ++ConstArrayIterator< T >::m_pElement;

    return *this;
}

/// Post-increment operator.
///
/// Increments this iterator's position by one.
///
/// @return  Copy of this iterator prior to incrementing.
template< typename T >
Helium::ArrayIterator< T > Helium::ArrayIterator< T >::operator++( int )
{
    ArrayIterator result = *this;
    ++ConstArrayIterator< T >::m_pElement;

    return result;
}

/// Pre-decrement operator.
///
/// Decrements this iterator's position by one.
///
/// @return  Reference to this iterator.
template< typename T >
Helium::ArrayIterator< T >& Helium::ArrayIterator< T >::operator--()
{
    --ConstArrayIterator< T >::m_pElement;

    return *this;
}

/// Post-decrement operator.
///
/// Decrements this iterator's position by one.
///
/// @return  Copy of this iterator prior to decrementing.
template< typename T >
Helium::ArrayIterator< T > Helium::ArrayIterator< T >::operator--( int )
{
    ArrayIterator result = *this;
    --ConstArrayIterator< T >::m_pElement;

    return result;
}

/// In-place addition operator.
///
/// @param[in] offset  Amount by which to increase this iterator's position.
///
/// @return  Reference to this iterator.
template< typename T >
Helium::ArrayIterator< T >& Helium::ArrayIterator< T >::operator+=( ptrdiff_t offset )
{
    ConstArrayIterator< T >::m_pElement += offset;

    return *this;
}

/// Addition operator.
///
/// @param[in] offset  Amount by which to increase this iterator's position.
///
/// @return  Copy of this iterator, increased by the specified amount.
template< typename T >
Helium::ArrayIterator< T > Helium::ArrayIterator< T >::operator+( ptrdiff_t offset ) const
{
    return ArrayIterator( ConstArrayIterator< T >::m_pElement + offset );
}

/// In-place subtraction operator.
///
/// @param[in] offset  Amount by which to decrease this iterator's position.
///
/// @return  Reference to this iterator.
template< typename T >
Helium::ArrayIterator< T >& Helium::ArrayIterator< T >::operator-=( ptrdiff_t offset )
{
    ConstArrayIterator< T >::m_pElement -= offset;

    return *this;
}

/// Subtraction operator.
///
/// @param[in] offset  Amount by which to decrease this iterator's position.
///
/// @return  Copy of this iterator, decreased by the specified amount.
template< typename T >
Helium::ArrayIterator< T > Helium::ArrayIterator< T >::operator-( ptrdiff_t offset ) const
{
    return ArrayIterator( ConstArrayIterator< T >::m_pElement - offset );
}
