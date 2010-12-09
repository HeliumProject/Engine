#pragma once

#include "Foundation/Container/HashTable.h"

#include "Foundation/Functions.h"
#include "Foundation/HashFunctions.h"

namespace Helium
{
    /// Non-thread safe hash set container.
    template<
        typename Key,
        typename HashFunction = Hash< Key >,
        typename EqualKey = Equals< Key >,
        typename Allocator = DefaultAllocator >
    class HashSet : public HashTable< Key, Key, HashFunction, Identity< Key >, EqualKey, Allocator, Key >
    {
    public:
        /// Default hash table bucket count (prime numbers are recommended).
        static const size_t DEFAULT_BUCKET_COUNT = 37;

        /// Parent class type.
        typedef HashTable< Key, Key, HashFunction, Identity< Key >, EqualKey, Allocator, Key > Super;

        /// Type for hash set keys.
        typedef typename Super::KeyType KeyType;
        /// Type for hash set entries.
        typedef typename Super::ValueType ValueType;

        /// Type for key hashing function.
        typedef typename Super::HasherType HasherType;
        /// Type for testing two keys for equality.
        typedef typename Super::KeyEqualType KeyEqualType;
        /// Allocator type.
        typedef typename Super::AllocatorType AllocatorType;

        /// Iterator type.
        typedef typename Super::Iterator Iterator;
        /// Constant iterator type.
        typedef typename Super::ConstIterator ConstIterator;

        /// @name Construction/Destruction
        //@{
        explicit HashSet( size_t bucketCount = DEFAULT_BUCKET_COUNT );
        HashSet( const HashSet& rSource );
        template< typename OtherAllocator > HashSet(
            const HashSet< Key, HashFunction, EqualKey, OtherAllocator >& rSource );
        ~HashSet();
        //@}

        /// @name Overloaded Operators
        //@{
        HashSet& operator=( const HashSet& rSource );
        template< typename OtherAllocator > HashSet& operator=(
            const HashSet< Key, HashFunction, EqualKey, OtherAllocator >& rSource );
        //@}
    };
}

#include "Foundation/Container/HashSet.inl"
