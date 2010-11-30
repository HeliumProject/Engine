/// Hash map key extract function.
///
/// @param[in] rValue  Value from which to extract the key.
///
/// @return  Constant reference to the key.
template< typename Key, typename Data >
const Key& Helium::ConcurrentHashMapExtractKey< Key, Data >::operator()( const KeyValue< Key, Data >& rValue ) const
{
    return rValue.First();
}

/// Constructor.
///
/// @param[in] bucketCount  Number of buckets to allocate in the hash table.
template< typename Key, typename Data, typename HashFunction, typename EqualKey, typename Allocator >
Helium::ConcurrentHashMap< Key, Data, HashFunction, EqualKey, Allocator >::ConcurrentHashMap( size_t bucketCount )
    : Super( bucketCount, HashFunction(), EqualKey() )
{
}

/// Copy constructor.
///
/// @param[in] rSource  Source hash set from which to copy.
template< typename Key, typename Data, typename HashFunction, typename EqualKey, typename Allocator >
Helium::ConcurrentHashMap< Key, Data, HashFunction, EqualKey, Allocator >::ConcurrentHashMap(
    const ConcurrentHashMap& rSource )
    : Super( rSource )
{
}

/// Destructor.
template< typename Key, typename Data, typename HashFunction, typename EqualKey, typename Allocator >
Helium::ConcurrentHashMap< Key, Data, HashFunction, EqualKey, Allocator >::~ConcurrentHashMap()
{
}

/// Assignment operator.
///
/// @param[in] rSource  Source hash map from which to copy.
///
/// @return  Reference to this object.
template< typename Key, typename Data, typename HashFunction, typename EqualKey, typename Allocator >
Helium::ConcurrentHashMap< Key, Data, HashFunction, EqualKey, Allocator >&
    Helium::ConcurrentHashMap< Key, Data, HashFunction, EqualKey, Allocator >::operator=(
        const ConcurrentHashMap& rSource )
{
    if( this != &rSource )
    {
        Super::operator=( rSource );
    }

    return *this;
}
