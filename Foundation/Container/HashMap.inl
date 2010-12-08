/// Constructor.
///
/// @param[in] bucketCount  Number of buckets to allocate in the hash table.
template< typename Key, typename Data, typename HashFunction, typename EqualKey, typename Allocator >
Helium::HashMap< Key, Data, HashFunction, EqualKey, Allocator >::HashMap( size_t bucketCount )
    : Super( bucketCount, HashFunction(), EqualKey() )
{
}

/// Copy constructor.
///
/// @param[in] rSource  Source hash set from which to copy.
template< typename Key, typename Data, typename HashFunction, typename EqualKey, typename Allocator >
Helium::HashMap< Key, Data, HashFunction, EqualKey, Allocator >::HashMap( const HashMap& rSource )
    : Super( rSource )
{
}

/// Copy constructor.
///
/// @param[in] rSource  Source hash set from which to copy.
template< typename Key, typename Data, typename HashFunction, typename EqualKey, typename Allocator >
template< typename OtherAllocator >
Helium::HashMap< Key, Data, HashFunction, EqualKey, Allocator >::HashMap(
    const HashMap< Key, Data, HashFunction, EqualKey, OtherAllocator >& rSource )
    : Super( rSource )
{
}

/// Destructor.
template< typename Key, typename Data, typename HashFunction, typename EqualKey, typename Allocator >
Helium::HashMap< Key, Data, HashFunction, EqualKey, Allocator >::~HashMap()
{
}

/// Assignment operator.
///
/// @param[in] rSource  Source hash map from which to copy.
///
/// @return  Reference to this object.
template< typename Key, typename Data, typename HashFunction, typename EqualKey, typename Allocator >
Helium::HashMap< Key, Data, HashFunction, EqualKey, Allocator >&
    Helium::HashMap< Key, Data, HashFunction, EqualKey, Allocator >::operator=( const HashMap& rSource )
{
    if( this != &rSource )
    {
        Super::operator=( rSource );
    }

    return *this;
}

/// Assignment operator.
///
/// @param[in] rSource  Source hash map from which to copy.
///
/// @return  Reference to this object.
template< typename Key, typename Data, typename HashFunction, typename EqualKey, typename Allocator >
template< typename OtherAllocator >
Helium::HashMap< Key, Data, HashFunction, EqualKey, Allocator >&
    Helium::HashMap< Key, Data, HashFunction, EqualKey, Allocator >::operator=(
        const HashMap< Key, Data, HashFunction, EqualKey, OtherAllocator >& rSource )
{
    if( this != &rSource )
    {
        Super::operator=( rSource );
    }

    return *this;
}
