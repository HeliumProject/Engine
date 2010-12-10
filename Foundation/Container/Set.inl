/// Constructor.
template< typename Key, typename EqualKey, typename Allocator >
Helium::Set< Key, EqualKey, Allocator >::Set()
{
}

/// Copy constructor.
///
/// @param[in] rSource  Source set from which to copy.
template< typename Key, typename EqualKey, typename Allocator >
Helium::Set< Key, EqualKey, Allocator >::Set( const Set& rSource )
    : Super( rSource )
{
}

/// Copy constructor.
///
/// @param[in] rSource  Source set from which to copy.
template< typename Key, typename EqualKey, typename Allocator >
template< typename OtherAllocator >
Helium::Set< Key, EqualKey, Allocator >::Set( const Set< Key, EqualKey, OtherAllocator >& rSource )
    : Super( rSource )
{
}

/// Set this set to the contents of the given set.
///
/// If the given set is not the same as this set, this will always destroy the current contents of this set and allocate
/// a fresh set whose capacity matches the size of the given set.
///
/// @param[in] rSource  Set from which to copy.
///
/// @return  Reference to this set.
template< typename Key, typename EqualKey, typename Allocator >
Helium::Set< Key, EqualKey, Allocator >& Helium::Set< Key, EqualKey, Allocator >::operator=( const Set& rSource )
{
    Super::operator=( rSource );

    return *this;
}

/// Set this set to the contents of the given set.
///
/// If the given set is not the same as this set, this will always destroy the current contents of this set and allocate
/// a fresh set whose capacity matches the size of the given set.
///
/// @param[in] rSource  Set from which to copy.
///
/// @return  Reference to this set.
template< typename Key, typename EqualKey, typename Allocator >
template< typename OtherAllocator >
Helium::Set< Key, EqualKey, Allocator >& Helium::Set< Key, EqualKey, Allocator >::operator=(
    const Set< Key, EqualKey, OtherAllocator >& rSource )
{
    Super::operator=( rSource );

    return *this;
}
