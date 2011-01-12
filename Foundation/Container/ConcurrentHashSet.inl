/// Constructor.
///
/// @param[in] bucketCount  Number of buckets to allocate in the hash table.
template< typename Key, typename HashFunction, typename EqualKey, typename Allocator >
Helium::ConcurrentHashSet< Key, HashFunction, EqualKey, Allocator >::ConcurrentHashSet( size_t bucketCount )
    : Base( bucketCount, HashFunction(), EqualKey() )
{
}

/// Copy constructor.
///
/// @param[in] rSource  Source hash set from which to copy.
template< typename Key, typename HashFunction, typename EqualKey, typename Allocator >
Helium::ConcurrentHashSet< Key, HashFunction, EqualKey, Allocator >::ConcurrentHashSet(
    const ConcurrentHashSet& rSource )
    : Base( rSource )
{
}

/// Destructor.
template< typename Key, typename HashFunction, typename EqualKey, typename Allocator >
Helium::ConcurrentHashSet< Key, HashFunction, EqualKey, Allocator >::~ConcurrentHashSet()
{
}

/// Assignment operator.
///
/// @param[in] rSource  Source hash set from which to copy.
///
/// @return  Reference to this object.
template< typename Key, typename HashFunction, typename EqualKey, typename Allocator >
Helium::ConcurrentHashSet< Key, HashFunction, EqualKey, Allocator >&
    Helium::ConcurrentHashSet< Key, HashFunction, EqualKey, Allocator >::operator=( const ConcurrentHashSet& rSource )
{
    if( this != &rSource )
    {
        Base::operator=( rSource );
    }

    return *this;
}
