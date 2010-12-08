#pragma once

#include "Foundation/Container/ConcurrentHashTable.h"

#include "Foundation/Container/Pair.h"
#include "Foundation/Functions.h"
#include "Foundation/HashFunctions.h"

#include <utility>

namespace Helium
{
    /// Thread-safe hash map container.
    template<
        typename Key,
        typename Data,
        typename HashFunction = Hash< Key >,
        typename EqualKey = Equals< Key >,
        typename Allocator = DefaultAllocator >
    class ConcurrentHashMap
        : public ConcurrentHashTable<
            KeyValue< Key, Data >, Key, HashFunction, SelectKey< KeyValue< Key, Data > >, EqualKey, Allocator,
            Pair< Key, Data > >
    {
    public:
        /// Default hash table bucket count (prime numbers are recommended).
        static const size_t DEFAULT_BUCKET_COUNT = 37;

        /// Parent class type.
        typedef ConcurrentHashTable<
            KeyValue< Key, Data >, Key, HashFunction, SelectKey< KeyValue< Key, Data > >, EqualKey, Allocator,
            Pair< Key, Data > >
                Super;

        /// Type for hash map keys.
        typedef typename Super::KeyType KeyType;
        /// Type for hash map data.
        typedef Data DataType;
        /// Type for hash map entries.
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
        explicit ConcurrentHashMap( size_t bucketCount = DEFAULT_BUCKET_COUNT );
        ConcurrentHashMap( const ConcurrentHashMap& rSource );
        ~ConcurrentHashMap();
        //@}

        /// @name Overloaded Operators
        //@{
        ConcurrentHashMap& operator=( const ConcurrentHashMap& rSource );
        //@}
    };
}

#include "Foundation/Container/ConcurrentHashMap.inl"
