#pragma once

#include "Foundation/Container/RbTree.h"

namespace Helium
{
    /// Key-sorted map.
    ///
    /// SortedMap stores elements using a red-black tree data structure.  Lookups, insertions, and deletions are
    /// performed in a worst-case of O(log n) time.  When iterating, values are guaranteed to be sorted by their keys.
    template< typename Key, typename Data, typename CompareKey = Less< Key >, typename Allocator = DefaultAllocator >
    class SortedMap : public RbTree< KeyValue< Key, Data >, Key, SelectKey< KeyValue< Key, Data > >, CompareKey, Allocator, Pair< Key, Data > >
    {
    public:
        /// Parent class type.
        typedef RbTree< KeyValue< Key, Data >, Key, SelectKey< KeyValue< Key, Data > >, CompareKey, Allocator, Pair< Key, Data > >
            Super;

        /// Type for map keys.
        typedef typename Super::KeyType KeyType;
        /// Type for map data.
        typedef Data DataType;
        /// Type for map entries.
        typedef typename Super::ValueType ValueType;

        /// Type for comparing two keys.
        typedef typename Super::KeyCompareType KeyCompareType;
        /// Allocator type.
        typedef typename Super::AllocatorType AllocatorType;

        /// @name Construction/Destruction
        //@{
        SortedMap();
        SortedMap( const SortedMap& rSource );
        template< typename OtherAllocator > SortedMap(
            const SortedMap< Key, Data, CompareKey, OtherAllocator >& rSource );
        //@}

        /// @name Overloaded Operators
        //@{
        SortedMap& operator=( const SortedMap& rSource );
        template< typename OtherAllocator > SortedMap& operator=(
            const SortedMap< Key, Data, CompareKey, OtherAllocator >& rSource );

        Data& operator[]( const Key& rKey );
        //@}
    };
}

#include "Foundation/Container/SortedMap.inl"
