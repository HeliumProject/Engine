#pragma once

#include "Platform/Locks.h"

#include "Foundation/DynArray.h"

namespace Helium
{
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
        typename InternalValue >
    class ConcurrentHashTable;

    /// Constant concurrent hash table accessor.
    template<
        typename Value,
        typename Key,
        typename HashFunction,
        typename ExtractKey,
        typename EqualKey,
        typename Allocator,
        typename InternalValue >
    class ConstConcurrentHashTableAccessor : NonCopyable
    {
        friend class ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >;

    public:
        /// Hash table value type.
        typedef Value ValueType;

        /// Type for pointers to hash table elements.
        typedef Value* PointerType;
        /// Type for references to hash table elements.
        typedef Value& ReferenceType;
        /// Type for constant pointers to hash table elements.
        typedef const Value* ConstPointerType;
        /// Type for constant references to hash table elements.
        typedef const Value& ConstReferenceType;

        /// Hash table type.
        typedef ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >
            TableType;

        /// @name Construction/Destruction
        //@{
        ConstConcurrentHashTableAccessor();
        ~ConstConcurrentHashTableAccessor();
        //@}

        /// @name Accessor Information
        //@{
        bool IsValid() const;
        void Release();
        //@}

        /// @name Overloaded Operators
        //@{
        const Value& operator*() const;
        const Value* operator->() const;

        ConstConcurrentHashTableAccessor& operator++();
        ConstConcurrentHashTableAccessor& operator--();

        bool operator==( const ConstConcurrentHashTableAccessor& rOther ) const;
        bool operator!=( const ConstConcurrentHashTableAccessor& rOther ) const;
        //@}

    private:
        /// Hash table currently referenced by this accessor.
        const TableType* m_pTable;
        /// Current table bucket index.
        size_t m_bucketIndex;
        /// Current table element index.
        size_t m_elementIndex;

        /// @name Private Utility Functions
        //@{
        void Set( const TableType* pTable, size_t bucketIndex, size_t elementIndex );
        //@}
    };

    /// Non-constant concurrent hash table accessor.
    template<
        typename Value,
        typename Key,
        typename HashFunction,
        typename ExtractKey,
        typename EqualKey,
        typename Allocator,
        typename InternalValue >
    class ConcurrentHashTableAccessor : NonCopyable
    {
        friend class ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >;

    public:
        /// Hash table value type.
        typedef Value ValueType;

        /// Type for pointers to hash table elements.
        typedef Value* PointerType;
        /// Type for references to hash table elements.
        typedef Value& ReferenceType;
        /// Type for constant pointers to hash table elements.
        typedef const Value* ConstPointerType;
        /// Type for constant references to hash table elements.
        typedef const Value& ConstReferenceType;

        /// Hash table type.
        typedef ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >
            TableType;

        /// @name Construction/Destruction
        //@{
        ConcurrentHashTableAccessor();
        ~ConcurrentHashTableAccessor();
        //@}

        /// @name Accessor Information
        //@{
        bool IsValid() const;
        void Release();
        //@}

        /// @name Overloaded Operators
        //@{
        Value& operator*() const;
        Value* operator->() const;

        ConcurrentHashTableAccessor& operator++();
        ConcurrentHashTableAccessor& operator--();

        bool operator==( const ConcurrentHashTableAccessor& rOther ) const;
        bool operator!=( const ConcurrentHashTableAccessor& rOther ) const;
        //@}

    private:
        /// Hash table currently referenced by this accessor.
        TableType* m_pTable;
        /// Current table bucket index.
        size_t m_bucketIndex;
        /// Current table element index.
        size_t m_elementIndex;

        /// @name Private Utility Functions
        //@{
        void Set( TableType* pTable, size_t bucketIndex, size_t elementIndex );
        //@}
    };

    /// Base class for hash table containers with thread-safe access support.
    template<
        typename Value,
        typename Key,
        typename HashFunction,
        typename ExtractKey,
        typename EqualKey,
        typename Allocator,
        typename InternalValue >
    class ConcurrentHashTable
    {
        friend class ConstConcurrentHashTableAccessor< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >;
        friend class ConcurrentHashTableAccessor< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >;

    public:
        /// Default hash table bucket count (prime numbers are recommended).
        static const size_t DEFAULT_BUCKET_COUNT = 37;

        /// Type for hash table keys.
        typedef Key KeyType;
        /// Type for hash table entries.
        typedef Value ValueType;

        /// Internal value type (type used for actual value storage).
        typedef InternalValue InternalValueType;

        /// Type for key hashing function.
        typedef HashFunction HasherType;
        /// Type for testing two keys for equality.
        typedef EqualKey KeyEqualType;
        /// Allocator type.
        typedef Allocator AllocatorType;

        /// Accessor type.
        typedef ConcurrentHashTableAccessor< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >
            Accessor;
        /// Constant accessor type.
        typedef ConstConcurrentHashTableAccessor< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >
            ConstAccessor;

        /// @name Hash Table Operations
        //@{
        size_t GetSize() const;
        bool IsEmpty() const;

        void Clear();
        void Trim();

        bool First( Accessor& rAccessor );
        bool First( ConstAccessor& rAccessor ) const;
        bool Last( Accessor& rAccessor );
        bool Last( ConstAccessor& rAccessor ) const;

        bool Find( Accessor& rAccessor, const Key& rKey );
        bool Find( ConstAccessor& rAccessor, const Key& rKey ) const;

        bool Insert( ConstAccessor& rAccessor, const Value& rValue );
        bool Insert( Accessor& rAccessor, const Value& rValue );

        bool Remove( const Key& rKey );
        bool Remove( Accessor& rAccessor );
        //@}

    protected:
        /// Hash table bucket.
        struct Bucket
        {
            /// Bucket entries.
            DynArray< InternalValue, Allocator > entries;
            /// Read-write lock for access synchronization.
            ReadWriteLock lock;
            /// State tag (incremented when entries are removed).
            volatile int32_t tag;
        };

        /// Hash table buckets.
        Bucket* m_pBuckets;
        /// Number of hash table buckets.
        size_t m_bucketCount;

        /// Number of elements currently in the hash table.
        volatile int32_t m_size;

        /// Key hashing functor.
        HasherType m_hasher;
        /// Key equal comparison functor.
        EqualKey m_keyEquals;
        /// Key extraction functor.
        ExtractKey m_extractKey;
        /// Allocator functor.
        AllocatorType m_allocator;

        /// @name Construction/Destruction, Protected
        //@{
        ConcurrentHashTable(
            size_t bucketCount, const HashFunction& rHasher, const EqualKey& rKeyEquals, const ExtractKey& rExtractKey,
            const Allocator& rAllocator = Allocator() );
        ConcurrentHashTable(
            size_t bucketCount, const HashFunction& rHasher, const EqualKey& rKeyEquals,
            const Allocator& rAllocator = Allocator() );
        ConcurrentHashTable( const ConcurrentHashTable& rSource );
        ~ConcurrentHashTable();
        //@}

        /// @name Overloaded Operators, Protected
        //@{
        ConcurrentHashTable& operator=( const ConcurrentHashTable& rSource );
        //@}

    private:
        /// @name Private Utility Functions
        //@{
        void AllocateBuckets();

        void CopyConstruct( const ConcurrentHashTable& rSource );
        void Finalize();
        //@}
    };
}

#include "Foundation/ConcurrentHashTable.inl"
