#pragma once

#include "Foundation/ConcurrentHashTable.h"
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
        : public ConcurrentHashTable< const Key, const Key, HashFunction, Identity< const Key >, EqualKey, Allocator, Key >
    {
    public:
        /// Default hash table bucket count (prime numbers are recommended).
        static const size_t DEFAULT_BUCKET_COUNT = 37;

        /// Parent class type.
        typedef ConcurrentHashTable< const Key, const Key, HashFunction, Identity< const Key >, EqualKey, Allocator, Key >
            Base;

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

        /// Accessor type.
        typedef typename Base::Accessor Accessor;
        /// Constant accessor type.
        typedef typename Base::ConstAccessor ConstAccessor;

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

#include "Foundation/ConcurrentHashSet.inl"
