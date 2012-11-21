#pragma once

#include "Foundation/RedBlackTree.h"

namespace Helium
{
    /// Sorted set.
    ///
    /// SortedSet stores elements using a red-black tree data structure.  Lookups, insertions, and deletions are
    /// performed in a worst-case of O(log n) time.  When iterating, values are guaranteed to be sorted.
    template< typename Key, typename CompareKey = Less< Key >, typename Allocator = DefaultAllocator >
    class SortedSet : public RedBlackTree< const Key, const Key, Identity< const Key >, CompareKey, Allocator, Key >
    {
    public:
        /// Parent class type.
        typedef RedBlackTree< const Key, const Key, Identity< const Key >, CompareKey, Allocator, Key > Base;

        /// Type for set keys.
        typedef typename Base::KeyType KeyType;
        /// Type for set entries.
        typedef typename Base::ValueType ValueType;

        /// Type for comparing two keys.
        typedef typename Base::KeyCompareType KeyCompareType;
        /// Allocator type.
        typedef typename Base::AllocatorType AllocatorType;

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

        bool operator==( const SortedSet& rOther ) const;
        template< typename OtherAllocator > bool operator==(
            const SortedSet< Key, CompareKey, OtherAllocator >& rOther ) const;

        bool operator!=( const SortedSet& rOther ) const;
        template< typename OtherAllocator > bool operator!=(
            const SortedSet< Key, CompareKey, OtherAllocator >& rOther ) const;
        //@}

    private:
        /// @name Private Utility Functions
        //@{
        template< typename OtherAllocator > bool Equals(
            const SortedSet< Key, CompareKey, OtherAllocator >& rOther ) const;
        //@}
    };
}

#include "Foundation/SortedSet.inl"
