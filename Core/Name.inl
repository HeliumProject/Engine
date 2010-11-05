//----------------------------------------------------------------------------------------------------------------------
// Name.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Constructor.
    ///
    /// When using the default constructor, the internal name entry pointer is left in an uninitialized state and must
    /// be manually initialized using Set() or some other assignment before being used.
    template< typename TableType >
    NameBase< TableType >::NameBase()
    {
    }

    /// Constructor.
    ///
    /// The name entry string will be initialized to null.
    template< typename TableType >
    NameBase< TableType >::NameBase( ENullName )
        : m_pEntry( NULL )
    {
    }

    /// Constructor.
    ///
    /// @param[in] pString  String to which the contents of this name should be initialized.
    template< typename TableType >
    NameBase< TableType >::NameBase( const CharType* pString )
    {
        Set( pString );
    }

    /// Constructor.
    ///
    /// @param[in] rString  String to which the contents of this name should be initialized.
    template< typename TableType >
    NameBase< TableType >::NameBase( const StringBase< CharType >& rString )
    {
        Set( rString );
    }

    /// Get the string contents for this name.
    ///
    /// @return  Null-terminated name string.  Even if this entry is empty, this will never return a null pointer.
    template< typename TableType >
    const typename NameBase< TableType >::CharType* NameBase< TableType >::Get() const
    {
        return( m_pEntry ? m_pEntry : TableType::sm_emptyString );
    }

    /// Get the string contents for this name.
    ///
    /// @return  Null-terminated name string if set to a name other than NULL_NAME, null pointer if the name is set to
    ///          NULL_NAME.
    template< typename TableType >
    const typename NameBase< TableType >::CharType* NameBase< TableType >::GetDirect() const
    {
        return m_pEntry;
    }

    /// Set this name.
    ///
    /// @param[in] pString  String to which this name should be set (can be null).
    ///
    /// @see Get()
    template< typename TableType >
    void NameBase< TableType >::Set( const CharType* pString )
    {
        // Check for empty strings first.
        if( !pString || pString[ 0 ] == L_T( '\0' ) )
        {
            m_pEntry = NULL;

            return;
        }

        // Lazily initialize the hash table.  Note that this is not inherently thread-safe, but there should always be
        // at least one name created before any sub-threads are spawned.
        if( !TableType::sm_pNameMemoryHeap )
        {
            TableType::sm_pNameMemoryHeap = new StackMemoryHeap<>( STACK_HEAP_BLOCK_SIZE );
            L_ASSERT( TableType::sm_pNameMemoryHeap );

            L_ASSERT( !TableType::sm_pTable );
            TableType::sm_pTable = new TableBucket [ TABLE_BUCKET_COUNT ];
            L_ASSERT( TableType::sm_pTable );
        }

        L_ASSERT( TableType::sm_pTable );

        // Compute the string's hash table index and retrieve the corresponding bucket.
        uint32_t bucketIndex = StringHash( pString ) % TABLE_BUCKET_COUNT;
        TableBucket& rBucket = TableType::sm_pTable[ bucketIndex ];

        // Locate the string in the table.  If it does not exist, add it.
        size_t entryCount = 0;
        m_pEntry = rBucket.Find( pString, entryCount );
        if( !m_pEntry )
        {
            m_pEntry = rBucket.Add( pString, entryCount );
            L_ASSERT( m_pEntry );
        }
    }

    /// Set this name.
    ///
    /// @param[in] rString  String to which this name should be set.
    ///
    /// @see Get()
    template< typename TableType >
    void NameBase< TableType >::Set( const StringBase< CharType >& rString )
    {
        Set( rString.GetData() );
    }

    /// Get whether this name is empty (null).
    ///
    /// @return  True if this name is empty, false if not.
    template< typename TableType >
    bool NameBase< TableType >::IsEmpty() const
    {
        return( m_pEntry == NULL );
    }

    /// Clear out the contents of this name (set it to null).
    template< typename TableType >
    void NameBase< TableType >::Clear()
    {
        m_pEntry = NULL;
    }

    /// Get the string contents for this name.
    ///
    /// @return  Null-terminated name string.  Even if this entry is empty, this will never return a null pointer.
    template< typename TableType >
    const typename NameBase< TableType >::CharType* NameBase< TableType >::operator*() const
    {
        return Get();
    }

    /// Equality comparison operator.
    ///
    /// @param[in] rName  Name with which to compare.
    ///
    /// @return  True if this name and the given name match, false if not.
    template< typename TableType >
    bool NameBase< TableType >::operator==( const NameBase& rName ) const
    {
        // Each name entry has a unique name pointer, so we can use the pointer itself in our comparisons.
        return( m_pEntry == rName.m_pEntry );
    }

    /// Inequality comparison operator.
    ///
    /// @param[in] rName  Name with which to compare.
    ///
    /// @return  True if this name and the given name do not match, false if they do match.
    template< typename TableType >
    bool NameBase< TableType >::operator!=( const NameBase& rName ) const
    {
        // Each name entry has a unique name pointer, so we can use the pointer itself in our comparisons.
        return( m_pEntry != rName.m_pEntry );
    }

    /// Release the name table and free all allocated memory.
    ///
    /// This should only be called immediately prior to application exit.
    template< typename TableType >
    void NameBase< TableType >::Shutdown()
    {
        L_LOG( LOG_INFO, L_T( "Shutting down Name table.\n" ) );

        delete [] TableType::sm_pTable;
        TableType::sm_pTable = NULL;

        delete TableType::sm_pNameMemoryHeap;
        TableType::sm_pNameMemoryHeap = NULL;

        L_LOG( LOG_INFO, L_T( "Name table shutdown complete.\n" ) );
    }

    /// Find an existing string in this table.
    ///
    /// @param[in]  pString      String to find.
    /// @param[out] rEntryCount  Number of entries in this bucket when the search was performed.
    ///
    /// @return  Entry string if found, null if not found.
    ///
    /// @see Add()
    template< typename TableType >
    const typename NameBase< TableType >::CharType* NameBase< TableType >::TableBucket::Find(
        const CharType* pString,
        size_t& rEntryCount )
    {
        L_ASSERT( pString );

        ScopeReadLock readLock( m_lock );

        const CharType** ppEntries = m_entries.GetData();
        size_t entryCount = m_entries.GetSize();
        L_ASSERT( ppEntries || entryCount == 0 );

        rEntryCount = entryCount;

        for( size_t entryIndex = 0; entryIndex < entryCount; ++entryIndex )
        {
            const CharType* pEntry = ppEntries[ entryIndex ];
            if( StringCompare( pEntry, pString ) == 0 )
            {
                return pEntry;
            }
        }

        return NULL;
    }

    /// Add a string to this table if it does not already exist.
    ///
    /// @param[in] pString             String to locate or add.
    /// @param[in] previousEntryCount  Number of entries already checked during a previous Find() call (existing entries
    ///                                are not expected to change).  In other words, the entry index from which to start
    ///                                checking for any additional string entries that may have been added since the
    ///                                previous Find() call.
    ///
    /// @return  Pointer to the string table entry for the string.
    ///
    /// @see Find()
    template< typename TableType >
    const typename NameBase< TableType >::CharType* NameBase< TableType >::TableBucket::Add(
        const CharType* pString,
        size_t previousEntryCount )
    {
        L_ASSERT( pString );

        ScopeWriteLock writeLock( m_lock );

        const CharType** ppEntries = m_entries.GetData();
        size_t entryCount = m_entries.GetSize();
        L_ASSERT( ppEntries || entryCount == 0 );
        L_ASSERT( previousEntryCount <= entryCount );
        for( size_t entryIndex = previousEntryCount; entryIndex < entryCount; ++entryIndex )
        {
            const CharType* pEntry = ppEntries[ entryIndex ];
            if( StringCompare( pEntry, pString ) == 0 )
            {
                return pEntry;
            }
        }

        size_t newEntryAllocSize = sizeof( CharType ) * ( StringLength( pString ) + 1 );

        L_ASSERT( TableType::sm_pNameMemoryHeap );
        CharType* pEntry = static_cast< CharType* >( TableType::sm_pNameMemoryHeap->Allocate( newEntryAllocSize ) );
        L_ASSERT( pEntry );
        MemoryCopy( pEntry, pString, newEntryAllocSize );

        m_entries.Push( pEntry );

        return pEntry;
    }

    /// Default Name hash.
    ///
    /// @param[in] rKey  Key for which to compute a hash value.
    ///
    /// @return  Hash value.
    template< typename TableType >
    size_t Hash< NameBase< TableType > >::operator()( const NameBase< TableType >& rKey ) const
    {
        // Each name entry has a unique name pointer, so we can use the pointer itself as our hash.
        return static_cast< size_t >( reinterpret_cast< uintptr_t >( rKey.GetDirect() ) );
    }
}
