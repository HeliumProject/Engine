/// Hash set key extract function.
///
/// @param[in] rValue  Value from which to extract the key.
///
/// @return  Constant reference to the key.
template< typename T >
const T& Helium::HashSetExtractKey< T >::operator()( const T& rValue ) const
{
    return rValue;
}

/// Constructor.
///
/// @param[in] bucketCount  Number of buckets to allocate in the hash table.
template< typename Key, typename HashFunction, typename EqualKey, typename Allocator >
Helium::HashSet< Key, HashFunction, EqualKey, Allocator >::HashSet( size_t bucketCount )
    : Super( bucketCount, HashFunction(), EqualKey() )
{
}

/// Copy constructor.
///
/// @param[in] rSource  Source hash set from which to copy.
template< typename Key, typename HashFunction, typename EqualKey, typename Allocator >
Helium::HashSet< Key, HashFunction, EqualKey, Allocator >::HashSet( const HashSet& rSource )
    : Super( rSource )
{
}

/// Copy constructor.
///
/// @param[in] rSource  Source hash set from which to copy.
template< typename Key, typename HashFunction, typename EqualKey, typename Allocator >
template< typename OtherAllocator >
Helium::HashSet< Key, HashFunction, EqualKey, Allocator >::HashSet(
    const HashSet< Key, HashFunction, EqualKey, OtherAllocator >& rSource )
    : Super( rSource )
{
}

/// Destructor.
template< typename Key, typename HashFunction, typename EqualKey, typename Allocator >
Helium::HashSet< Key, HashFunction, EqualKey, Allocator >::~HashSet()
{
}

/// Assignment operator.
///
/// @param[in] rSource  Source hash set from which to copy.
///
/// @return  Reference to this object.
template< typename Key, typename HashFunction, typename EqualKey, typename Allocator >
Helium::HashSet< Key, HashFunction, EqualKey, Allocator >&
    Helium::HashSet< Key, HashFunction, EqualKey, Allocator >::operator=( const HashSet& rSource )
{
    if( this != &rSource )
    {
        Super::operator=( rSource );
    }

    return *this;
}

/// Assignment operator.
///
/// @param[in] rSource  Source hash set from which to copy.
///
/// @return  Reference to this object.
template< typename Key, typename HashFunction, typename EqualKey, typename Allocator >
template< typename OtherAllocator >
Helium::HashSet< Key, HashFunction, EqualKey, Allocator >&
    Helium::HashSet< Key, HashFunction, EqualKey, Allocator >::operator=(
        const HashSet< Key, HashFunction, EqualKey, OtherAllocator >& rSource )
{
    if( this != &rSource )
    {
        Super::operator=( rSource );
    }

    return *this;
}
