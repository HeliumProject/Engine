#pragma once

#include "Foundation/DynArray.h"
#include "Foundation/Pair.h"

namespace Helium
{
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
        typename InternalValue >
    class HashTable;

    /// Constant hash table iterator.
    template<
        typename Value,
        typename Key,
        typename HashFunction,
        typename ExtractKey,
        typename EqualKey,
        typename Allocator,
        typename InternalValue >
    class ConstHashTableIterator
    {
        friend class HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >;

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
        typedef HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue > TableType;

        /// @name Construction/Destruction
        //@{
        ConstHashTableIterator();
        //@}

        /// @name Overloaded Operators
        //@{
        const Value& operator*() const;
        const Value* operator->() const;

        ConstHashTableIterator& operator++();
        ConstHashTableIterator operator++( int );
        ConstHashTableIterator& operator--();
        ConstHashTableIterator operator--( int );

        bool operator==( const ConstHashTableIterator& rOther ) const;
        bool operator!=( const ConstHashTableIterator& rOther ) const;
        bool operator<( const ConstHashTableIterator& rOther ) const;
        bool operator>( const ConstHashTableIterator& rOther ) const;
        bool operator<=( const ConstHashTableIterator& rOther ) const;
        bool operator>=( const ConstHashTableIterator& rOther ) const;
        //@}

    protected:
        /// Hash table currently referenced by this iterator.
        TableType* m_pTable;
        /// Current table bucket index.
        size_t m_bucketIndex;
        /// Current table element index.
        size_t m_elementIndex;

        /// @name Construction/Destruction, Protected
        //@{
        ConstHashTableIterator( const TableType* pTable, size_t bucketIndex, size_t elementIndex );
        //@}
    };

    /// Non-constant hash table iterator.
    template<
        typename Value,
        typename Key,
        typename HashFunction,
        typename ExtractKey,
        typename EqualKey,
        typename Allocator,
        typename InternalValue >
    class HashTableIterator :
        public ConstHashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >
    {
        friend class HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >;

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
        typedef HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue > TableType;

        /// @name Construction/Destruction
        //@{
        HashTableIterator();
        //@}

        /// @name Overloaded Operators
        //@{
        Value& operator*() const;
        Value* operator->() const;

        HashTableIterator& operator++();
        HashTableIterator operator++( int );
        HashTableIterator& operator--();
        HashTableIterator operator--( int );
        //@}

    protected:
        /// @name Construction/Destruction, Protected
        //@{
        HashTableIterator( TableType* pTable, size_t bucketIndex, size_t elementIndex );
        //@}
    };

    /// Base class for non-thread safe hash table containers.
    template<
        typename Value,
        typename Key,
        typename HashFunction,
        typename ExtractKey,
        typename EqualKey,
        typename Allocator,
        typename InternalValue >
    class HashTable
    {
        friend class ConstHashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >;
        friend class HashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >;

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

        /// Iterator type.
        typedef HashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue > Iterator;
        /// Constant iterator type.
        typedef ConstHashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >
            ConstIterator;

        /// @name Hash Table Operations
        //@{
        size_t GetSize() const;
        bool IsEmpty() const;

        void Clear();
        void Trim();

        Iterator Begin();
        ConstIterator Begin() const;
        Iterator End();
        ConstIterator End() const;

        Iterator Find( const Key& rKey );
        ConstIterator Find( const Key& rKey ) const;

        Pair< Iterator, bool > Insert( const ValueType& rValue );
        bool Insert( ConstIterator& rIterator, const ValueType& rValue );

        bool Remove( const Key& rKey );
        void Remove( Iterator iterator );
        void Remove( Iterator start, Iterator end );

        void Swap( HashTable& rTable );
        //@}

    protected:
        /// Hash table bucket.
        typedef DynArray< InternalValue, Allocator > Bucket;

        /// Hash table buckets.
        Bucket* m_pBuckets;
        /// Number of hash table buckets.
        size_t m_bucketCount;

        /// Number of elements currently in the hash table.
        size_t m_size;

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
        HashTable(
            size_t bucketCount, const HashFunction& rHasher, const EqualKey& rKeyEquals, const ExtractKey& rExtractKey,
            const Allocator& rAllocator = Allocator() );
        HashTable(
            size_t bucketCount, const HashFunction& rHasher, const EqualKey& rKeyEquals,
            const Allocator& rAllocator = Allocator() );
        HashTable( const HashTable& rSource );
        template< typename OtherAllocator > HashTable(
            const HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, OtherAllocator, InternalValue >& rSource );
        ~HashTable();
        //@}

        /// @name Overloaded Operators, Protected
        //@{
        HashTable& operator=( const HashTable& rSource );
        template< typename OtherAllocator > HashTable& operator=(
            const HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, OtherAllocator, InternalValue >& rSource );
        //@}

    private:
        /// @name Private Utility Functions
        //@{
        void AllocateBuckets();

        template< typename OtherAllocator > void CopyConstruct(
            const HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, OtherAllocator, InternalValue >& rSource );
        void Finalize();
        //@}
    };
}

#include "Foundation/HashTable.inl"
