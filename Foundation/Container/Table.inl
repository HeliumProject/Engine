/// Constructor.
template< typename Value, typename Key, typename ExtractKey, typename EqualKey, typename Allocator, typename InternalValue >
Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::Table()
{
}

/// Copy constructor.
///
/// @param[in] rSource  Source table from which to copy.
template< typename Value, typename Key, typename ExtractKey, typename EqualKey, typename Allocator, typename InternalValue >
Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::Table( const Table& rSource )
    : m_elements( rSource.m_elements )
{
}

/// Copy constructor.
///
/// @param[in] rSource  Source table from which to copy.
template< typename Value, typename Key, typename ExtractKey, typename EqualKey, typename Allocator, typename InternalValue >
template< typename OtherAllocator >
Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::Table(
    const Table< Value, Key, ExtractKey, EqualKey, OtherAllocator, InternalValue >& rSource )
    : m_elements( rSource.m_elements )
{
}

/// Get the number of elements in this table.
///
/// @return  Number of elements in this table.
///
/// @see GetCapacity(), IsEmpty()
template< typename Value, typename Key, typename ExtractKey, typename EqualKey, typename Allocator, typename InternalValue >
size_t Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::GetSize() const
{
    return m_elements.GetSize();
}

/// Get whether this table is currently empty.
///
/// @return  True if this table empty, false if not.
///
/// @see GetSize()
template< typename Value, typename Key, typename ExtractKey, typename EqualKey, typename Allocator, typename InternalValue >
bool Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::IsEmpty() const
{
    return m_elements.IsEmpty();
}

/// Get the maximum number of elements which this table can contain without requiring reallocation of memory.
///
/// @return  Current table capacity.
///
/// @see GetSize(), Reserve()
template< typename Value, typename Key, typename ExtractKey, typename EqualKey, typename Allocator, typename InternalValue >
size_t Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::GetCapacity() const
{
    return m_elements.GetCapacity();
}

/// Explicitly increase the capacity of this array to support at least the specified number of elements.
///
/// If the requested capacity is less than the current capacity, no memory will be reallocated.
///
/// @param[in] capacity  Desired capacity.
///
/// @see GetCapacity()
template< typename Value, typename Key, typename ExtractKey, typename EqualKey, typename Allocator, typename InternalValue >
void Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::Reserve( size_t capacity )
{
    m_elements.Reserve( capacity );
}

/// Resize the allocated table memory to match the size actually in use.
///
/// @see GetCapacity()
template< typename Value, typename Key, typename ExtractKey, typename EqualKey, typename Allocator, typename InternalValue >
void Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::Trim()
{
    m_elements.Trim();
}

/// Resize the table to zero and free all allocated memory.
template< typename Value, typename Key, typename ExtractKey, typename EqualKey, typename Allocator, typename InternalValue >
void Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::Clear()
{
    m_elements.Clear();
}

/// Retrieve an iterator referencing the beginning of this table.
///
/// @return  Iterator at the beginning of this table.
///
/// @see End()
template< typename Value, typename Key, typename ExtractKey, typename EqualKey, typename Allocator, typename InternalValue >
typename Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::Iterator
    Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::Begin()
{
    return Iterator( m_elements.GetData() );
}

/// Retrieve a constant iterator referencing the beginning of this table.
///
/// @return  Constant iterator at the beginning of this table.
///
/// @see End()
template< typename Value, typename Key, typename ExtractKey, typename EqualKey, typename Allocator, typename InternalValue >
typename Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::ConstIterator
    Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::Begin() const
{
    return ConstIterator( m_elements.GetData() );
}

/// Retrieve an iterator referencing the end of this table.
///
/// @return  Iterator at the end of this table.
///
/// @see Begin()
template< typename Value, typename Key, typename ExtractKey, typename EqualKey, typename Allocator, typename InternalValue >
typename Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::Iterator
    Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::End()
{
    return Iterator( m_elements.GetData() + m_elements.GetSize() );
}

/// Retrieve a constant iterator referencing the end of this table.
///
/// @return  Constant iterator at the end of this table.
///
/// @see Begin()
template< typename Value, typename Key, typename ExtractKey, typename EqualKey, typename Allocator, typename InternalValue >
typename Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::ConstIterator
    Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::End() const
{
    return ConstIterator( m_elements.GetData() + m_elements.GetSize() );
}

/// Find an entry in this table associated with the given key.
///
/// @param[in] rKey  Key to locate.
///
/// @return  Iterator referencing the element with the given key if found, otherwise an iterator referencing the end of
///          this table if not found.
template< typename Value, typename Key, typename ExtractKey, typename EqualKey, typename Allocator, typename InternalValue >
typename Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::Iterator
    Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::Find(
        const Key& rKey )
{
    EqualKey keyEquals;
    ExtractKey keyExtract;

    Iterator endIterator = End();
    for( Iterator iterator = Begin(); iterator != endIterator; ++iterator )
    {
        if( keyEquals( rKey, keyExtract( *iterator ) ) )
        {
            return iterator;
        }
    }

    return endIterator;
}

/// Find an entry in this table associated with the given key.
///
/// @param[in] rKey  Key to locate.
///
/// @return  Constant iterator referencing the element with the given key if found, otherwise a constant iterator
///          referencing the end of this table if not found.
template< typename Value, typename Key, typename ExtractKey, typename EqualKey, typename Allocator, typename InternalValue >
typename Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::ConstIterator
    Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::Find( const Key& rKey ) const
{
    EqualKey keyEquals;
    ExtractKey keyExtract;

    ConstIterator endIterator = End();
    for( ConstIterator iterator = Begin(); iterator != endIterator; ++iterator )
    {
        if( keyEquals( rKey, keyExtract( *iterator ) ) )
        {
            return iterator;
        }
    }

    return endIterator;
}

/// Insert an element into this table if an element with the same key does not already exist.
///
/// @param[in] rValue  Element to insert.
///
/// @return  A pair containing an iterator and a boolean value.  If the element was inserted, the iterator will point to
///          the inserted entry, and the boolean value will be set to true.  If an element already existed with the same
///          key, the iterator will point to the existing entry, and the boolean value will be set to false.
template< typename Value, typename Key, typename ExtractKey, typename EqualKey, typename Allocator, typename InternalValue >
Helium::Pair< typename Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::Iterator, bool >
    Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::Insert( const ValueType& rValue )
{
    ExtractKey keyExtract;

    Iterator iterator = Find( keyExtract( rValue ) );
    bool bInserted = ( iterator == End() );

    if( bInserted )
    {
        size_t index = m_elements.Push( rValue );
        iterator = Begin() + index;
    }

    return Pair< Iterator, bool >( iterator, bInserted );
}

/// Insert an element into this table if an element with the same key does not already exist.
///
/// @param[out] rIterator  If the element was inserted, this iterator will point to the inserted entry.  If an element
///                        already existed with the same key, this iterator will point to the existing entry.
/// @param[in]  rValue     Element to insert.
///
/// @return  True if the element was inserted, false if an element already exists in this array with the same key.
template< typename Value, typename Key, typename ExtractKey, typename EqualKey, typename Allocator, typename InternalValue >
bool Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::Insert(
    ConstIterator& rIterator,
    const ValueType& rValue )
{
    ExtractKey keyExtract;

    rIterator = Find( keyExtract( rValue ) );
    bool bInserted = ( rIterator == End() );

    if( bInserted )
    {
        size_t index = m_elements.Push( rValue );
        rIterator = Begin() + index;
    }

    return bInserted;
}

/// Remove the element from this table with the given key if one exists.
///
/// @param[in] rKey  Key of the element to remove.
///
/// @return  True if an element with the given key was found and removed, false if not.
template< typename Value, typename Key, typename ExtractKey, typename EqualKey, typename Allocator, typename InternalValue >
bool Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::Remove( const Key& rKey )
{
    Iterator iterator = Find( rKey );
    if( iterator == End() )
    {
        return false;
    }

    Remove( iterator );

    return true;
}

/// Remove the element referenced by the given iterator from this table.
///
/// @param[in] iterator  Iterator pointing to the element to remove.
template< typename Value, typename Key, typename ExtractKey, typename EqualKey, typename Allocator, typename InternalValue >
void Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::Remove( Iterator iterator )
{
    m_elements.RemoveSwap( static_cast< size_t >( &( *iterator ) - m_elements.GetData() ) );
}

/// Remove the elements referenced by the given range from this table.
///
/// @param[in] start  Iterator pointing to the starting element to remove.
/// @param[in] end    Iterator pointing to the end of the range (just past the last element) to remove.
template< typename Value, typename Key, typename ExtractKey, typename EqualKey, typename Allocator, typename InternalValue >
void Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::Remove( Iterator start, Iterator end )
{
    m_elements.RemoveSwap(
        static_cast< size_t >( &( *start ) - m_elements.GetData() ),
        static_cast< size_t >( end - start ) );
}

/// Swap the contents of this table with another table.
///
/// @param[in] rTable  Table with which to swap.
template< typename Value, typename Key, typename ExtractKey, typename EqualKey, typename Allocator, typename InternalValue >
void Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::Swap( Table& rTable )
{
    m_elements.Swap( rTable.m_elements );
}

/// Set this table to the contents of the given table.
///
/// If the given table is not the same as this table, this will always destroy the current contents of this table and
/// allocate a fresh table whose capacity matches the size of the given table.
///
/// @param[in] rSource  Table from which to copy.
///
/// @return  Reference to this table.
template< typename Value, typename Key, typename ExtractKey, typename EqualKey, typename Allocator, typename InternalValue >
Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >&
    Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::operator=(
        const Table& rSource )
{
    m_elements = rSource.m_elements;

    return *this;
}

/// Set this table to the contents of the given table.
///
/// If the given table is not the same as this table, this will always destroy the current contents of this table and
/// allocate a fresh table whose capacity matches the size of the given table.
///
/// @param[in] rSource  Table from which to copy.
///
/// @return  Reference to this table.
template< typename Value, typename Key, typename ExtractKey, typename EqualKey, typename Allocator, typename InternalValue >
template< typename OtherAllocator >
Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >&
    Helium::Table< Value, Key, ExtractKey, EqualKey, Allocator, InternalValue >::operator=(
        const Table< Value, Key, ExtractKey, EqualKey, OtherAllocator, InternalValue >& rSource )
{
    m_elements = rSource.m_elements;

    return *this;
}
