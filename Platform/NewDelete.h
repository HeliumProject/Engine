#pragma once

#ifndef L_NEW_DELETE_SPEC
#define L_NEW_DELETE_SPEC
#endif

/// Global "new" operator.
///
/// @param[in] size  Allocation size.
///
/// @return  Base address of the requested allocation.
L_NEW_DELETE_SPEC void* operator new( size_t size ) throw( std::bad_alloc )
{
    Lunar::DefaultAllocator allocator;
    void* pMemory = Lunar::AllocateAlignmentHelper( allocator, size );
    HELIUM_ASSERT( pMemory );
    if( !pMemory )
    {
        throw std::bad_alloc();
    }

    return pMemory;
}

/// Global "new" operator, no-throw variant.
///
/// @param[in] size  Allocation size.
///
/// @return  Base address of the requested allocation if successful, null pointer if not successful.
L_NEW_DELETE_SPEC void* operator new( size_t size, const std::nothrow_t& ) throw()
{
    Lunar::DefaultAllocator allocator;
    void* pMemory = Lunar::AllocateAlignmentHelper( allocator, size );
    HELIUM_ASSERT( pMemory );

    return pMemory;
}

/// Global array "new" operator.
///
/// @param[in] size  Allocation size.
///
/// @return  Base address of the requested allocation if successful.
L_NEW_DELETE_SPEC void* operator new[]( size_t size ) throw( std::bad_alloc )
{
    Lunar::DefaultAllocator allocator;
    void* pMemory = Lunar::AllocateAlignmentHelper( allocator, size );
    HELIUM_ASSERT( pMemory );
    if( !pMemory )
    {
        throw std::bad_alloc();
    }

    return pMemory;
}

/// Global array "new" operator, no-throw variant.
///
/// @param[in] size  Allocation size.
///
/// @return  Base address of the requested allocation if successful, null pointer if not successful.
L_NEW_DELETE_SPEC void* operator new[]( size_t size, const std::nothrow_t& ) throw()
{
    Lunar::DefaultAllocator allocator;
    void* pMemory = Lunar::AllocateAlignmentHelper( allocator, size );
    HELIUM_ASSERT( pMemory );

    return pMemory;
}

/// Global "delete" operator.
///
/// @param[in] pMemory  Base address of the memory to free.
L_NEW_DELETE_SPEC void operator delete( void* pMemory ) throw()
{
    Lunar::DefaultAllocator().Free( pMemory );
}

/// Global "delete" operator, no-throw variant.
///
/// @param[in] pMemory  Base address of the memory to free.
L_NEW_DELETE_SPEC void operator delete( void* pMemory, const std::nothrow_t& ) throw()
{
    Lunar::DefaultAllocator().Free( pMemory );
}

/// Global array "delete" operator.
///
/// @param[in] pMemory  Base address of the memory to free.
L_NEW_DELETE_SPEC void operator delete[]( void* pMemory ) throw()
{
    Lunar::DefaultAllocator().Free( pMemory );
}

/// Global array "delete" operator, no-throw variant.
///
/// @param[in] pMemory  Base address of the memory to free.
L_NEW_DELETE_SPEC void operator delete[]( void* pMemory, const std::nothrow_t& ) throw()
{
    Lunar::DefaultAllocator().Free( pMemory );
}
