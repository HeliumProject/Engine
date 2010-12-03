/// Constructor.
///
/// Creates an uninitialized iterator.  Using this is not safe until it is initialized.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
Helium::ConstHashTableIterator<
    Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::ConstHashTableIterator()
{
}

/// Constructor.
///
/// @param[in] pTable        Table to iterate.
/// @param[in] bucketIndex   Current table bucket index.
/// @param[in] elementIndex  Current bucket element index.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
Helium::ConstHashTableIterator<
    Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::ConstHashTableIterator(
        const TableType* pTable, size_t bucketIndex, size_t elementIndex )
    : m_pTable( const_cast< TableType* >( pTable ) )
    , m_bucketIndex( bucketIndex )
    , m_elementIndex( elementIndex )
{
}

/// Access the current hash table entry.
///
/// @return  Constant reference to the current hash table entry.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
const Value& Helium::ConstHashTableIterator<
    Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::operator*() const
{
    HELIUM_ASSERT( m_pTable );
    HELIUM_ASSERT( m_pTable->m_pBuckets );

    return m_pTable->m_pBuckets[ m_bucketIndex ][ m_elementIndex ];
}

/// Access the current hash table entry.
///
/// @return  Constant pointer to the current hash table entry.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
const Value* Helium::ConstHashTableIterator<
    Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::operator->() const
{
    HELIUM_ASSERT( m_pTable );
    HELIUM_ASSERT( m_pTable->m_pBuckets );

    return &m_pTable->m_pBuckets[ m_bucketIndex ][ m_elementIndex ];
}

/// Increment this iterator to the next hash table entry.
///
/// @return  Reference to this iterator.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
Helium::ConstHashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >&
    Helium::ConstHashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::operator++()
{
    HELIUM_ASSERT( m_pTable );
    HELIUM_ASSERT( m_bucketIndex < m_pTable->m_bucketCount );

    ++m_elementIndex;

    TableType::Bucket* pBuckets = m_pTable->m_pBuckets;
    HELIUM_ASSERT( pBuckets );

    if( m_elementIndex >= pBuckets[ m_bucketIndex ].GetSize() )
    {
        m_elementIndex = 0;

        size_t bucketCount = m_pTable->m_bucketCount;
        for( size_t bucketIndex = m_bucketIndex + 1; bucketIndex < bucketCount; ++bucketIndex )
        {
            if( !pBuckets[ bucketIndex ].IsEmpty() )
            {
                m_bucketIndex = bucketIndex;

                return *this;
            }
        }

        m_bucketIndex = bucketCount;
    }

    return *this;
}

/// Post-increment this iterator to the next hash table entry.
///
/// @return  Copy of this iterator at the location prior to incrementing.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
Helium::ConstHashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >
    Helium::ConstHashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::operator++( int )
{
    ConstHashTableIterator iterator = *this;
    ++( *this );

    return iterator;
}

/// Decrement this iterator to the previous hash table entry.
///
/// @return  Reference to this iterator.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
Helium::ConstHashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >&
    Helium::ConstHashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::operator--()
{
    HELIUM_ASSERT( m_pTable );
    HELIUM_ASSERT( m_bucketIndex <= m_pTable->m_bucketCount );  // Allow decrementing from the End() iterator.

    if( m_elementIndex > 0 )
    {
        --m_elementIndex;

        return *this;
    }

    TableType::Bucket* pBuckets = m_pTable->m_pBuckets;
    HELIUM_ASSERT( pBuckets );

    size_t bucketIndex = m_bucketIndex;
    while( bucketIndex != 0 )
    {
        --bucketIndex;

        size_t entryCount = pBuckets[ bucketIndex ].GetSize();
        if( entryCount != 0 )
        {
            m_bucketIndex = bucketIndex;
            m_elementIndex = entryCount - 1;

            return *this;
        }
    }

    HELIUM_ASSERT_MSG_FALSE( TXT( "Attempted backward HashTable iteration past the start of the table" ) );

    --m_pTable;
    m_bucketIndex = 0;
    m_elementIndex = 0;

    return *this;
}

/// Post-decrement this iterator to the previous hash table entry.
///
/// @return  Copy of this iterator at the location prior to decrementing.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
Helium::ConstHashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >
    Helium::ConstHashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::operator--( int )
{
    ConstHashTableIterator iterator = *this;
    --( *this );

    return iterator;
}

/// Get whether this iterator references the same hash table location as another iterator.
///
/// @param[in] rOther  Iterator against which to compare.
///
/// @return  True if this iterator references the same hash table location as the given iterator, false if not.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
bool Helium::ConstHashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::operator==(
    const ConstHashTableIterator& rOther ) const
{
    return ( m_pTable == rOther.m_pTable &&
             m_bucketIndex == rOther.m_bucketIndex &&
             m_elementIndex == rOther.m_elementIndex );
}

/// Get whether this iterator does not reference the same hash table location as another iterator.
///
/// @param[in] rOther  Iterator against which to compare.
///
/// @return  True if this iterator does not reference the same hash table location as the given iterator, false if
///          they do match.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
bool Helium::ConstHashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::operator!=(
    const ConstHashTableIterator& rOther ) const
{
    return ( m_pTable != rOther.m_pTable ||
             m_bucketIndex != rOther.m_bucketIndex ||
             m_elementIndex != rOther.m_elementIndex );
}

/// Get whether this iterator references a hash table location that precedes that of another iterator.
///
/// @param[in] rOther  Iterator against which to compare.
///
/// @return  True if this iterator references a hash table location that precedes that of the given iterator, false if
///          not.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
bool Helium::ConstHashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::operator<(
    const ConstHashTableIterator& rOther ) const
{
    return ( m_pTable < rOther.m_pTable ||
             ( m_pTable == rOther.m_pTable &&
               ( m_bucketIndex < rOther.m_bucketIndex ||
                 m_bucketIndex == rOther.m_bucketIndex && m_elementIndex < rOther.m_elementIndex ) ) );
}

/// Get whether this iterator references a hash table location that succeeds that of another iterator.
///
/// @param[in] rOther  Iterator against which to compare.
///
/// @return  True if this iterator references a hash table location that succeeds that of the given iterator, false if
///          not.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
bool Helium::ConstHashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::operator>(
    const ConstHashTableIterator& rOther ) const
{
    return ( m_pTable > rOther.m_pTable ||
             ( m_pTable == rOther.m_pTable &&
               ( m_bucketIndex > rOther.m_bucketIndex ||
                 m_bucketIndex == rOther.m_bucketIndex && m_elementIndex > rOther.m_elementIndex ) ) );
}

/// Get whether this iterator references a hash table location that matches or precedes that of another iterator.
///
/// @param[in] rOther  Iterator against which to compare.
///
/// @return  True if this iterator references a hash table location that matches or precedes that of the given iterator,
///          false if not.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
bool Helium::ConstHashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::operator<=(
    const ConstHashTableIterator& rOther ) const
{
    return ( m_pTable < rOther.m_pTable ||
             ( m_pTable == rOther.m_pTable &&
               ( m_bucketIndex < rOther.m_bucketIndex ||
                 m_bucketIndex == rOther.m_bucketIndex && m_elementIndex <= rOther.m_elementIndex ) ) );
}

/// Get whether this iterator references a hash table location that matches or succeeds that of another iterator.
///
/// @param[in] rOther  Iterator against which to compare.
///
/// @return  True if this iterator references a hash table location that matches or succeeds that of the given iterator,
///          false if not.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
bool Helium::ConstHashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::operator>=(
    const ConstHashTableIterator& rOther ) const
{
    return ( m_pTable > rOther.m_pTable ||
             ( m_pTable == rOther.m_pTable &&
               ( m_bucketIndex > rOther.m_bucketIndex ||
                 m_bucketIndex == rOther.m_bucketIndex && m_elementIndex >= rOther.m_elementIndex ) ) );
}

/// Constructor.
///
/// Creates an uninitialized iterator.  Using this is not safe until it is initialized.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
Helium::HashTableIterator<
    Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::HashTableIterator()
{
}

/// Constructor.
///
/// @param[in] pTable        Table to iterate.
/// @param[in] bucketIndex   Current table bucket index.
/// @param[in] elementIndex  Current bucket element index.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
Helium::HashTableIterator<
    Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::HashTableIterator(
        TableType* pTable, size_t bucketIndex, size_t elementIndex )
    : ConstHashTableIterator( pTable, bucketIndex, elementIndex )
{
}

/// Access the current hash table entry.
///
/// @return  Reference to the current hash table entry.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
Value& Helium::HashTableIterator<
    Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::operator*() const
{
    HELIUM_ASSERT( m_pTable );
    HELIUM_ASSERT( m_pTable->m_pBuckets );

    return m_pTable->m_pBuckets[ m_bucketIndex ][ m_elementIndex ];
}

/// Access the current hash table entry.
///
/// @return  Pointer to the current hash table entry.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
Value* Helium::HashTableIterator<
    Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::operator->() const
{
    HELIUM_ASSERT( m_pTable );
    HELIUM_ASSERT( m_pTable->m_pBuckets );

    return &m_pTable->m_pBuckets[ m_bucketIndex ][ m_elementIndex ];
}

/// Increment this iterator to the next hash table entry.
///
/// @return  Reference to this iterator.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
Helium::HashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >&
    Helium::HashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::operator++()
{
    ConstHashTableIterator::operator++();

    return *this;
}

/// Post-increment this iterator to the next hash table entry.
///
/// @return  Copy of this iterator at the location prior to incrementing.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
Helium::HashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >
    Helium::HashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::operator++( int )
{
    HashTableIterator iterator = *this;
    ++( *this );

    return iterator;
}

/// Decrement this iterator to the previous hash table entry.
///
/// @return  Reference to this iterator.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
Helium::HashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >&
    Helium::HashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::operator--()
{
    ConstHashTableIterator::operator--();

    return *this;
}

/// Post-decrement this iterator to the previous hash table entry.
///
/// @return  Copy of this iterator at the location prior to decrementing.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
Helium::HashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >
    Helium::HashTableIterator< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::operator--( int )
{
    HashTableIterator iterator = *this;
    --( *this );

    return iterator;
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
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::HashTable(
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
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::HashTable(
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
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::HashTable(
    const HashTable& rSource )
{
    CopyConstruct( rSource );
}

/// Copy constructor.
///
/// @param[in] rSource  Hash table from which to construct a copy.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
template< typename OtherAllocator >
Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::HashTable(
    const HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, OtherAllocator, InternalValue >& rSource )
{
    CopyConstruct( rSource );
}

/// Destructor.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::~HashTable()
{
    Finalize();
}

/// Get the number of entries currently in this table.
///
/// @return  Number of hash table entries.
///
/// @see IsEmpty()
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
size_t Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::GetSize() const
{
    return m_size;
}

/// Get whether this table is currently empty.
///
/// @return  True if this table is empty, false if not.
///
/// @see GetSize()
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
bool Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::IsEmpty() const
{
    return ( m_size == 0 );
}

/// Clear out all entries in this table.
///
/// @see Trim()
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
void Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::Clear()
{
    size_t bucketCount = m_bucketCount;
    for( size_t bucketIndex = 0; bucketIndex < bucketCount; ++bucketIndex )
    {
        m_pBuckets[ bucketIndex ].Clear();
    }

    m_size = 0;
}

/// Trim all excess memory used in each table bucket.
///
/// @see Clear()
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
void Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::Trim()
{
    size_t bucketCount = m_bucketCount;
    for( size_t bucketIndex = 0; bucketIndex < bucketCount; ++bucketIndex )
    {
        m_pBuckets[ bucketIndex ].Trim();
    }
}

/// Retrieve an iterator referencing the beginning of this table.
///
/// @return  Iterator at the beginning of this table.
///
/// @see End()
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
typename Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::Iterator
    Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::Begin()
{
    if( m_size != 0 )
    {
        size_t bucketCount = m_bucketCount;
        for( size_t bucketIndex = 0; bucketIndex < bucketCount; ++bucketIndex )
        {
            if( !m_pBuckets[ bucketIndex ].IsEmpty() )
            {
                return Iterator( this, bucketIndex, 0 );
            }
        }
    }

    return End();
}

/// Retrieve a constant iterator referencing the beginning of this table.
///
/// @return  Constant iterator at the beginning of this table.
///
/// @see End()
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
typename Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::ConstIterator
    Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::Begin() const
{
    if( m_size != 0 )
    {
        size_t bucketCount = m_bucketCount;
        for( size_t bucketIndex = 0; bucketIndex < bucketCount; ++bucketIndex )
        {
            if( !m_pBuckets[ bucketIndex ].IsEmpty() )
            {
                return ConstIterator( this, bucketIndex, 0 );
            }
        }
    }

    return End();
}

/// Retrieve an iterator referencing the end of this table.
///
/// @return  Iterator at the end of this table.
///
/// @see Begin()
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
typename Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::Iterator
    Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::End()
{
    return Iterator( this, m_bucketCount, 0 );
}

/// Retrieve a constant iterator referencing the end of this table.
///
/// @return  Constant iterator at the end of this table.
///
/// @see End()
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
typename Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::ConstIterator
    Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::End() const
{
    return ConstIterator( this, m_bucketCount, 0 );
}

/// Search for an entry in this table with the given key, acquiring read-write access to the element if found.
///
/// @param[in] rKey  Key to locate.
///
/// @return  Iterator referencing the element in this table with the given key if found, otherwise referencing the table
///          end if not found.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
typename Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::Iterator
    Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::Find(
        const Key& rKey )
{
    if( m_size != 0 )
    {
        size_t bucketIndex = ( m_hasher( rKey ) % m_bucketCount );

        HELIUM_ASSERT( m_pBuckets );
        Bucket& rEntries = m_pBuckets[ bucketIndex ];
        size_t entryCount = rEntries.GetSize();
        for( size_t entryIndex = 0; entryIndex < entryCount; ++entryIndex )
        {
            if( m_keyEquals( m_extractKey( rEntries[ entryIndex ] ), rKey ) )
            {
                return Iterator( this, bucketIndex, entryIndex );
            }
        }
    }

    return End();
}

/// Search for an entry in this table with the given key, acquiring read-only access to the element if found.
///
/// @param[in] rKey  Key to locate.
///
/// @return  Constant iterator referencing the element in this table with the given key if found, otherwise referencing
///          the table end if not found.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
typename Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::ConstIterator
    Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::Find(
        const Key& rKey ) const
{
    if( m_size != 0 )
    {
        size_t bucketIndex = ( m_hasher( rKey ) % m_bucketCount );

        HELIUM_ASSERT( m_pBuckets );
        Bucket& rEntries = m_pBuckets[ bucketIndex ];
        size_t entryCount = rEntries.GetSize();
        for( size_t entryIndex = 0; entryIndex < entryCount; ++entryIndex )
        {
            if( m_keyEquals( m_extractKey( rEntries[ entryIndex ] ), rKey ) )
            {
                return ConstIterator( this, bucketIndex, entryIndex );
            }
        }
    }

    return End();
}

/// Locate the entry in this table with a key that matches that of a given value, inserting a copy of the given value if
/// one does not already exist.
///
/// @param[in] rValue  Value containing the key to find as well as providing the value to insert if an entry does not
///                    already exist with the given key.
///
/// @return  Pair containing an iterator and a boolean value.  The iterator will be set to reference the entry in this
///          table with the given key, while the boolean value will be set to true if the entry was inserted, false if
///          an entry already existed in this table (in which case the value won't automatically be inserted.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
Helium::Pair< typename Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::Iterator, bool >
    Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::Insert(
        const Value& rValue )
{
    Pair< Iterator, bool > result;
    result.Second() = Insert( result.First(), rValue );

    return result;
}

/// Locate the entry in this table with a key that matches that of a given value, inserting a copy of the given value if
/// one does not already exist.
///
/// @param[out] rIterator  Iterator set to reference the entry in this table with the given key.
/// @param[in]  rValue     Value containing the key to find as well as providing the value to insert if an entry does
///                        not already exist with the given key.
///
/// @return  True if a new entry was inserted, false if one already exists.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
bool Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::Insert(
    ConstIterator& rIterator,
    const Value& rValue )
{
    const Key& rKey = m_extractKey( rValue );
    size_t bucketIndex = m_hasher( rKey ) % m_bucketCount;

    // Search for an existing entry.
    HELIUM_ASSERT( m_pBuckets );
    Bucket& rEntries = m_pBuckets[ bucketIndex ];
    size_t entryCount = rEntries.GetSize();
    for( size_t entryIndex = 0; entryIndex < entryCount; ++entryIndex )
    {
        if( m_keyEquals( m_extractKey( rEntries[ entryIndex ] ), rKey ) )
        {
            rIterator = ConstIterator( this, bucketIndex, entryIndex );

            return false;
        }
    }

    // Entry not found, so add it to the table.
    rEntries.Add( rValue );
    ++m_size;

    rIterator = ConstIterator( this, bucketIndex, entryCount );

    return true;
}

/// Remove any entry with the specified key from this table.
///
/// @param[in] rKey  Key to locate.
///
/// @return  True if an entry was found and removed, false if not.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
bool Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::Remove( const Key& rKey )
{
    size_t bucketIndex = m_hasher( rKey ) % m_bucketCount;

    HELIUM_ASSERT( m_pBuckets );
    Bucket& rEntries = m_pBuckets[ bucketIndex ];
    size_t entryCount = rEntries.GetSize();
    for( size_t entryIndex = 0; entryIndex < entryCount; ++entryIndex )
    {
        if( m_keyEquals( m_extractKey( rEntries[ entryIndex ] ), rKey ) )
        {
            rEntries.RemoveSwap( entryIndex );
            --m_size;

            return true;
        }
    }

    // Entry not found, so no action has been taken.
    return false;
}

/// Remove the entry referenced by the specified iterator.
///
/// @param[in] iterator  Iterator for the entry to remove.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
void Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::Remove(
    Iterator iterator )
{
    HELIUM_ASSERT( iterator.m_pTable == this );
    HELIUM_ASSERT( iterator.m_bucketIndex < m_bucketCount );
    HELIUM_ASSERT( iterator.m_elementIndex < m_pBuckets[ iterator.m_bucketIndex ].GetSize() );
    m_pBuckets[ iterator.m_bucketIndex ].RemoveSwap( iterator.m_elementIndex );
    --m_size;

    return true;
}

/// Remove a range of entries between the specified iterators.
///
/// @param[in] start  Iterator referencing the first entry in the range to remove.
/// @param[in] end    Iterator referencing the end of the range to remove (one entry past the last entry to remove.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
void Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::Remove(
    Iterator start,
    Iterator end )
{
    HELIUM_ASSERT( start.m_pTable == this );
    HELIUM_ASSERT( end.m_pTable == this );

    size_t bucketIndex = start.m_bucketIndex;
    size_t elementIndex = start.m_elementIndex;
    HELIUM_ASSERT(
        ( bucketIndex < m_bucketCount && elementIndex < m_pBuckets[ bucketIndex ].GetSize() ) ||
        ( bucketIndex == m_bucketCount && elementIndex == 0 ) );

    size_t endBucketIndex = end.m_bucketIndex;
    size_t endElementIndex = end.m_elementIndex;
    HELIUM_ASSERT(
        ( endBucketIndex < m_bucketCount && endElementIndex < m_pBuckets[ endBucketIndex ].GetSize() ) ||
        ( endBucketIndex == m_bucketCount && endElementIndex == 0 ) );

    for( ; bucketIndex < endBucketIndex; ++bucketIndex )
    {
        Bucket& rEntries = m_pBuckets[ bucketIndex ];
        size_t elementCount = rEntries.GetSize();
        rEntries.Remove( elementIndex, elementCount - elementIndex );

        elementIndex = 0;
    }

    if( endBucketIndex < m_bucketCount )
    {
        rEntries.Remove( elementIndex, endElementIndex - elementIndex );
    }
}

/// Swap the contents of this table with another table.
///
/// @param[in] rTable  Table with which to swap.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
void Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::Swap(
    HashTable& rTable )
{
    Bucket* pBuckets = m_pBuckets;
    size_t bucketCount = m_bucketCount;
    size_t size = m_size;
    HasherType hasher = m_hasher;
    EqualKey keyEquals = m_keyEquals;
    ExtractKey extractKey = m_extractKey;
    AllocatorType allocator = m_allocator;

    m_pBuckets = rTable.m_pBuckets;
    m_bucketCount = rTable.m_bucketCount;
    m_size = rTable.m_size;
    m_hasher = rTable.m_hasher;
    m_keyEquals = rTable.m_keyEquals;
    m_extractKey = rTable.m_extractKey;
    m_allocator = rTable.m_allocator;

    rTable.m_pBuckets = pBuckets;
    rTable.m_bucketCount = bucketCount;
    rTable.m_size = size;
    rTable.m_hasher = hasher;
    rTable.m_keyEquals = keyEquals;
    rTable.m_extractKey = extractKey;
    rTable.m_allocator = allocator;
}

/// Assignment operator.
///
/// @param[in] rSource  Source table from which to copy.
///
/// @return  Reference to this object.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >&
    Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::operator=(
        const HashTable& rSource )
{
    if( this != &rSource )
    {
        Finalize();
        CopyConstruct( rSource );
    }

    return *this;
}

/// Assignment operator.
///
/// @param[in] rSource  Source table from which to copy.
///
/// @return  Reference to this object.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
template< typename OtherAllocator >
Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >&
    Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::operator=(
        const HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, OtherAllocator, InternalValue >& rSource )
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
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
void Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::AllocateBuckets()
{
    size_t bucketCount = m_bucketCount;

    void* pBuffer = m_allocator.Allocate( sizeof( Bucket ) * bucketCount );
    HELIUM_ASSERT( pBuffer );

    m_pBuckets = ArrayInPlaceConstruct< Bucket >( pBuffer, bucketCount );
    HELIUM_ASSERT( m_pBuckets == pBuffer );
}

/// Allocate and construct a copy of the specified object, assuming all data in this object is uninitialized.
///
/// @param[in] rSource  Object to copy.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
template< typename OtherAllocator >
void Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::CopyConstruct(
    const HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, OtherAllocator, InternalValue >& rSource )
{
    size_t bucketCount = rSource.m_bucketCount;
    m_bucketCount = bucketCount;

    m_size = rSource.m_size;

    m_hasher = rSource.m_hasher;
    m_keyEquals = rSource.m_keyEquals;
    m_extractKey = rSource.m_extractKey;

    AllocateBuckets();

    for( size_t bucketIndex = 0; bucketIndex < bucketCount; ++bucketIndex )
    {
        m_pBuckets[ bucketIndex ] = rSource.m_pBuckets[ bucketIndex ];
    }
}

/// Free all allocated resources, but don't clear out any variables unless necessary.
template<
    typename Value, typename Key, typename HashFunction, typename ExtractKey, typename EqualKey, typename Allocator,
    typename InternalValue >
void Helium::HashTable< Value, Key, HashFunction, ExtractKey, EqualKey, Allocator, InternalValue >::Finalize()
{
    ArrayInPlaceDestroy( m_pBuckets, m_bucketCount );
    m_allocator.Free( m_pBuckets );
}
