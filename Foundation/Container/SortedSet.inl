/// Constructor
template< typename Key, typename CompareKey, typename Allocator >
Helium::SortedSet< Key, CompareKey, Allocator >::SortedSet()
{
}

/// Copy constructor.
///
/// @param[in] rSource  Source object from which to copy.
template< typename Key, typename CompareKey, typename Allocator >
Helium::SortedSet< Key, CompareKey, Allocator >::SortedSet( const SortedSet& rSource )
    : RbTree( rSource )
{
}

/// Copy constructor.
///
/// @param[in] rSource  Source object from which to copy.
template< typename Key, typename CompareKey, typename Allocator >
template< typename OtherAllocator >
Helium::SortedSet< Key, CompareKey, Allocator >::SortedSet(
    const SortedSet< Key, CompareKey, OtherAllocator >& rSource )
    : RbTree( rSource )
{
}

/// Assignment operator.
///
/// @param[in] rSource  Source object from which to copy.
///
/// @return  Reference to this object.
template< typename Key, typename CompareKey, typename Allocator >
Helium::SortedSet< Key, CompareKey, Allocator >& Helium::SortedSet< Key, CompareKey, Allocator >::operator=(
    const SortedSet& rSource )
{
    Super::operator=( rSource );

    return *this;
}

/// Assignment operator.
///
/// @param[in] rSource  Source object from which to copy.
///
/// @return  Reference to this object.
template< typename Key, typename CompareKey, typename Allocator >
template< typename OtherAllocator >
Helium::SortedSet< Key, CompareKey, Allocator >& Helium::SortedSet< Key, CompareKey, Allocator >::operator=(
    const SortedSet< Key, CompareKey, OtherAllocator >& rSource )
{
    Super::operator=( rSource );

    return *this;
}
