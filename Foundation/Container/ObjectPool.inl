/// Constructor.
///
/// @param[in] blockSize      Number of objects to allocate for each pool block.
/// @param[in] blockCountMax  Maximum number of blocks that can be allocated, clamped to a minimum of one.  If this
///                           is Invalid< size_t >(), no limit will be applied to the number of blocks that can be
///                           allocated.
template< typename T, typename Allocator >
Helium::ObjectPool< T, Allocator >::ObjectPool( size_t blockSize, size_t blockCountMax )
    : m_ppFreeObjects( NULL )
    , m_freeObjectCount( 0 )
    , m_pHeadBlock( NULL )
    , m_blockSize( Max< size_t >( blockSize, 1 ) )
    , m_allocatedBlockCount( 0 )
    , m_blockCountMax( Max< size_t >( blockCountMax, 1 ) )
{
    // Allocate the initial block.
    AllocateBlock();
    HELIUM_ASSERT( m_ppFreeObjects );
    HELIUM_ASSERT( m_freeObjectCount == m_blockSize );
    HELIUM_ASSERT( m_pHeadBlock );
    HELIUM_ASSERT( m_allocatedBlockCount == 1 );
}

/// Destructor.
template< typename T, typename Allocator >
Helium::ObjectPool< T, Allocator >::~ObjectPool()
{
    // Blocks are allocated as part of the object array associated with them, so we only need to free the buffer
    // addresses.
    Allocator allocator;

    Block* pNextBlock = m_pHeadBlock;
    while( pNextBlock )
    {
        Block* pBlock = pNextBlock;
        pNextBlock = pNextBlock->pNext;

        T* pObjects = pBlock->pObjects;
        ArrayInPlaceDestroy( pObjects, m_blockSize );
        allocator.Free( pObjects );
    }

    // Free the free object pool array.
    allocator.Free( const_cast< T** >( m_ppFreeObjects ) );
}

/// Allocate an object from this pool.
///
/// @return  Pointer to an object instance if one could be retrieved, null if the pool is empty and no more blocks
///          can be allocated.
///
/// @see Release()
template< typename T, typename Allocator >
T* Helium::ObjectPool< T, Allocator >::Allocate()
{
    {
        // Acquire a reader lock on the pool to synchronize block allocations.
        ScopeReadLock readLock( m_poolBlockAllocationLock );

        // Synchronize access to the free object list.
        ScopeLock< SpinLock > scopeLock( m_freeObjectSpinLock );

        size_t freeObjectCount = m_freeObjectCount;
        if( freeObjectCount != 0 )
        {
            // An object is in the free list, so grab it, release all locks, and return it.
            --freeObjectCount;

            T* pObject = m_ppFreeObjects[ freeObjectCount ];
            HELIUM_ASSERT( pObject );

            m_freeObjectCount = freeObjectCount;

            return pObject;
        }
    }

    // Acquire a heavy-weight lock for checking for and allocating new blocks.
    ScopeWriteLock writeLock( m_poolBlockAllocationLock );

    // Check if the free list is still empty (in case another thread managed to release an object or lock and
    // allocate a new block before we could acquire the write lock.
    size_t freeObjectCount = m_freeObjectCount;
    if( freeObjectCount != 0 )
    {
        // Free list is no longer empty, so grab an object normally.
        --freeObjectCount;

        T* pObject = m_ppFreeObjects[ freeObjectCount ];
        HELIUM_ASSERT( pObject );

        m_freeObjectCount = freeObjectCount;

        return pObject;
    }

    // Free list is still empty, so attempt to allocate a new block if possible.
    if( m_allocatedBlockCount == m_blockCountMax )
    {
        // Out of blocks that we can allocate.
        return NULL;
    }

    AllocateBlock();

    freeObjectCount = m_freeObjectCount;
    HELIUM_ASSERT( freeObjectCount != 0 );
    --freeObjectCount;

    T* pObject = m_ppFreeObjects[ freeObjectCount ];
    HELIUM_ASSERT( pObject );

    m_freeObjectCount = freeObjectCount;

    return pObject;
}

/// Release an object previously retrieved using Allocate() back into this pool.
///
/// @param[in] pObject  Pointer to the object to release.
///
/// @see Allocate()
template< typename T, typename Allocator >
void Helium::ObjectPool< T, Allocator >::Release( T* pObject )
{
    HELIUM_ASSERT( pObject );

    // Acquire a reader lock on the pool to synchronize block allocations.
    ScopeReadLock readLock( m_poolBlockAllocationLock );

    // Synchronize access to the free object list.
    ScopeLock< SpinLock > scopeLock( m_freeObjectSpinLock );

    // Release the object to the pool.
    size_t freeObjectCount = m_freeObjectCount;

    HELIUM_ASSERT( m_ppFreeObjects );
    m_ppFreeObjects[ freeObjectCount ] = pObject;
    ++freeObjectCount;

    HELIUM_ASSERT( freeObjectCount <= m_blockSize * m_allocatedBlockCount );

    m_freeObjectCount = freeObjectCount;
}

/// Get a unique index associated with the given object
///
/// @param[in] pObject  Object from this pool for which to retrieve an index.
///
/// @return  Object index, or an invalid index if the object is not part of this pool.
///
/// @see GetObject()
template< typename T, typename Allocator >
size_t Helium::ObjectPool< T, Allocator >::GetIndex( T* pObject ) const
{
    HELIUM_ASSERT( pObject );

    size_t blockSize = m_blockSize;

    ScopeReadLock readLock( m_poolBlockAllocationLock );

    size_t baseIndex = 0;
    for( Block* pBlock = m_pHeadBlock; pBlock != NULL; pBlock = pBlock->pNext )
    {
        T* pBlockObjects = pBlock->pObjects;
        HELIUM_ASSERT( pBlockObjects );

        if( pObject >= pBlockObjects && pObject < pBlockObjects + blockSize )
        {
            return baseIndex + ( static_cast< size_t >( pObject - pBlockObjects ) );
        }

        baseIndex += blockSize;
    }

    return Invalid< size_t >();
}

/// Get the object associated with the given index.
///
/// Note that this does not check whether the object has been allocated, so it is possible to retrieve instances of
/// objects that have been released.
///
/// @param[in] index  Object index.
///
/// @return  Pointer to the specified object, or null if the index does not reference a valid object in this pool.
template< typename T, typename Allocator >
T* Helium::ObjectPool< T, Allocator >::GetObject( size_t index ) const
{
    HELIUM_ASSERT( IsValid( index ) );

    size_t blockSize = m_blockSize;

    ScopeReadLock readLock( m_poolBlockAllocationLock );

    for( Block* pBlock = m_pHeadBlock; pBlock != NULL; pBlock = pBlock->pNext )
    {
        if( index < blockSize )
        {
            HELIUM_ASSERT( pBlock->pObjects );

            return pBlock->pObjects + index;
        }

        index -= blockSize;
    }

    return NULL;
}

/// Allocate a new block of objects.  Assumes any necessary locks are in place.
template< typename T, typename Allocator >
void Helium::ObjectPool< T, Allocator >::AllocateBlock()
{
    Allocator allocator;

    // Update the allocated block count.
    size_t allocatedBlockCount = m_allocatedBlockCount;
    HELIUM_ASSERT( allocatedBlockCount != m_blockCountMax );
    ++allocatedBlockCount;
    m_allocatedBlockCount = allocatedBlockCount;

    // Allocate a new block.
    size_t blockSize = m_blockSize;

    size_t alignment = Max< size_t >( boost::alignment_of< T >::value, boost::alignment_of< Block >::value );
    size_t alignedBufferSize = Align( sizeof( T ) * blockSize, alignment );

    void* pBuffer = allocator.AllocateAligned( alignment, alignedBufferSize + sizeof( Block ) );
    HELIUM_ASSERT( pBuffer );

    T* pObjects = ArrayInPlaceConstruct< T >( pBuffer, blockSize );
    HELIUM_ASSERT( pObjects );

    Block* pBlock = reinterpret_cast< Block* >( static_cast< uint8_t* >( pBuffer ) + alignedBufferSize );
    pBlock->pObjects = pObjects;

    pBlock->pNext = m_pHeadBlock;
    m_pHeadBlock = pBlock;

    // Reallocate the free object pool array.
    T* volatile * ppFreeObjects = m_ppFreeObjects;
    ppFreeObjects = static_cast< T* volatile * >( allocator.Reallocate(
        const_cast< T** >( m_ppFreeObjects ),
        sizeof( T* ) * allocatedBlockCount * blockSize ) );
    HELIUM_ASSERT( ppFreeObjects );

    m_ppFreeObjects = ppFreeObjects;

    // Add the new block of objects to the free list.
    HELIUM_ASSERT( m_freeObjectCount == 0 );
    m_freeObjectCount = blockSize;

    for( size_t objectIndex = 0; objectIndex < blockSize; ++objectIndex )
    {
        ppFreeObjects[ objectIndex ] = pObjects;
        ++pObjects;
    }
}
