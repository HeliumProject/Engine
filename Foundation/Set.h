#pragma once

#include "Foundation/Table.h"

#include "Foundation/Functions.h"

namespace Helium
{
    /// Unique set container type (not thread-safe).
    ///
    /// Set stores elements in a contiguous array, similar to DynArray.  Compared to SortedSet and HashSet, the memory
    /// footprint is smaller, as no additional data is stored aside from the set values.  Performance is potentially
    /// worse than HashSet and SortedSet, with element lookups and deletions taking O(n) time in the worst-case
    /// scenario, while element insertions will always take O(n) time, but for sets with a small enough number of
    /// elements, performance may end up better in practice due to somewhat better cache coherency.
    ///
    /// Set elements are always inserted at the end of the set, maintaining the same order of elements during insertion.
    /// Removal does not maintain such order, as elements are moved from the end of the set to the space occupied by the
    /// set slot being removed in order to reduce the amount of data copied during removal.
    template< typename Key, typename EqualKey = Equals< Key >, typename Allocator = DefaultAllocator >
    class Set : public Table< const Key, const Key, Identity< const Key >, EqualKey, Allocator, Key >
    {
    public:
        /// Parent class type.
        typedef Table< const Key, const Key, Identity< const Key >, EqualKey, Allocator, Key > Base;

        /// Type for set keys.
        typedef typename Base::KeyType KeyType;
        /// Type for set entries.
        typedef typename Base::ValueType ValueType;

        /// Type for testing two keys for equality.
        typedef typename Base::KeyEqualType KeyEqualType;
        /// Allocator type.
        typedef typename Base::AllocatorType AllocatorType;

        /// @name Construction/Destruction
        //@{
        Set();
        Set( const Set& rSource );
        template< typename OtherAllocator > Set( const Set< Key, EqualKey, OtherAllocator >& rSource );
        //@}

        /// @name Overloaded Operators
        //@{
        Set& operator=( const Set& rSource );
        template< typename OtherAllocator > Set& operator=( const Set< Key, EqualKey, OtherAllocator >& rSource );
        //@}
    };
}

#include "Foundation/Set.inl"
