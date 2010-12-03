#pragma once

#include "Foundation/Container/HashTable.h"

namespace Helium
{
    /// Hash map key extract function.
    template< typename Key, typename Data >
    class HashMapExtractKey
    {
    public:
        const Key& operator()( const KeyValue< Key, Data >& rValue ) const;
    };

    /// Non-thread safe hash map container.
    template<
        typename Key,
        typename Data,
        typename HashFunction = Hash< Key >,
        typename EqualKey = Equals< Key >,
        typename Allocator = DefaultAllocator >
    class HashMap
        : public HashTable<
            KeyValue< Key, Data >, Key, HashFunction, HashMapExtractKey< Key, Data >, EqualKey, Allocator,
            Pair< Key, Data > >
    {
    public:
        /// Default hash table bucket count (prime numbers are recommended).
        static const size_t DEFAULT_BUCKET_COUNT = 37;

        /// Parent class type.
        typedef HashTable<
            KeyValue< Key, Data >, Key, HashFunction, HashMapExtractKey< Key, Data >, EqualKey, Allocator,
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

        /// Iterator type.
        typedef typename Super::Iterator Iterator;
        /// Constant iterator type.
        typedef typename Super::ConstIterator ConstIterator;

        /// @name Construction/Destruction
        //@{
        explicit HashMap( size_t bucketCount = DEFAULT_BUCKET_COUNT );
        HashMap( const HashMap& rSource );
        template< typename OtherAllocator > HashMap(
            const HashMap< Key, Data, HashFunction, EqualKey, OtherAllocator >& rSource );
        ~HashMap();
        //@}

        /// @name Overloaded Operators
        //@{
        HashMap& operator=( const HashMap& rSource );
        template< typename OtherAllocator > HashMap& operator=(
            const HashMap< Key, Data, HashFunction, EqualKey, OtherAllocator >& rSource );
        //@}
    };
}

#include "Foundation/Container/HashMap.inl"
