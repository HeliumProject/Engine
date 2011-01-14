#pragma once

#include "Foundation/Container/Table.h"

namespace Helium
{
    /// Associative array (not thread-safe).
    ///
    /// Map stores elements in a contiguous array, similar to DynArray.  Compared to SortedMap and HashMap, the memory
    /// footprint is smaller, as no additional data is stored aside from the key-value pairs.  Performance is
    /// potentially worse than HashMap and SortedMap, with element lookups and deletions taking O(n) time in the
    /// worst-case scenario, while element insertions will always take O(n) time, but for maps with a small enough
    /// number of elements, performance may end up better in practice due to somewhat better cache coherency.
    ///
    /// Map elements are always inserted at the end of the map, maintaining the same order of elements during insertion.
    /// Removal does not maintain such order, as elements are moved from the end of the map to the space occupied by the
    /// map slot being removed in order to reduce the amount of data copied during removal.
    template< typename Key, typename Data, typename EqualKey = Equals< Key >, typename Allocator = DefaultAllocator >
    class Map : public Table< KeyValue< Key, Data >, Key, SelectKey< KeyValue< Key, Data > >, EqualKey, Allocator, Pair< Key, Data > >
    {
    public:
        /// Parent class type.
        typedef Table< KeyValue< Key, Data >, Key, SelectKey< KeyValue< Key, Data > >, EqualKey, Allocator, Pair< Key, Data > >
            Base;

        /// Type for map keys.
        typedef typename Base::KeyType KeyType;
        /// Type for map data.
        typedef Data DataType;
        /// Type for map entries.
        typedef typename Base::ValueType ValueType;

        /// Type for testing two keys for equality.
        typedef typename Base::KeyEqualType KeyEqualType;
        /// Allocator type.
        typedef typename Base::AllocatorType AllocatorType;

        /// @name Construction/Destruction
        //@{
        Map();
        Map( const Map& rSource );
        template< typename OtherAllocator > Map( const Map< Key, Data, EqualKey, OtherAllocator >& rSource );
        //@}

        /// @name Overloaded Operators
        //@{
        Map& operator=( const Map& rSource );
        template< typename OtherAllocator > Map& operator=( const Map< Key, Data, EqualKey, OtherAllocator >& rSource );

        Data& operator[]( const Key& rKey );
        //@}
    };
}

#include "Foundation/Container/Map.inl"
