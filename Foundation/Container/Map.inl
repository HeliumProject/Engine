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
    : Super( rSource.m_elements )
{
}

/// Copy constructor.
///
/// @param[in] rSource  Source map from which to copy.
template< typename Key, typename Data, typename EqualKey, typename Allocator >
template< typename OtherAllocator >
Helium::Map< Key, Data, EqualKey, Allocator >::Map( const Map< Key, Data, EqualKey, OtherAllocator >& rSource )
    : Super( rSource.m_elements )
{
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
    Super::operator=( rSource );

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
    Super::operator=( rSource );

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
    Insert( iterator, InternalValueType( rKey, Data() ) );

    return iterator->Second();
}
