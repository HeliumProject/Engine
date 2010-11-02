//----------------------------------------------------------------------------------------------------------------------
// ConcurrentHashSet.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_CONCURRENT_HASH_SET_H
#define LUNAR_CORE_CONCURRENT_HASH_SET_H

#include "Core/ConcurrentHashTable.h"
#include "Core/Functions.h"
#include "Core/HashFunctions.h"

namespace Lunar
{
    /// Hash set key extract function.
    template< typename T >
    class ConcurrentHashSetExtractKey
    {
    public:
        const T& operator()( const T& rValue ) const;
    };

    /// Thread-safe hash set container.
    template<
        typename Key,
        typename HashFunction = Hash< Key >,
        typename EqualKey = Equals< Key >,
        typename Allocator = DefaultAllocator >
    class ConcurrentHashSet
        : public ConcurrentHashTable< Key, Key, HashFunction, ConcurrentHashSetExtractKey< Key >, EqualKey, Allocator >
    {
    public:
        /// Default hash table bucket count (prime numbers are recommended).
        static const size_t DEFAULT_BUCKET_COUNT = 37;

        /// Parent class type.
        typedef ConcurrentHashTable< Key, Key, HashFunction, ConcurrentHashSetExtractKey< Key >, EqualKey, Allocator >
            Super;

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

#include "Core/ConcurrentHashSet.inl"

#endif  // LUNAR_CORE_CONCURRENT_HASH_SET_H
