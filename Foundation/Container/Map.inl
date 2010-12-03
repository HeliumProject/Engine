/// Constructor.
template< typename Key, typename Data, typename EqualKey, typename Allocator >
Helium::Map< Key, Data, EqualKey, Allocator >::Map()
{
}

/// Copy constructor.
///
/// @param[in] rSource  Source map from which to copy.
template< typename Key, typename Data, typename EqualKey, typename Allocator >
Helium::Map< Key, Data, EqualKey, Allocator >::Map( const Map& rSource )
    : m_elements( rSource.m_elements )
{
}

/// Copy constructor.
///
/// @param[in] rSource  Source map from which to copy.
template< typename Key, typename Data, typename EqualKey, typename Allocator >
template< typename OtherAllocator >
Helium::Map< Key, Data, EqualKey, Allocator >::Map( const Map< Key, Data, EqualKey, OtherAllocator >& rSource )
    : m_elements( rSource.m_elements )
{
}

/// Get the number of elements in this map.
///
/// @return  Number of elements in this map.
///
/// @see GetCapacity(), IsEmpty()
template< typename Key, typename Data, typename EqualKey, typename Allocator >
size_t Helium::Map< Key, Data, EqualKey, Allocator >::GetSize() const
{
    return m_elements.GetSize();
}

/// Get whether this map is currently empty.
///
/// @return  True if this map empty, false if not.
///
/// @see GetSize()
template< typename Key, typename Data, typename EqualKey, typename Allocator >
bool Helium::Map< Key, Data, EqualKey, Allocator >::IsEmpty() const
{
    return m_elements.IsEmpty();
}

/// Get the maximum number of elements which this map can contain without requiring reallocation of memory.
///
/// @return  Current map capacity.
///
/// @see GetSize(), Reserve()
template< typename Key, typename Data, typename EqualKey, typename Allocator >
size_t Helium::Map< Key, Data, EqualKey, Allocator >::GetCapacity() const
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
template< typename Key, typename Data, typename EqualKey, typename Allocator >
void Helium::Map< Key, Data, EqualKey, Allocator >::Reserve( size_t capacity )
{
    m_elements.Reserve( capacity );
}

/// Resize the allocated map memory to match the size actually in use.
///
/// @see GetCapacity()
template< typename Key, typename Data, typename EqualKey, typename Allocator >
void Helium::Map< Key, Data, EqualKey, Allocator >::Trim()
{
    m_elements.Trim();
}

/// Resize the map to zero and free all allocated memory.
template< typename Key, typename Data, typename EqualKey, typename Allocator >
void Helium::Map< Key, Data, EqualKey, Allocator >::Clear()
{
    m_elements.Clear();
}

/// Retrieve an iterator referencing the beginning of this map.
///
/// @return  Iterator at the beginning of this map.
///
/// @see End()
template< typename Key, typename Data, typename EqualKey, typename Allocator >
typename Helium::Map< Key, Data, EqualKey, Allocator >::Iterator Helium::Map< Key, Data, EqualKey, Allocator >::Begin()
{
    return Iterator( m_elements.GetData() );
}

/// Retrieve a constant iterator referencing the beginning of this map.
///
/// @return  Constant iterator at the beginning of this map.
///
/// @see End()
template< typename Key, typename Data, typename EqualKey, typename Allocator >
typename Helium::Map< Key, Data, EqualKey, Allocator >::ConstIterator
    Helium::Map< Key, Data, EqualKey, Allocator >::Begin() const
{
    return ConstIterator( m_elements.GetData() );
}

/// Retrieve an iterator referencing the end of this map.
///
/// @return  Iterator at the end of this map.
///
/// @see Begin()
template< typename Key, typename Data, typename EqualKey, typename Allocator >
typename Helium::Map< Key, Data, EqualKey, Allocator >::Iterator Helium::Map< Key, Data, EqualKey, Allocator >::End()
{
    return Iterator( m_elements.GetData() + m_elements.GetSize() );
}

/// Retrieve a constant iterator referencing the end of this map.
///
/// @return  Constant iterator at the end of this map.
///
/// @see Begin()
template< typename Key, typename Data, typename EqualKey, typename Allocator >
typename Helium::Map< Key, Data, EqualKey, Allocator >::ConstIterator
    Helium::Map< Key, Data, EqualKey, Allocator >::End() const
{
    return ConstIterator( m_elements.GetData() + m_elements.GetSize() );
}

/// Find an entry in this map associated with the given key.
///
/// @param[in] rKey  Key to locate.
///
/// @return  Iterator referencing the element with the given key if found, otherwise an iterator referencing the end of
///          this map if not found.
template< typename Key, typename Data, typename EqualKey, typename Allocator >
typename Helium::Map< Key, Data, EqualKey, Allocator >::Iterator Helium::Map< Key, Data, EqualKey, Allocator >::Find(
    const Key& rKey )
{
    EqualKey keyEquals;

    Iterator endIterator = End();
    for( Iterator iterator = Begin(); iterator != endIterator; ++iterator )
    {
        if( keyEquals( rKey, iterator->First() ) )
        {
            return iterator;
        }
    }

    return endIterator;
}

/// Find an entry in this map associated with the given key.
///
/// @param[in] rKey  Key to locate.
///
/// @return  Constant iterator referencing the element with the given key if found, otherwise a constant iterator
///          referencing the end of this map if not found.
template< typename Key, typename Data, typename EqualKey, typename Allocator >
typename Helium::Map< Key, Data, EqualKey, Allocator >::ConstIterator
    Helium::Map< Key, Data, EqualKey, Allocator >::Find( const Key& rKey ) const
{
    EqualKey keyEquals;

    ConstIterator endIterator = End();
    for( ConstIterator iterator = Begin(); iterator != endIterator; ++iterator )
    {
        if( keyEquals( rKey, iterator->First() ) )
        {
            return iterator;
        }
    }

    return endIterator;
}

/// Insert an element into this map if an element with the same key does not already exist.
///
/// @param[in] rValue  Element to insert.
///
/// @return  A pair containing an iterator and a boolean value.  If the element was inserted, the iterator will point to
///          the inserted entry, and the boolean value will be set to true.  If an element already existed with the same
///          key, the iterator will point to the existing entry, and the boolean value will be set to false.
template< typename Key, typename Data, typename EqualKey, typename Allocator >
Helium::Pair< typename Helium::Map< Key, Data, EqualKey, Allocator >::Iterator, bool >
    Helium::Map< Key, Data, EqualKey, Allocator >::Insert( const ValueType& rValue )
{
    Iterator iterator = Find( rValue.First() );
    bool bInserted = ( iterator == End() );

    if( bInserted )
    {
        size_t index = m_elements.Push( rValue );
        iterator = Begin() + index;
    }

    return Pair< Iterator, bool >( iterator, bInserted );
}

/// Insert an element into this map if an element with the same key does not already exist.
///
/// @param[out] rIterator  If the element was inserted, this iterator will point to the inserted entry.  If an element
///                        already existed with the same key, this iterator will point to the existing entry.
/// @param[in]  rValue     Element to insert.
///
/// @return  True if the element was inserted, false if an element already exists in this array with the same key.
template< typename Key, typename Data, typename EqualKey, typename Allocator >
bool Helium::Map< Key, Data, EqualKey, Allocator >::Insert( ConstIterator& rIterator, const ValueType& rValue )
{
    rIterator = Find( rValue.First() );
    bool bInserted = ( rIterator == End() );

    if( bInserted )
    {
        size_t index = m_elements.Push( rValue );
        rIterator = Begin() + index;
    }

    return bInserted;
}

/// Remove the element from this map with the given key if one exists.
///
/// @param[in] rKey  Key of the element to remove.
///
/// @return  True if an element with the given key was found and removed, false if not.
template< typename Key, typename Data, typename EqualKey, typename Allocator >
bool Helium::Map< Key, Data, EqualKey, Allocator >::Remove( const Key& rKey )
{
    Iterator iterator = Find( rKey );
    if( iterator == End() )
    {
        return false;
    }

    Remove( iterator );

    return true;
}

/// Remove the element referenced by the given iterator from this map.
///
/// @param[in] iterator  Iterator pointing to the element to remove.
template< typename Key, typename Data, typename EqualKey, typename Allocator >
void Helium::Map< Key, Data, EqualKey, Allocator >::Remove( Iterator iterator )
{
    m_elements.RemoveSwap( static_cast< size_t >( iterator - m_elements.Begin() ) );
}

/// Remove the elements referenced by the given range from this map.
///
/// @param[in] start  Iterator pointing to the starting element to remove.
/// @param[in] end    Iterator pointing to the end of the range (just past the last element) to remove.
template< typename Key, typename Data, typename EqualKey, typename Allocator >
void Helium::Map< Key, Data, EqualKey, Allocator >::Remove( Iterator start, Iterator end )
{
    m_elements.RemoveSwap( static_cast< size_t >( start - m_elements.Begin() ), static_cast< size_t >( end - start ) );
}

/// Swap the contents of this map with another map.
///
/// @param[in] rMap  Map with which to swap.
template< typename Key, typename Data, typename EqualKey, typename Allocator >
void Helium::Map< Key, Data, EqualKey, Allocator >::Swap( Map& rMap )
{
    m_elements.Swap( rMap.m_elements );
}

/// Set this map to the contents of the given map.
///
/// If the given map is not the same as this map, this will always destroy the current contents of this map and allocate
/// a fresh map whose capacity matches the size of the given map.
///
/// @param[in] rSource  Map from which to copy.
///
/// @return  Reference to this map.
template< typename Key, typename Data, typename EqualKey, typename Allocator >
Helium::Map< Key, Data, EqualKey, Allocator >& Helium::Map< Key, Data, EqualKey, Allocator >::operator=(
    const Map& rSource )
{
    m_elements = rSource.m_elements;

    return *this;
}

/// Set this map to the contents of the given map.
///
/// If the given map is not the same as this map, this will always destroy the current contents of this map and allocate
/// a fresh map whose capacity matches the size of the given map.
///
/// @param[in] rSource  Map from which to copy.
///
/// @return  Reference to this map.
template< typename Key, typename Data, typename EqualKey, typename Allocator >
template< typename OtherAllocator >
Helium::Map< Key, Data, EqualKey, Allocator >& Helium::Map< Key, Data, EqualKey, Allocator >::operator=(
    const Map< Key, Data, EqualKey, OtherAllocator >& rSource )
{
    m_elements = rSource.m_elements;

    return *this;
}

/// Retrieve the data associated with the specified key in this map, creating a new entry with the default data value if
/// no such entry currently exists.
///
/// @param[in] rKey  Key to locate.
///
/// @return  Reference to the data associated with the given key.
template< typename Key, typename Data, typename EqualKey, typename Allocator >
Data& Helium::Map< Key, Data, EqualKey, Allocator >::operator[]( const Key& rKey )
{
    Iterator iterator;
    Insert( iterator, Pair< Key, Data >( rKey, Data() ) );

    return iterator->Second();
}
