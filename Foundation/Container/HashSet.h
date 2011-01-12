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
    class HashSet : public HashTable< const Key, const Key, HashFunction, Identity< const Key >, EqualKey, Allocator, Key >
    {
    public:
        /// Default hash table bucket count (prime numbers are recommended).
        static const size_t DEFAULT_BUCKET_COUNT = 37;

        /// Parent class type.
        typedef HashTable< const Key, const Key, HashFunction, Identity< const Key >, EqualKey, Allocator, Key > Base;

        /// Type for hash set keys.
        typedef typename Base::KeyType KeyType;
        /// Type for hash set entries.
        typedef typename Base::ValueType ValueType;

        /// Type for key hashing function.
        typedef typename Base::HasherType HasherType;
        /// Type for testing two keys for equality.
        typedef typename Base::KeyEqualType KeyEqualType;
        /// Allocator type.
        typedef typename Base::AllocatorType AllocatorType;

        /// Iterator type.
        typedef typename Base::Iterator Iterator;
        /// Constant iterator type.
        typedef typename Base::ConstIterator ConstIterator;

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
