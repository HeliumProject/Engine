//----------------------------------------------------------------------------------------------------------------------
// ConcurrentHashTable.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Constructor.
    ///
    /// Creates a read-only hash table accessor, initialized in an invalid state (not referencing any hash table entry).
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    ConstConcurrentHashTableAccessor<
        Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::ConstConcurrentHashTableAccessor()
        : m_pTable( NULL )
        , m_bucketIndex( 0 )
        , m_elementIndex( 0 )
    {
    }

    /// Destructor.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    ConstConcurrentHashTableAccessor<
        Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::~ConstConcurrentHashTableAccessor()
    {
        Release();
    }

    /// Get whether this accessor is referencing a hash table entry.
    ///
    /// @return  True if referencing a hash table entry, false if not.
    ///
    /// @see Release()
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    bool ConstConcurrentHashTableAccessor< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::IsValid() const
    {
        return( m_pTable != NULL );
    }

    /// Clear out any reference to a hash table entry, releasing any necessary locks as well.
    ///
    /// @see IsValid()
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    void ConstConcurrentHashTableAccessor< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::Release()
    {
        if( m_pTable )
        {
            HELIUM_ASSERT( m_pTable->m_pBuckets );
            m_pTable->m_pBuckets[ m_bucketIndex ].lock.UnlockRead();
            m_pTable = NULL;
            m_bucketIndex = 0;
            m_elementIndex = 0;
        }
    }

    /// Access the current hash table entry.
    ///
    /// @return  Constant reference to the current hash table entry.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    const Value& ConstConcurrentHashTableAccessor<
        Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::operator*() const
    {
        HELIUM_ASSERT( m_pTable );
        HELIUM_ASSERT( m_pTable->m_pBuckets );

        return m_pTable->m_pBuckets[ m_bucketIndex ].entries[ m_elementIndex ];
    }

    /// Access the current hash table entry.
    ///
    /// @return  Constant pointer to the current hash table entry.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    const Value* ConstConcurrentHashTableAccessor<
        Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::operator->() const
    {
        HELIUM_ASSERT( m_pTable );
        HELIUM_ASSERT( m_pTable->m_pBuckets );

        return &m_pTable->m_pBuckets[ m_bucketIndex ].entries[ m_elementIndex ];
    }

    /// Increment this accessor to the next hash table entry.
    ///
    /// @return  Reference to this accessor.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    ConstConcurrentHashTableAccessor< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >&
        ConstConcurrentHashTableAccessor< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::operator++()
    {
        HELIUM_ASSERT( m_pTable );

        ++m_elementIndex;

        TableType::Bucket* pBuckets = m_pTable->m_pBuckets;
        HELIUM_ASSERT( pBuckets );

        TableType::Bucket& rCurrentBucket = pBuckets[ m_bucketIndex ];
        if( m_elementIndex >= rCurrentBucket.entries.GetSize() )
        {
            rCurrentBucket.lock.UnlockRead();

            m_elementIndex = 0;

            size_t bucketCount = m_pTable->m_bucketCount;
            for( size_t bucketIndex = m_bucketIndex + 1; bucketIndex < bucketCount; ++bucketIndex )
            {
                TableType::Bucket& rBucket = pBuckets[ bucketIndex ];
                rBucket.lock.LockRead();
                if( !rBucket.entries.IsEmpty() )
                {
                    m_bucketIndex = bucketIndex;

                    return *this;
                }

                rBucket.lock.UnlockRead();
            }

            m_pTable = NULL;
            m_bucketIndex = 0;
        }

        return *this;
    }

    /// Decrement this accessor to the previous hash table entry.
    ///
    /// @return  Reference to this accessor.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    ConstConcurrentHashTableAccessor< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >&
        ConstConcurrentHashTableAccessor< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::operator--()
    {
        HELIUM_ASSERT( m_pTable );

        if( m_elementIndex > 0 )
        {
            --m_elementIndex;

            return *this;
        }

        TableType::Bucket* pBuckets = m_pTable->m_pBuckets;
        HELIUM_ASSERT( pBuckets );

        size_t bucketIndex = m_bucketIndex;
        pBuckets[ bucketIndex ].lock.UnlockRead();

        while( bucketIndex != 0 )
        {
            --bucketIndex;

            TableType::Bucket& rBucket = pBuckets[ bucketIndex ];
            rBucket.lock.LockRead();

            size_t entryCount = rBucket.entries.GetSize();
            if( entryCount != 0 )
            {
                m_bucketIndex = bucketIndex;
                m_elementIndex = entryCount - 1;

                return *this;
            }

            rBucket.lock.UnlockRead();
        }

        m_pTable = NULL;
        m_bucketIndex = 0;
        m_elementIndex = 0;

        return *this;
    }

    /// Get whether this accessor references the same hash table location as another accessor.
    ///
    /// @param[in] rOther  Accessor against which to compare.
    ///
    /// @return  True if this accessor references the same hash table location as the given accessor, false if not.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    bool ConstConcurrentHashTableAccessor< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::operator==(
        const ConstConcurrentHashTableAccessor& rOther ) const
    {
        return( m_pTable == rOther.m_pTable &&
                m_bucketIndex == rOther.m_bucketIndex &&
                m_elementIndex == rOther.m_elementIndex );
    }

    /// Get whether this accessor does not reference the same hash table location as another accessor.
    ///
    /// @param[in] rOther  Accessor against which to compare.
    ///
    /// @return  True if this accessor does not reference the same hash table location as the given accessor, false if
    ///          they do match.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    bool ConstConcurrentHashTableAccessor< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::operator!=(
        const ConstConcurrentHashTableAccessor& rOther ) const
    {
        return( m_pTable != rOther.m_pTable ||
                m_bucketIndex != rOther.m_bucketIndex ||
                m_elementIndex != rOther.m_elementIndex );
    }

    /// Directly set the hash table entry reference for this accessor.
    ///
    /// This should only be called by the table itself, and only on entries for which a lock already exists.  Control of
    /// the lock is passed onto this accessor to release at a later time.
    ///
    /// @param[in] pTable        Table to reference.
    /// @param[in] bucketIndex   Index of the bucket containing the entry to reference.
    /// @param[in] elementIndex  Index of the bucket element in which the table entry is stored.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    void ConstConcurrentHashTableAccessor< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::Set(
        const TableType* pTable,
        size_t bucketIndex,
        size_t elementIndex )
    {
        HELIUM_ASSERT( m_pTable == NULL );

        m_pTable = pTable;
        m_bucketIndex = bucketIndex;
        m_elementIndex = elementIndex;
    }

    /// Constructor.
    ///
    /// Creates a read-write hash table accessor, initialized in an invalid state (not referencing any hash table
    /// entry).
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    ConcurrentHashTableAccessor<
        Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::ConcurrentHashTableAccessor()
        : m_pTable( NULL )
        , m_bucketIndex( 0 )
        , m_elementIndex( 0 )
    {
    }

    /// Destructor.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    ConcurrentHashTableAccessor<
        Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::~ConcurrentHashTableAccessor()
    {
        Release();
    }

    /// Get whether this accessor is referencing a hash table entry.
    ///
    /// @return  True if referencing a hash table entry, false if not.
    ///
    /// @see Release()
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    bool ConcurrentHashTableAccessor< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::IsValid() const
    {
        return( m_pTable != NULL );
    }

    /// Clear out any reference to a hash table entry, releasing any necessary locks as well.
    ///
    /// @see IsValid()
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    void ConcurrentHashTableAccessor< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::Release()
    {
        if( m_pTable )
        {
            HELIUM_ASSERT( m_pTable->m_pBuckets );
            m_pTable->m_pBuckets[ m_bucketIndex ].lock.UnlockWrite();
            m_pTable = NULL;
            m_bucketIndex = 0;
            m_elementIndex = 0;
        }
    }

    /// Access the current hash table entry.
    ///
    /// @return  Reference to the current hash table entry.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    Value& ConcurrentHashTableAccessor< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::operator*() const
    {
        HELIUM_ASSERT( m_pTable );
        HELIUM_ASSERT( m_pTable->m_pBuckets );

        return m_pTable->m_pBuckets[ m_bucketIndex ].entries[ m_elementIndex ];
    }

    /// Access the current hash table entry.
    ///
    /// @return  Pointer to the current hash table entry.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    Value* ConcurrentHashTableAccessor< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::operator->() const
    {
        HELIUM_ASSERT( m_pTable );
        HELIUM_ASSERT( m_pTable->m_pBuckets );

        return &m_pTable->m_pBuckets[ m_bucketIndex ].entries[ m_elementIndex ];
    }

    /// Increment this accessor to the next hash table entry.
    ///
    /// @return  Reference to this accessor.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    ConcurrentHashTableAccessor< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >&
        ConcurrentHashTableAccessor< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::operator++()
    {
        HELIUM_ASSERT( m_pTable );

        ++m_elementIndex;

        TableType::Bucket* pBuckets = m_pTable->m_pBuckets;
        HELIUM_ASSERT( pBuckets );

        TableType::Bucket& rCurrentBucket = pBuckets[ m_bucketIndex ];
        if( m_elementIndex >= rCurrentBucket.entries.GetSize() )
        {
            rCurrentBucket.lock.UnlockWrite();

            m_elementIndex = 0;

            size_t bucketCount = m_pTable->m_bucketCount;
            for( size_t bucketIndex = m_bucketIndex + 1; bucketIndex < bucketCount; ++bucketIndex )
            {
                TableType::Bucket& rBucket = pBuckets[ bucketIndex ];
                rBucket.lock.LockWrite();
                if( !rBucket.entries.IsEmpty() )
                {
                    m_bucketIndex = bucketIndex;

                    return *this;
                }

                rBucket.lock.UnlockWrite();
            }

            m_pTable = NULL;
            m_bucketIndex = 0;
        }

        return *this;
    }

    /// Decrement this accessor to the previous hash table entry.
    ///
    /// @return  Reference to this accessor.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    ConcurrentHashTableAccessor< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >&
        ConcurrentHashTableAccessor< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::operator--()
    {
        HELIUM_ASSERT( m_pTable );

        if( m_elementIndex > 0 )
        {
            --m_elementIndex;

            return *this;
        }

        TableType::Bucket* pBuckets = m_pTable->m_pBuckets;
        HELIUM_ASSERT( pBuckets );

        size_t bucketIndex = m_bucketIndex;
        pBuckets[ bucketIndex ].lock.UnlockWrite();

        while( bucketIndex != 0 )
        {
            --bucketIndex;

            TableType::Bucket& rBucket = pBuckets[ bucketIndex ];
            rBucket.lock.LockWrite();

            size_t entryCount = rBucket.entries.GetSize();
            if( entryCount != 0 )
            {
                m_bucketIndex = bucketIndex;
                m_elementIndex = entryCount - 1;

                return *this;
            }

            rBucket.lock.UnlockWrite();
        }

        m_pTable = NULL;
        m_bucketIndex = 0;
        m_elementIndex = 0;

        return *this;
    }

    /// Get whether this accessor references the same hash table location as another accessor.
    ///
    /// @param[in] rOther  Accessor against which to compare.
    ///
    /// @return  True if this accessor references the same hash table location as the given accessor, false if not.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    bool ConcurrentHashTableAccessor< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::operator==(
        const ConcurrentHashTableAccessor& rOther ) const
    {
        return( m_pTable == rOther.m_pTable &&
                m_bucketIndex == rOther.m_bucketIndex &&
                m_elementIndex == rOther.m_elementIndex );
    }

    /// Get whether this accessor does not reference the same hash table location as another accessor.
    ///
    /// @param[in] rOther  Accessor against which to compare.
    ///
    /// @return  True if this accessor does not reference the same hash table location as the given accessor, false if
    ///          they do match.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    bool ConcurrentHashTableAccessor< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::operator!=(
        const ConcurrentHashTableAccessor& rOther ) const
    {
        return( m_pTable != rOther.m_pTable ||
                m_bucketIndex != rOther.m_bucketIndex ||
                m_elementIndex != rOther.m_elementIndex );
    }

    /// Directly set the hash table entry reference for this accessor.
    ///
    /// This should only be called by the table itself, and only on entries for which a lock already exists.  Control of
    /// the lock is passed onto this accessor to release at a later time.
    ///
    /// @param[in] pTable        Table to reference.
    /// @param[in] bucketIndex   Index of the bucket containing the entry to reference.
    /// @param[in] elementIndex  Index of the bucket element in which the table entry is stored.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    void ConcurrentHashTableAccessor< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::Set(
        TableType* pTable,
        size_t bucketIndex,
        size_t elementIndex )
    {
        HELIUM_ASSERT( m_pTable == NULL );

        m_pTable = pTable;
        m_bucketIndex = bucketIndex;
        m_elementIndex = elementIndex;
    }

    /// Constructor.
    ///
    /// @param[in] bucketCount  Number of buckets to allocate in the table.  Prime numbers are recommended for more
    ///                         efficient distribution.  This will be clamped to a minimum of one.
    /// @param[in] rHasher      Key hashing functor.
    /// @param[in] rKeyEquals   Key equal comparison functor.
    /// @param[in] rExtractKey  Key extraction functor.
    /// @param[in] rAllocator   Allocator functor.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::ConcurrentHashTable(
        size_t bucketCount,
        const HashFunction& rHasher,
        const EqualKey& rKeyEquals,
        const ExtractKey& rExtractKey,
        const Allocator& rAllocator )
        : m_bucketCount( Max( bucketCount, 1 ) )
        , m_size( 0 )
        , m_hasher( rHasher )
        , m_keyEquals( rKeyEquals )
        , m_extractKey( rExtractKey )
        , m_allocator( rAllocator )
    {
        AllocateBuckets();
    }

    /// Constructor.
    ///
    /// @param[in] bucketCount  Number of buckets to allocate in the table.  Prime numbers are recommended for more
    ///                         efficient distribution.  This will be clamped to a minimum of one.
    /// @param[in] rHasher      Key hashing functor.
    /// @param[in] rKeyEquals   Key equal comparison functor.
    /// @param[in] rAllocator   Allocator functor.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::ConcurrentHashTable(
        size_t bucketCount,
        const HashFunction& rHasher,
        const EqualKey& rKeyEquals,
        const Allocator& rAllocator )
        : m_bucketCount( Max< size_t >( bucketCount, 1 ) )
        , m_size( 0 )
        , m_hasher( rHasher )
        , m_keyEquals( rKeyEquals )
        , m_allocator( rAllocator )
    {
        AllocateBuckets();
    }

    /// Copy constructor.
    ///
    /// @param[in] rSource  Hash table from which to construct a copy.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::ConcurrentHashTable(
        const ConcurrentHashTable& rSource )
    {
        CopyConstruct( rSource );
    }

    /// Destructor.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::~ConcurrentHashTable()
    {
        Finalize();
    }

    /// Get the number of entries currently in this table.
    ///
    /// @return  Number of hash table entries.
    ///
    /// @see IsEmpty()
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    size_t ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::GetSize() const
    {
        return static_cast< uint32_t >( m_size );
    }

    /// Get whether this table is currently empty.
    ///
    /// @return  True if this table is empty, false if not.
    ///
    /// @see GetSize()
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    bool ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::IsEmpty() const
    {
        return( m_size == 0 );
    }

    /// Clear out all entries in this table.
    ///
    /// @see Trim()
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    void ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::Clear()
    {
        size_t bucketCount = m_bucketCount;
        for( size_t bucketIndex = 0; bucketIndex < bucketCount; ++bucketIndex )
        {
            Bucket& rBucket = m_pBuckets[ bucketIndex ];
            rBucket.lock.LockWrite();
            rBucket.entries.Clear();
            AtomicIncrementRelease( rBucket.tag );
            rBucket.lock.UnlockWrite();
        }

        AtomicExchangeRelease( m_size, 0 );
    }

    /// Trim all excess memory used in each table bucket.
    ///
    /// @see Clear()
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    void ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::Trim()
    {
        size_t bucketCount = m_bucketCount;
        for( size_t bucketIndex = 0; bucketIndex < bucketCount; ++bucketIndex )
        {
            Bucket& rBucket = m_pBuckets[ bucketIndex ];
            rBucket.lock.LockWrite();
            rBucket.entries.Trim();
            rBucket.lock.UnlockWrite();
        }
    }

    /// Retrieve an accessor referencing the first element in this table.
    ///
    /// @param[out] rAccessor  Set to reference the first element in this table if one exists, released if this table is
    ///                        empty.
    ///
    /// @return  True if this table is not empty and the accessor was set, false if this table is empty and the accessor
    ///          was released.
    ///
    /// @see Last()
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    bool ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::First( Accessor& rAccessor )
    {
        rAccessor.Release();

        // Initial quick test for an empty table (table can still become empty while we search for the first element, as
        // well).
        if( m_size == 0 )
        {
            return false;
        }

        // Search through the table for the first element.
        HELIUM_ASSERT( m_pBuckets );
        size_t bucketCount = m_bucketCount;
        for( size_t bucketIndex = 0; bucketIndex < bucketCount; ++bucketIndex )
        {
            Bucket& rBucket = m_pBuckets[ bucketIndex ];
            rBucket.lock.LockWrite();
            if( !rBucket.entries.IsEmpty() )
            {
                // Leave the lock intact.
                rAccessor.Set( this, bucketIndex, 0 );

                return true;
            }

            rBucket.lock.UnlockWrite();
        }

        return false;
    }

    /// Retrieve a constant accessor referencing the first element in this table.
    ///
    /// @param[out] rAccessor  Set to reference the first element in this table if one exists, released if this table is
    ///                        empty.
    ///
    /// @return  True if this table is not empty and the accessor was set, false if this table is empty and the accessor
    ///          was released.
    ///
    /// @see Last()
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    bool ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::First(
        ConstAccessor& rAccessor ) const
    {
        rAccessor.Release();

        // Initial quick test for an empty table (table can still become empty while we search for the first element, as
        // well).
        if( m_size == 0 )
        {
            return false;
        }

        // Search through the table for the first element.
        HELIUM_ASSERT( m_pBuckets );
        size_t bucketCount = m_bucketCount;
        for( size_t bucketIndex = 0; bucketIndex < bucketCount; ++bucketIndex )
        {
            Bucket& rBucket = m_pBuckets[ bucketIndex ];
            rBucket.lock.LockRead();
            if( !rBucket.entries.IsEmpty() )
            {
                // Leave the lock intact.
                rAccessor.Set( this, bucketIndex, 0 );

                return true;
            }

            rBucket.lock.UnlockRead();
        }

        return false;
    }

    /// Retrieve an accessor referencing the last element in this table.
    ///
    /// @param[out] rAccessor  Set to reference the last element in this table if one exists, released if this table is
    ///                        empty.
    ///
    /// @return  True if this table is not empty and the accessor was set, false if this table is empty and the accessor
    ///          was released.
    ///
    /// @see First()
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    bool ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::Last( Accessor& rAccessor )
    {
        rAccessor.Release();

        // Initial quick test for an empty table (table can still become empty while we search for the last element, as
        // well).
        if( m_size == 0 )
        {
            return false;
        }

        // Search through the table for the last element.
        HELIUM_ASSERT( m_pBuckets );
        size_t bucketCount = m_bucketCount;
        while( bucketCount != 0 )
        {
            --bucketCount;

            Bucket& rBucket = m_pBuckets[ bucketCount ];
            rBucket.lock.LockWrite();
            size_t entryCount = rBucket.entries.GetSize();
            if( entryCount != 0 )
            {
                // Leave the lock intact.
                rAccessor.Set( this, bucketCount, entryCount - 1 );

                return true;
            }

            rBucket.lock.UnlockWrite();
        }

        return false;
    }

    /// Retrieve a constant accessor referencing the last element in this table.
    ///
    /// @param[out] rAccessor  Set to reference the last element in this table if one exists, released if this table is
    ///                        empty.
    ///
    /// @return  True if this table is not empty and the accessor was set, false if this table is empty and the accessor
    ///          was released.
    ///
    /// @see First()
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    bool ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::Last(
        ConstAccessor& rAccessor ) const
    {
        rAccessor.Release();

        // Initial quick test for an empty table (table can still become empty while we search for the last element, as
        // well).
        if( m_size == 0 )
        {
            return false;
        }

        // Search through the table for the last element.
        HELIUM_ASSERT( m_pBuckets );
        size_t bucketCount = m_bucketCount;
        while( bucketCount != 0 )
        {
            --bucketCount;

            Bucket& rBucket = m_pBuckets[ bucketCount ];
            rBucket.lock.LockRead();
            size_t entryCount = rBucket.entries.GetSize();
            if( entryCount != 0 )
            {
                // Leave the lock intact.
                rAccessor.Set( this, bucketCount, entryCount - 1 );

                return true;
            }

            rBucket.lock.UnlockRead();
        }

        return false;
    }

    /// Search for an entry in this table with the given key, acquiring read-write access to the element if found.
    ///
    /// @param[out] rAccessor  Set to reference the element in this table with the given key if found, released if not
    ///                        found.
    /// @param[in]  rKey       Key to locate.
    ///
    /// @return  True if an entry with the given key was found, false if not.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    bool ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::Find(
        Accessor& rAccessor,
        const Key& rKey )
    {
        rAccessor.Release();

        size_t bucketIndex = ( m_hasher( rKey ) % m_bucketCount );

        HELIUM_ASSERT( m_pBuckets );
        Bucket& rBucket = m_pBuckets[ bucketIndex ];
        rBucket.lock.LockWrite();

        DynArray< Value >& rEntries = rBucket.entries;
        size_t entryCount = rEntries.GetSize();
        for( size_t entryIndex = 0; entryIndex < entryCount; ++entryIndex )
        {
            if( m_keyEquals( m_extractKey( rEntries[ entryIndex ] ), rKey ) )
            {
                // Leave the lock intact.
                rAccessor.Set( this, bucketIndex, entryIndex );

                return true;
            }
        }

        rBucket.lock.UnlockWrite();

        return false;
    }

    /// Search for an entry in this table with the given key, acquiring read-only access to the element if found.
    ///
    /// @param[out] rAccessor  Set to reference the element in this table with the given key if found, released if not
    ///                        found.
    /// @param[in]  rKey       Key to locate.
    ///
    /// @return  True if an entry with the given key was found, false if not.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    bool ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::Find(
        ConstAccessor& rAccessor,
        const Key& rKey ) const
    {
        rAccessor.Release();

        size_t bucketIndex = ( m_hasher( rKey ) % m_bucketCount );

        HELIUM_ASSERT( m_pBuckets );
        Bucket& rBucket = m_pBuckets[ bucketIndex ];
        rBucket.lock.LockRead();

        DynArray< Value >& rEntries = rBucket.entries;
        size_t entryCount = rEntries.GetSize();
        for( size_t entryIndex = 0; entryIndex < entryCount; ++entryIndex )
        {
            if( m_keyEquals( m_extractKey( rEntries[ entryIndex ] ), rKey ) )
            {
                // Leave the lock intact.
                rAccessor.Set( this, bucketIndex, entryIndex );

                return true;
            }
        }

        rBucket.lock.UnlockRead();

        return false;
    }

    /// Locate the entry in this table with a key that matches that of a given value, inserting a copy of the given
    /// value if one does not already exist.
    ///
    /// @param[out] rAccessor  Accessor set to reference the entry in this table with the given key.
    /// @param[in]  rValue     Value containing the key to find as well as providing the value to insert if an entry
    ///                        does not already exist with the given key.
    ///
    /// @return  True if a new entry was inserted, false if one already exists.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    bool ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::Insert(
        ConstAccessor& rAccessor,
        const Value& rValue )
    {
        rAccessor.Release();

        // Acquire a read-only lock on the target bucket.
        const Key& rKey = m_extractKey( rValue );
        size_t bucketIndex = m_hasher( rKey ) % m_bucketCount;

        HELIUM_ASSERT( m_pBuckets );
        Bucket& rBucket = m_pBuckets[ bucketIndex ];
        rBucket.lock.LockRead();

        int32_t currentTag = rBucket.tag;

        // Loop as long as entries are removed from the list in between lock switches.
        for( ; ; )
        {
            // Search for an existing entry.
            DynArray< Value, Allocator >& rEntries = rBucket.entries;
            size_t entryCount = rEntries.GetSize();
            size_t entryIndex;
            for( entryIndex = 0; entryIndex < entryCount; ++entryIndex )
            {
                if( m_keyEquals( m_extractKey( rEntries[ entryIndex ] ), rKey ) )
                {
                    rAccessor.Set( this, bucketIndex, entryIndex );

                    return false;
                }
            }

            // Entry not found, so switch to an exclusive lock so we can attempt to add the new entry.
            rBucket.lock.UnlockRead();
            rBucket.lock.LockWrite();

            // If entries were removed, we need to re-search through the entry list, otherwise we know we only need to
            // search for new entries that may have been added during the lock switch.
            bool bInserted = false;

            int32_t newTag = rBucket.tag;
            size_t startIndex = ( currentTag == newTag ? entryCount : 0 );
            currentTag = newTag;

            entryCount = rEntries.GetSize();
            for( entryIndex = startIndex; entryIndex < entryCount; ++entryIndex )
            {
                if( m_keyEquals( m_extractKey( rEntries[ entryIndex ] ), rKey ) )
                {
                    // An entry was added, but we can't set the accessor to it quite yet since we are still in the
                    // middle of a read-write lock.
                    break;
                }
            }

            if( entryIndex >= entryCount )
            {
                // Entry still doesn't exist, so add a new entry.  Again, we can't set the accessor to it yet since we
                // still need to switch back to a read-only lock.
                rEntries.Add( rValue );
                HELIUM_ASSERT( entryIndex == entryCount );

                AtomicIncrementRelease( m_size );

                bInserted = true;
            }

            // Switch back to a read lock.
            rBucket.lock.UnlockWrite();
            rBucket.lock.LockRead();

            // We can finally set the accessor and return if no entries were removed while switching locks.
            newTag = rBucket.tag;
            if( currentTag == newTag )
            {
                rAccessor.Set( this, bucketIndex, entryIndex );

                return bInserted;
            }
        }
    }

    /// Locate the entry in this table with a key that matches that of a given value, inserting a copy of the given
    /// value if one does not already exist.
    ///
    /// @param[out] rAccessor  Accessor set to reference the entry in this table with the given key.
    /// @param[in]  rValue     Value containing the key to find as well as providing the value to insert if an entry
    ///                        does not already exist with the given key.
    ///
    /// @return  True if a new entry was inserted, false if one already exists.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    bool ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::Insert(
        Accessor& rAccessor,
        const Value& rValue )
    {
        rAccessor.Release();

        // Acquire a read-write lock on the target bucket.
        const Key& rKey = m_extractKey( rValue );
        size_t bucketIndex = m_hasher( rKey ) % m_bucketCount;

        HELIUM_ASSERT( m_pBuckets );
        Bucket& rBucket = m_pBuckets[ bucketIndex ];
        rBucket.lock.LockWrite();

        // Search for an existing entry.
        DynArray< Value, Allocator >& rEntries = rBucket.entries;
        size_t entryCount = rEntries.GetSize();
        for( size_t entryIndex = 0; entryIndex < entryCount; ++entryIndex )
        {
            if( m_keyEquals( m_extractKey( rEntries[ entryIndex ] ), rKey ) )
            {
                rAccessor.Set( this, bucketIndex, entryIndex );

                return false;
            }
        }

        // Entry not found, so add it to the table.
        rEntries.Add( rValue );
        AtomicIncrementRelease( m_size );

        rAccessor.Set( this, bucketIndex, entryCount );

        return true;
    }

    /// Remove any entry with the specified key from this table.
    ///
    /// @param[in] rKey  Key to locate.
    ///
    /// @return  True if an entry was found and removed, false if not.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    bool ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::Remove( const Key& rKey )
    {
        // Acquire a read-write lock on the target bucket.
        size_t bucketIndex = m_hasher( rKey ) % m_bucketCount;

        HELIUM_ASSERT( m_pBuckets );
        Bucket& rBucket = m_pBuckets[ bucketIndex ];
        rBucket.lock.LockWrite();

        // Search for an entry with the specified key.
        DynArray< Value, Allocator >& rEntries = rBucket.entries;
        size_t entryCount = rEntries.GetSize();
        for( size_t entryIndex = 0; entryIndex < entryCount; ++entryIndex )
        {
            if( m_keyEquals( m_extractKey( rEntries[ entryIndex ] ), rKey ) )
            {
                rEntries.RemoveSwap( entryIndex );
                AtomicIncrementRelease( rBucket.tag );
                AtomicDecrementRelease( m_size );

                rBucket.lock.UnlockWrite();

                return true;
            }
        }

        // Entry not found, so no action has been taken.
        rBucket.lock.UnlockWrite();

        return false;
    }

    /// Remove the entry referenced by the specified accessor.
    ///
    /// @param[in] rAccessor  Accessor for the entry to remove.
    ///
    /// @return  True if the entry was removed, false if not (accessor is not valid or references an entry in another
    ///          table).
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    bool ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::Remove( Accessor& rAccessor )
    {
        // Accessor must reference this table.
        HELIUM_ASSERT( rAccessor.m_pTable == this );
        if( rAccessor.m_pTable != this )
        {
            return false;
        }

        // Lock already exists, so just remove the entry.
        HELIUM_ASSERT( m_pBuckets );
        Bucket& rBucket = m_pBuckets[ rAccessor.m_bucketIndex ];

        HELIUM_ASSERT( rAccessor.m_elementIndex < rBucket.entries.GetSize() );
        rBucket.entries.RemoveSwap( rAccessor.m_elementIndex );
        AtomicIncrementRelease( rBucket.tag );
        AtomicDecrementRelease( m_size );

        // Release the accessor (this removes the lock as well).
        rAccessor.Release();

        return true;
    }

    /// Assignment operator.
    ///
    /// @param[in] rSource  Source table from which to copy.
    ///
    /// @return  Reference to this object.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >&
        ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::operator=(
            const ConcurrentHashTable& rSource )
    {
        if( this != &rSource )
        {
            Finalize();
            CopyConstruct( rSource );
        }

        return *this;
    }

    /// Allocate hash table buckets.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    void ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::AllocateBuckets()
    {
        size_t bucketCount = m_bucketCount;

        void* pBuffer = m_allocator.Allocate( sizeof( Bucket ) * bucketCount );
        HELIUM_ASSERT( pBuffer );

        Bucket* pBuckets = ArrayInPlaceConstruct< Bucket >( pBuffer, bucketCount );
        HELIUM_ASSERT( pBuckets == pBuffer );
        for( size_t bucketIndex = 0; bucketIndex < bucketCount; ++bucketIndex )
        {
            pBuckets[ bucketIndex ].tag = 0;
        }

        m_pBuckets = pBuckets;
    }

    /// Allocate and construct a copy of the specified object, assuming all data in this object is uninitialized.
    ///
    /// @param[in] rSource  Object to copy.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    void ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::CopyConstruct(
        const ConcurrentHashTable& rSource )
    {
        size_t bucketCount = rSource.m_bucketCount;
        m_bucketCount = bucketCount;

        m_size = rSource.m_size;

        m_hasher = rSource.m_hasher;
        m_keyEquals = rSource.m_keyEquals;
        m_extractKey = rSource.m_extractKey;
        m_allocator = rSource.m_allocator;

        AllocateBuckets();

        for( size_t bucketIndex = 0; bucketIndex < bucketCount; ++bucketIndex )
        {
            m_pBuckets[ bucketIndex ].entries = rSource.m_pBuckets[ bucketIndex ].entries;
        }
    }

    /// Free all allocated resources, but don't clear out any variables unless necessary.
    template<
        typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey,
        typename Allocator >
    void ConcurrentHashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator >::Finalize()
    {
        ArrayInPlaceDestroy( m_pBuckets, m_bucketCount );
        m_allocator.Free( m_pBuckets );
    }
}
