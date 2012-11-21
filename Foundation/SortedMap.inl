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
    : RedBlackTree( rSource )
{
}

/// Copy constructor.
///
/// @param[in] rSource  Source object from which to copy.
template< typename Key, typename Data, typename CompareKey, typename Allocator >
template< typename OtherAllocator >
Helium::SortedMap< Key, Data, CompareKey, Allocator >::SortedMap(
    const SortedMap< Key, Data, CompareKey, OtherAllocator >& rSource )
    : RedBlackTree( rSource )
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

/// Equality comparison operator.
///
/// @param[in] rOther  Map with which to compare.
///
/// @return  True if this map and the given map match, false if they differ.
///
/// @see operator!=()
template< typename Key, typename Data, typename CompareKey, typename Allocator >
bool Helium::SortedMap< Key, Data, CompareKey, Allocator >::operator==( const SortedMap& rOther ) const
{
    return Equals( rOther );
}

/// Equality comparison operator.
///
/// @param[in] rOther  Map with which to compare.
///
/// @return  True if this map and the given map match, false if they differ.
///
/// @see operator!=()
template< typename Key, typename Data, typename CompareKey, typename Allocator >
template< typename OtherAllocator >
bool Helium::SortedMap< Key, Data, CompareKey, Allocator >::operator==(
    const SortedMap< Key, Data, CompareKey, OtherAllocator >& rOther ) const
{
    return Equals( rOther );
}

/// Inequality comparison operator.
///
/// @param[in] rOther  Map with which to compare.
///
/// @return  True if this map and the given map differ, false if they match.
///
/// @see operator==()
template< typename Key, typename Data, typename CompareKey, typename Allocator >
bool Helium::SortedMap< Key, Data, CompareKey, Allocator >::operator!=( const SortedMap& rOther ) const
{
    return !Equals( rOther );
}

/// Inequality comparison operator.
///
/// @param[in] rOther  Map with which to compare.
///
/// @return  True if this map and the given map differ, false if they match.
///
/// @see operator==()
template< typename Key, typename Data, typename CompareKey, typename Allocator >
template< typename OtherAllocator >
bool Helium::SortedMap< Key, Data, CompareKey, Allocator >::operator!=(
    const SortedMap< Key, Data, CompareKey, OtherAllocator >& rOther ) const
{
    return !Equals( rOther );
}

/// Test whether the contents of this map match those of a given map.
///
/// @param[in] rOther  Map with which to compare.
///
/// @return  True if this map and the given map match, false if they differ.
template< typename Key, typename Data, typename CompareKey, typename Allocator >
template< typename OtherAllocator >
bool Helium::SortedMap< Key, Data, CompareKey, Allocator >::Equals(
    const SortedMap< Key, Data, CompareKey, OtherAllocator >& rOther ) const
{
    if( GetSize() != rOther.GetSize() )
    {
        return false;
    }

    ConstIterator thisIter = Begin();
    ConstIterator thisEnd = End();

    ConstIterator otherIter = rOther.Begin();
    ConstIterator otherEnd = rOther.End();

    CompareKey keyCompare;

    for( ; thisIter != thisEnd; ++thisIter, ++otherIter )
    {
        HELIUM_ASSERT( otherIter != otherEnd );

        const Key& rThisKey = thisIter->First();
        const Key& rOtherKey = otherIter->First();

        if( keyCompare( rThisKey, rOtherKey ) || keyCompare( rOtherKey, rThisKey ) )
        {
            return false;
        }

        if( thisIter->Second() != otherIter->Second() )
        {
            return false;
        }
    }

    return true;
}
