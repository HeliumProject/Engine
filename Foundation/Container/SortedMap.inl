/// Constructor
template< typename Key, typename Data, typename CompareKey, typename Allocator >
Helium::SortedMap< Key, Data, CompareKey, Allocator >::SortedMap()
{
}

/// Copy constructor.
///
/// @param[in] rSource  Source object from which to copy.
template< typename Key, typename Data, typename CompareKey, typename Allocator >
Helium::SortedMap< Key, Data, CompareKey, Allocator >::SortedMap( const SortedMap& rSource )
    : RbTree( rSource )
{
}

/// Copy constructor.
///
/// @param[in] rSource  Source object from which to copy.
template< typename Key, typename Data, typename CompareKey, typename Allocator >
template< typename OtherAllocator >
Helium::SortedMap< Key, Data, CompareKey, Allocator >::SortedMap(
    const SortedMap< Key, Data, CompareKey, OtherAllocator >& rSource )
    : RbTree( rSource )
{
}

/// Assignment operator.
///
/// @param[in] rSource  Source object from which to copy.
///
/// @return  Reference to this object.
template< typename Key, typename Data, typename CompareKey, typename Allocator >
Helium::SortedMap< Key, Data, CompareKey, Allocator >& Helium::SortedMap< Key, Data, CompareKey, Allocator >::operator=(
    const SortedMap& rSource )
{
    Base::operator=( rSource );

    return *this;
}

/// Assignment operator.
///
/// @param[in] rSource  Source object from which to copy.
///
/// @return  Reference to this object.
template< typename Key, typename Data, typename CompareKey, typename Allocator >
template< typename OtherAllocator >
Helium::SortedMap< Key, Data, CompareKey, Allocator >& Helium::SortedMap< Key, Data, CompareKey, Allocator >::operator=(
    const SortedMap< Key, Data, CompareKey, OtherAllocator >& rSource )
{
    Base::operator=( rSource );

    return *this;
}

/// Retrieve the data associated with the specified key in this map, creating a new entry with the default data value if
/// no such entry currently exists.
///
/// @param[in] rKey  Key to locate.
///
/// @return  Reference to the data associated with the given key.
template< typename Key, typename Data, typename CompareKey, typename Allocator >
Data& Helium::SortedMap< Key, Data, CompareKey, Allocator >::operator[]( const Key& rKey )
{
    Iterator iterator;
    Insert( iterator, Pair< Key, Data >( rKey, Data() ) );

    return iterator->Second();
}
