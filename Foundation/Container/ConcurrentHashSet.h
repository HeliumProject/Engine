#pragma once

#include "Foundation/Container/ConcurrentHashTable.h"
#include "Foundation/Functions.h"
#include "Foundation/HashFunctions.h"

namespace Helium
{
    /// Thread-safe hash set container.
    template<
        typename Key,
        typename HashFunction = Hash< Key >,
        typename EqualKey = Equals< Key >,
        typename Allocator = DefaultAllocator >
    class ConcurrentHashSet
        : public ConcurrentHashTable< Key, Key, HashFunction, Identity< Key >, EqualKey, Allocator, Key >
    {
    public:
        /// Default hash table bucket count (prime numbers are recommended).
        static const size_t DEFAULT_BUCKET_COUNT = 37;

        /// Parent class type.
        typedef ConcurrentHashTable< Key, Key, HashFunction, Identity< Key >, EqualKey, Allocator, Key > Super;

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

        /// Accessor type.
        typedef typename Super::Accessor Accessor;
        /// Constant accessor type.
        typedef typename Super::ConstAccessor ConstAccessor;

        /// @name Construction/Destruction
        //@{
        explicit ConcurrentHashSet( size_t bucketCount = DEFAULT_BUCKET_COUNT );
        ConcurrentHashSet( const ConcurrentHashSet& rSource );
        ~ConcurrentHashSet();
        //@}

        /// @name Overloaded Operators
        //@{
        ConcurrentHashSet& operator=( const ConcurrentHashSet& rSource );
        //@}
    };
}

#include "Foundation/Container/ConcurrentHashSet.inl"
