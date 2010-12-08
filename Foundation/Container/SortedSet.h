#pragma once

#include "Foundation/Container/RbTree.h"

namespace Helium
{
    /// Sorted set.
    ///
    /// SortedSet stores elements using a red-black tree data structure.  Lookups, insertions, and deletions are
    /// performed in a worst-case of O(log n) time.  When iterating, values are guaranteed to be sorted.
    template< typename Key, typename CompareKey = Less< Key >, typename Allocator = DefaultAllocator >
    class SortedSet : public RbTree< Key, Key, Identity< Key >, CompareKey, Allocator >
    {
    public:
        /// Parent class type.
        typedef RbTree< Key, Key, Identity< Key >, CompareKey, Allocator > Super;

        /// Type for set keys.
        typedef typename Super::KeyType KeyType;
        /// Type for set entries.
        typedef typename Super::ValueType ValueType;

        /// Type for comparing two keys.
        typedef typename Super::KeyCompareType KeyCompareType;
        /// Allocator type.
        typedef typename Super::AllocatorType AllocatorType;

        /// @name Construction/Destruction
        //@{
        SortedSet();
        SortedSet( const SortedSet& rSource );
        template< typename OtherAllocator > SortedSet(
            const SortedSet< Key, CompareKey, OtherAllocator >& rSource );
        //@}

        /// @name Overloaded Operators
        //@{
        SortedSet& operator=( const SortedSet& rSource );
        template< typename OtherAllocator > SortedSet& operator=(
            const SortedSet< Key, CompareKey, OtherAllocator >& rSource );
        //@}
    };
}

#include "Foundation/Container/SortedSet.inl"
