//----------------------------------------------------------------------------------------------------------------------
// ConcurrentHashMap.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_CONCURRENT_HASH_MAP_H
#define LUNAR_CORE_CONCURRENT_HASH_MAP_H

#include "Core/ConcurrentHashTable.h"
#include "Foundation/Functions.h"
#include "Foundation/HashFunctions.h"
#include <utility>

namespace Lunar
{
    /// Hash map key extract function.
    template< typename Key, typename Data >
    class ConcurrentHashMapExtractKey
    {
    public:
        const Key& operator()( const std::pair< Key, Data >& rValue ) const;
    };

    /// Thread-safe hash map container.
    template<
        typename Key,
        typename Data,
        typename HashFunction = Hash< Key >,
        typename EqualKey = Equals< Key >,
        typename Allocator = DefaultAllocator >
    class ConcurrentHashMap
        : public ConcurrentHashTable<
            std::pair< Key, Data >, Key, HashFunction, ConcurrentHashMapExtractKey< Key, Data >, EqualKey, Allocator >
    {
    public:
        /// Default hash table bucket count (prime numbers are recommended).
        static const size_t DEFAULT_BUCKET_COUNT = 37;

        /// Parent class type.
        typedef ConcurrentHashTable<
            std::pair< Key, Data >, Key, HashFunction, ConcurrentHashMapExtractKey< Key, Data >, EqualKey, Allocator >
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

#include "Core/ConcurrentHashMap.inl"

#endif  // LUNAR_CORE_CONCURRENT_HASH_MAP_H
