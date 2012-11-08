/// Get the previous dynamic memory heap in the global list.
///
/// @return  Previous heap in the list.
///
/// @see GetNextHeap(), LockReadGlobalHeapList(), UnlockReadGlobalHeapList()
Helium::DynamicMemoryHeap* Helium::DynamicMemoryHeap::GetPreviousHeap() const
{
    return m_pPreviousHeap;
}

/// Get the next dynamic memory heap in the global list.
///
/// @return  Next heap in the list.
///
/// @see GetPreviousHeap(), LockReadGlobalHeapList(), UnlockReadGlobalHeapList()
Helium::DynamicMemoryHeap* Helium::DynamicMemoryHeap::GetNextHeap() const
{
    return m_pNextHeap;
}

#if !HELIUM_RELEASE && !HELIUM_PROFILE
/// Get the debugging name associated with this heap.
///
/// @return  Heap name string.
const tchar_t* Helium::DynamicMemoryHeap::GetName() const
{
    return m_pName;
}
#endif

#if HELIUM_ENABLE_MEMORY_TRACKING
/// Get the current number of active allocations in this heap.
///
/// @return  Active allocation count.
///
/// @see GetBytesActual()
size_t Helium::DynamicMemoryHeap::GetAllocationCount() const
{
    return m_allocationCount;
}

/// Get the total number of usable bytes allocated from this heap.
///
/// @return  Usable bytes allocated.
///
/// @see GetAllocationCount()
size_t Helium::DynamicMemoryHeap::GetBytesActual() const
{
    return m_bytesActual;
}
#endif

/// Allocate a block of memory from this allocator's heap.
///
/// @param[in] size  Number of bytes to allocate.
///
/// @return  Base address of the allocation if successful, null pointer if not.
///
/// @see MemoryHeap::Allocate(), Reallocate(), Free()
HELIUM_FORCEINLINE void* Helium::DefaultAllocator::Allocate( size_t size )
{
    return HELIUM_DEFAULT_HEAP.Allocate( size );
}

/// Resize an allocation previously allocated using Allocate() or Reallocate().
///
/// Note that this should not be called on allocations made using AllocateAligned().
///
/// @param[in] pMemory  Base address of an allocation to resize.  If this is null, this will merely behave in the
///                     same fashion as if Allocate() was called directly.
/// @param[in] size     Size to which the allocation should be reallocated, in bytes.  If this is zero, this will
///                     merely behave in the same fashion as if Free() was called directly.
///
/// @return  Base address of the resized allocation if successful, null pointer if the memory could not be
///          reallocated or, in the case size is zero, was freed.  The original address provided should be discarded
///          if a valid pointer was returned or a zero-byte reallocation was requested.
///
/// @see MemoryHeap::Reallocate(), Allocate(), Free()
HELIUM_FORCEINLINE void* Helium::DefaultAllocator::Reallocate( void* pMemory, size_t size )
{
    return HELIUM_DEFAULT_HEAP.Reallocate( pMemory, size );
}

/// Allocate an aligned block of memory from this allocator's heap.
///
/// @param[in] alignment  Alignment of the allocation, in bytes.  This must be a power of two.
/// @param[in] size       Number of bytes to allocate.
///
/// @return  Base address of the allocation if successful, null pointer if not.
///
/// @see MemoryHeap::AllocateAligned(), Free()
HELIUM_FORCEINLINE void* Helium::DefaultAllocator::AllocateAligned( size_t alignment, size_t size )
{
    return HELIUM_DEFAULT_HEAP.AllocateAligned( alignment, size );
}

/// Free a block of memory previously allocated using Allocate(), Reallocate(), or AllocateAligned().
///
/// @param[in] pMemory  Base address of the allocation to free.  If this is a null pointer, no action will be
///                     performed.
///
/// @see MemoryHeap::AllocateAligned(), Allocate(), Reallocate(), AllocateAligned()
HELIUM_FORCEINLINE void Helium::DefaultAllocator::Free( void* pMemory )
{
    HELIUM_DEFAULT_HEAP.Free( pMemory );
}

/// Get the size of an allocated memory block.
///
/// @param[in] pMemory  Base address of the allocation.
///
/// @return  Allocation size in bytes.
HELIUM_FORCEINLINE size_t Helium::DefaultAllocator::GetMemorySize( void* pMemory )
{
    return HELIUM_DEFAULT_HEAP.GetMemorySize( pMemory );
}

/// Constructor.
///
/// @param[in] blockSize         Number of bytes to allocate for each stack block.
/// @param[in] blockCountMax     Maximum number of blocks that can be allocated, clamped to a minimum of one.  If
///                              this is Invalid< size_t >(), no limit will be applied to the number of blocks that
///                              can be allocated.
/// @param[in] defaultAlignment  Byte alignment for allocations made using Allocate() (must be a power of two).
template< typename Allocator >
Helium::StackMemoryHeap< Allocator >::StackMemoryHeap(
    size_t blockSize,
    size_t blockCountMax,
    size_t defaultAlignment )
    : m_blockSize( blockSize > 1 ? blockSize : 1 )
    , m_defaultAlignment( defaultAlignment > 1 ? defaultAlignment : 1 )
    , m_remainingBlockCount( blockCountMax > 1 ? blockCountMax : 1 )
{
    HELIUM_ASSERT( ( m_defaultAlignment & ( m_defaultAlignment - 1 ) ) == 0 ); // affirm power of two

    Block* pBlock = AllocateBlock();
    HELIUM_ASSERT( pBlock );
    pBlock->m_pPreviousBlock = NULL;
    pBlock->m_pNextBlock = NULL;

    m_pHeadBlock = pBlock;
    m_pTailBlock = pBlock;
    m_pCurrentBlock = pBlock;

    HELIUM_ASSERT( pBlock->m_pBuffer );
    m_pStackPointer = static_cast< uint8_t* >( pBlock->m_pBuffer ) + m_blockSize;
}

/// Destructor.
template< typename Allocator >
Helium::StackMemoryHeap< Allocator >::~StackMemoryHeap()
{
    // Blocks are allocated as part of the buffer associated with them, so we only need to free the buffer
    // addresses.
    Allocator allocator;

    Block* pNextBlock = m_pHeadBlock;
    while( pNextBlock )
    {
        Block* pBlock = pNextBlock;
        pNextBlock = pBlock->m_pNextBlock;

        HELIUM_ASSERT( pBlock->m_pBuffer );
        allocator.Free( pBlock->m_pBuffer );
    }
}

/// @copydoc MemoryHeap::Allocate()
template< typename Allocator >
void* Helium::StackMemoryHeap< Allocator >::Allocate( size_t size )
{
    return AllocateAligned( m_defaultAlignment, size );
}

/// @copydoc MemoryHeap::Reallocate()
template< typename Allocator >
void* Helium::StackMemoryHeap< Allocator >::Reallocate( void* /*pMemory*/, size_t /*size*/ )
{
    HELIUM_ASSERT_MSG_FALSE( TXT( "Reallocate() not supported by StackMemoryHeap" ) );

    return NULL;
}

/// @copydoc MemoryHeap::AllocateAligned()
template< typename Allocator >
void* Helium::StackMemoryHeap< Allocator >::AllocateAligned( size_t alignment, size_t size )
{
    HELIUM_ASSERT( ( alignment & ( alignment - 1 ) ) == 0 ); // affirm power of two

    // Check whether the allocation will fit within the current block.
    Block* pBlock = m_pCurrentBlock;
    HELIUM_ASSERT( pBlock );

    uint8_t* pBasePointer = static_cast< uint8_t* >( pBlock->m_pBuffer );
    HELIUM_ASSERT( pBasePointer );

    uint8_t* pStackPointerAligned = Align( static_cast< uint8_t* >( m_pStackPointer ), alignment );
    HELIUM_ASSERT( pStackPointerAligned );

    size_t usedBlockSize = pStackPointerAligned - pBasePointer;
    if( usedBlockSize > m_blockSize || size > m_blockSize - usedBlockSize )
    {
        // Check whether the allocation will fit within the next block (which should either be unused or not yet
        // allocated).
        pBlock = pBlock->m_pNextBlock;
        if( !pBlock )
        {
            // Allocate a fresh block so we can test it.
            if( m_remainingBlockCount == 0 )
            {
                // We can't allocate any more blocks, so the allocation cannot succeed.
                return NULL;
            }

            pBlock = AllocateBlock();
            HELIUM_ASSERT( pBlock );
            pBlock->m_pPreviousBlock = m_pTailBlock;
            pBlock->m_pNextBlock = NULL;

            if( m_pTailBlock )
            {
                m_pTailBlock->m_pNextBlock = pBlock;
            }
            else
            {
                m_pHeadBlock = pBlock;
            }

            m_pTailBlock = pBlock;
        }

        pBasePointer = static_cast< uint8_t* >( pBlock->m_pBuffer );
        HELIUM_ASSERT( pBasePointer );

        pStackPointerAligned = Align( pBasePointer, alignment );

        size_t usedBlockSize = pStackPointerAligned - pBasePointer;
        if( usedBlockSize > m_blockSize || size > m_blockSize - usedBlockSize )
        {
            // There is no hope for the requested allocation.
            return NULL;
        }
    }

    // Allocation will fit, so update the stack data and return the properly aligned allocation base address.
    m_pCurrentBlock = pBlock;
    m_pStackPointer = pStackPointerAligned + size;

    return pStackPointerAligned;
}

// @copydoc Free()
template< typename Allocator >
void Helium::StackMemoryHeap< Allocator >::Free( void* pMemory )
{
    // Silently ignore null addresses.
    if( !pMemory )
    {
        return;
    }

    // Check if the allocation exists in the current block.
    HELIUM_ASSERT( m_pCurrentBlock );
    if( pMemory <= m_pStackPointer && pMemory >= m_pCurrentBlock->m_pBuffer )
    {
        // Allocation is in the current block, so we only need to adjust the stack pointer.
        m_pStackPointer = pMemory;

        return;
    }

    // Pointer possibly belongs to another level in the stack, so search for the block in which it resides.
    Block* pBlock = m_pCurrentBlock->m_pPreviousBlock;
    while( pBlock )
    {
        uint8_t* pBaseAddress = static_cast< uint8_t* >( pBlock->m_pBuffer );
        if( pMemory >= pBaseAddress && pMemory <= pBaseAddress + m_blockSize )
        {
            m_pCurrentBlock = pBlock;
            m_pStackPointer = pMemory;

            return;
        }

        pBlock = pBlock->m_pPreviousBlock;
    }

    HELIUM_ASSERT_MSG_FALSE(
        TXT( "Allocation does not exist in the current stack (may have already been popped via another " )
        TXT( "allocation)" ) );
}

// @copydoc GetMemorySize()
template< typename Allocator >
size_t Helium::StackMemoryHeap< Allocator >::GetMemorySize( void* /*pMemory*/ )
{
    HELIUM_ASSERT_MSG_FALSE( TXT( "GetMemorySize() is not supported by StackMemoryHeap" ) );

    return static_cast< size_t >( -1 ); 
}

/// Allocate an uninitialized block of memory for this heap.
///
/// @return  Allocated block.
template< typename Allocator >
typename Helium::StackMemoryHeap< Allocator >::Block* Helium::StackMemoryHeap< Allocator >::AllocateBlock()
{
    HELIUM_ASSERT( m_remainingBlockCount != 0 );
    if( IsValid( m_remainingBlockCount ) )
    {
        --m_remainingBlockCount;
    }

    size_t alignedBufferSize = Align( m_blockSize, std::alignment_of< Block >::value );

    void* pBuffer = Allocator().AllocateAligned( HELIUM_SIMD_ALIGNMENT, alignedBufferSize + sizeof( Block ) );
    HELIUM_ASSERT( pBuffer );

    Block* pBlock = reinterpret_cast< Block* >( static_cast< uint8_t* >( pBuffer ) + alignedBufferSize );
    pBlock->m_pBuffer = pBuffer;

    return pBlock;
}

/// Constructor.
template< typename Allocator >
Helium::StackMemoryHeap< Allocator >::Marker::Marker()
    : m_pHeap( NULL )
    , m_pStackPointer( NULL )
{
}

/// Constructor.
///
/// @param[in] rHeap  Heap from which to initialize this marker.
template< typename Allocator >
Helium::StackMemoryHeap< Allocator >::Marker::Marker( StackMemoryHeap& rHeap )
    : m_pHeap( &rHeap )
    , m_pStackPointer( rHeap.m_pStackPointer )
{
    HELIUM_ASSERT( m_pStackPointer );
}

/// Destructor.
///
/// This will automatically pop any currently set heap marker.
template< typename Allocator >
Helium::StackMemoryHeap< Allocator >::Marker::~Marker()
{
    Pop();
}

/// Set the current heap marker.
///
/// If this marker is already set to an existing heap location, that location will automatically be popped.
///
/// @param[in] rHeap  Heap from which to set this marker.
template< typename Allocator >
void Helium::StackMemoryHeap< Allocator >::Marker::Set( StackMemoryHeap& rHeap )
{
    Pop();

    m_pHeap = &rHeap;
    m_pStackPointer = rHeap.m_pStackPointer;
}

/// Pop the current heap marker.
///
/// This will pop any currently marked heap location, freeing all allocations made after the marker was set.  If the
/// marker is not currently set, this will have no effect.
template< typename Allocator >
void Helium::StackMemoryHeap< Allocator >::Marker::Pop()
{
    if( m_pHeap )
    {
        HELIUM_ASSERT( m_pStackPointer );
        m_pHeap->Free( m_pStackPointer );

        m_pHeap = NULL;
        m_pStackPointer = NULL;
    }
}

/// Delete an object created from a specific allocator or heap.
///
/// @param[in] rAllocator  Reference to an allocator or Helium::MemoryHeap to use for allocations.
/// @param[in] pObject     Object to delete.
template< typename T, typename Allocator >
void Helium::DeleteHelper( Allocator& rAllocator, T* pObject )
{
    if( pObject )
    {
        pObject->~T();
        rAllocator.Free( pObject );
    }
}

/// Construct a new array.
///
/// @param[in] rAllocator  Reference to an allocator or Helium::MemoryHeap to use for allocations.
/// @param[in] count       Number of elements in the array to create.
///
/// @return  Pointer to the first element in the newly constructed array.
template< typename T, typename Allocator >
T* Helium::NewArrayHelper( Allocator& rAllocator, size_t count )
{
    return NewArrayHelper< T >( rAllocator, count, std::has_trivial_destructor< T >() );
}

/// Construct a new array of a type with a trivial destructor.
///
/// @param[in] rAllocator             Reference to an allocator or Helium::MemoryHeap to use for allocations.
/// @param[in] count                  Number of elements in the array to create.
/// @param[in] rHasTrivialDestructor  std::true_type.
///
/// @return  Pointer to the first element in the newly constructed array.
template< typename T, typename Allocator >
T* Helium::NewArrayHelper( Allocator& rAllocator, size_t count, const std::true_type& /*rHasTrivialDestructor*/ )
{
    size_t size = count * sizeof( T );

    // For allocations that may need to be SIMD-aligned, allocate aligned memory.
    void* pMemory;
    if( sizeof( T ) >= HELIUM_SIMD_SIZE )
    {
        pMemory = rAllocator.AllocateAligned( HELIUM_SIMD_ALIGNMENT, size );
    }
    else
    {
        pMemory = rAllocator.Allocate( size );
    }

    HELIUM_ASSERT( pMemory );

    return ArrayInPlaceConstruct< T >( pMemory, count );
}

/// Construct a new array of a type with a non-trivial destructor.
///
/// @param[in] rAllocator             Reference to an allocator or Helium::MemoryHeap to use for allocations.
/// @param[in] count                  Number of elements in the array to create.
/// @param[in] rHasTrivialDestructor  std::false_type.
///
/// @return  Pointer to the first element in the newly constructed array.
template< typename T, typename Allocator >
T* Helium::NewArrayHelper( Allocator& rAllocator, size_t count, const std::false_type& /*rHasTrivialDestructor*/ )
{
    size_t size = count * sizeof( T );
    size_t allocationOffset = sizeof( size_t );

    // Since we need to worry about destruction of the array elements, we want to allocate some extra space to store
    // the number of elements that need to be deleted.  The array count will be stored in a size_t value stored
    // immediately before the allocation pointer we return to the application.  Note that for possible SIMD-aligned
    // types, we need to allocate an extra HELIUM_SIMD_ALIGNMENT bytes instead of just sizeof( size_t ) to keep the
    // address returned to the application properly aligned.
    void* pMemory;
    if( sizeof( T ) >= HELIUM_SIMD_SIZE )
    {
        allocationOffset = HELIUM_SIMD_ALIGNMENT;
        HELIUM_ASSERT( allocationOffset >= sizeof( size_t ) );
        pMemory = rAllocator.AllocateAligned( HELIUM_SIMD_ALIGNMENT, size + HELIUM_SIMD_ALIGNMENT );
    }
    else
    {
        pMemory = rAllocator.Allocate( size + sizeof( size_t ) );
    }

    HELIUM_ASSERT( pMemory );

    // Offset the allocation and store the array element count.
    if( pMemory )
    {
        pMemory = static_cast< uint8_t* >( pMemory ) + allocationOffset;
        *( static_cast< size_t* >( pMemory ) - 1 ) = count;
    }

    return ArrayInPlaceConstruct< T >( pMemory, count );
}

/// Delete an allocated array.
///
/// @param[in] rAllocator  Reference to an allocator or Helium::MemoryHeap to use for allocations.
/// @param[in] pArray      Array to delete.
template< typename T, typename Allocator >
void Helium::DeleteArrayHelper( Allocator& rAllocator, T* pArray )
{
    DeleteArrayHelper< T >( rAllocator, pArray, std::has_trivial_destructor< T >() );
}

/// Delete an allocated array of a type with a trivial destructor.
///
/// @param[in] rAllocator             Reference to an allocator or Helium::MemoryHeap to use for allocations.
/// @param[in] pArray                 Array to delete.
/// @param[in] rHasTrivialDestructor  std::true_type.
template< typename T, typename Allocator >
void Helium::DeleteArrayHelper( Allocator& rAllocator, T* pArray, const std::true_type& /*rHasTrivialDestructor*/ )
{
    rAllocator.Free( pArray );
}

/// Delete an allocated array of a type with a non-trivial destructor.
///
/// @param[in] rAllocator             Reference to an allocator or Helium::MemoryHeap to use for allocations.
/// @param[in] pArray                 Array to delete.
/// @param[in] rHasTrivialDestructor  std::false_type.
template< typename T, typename Allocator >
void Helium::DeleteArrayHelper( Allocator& rAllocator, T* pArray, const std::false_type& /*rHasTrivialDestructor*/ )
{
    if( pArray )
    {
        // Get the array element count stored immediately prior to the given memory allocation.
        size_t count = *( reinterpret_cast< size_t* >( pArray ) - 1 );

        // Destroy the array elements.
        for( size_t index = 0; index < count; ++index )
        {
            pArray[ index ].~T();
        }

        // Free the actual allocation, taking into account the offset applied by NewArrayHelper().
        size_t allocationOffset = sizeof( size_t );
        if( sizeof( T ) >= HELIUM_SIMD_SIZE )
        {
            allocationOffset = HELIUM_SIMD_ALIGNMENT;
        }

        void* pMemory = reinterpret_cast< uint8_t* >( pArray ) - allocationOffset;
        rAllocator.Free( pMemory );
    }
}

/// Allocate memory for a "new" operator override, aligning the allocation if it is potentially necessary.
///
/// Since global "new" overrides don't provide type information, this provides proper alignment in case the memory
/// being allocated contains any data types that need to be aligned for SIMD operation usage.  If an allocation is
/// larger than the size of SIMD vector types, the allocation will be aligned to the platform's SIMD alignment in
/// case the allocation contains any elements that need to be SIMD aligned.
///
/// @param[in] rAllocator  Reference to an allocator or Helium::MemoryHeap to use for allocations.
/// @param[in] size        Number of bytes to allocate.
///
/// @return  Pointer to the requested memory if allocation was successful, null pointer if allocation failed.
template< typename Allocator > void* Helium::AllocateAlignmentHelper( Allocator& rAllocator, size_t size )
{
    HELIUM_ASSERT( size != 0 );

    if ( size >= HELIUM_SIMD_SIZE )
    {
        return rAllocator.AllocateAligned( HELIUM_SIMD_ALIGNMENT, size );
    }

    return rAllocator.Allocate( size );
}

#if !HELIUM_DEBUG
#define HELIUM_NEW_DELETE_SPEC HELIUM_FORCEINLINE
#include "Platform/NewDelete.h"
#undef HELIUM_NEW_DELETE_SPEC
#endif

/// Create a new object using a Helium::MemoryHeap instance.
///
/// @param[in] size   Allocation size.
/// @param[in] rHeap  Reference to the MemoryHeap from which to allocate memory.
///
/// @return  Base address of the requested allocation if successful.
void* operator new( size_t size, Helium::MemoryHeap& rHeap )
{
    void* pMemory = Helium::AllocateAlignmentHelper( rHeap, size );
    HELIUM_ASSERT( pMemory );

    return pMemory;
}
