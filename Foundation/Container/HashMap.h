#pragma once

#include "Foundation/Container/HashTable.h"

#include "Foundation/Functions.h"
#include "Foundation/HashFunctions.h"

namespace Helium
{
    /// Non-thread safe hash map container.
    template<
        typename Key,
        typename Data,
        typename HashFunction = Hash< Key >,
        typename EqualKey = Equals< Key >,
        typename Allocator = DefaultAllocator >
    class HashMap
        : public HashTable<
            KeyValue< Key, Data >, Key, HashFunction, SelectKey< KeyValue< Key, Data > >, EqualKey, Allocator,
            Pair< Key, Data > >
    {
    public:
        /// Default hash table bucket count (prime numbers are recommended).
        static const size_t DEFAULT_BUCKET_COUNT = 37;

        /// Parent class type.
        typedef HashTable<
            KeyValue< Key, Data >, Key, HashFunction, SelectKey< KeyValue< Key, Data > >, EqualKey, Allocator,
            Pair< Key, Data > >
                Base;

        /// Type for hash map keys.
        typedef typename Base::KeyType KeyType;
        /// Type for hash map data.
        typedef Data DataType;
        /// Type for hash map entries.
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
